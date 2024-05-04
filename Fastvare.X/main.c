/* 
 * File:   main.c
 * Author: Henrik (og Steinar)
 *
 * Created on 12 March 2024, 14:06
 */


#define F_CPU 4000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>  //library neccesary for printf USART function
#include "Fan_read_thermistor_and_voltage.h"
#include "Fan_monitor_TWI.h"
#include "Fan_tacho.h"


int main(void)
{
    TWI0_client_init();
    tacho_init();
    
    PORTC.DIR = 0xFF; //Error outputs.
    PORTC.OUT = 0x00; //No errors at the start.
    
	// en "1" i registeret betyr innput
	adc_init();
    USART3_init();
    
    stdout = &USART_stream;
	while (1) 
    {	
        _delay_ms(100);
        
        printf("\n");
        
        uint16_t adcValInternal = adc_internal_read();
        Draw_to_terminal(adcValInternal, "Internal");
        
        uint16_t adcValExternal = adc_external_read();
        Draw_to_terminal(adcValExternal, "External");
        
        uint16_t adcValThermistor = adc_thermistor_read();
        Draw_to_terminal(adcValThermistor, "Thermistor");
        float thermistorTemp = find_temp(adcValThermistor);
        char thermistorTempStr[10];
        dtostrf(thermistorTemp, 6, 3, thermistorTempStr); // Converts the value to string with 3 decimal places
        printf("\n %s %s", "Thermistor temperature: ", thermistorTempStr);
      
        
        
        printf("\n");
        
        PORTC.OUT &= (1 << 7);
        PORTC.OUT |= Fan_reg[ERROR_BYTE]; //Periodically set error outputs.
        
    }
}