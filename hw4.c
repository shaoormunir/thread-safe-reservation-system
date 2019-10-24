/**
 * @Author: add your name here
 * This file contains the code for a simple restaurant management system.
 */
/**
 * Unit test of your table management software. This will
 * reserve and release tables.
 */

#include <stdio.h>
#include "hw4_test.c"

extern void hw4_test(void);

char reservations[16][7];
size_t rows = 16;
size_t cols = 7;
size_t waiting = 0;

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
            i = 0;
    }
}

void *reserve_table(size_t num_guests, const char *names)
{
    for (size_t i = 0; i < rows; i++)
    {
        printf("Checking for row: %zu\n", i);

        if (check_if_space_available(i, num_guests))
        {
            printf("Space is available at row: %zu\n", i);
            assign_table_to_party(i, num_guests, names);
            return (void *)reservations[i];
        }
    }
    printf("No space is available.\n");
}

const char get_guest_name(void *party, size_t seat_num)
{
}

void main()
{
    initialize_tables();
    print_restaurant_status();
    // hw4_test();
    reserve_table(8, "ABCDEFGH");
    print_restaurant_status();
    reserve_table(6, "IJKLMN");
    print_restaurant_status();
    reserve_table(12, "OPQRSTUVWXYZ");
    print_restaurant_status();
    reserve_table(12, "OPQRSTUVWXYZ");
    print_restaurant_status();
    reserve_table(12, "OPQRSTUVWXYZ");
    print_restaurant_status();
    reserve_table(12, "OPQRSTUVWXYZ");
    print_restaurant_status();
    reserve_table(12, "OPQRSTUVWXYZ");
    print_restaurant_status();
    reserve_table(12, "OPQRSTUVWXYZ");
    print_restaurant_status();
    reserve_table(12, "OPQRSTUVWXYZ");
    print_restaurant_status();
    reserve_table(12, "OPQRSTUVWXYZ");
    print_restaurant_status();
    reserve_table(12, "OPQRSTUVWXYZ");
    print_restaurant_status();
    reserve_table(12, "OPQRSTUVWXYZ");
    print_restaurant_status();
}