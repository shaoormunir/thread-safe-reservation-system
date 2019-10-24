/**
 * @Author: add your name here
 * This file contains the code for a simple restaurant management system.
 */
/**
 * Unit test of your table management software. This will
 * reserve and release tables.
 */

/**
 * release_table() takes the pointer and passes it to get_row_number() function, 
 * which compares the memory address of that given pointer with other pointers in the array, 
 * if there is a match, it returns the row number back to caller, 
 * otherwise it returns -1. The caller then checks if there is already an existing party
 * seated on that table or not, if there isn't, it means that either the pointer is invalid,
 * or it has been already cleared.
 * 
 */

/**
 * If during the call for either get_num_waiting() or get_max_party_size(), some other
 * caller invokes the reserve_table() function in another thread, there is a possiblity
 * that the value of the functions would be outdated when it is returned back to the caller. 
 * 
 */

#include <errno.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include "hw4_test.c"

extern void hw4_test(void);

char reservations[16][7];
size_t rows = 16;
size_t cols = 7;
size_t waiting = 0;
size_t bookings;
sem_t book;
sem_t change_value;

void initialize_tables()
{
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 1; j < cols; j++)
        {
            reservations[i][j] = 0;
        }
    }
}

void print_restaurant_status()
{
    for (size_t i = 0; i < rows; i++)
    {
        printf("%2ld", i);
        printf(": ");

        for (size_t j = 1; j < cols; j++)
        {
            char reservation = reservations[i][j];
            if (reservation == 0)
                printf(".");
            else
                printf("%c", reservation);
        }
        printf("\n");
    }
    printf("Number of waiting parties: %ld\n", waiting);
}

bool check_if_space_available(size_t starting_row, size_t party_size)
{
    size_t current_row = starting_row;
    size_t current_party_size = party_size;

    do
    {
        if (reservations[current_row][0] == 0 && current_party_size <= cols - 1)
            return true;

        if (reservations[current_row][0] != 0)
            return false;

        if (current_row == rows - 1)
            current_row = 0;
        else
            current_row = current_row + 1;
        current_party_size = current_party_size - (cols - 1);

    } while (current_party_size > 0);
}

void assign_table_to_party(size_t row, size_t party_size, const char *names)
{
    size_t current_party_size = party_size;
    reservations[row][0] = party_size;
    size_t current_name = 0;

    for (size_t i = row; i < rows && current_party_size > 0; i++)
    {
        if (i != row)
            reservations[i][0] = -1;

        for (size_t j = 1; j < cols && current_party_size > 0; j++, current_party_size--, current_name++)
        {
            reservations[i][j] = names[current_name];
        }

        if (i == rows - 1)
            i = -1;
    }
}

void increase_waiting()
{
    sem_wait(&change_value);
    waiting++;
    sem_post(&change_value);
}

void decrease_waiting()
{
    sem_wait(&change_value);
    waiting--;
    sem_post(&change_value);
}

void increase_bookings(int increase)
{

    sem_wait(&change_value);
    bookings = bookings + (size_t)increase;
    sem_post(&change_value);
}

void decrease_bookings(int decrease)
{
    sem_wait(&change_value);
    bookings = bookings - (size_t)decrease;
    sem_post(&change_value);
}

int get_row_number(void *party)
{
    char *party_mapping = (char *)party;

    for (int i = 0; i < rows; i++)
    {
        char *row_location = &reservations[i][0];
        if (party_mapping == &reservations[i][0])
            return i;
    }
    return -1;
}

void *reserve_table(size_t num_guests, const char *names)
{

    if (num_guests >= 96)
    {
        errno = ENOMEM;
        return NULL;
    }
    increase_waiting();

    while (true)
    {
        sem_wait(&book);
        decrease_waiting();
        for (size_t i = 0; i < rows; i++)
        {
            if (check_if_space_available(i, num_guests))
            {
                increase_bookings(ceil((int)num_guests / 6.0));
                assign_table_to_party(i, num_guests, names);
                sem_post(&book);
                return (void *)&reservations[i][0];
            }
        }
        increase_waiting();
    }
}

const char get_guest_name(void *party, size_t seat_num)
{
    int row = get_row_number(party);

    if (row != -1)
    {
        int party_size = (int)reservations[row][0];

        if (party_size > (int)seat_num)
        {
            int guest_row = row + (int)floor(seat_num / 6.0);
            int guest_seat = (int)seat_num % 5;
            return reservations[guest_row][guest_seat];
        }
    }
    errno = EINVAL;
    return 0;
}

void release_table(void *party)
{
    int row = get_row_number(party);

    if (row != -1)
    {
        int party_size = (int)reservations[row][0];

        int total_rows = ceil(party_size / 6.0);
        if (party_size > 0)
        {
            decrease_bookings(ceil(party_size / 6.0));

            for (int i = row; party_size > 0; i++)
            {
                reservations[i][0] = 0;
                for (int j = 1; j < cols && party_size > 0; j++, party_size--)
                {
                    reservations[i][j] = 0;
                }

                if (i == rows - 1)
                    i = -1;
            }
            sem_post(&book);
        }
        else
        {
            printf("Sending error singal.\n");
            signal(SIGSEGV, fault_handler);
        }
    }
    else
    {
        printf("Sending error singal.\n");
        signal(SIGSEGV, fault_handler);
    }
}
size_t get_max_party_size(void)
{
    return (16 - bookings) * 6;
}
unsigned int get_num_waiting(void)
{
    return waiting;
}
void initialize_wait_state()
{
    sem_init(&book, 0, 1);
    sem_init(&change_value, 0, 1);
}

void main()
{
    initialize_tables();
    initialize_wait_state();
    hw4_test();
}