/*
===============================================================================
 Name        : edu_ciaa_nxp.c
 Author      : Fabian Nardone
 Version     : 1.0
 Description : main definition
===============================================================================
*/

#include <stdio.h>

int main(void) {

    printf("Hello World\n");

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
    }
    return 0 ;
}
