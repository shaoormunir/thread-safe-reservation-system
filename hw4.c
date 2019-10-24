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
int rows = 16;
int cols = 7;
int waiting = 0;

void initialize_tables()
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 1; j < cols; j++)
        {
            reservations[i][j] = 0;
        }
    }
}

void print_restaurant_status()
{
    for (int i = 0; i < rows; i++)
    {
        printf("%2d", i);
        printf(": ");

        for (int j = 1; j < cols; j++)
        {
            char reservation = reservations[i][j];
            if (reservation == 0)
                printf(".");
            else
                printf("%c", reservation);
        }
        printf("\n");
    }
    printf("Number of waiting parties: %d\n", waiting);
}

void *reserve_table(size_t num_guests, const char *names)
{
}

const char get_guest_name(void *party, size_t seat_num)
{
}

void main()
{
    initialize_tables();
    hw4_test();
}