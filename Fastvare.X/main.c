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
    PORTC.DIR = 0xFF; //Setup debug LEDs
    while(1)
    {
        PORTC.OUT = Fan_reg[DEBUG_LEDS];
    }
}

