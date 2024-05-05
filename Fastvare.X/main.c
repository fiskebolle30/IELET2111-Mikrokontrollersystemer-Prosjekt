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
        check_temperature_error(adcValThermistor);
        Draw_to_terminal(adcValThermistor, "Thermistor");
        float thermistorTemp = find_temp(adcValThermistor);
        char thermistorTempStr[10];
        dtostrf(thermistorTemp, 6, 3, thermistorTempStr); // Converts the value to string with 3 decimal places
        printf("\n %s %s", "Thermistor temperature: ", thermistorTempStr);
      
        
        printf("\n");
        
        
        cli(); //Clear interrupts, so none of the registers are read in the middle of being written.
        Fan_reg[CURR_TEMP_H] = (adcValThermistor >> 8) & 0xFF; //Write the high byte of adcValThermistor.
        Fan_reg[CURR_TEMP_L] = adcValThermistor & 0xFF; //Write low byte.
        
        Fan_reg[CURR_EXT_VOLTAGE_H] = (adcValExternal >> 8) & 0xFF; //Write the high byte of adcValExternal.
        Fan_reg[CURR_EXT_VOLTAGE_L] = adcValExternal & 0xFF; //Write low byte.
        Fan_reg[CURR_INT_VOLTAGE_H] = (adcValInternal >> 8) & 0xFF; //Write the high byte of adcValInternal.
        Fan_reg[CURR_INT_VOLTAGE_L] = adcValInternal & 0xFF; //Write low byte.
        sei();
        
        PORTC.OUT &= (1 << 7); //Clear entire error register except for the debug output.
        PORTC.OUT |= Fan_reg[ERROR_BYTE]; //Periodically set error outputs.
        
    }
}