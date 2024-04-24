/* 
 * File:   main.c
 * Author: steinar
 *
 * Created on 12 March 2024, 14:06
 */

#include <avr/io.h>
#include "Fan_monitor_TWI.h"



int main()
{
    TWI0_client_init();
    while(1)
    {
        asm("nop");
    }
}

