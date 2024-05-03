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
	// en "1" i registeret betyr innput
	adc_init();
    USART3_init();
    
    TWI0_client_init();
    tacho_init();
    
    stdout = &USART_stream;
	while (1) 
    {	
        _delay_ms(1000);
        
        printf("\n");
        
        /*
        uint16_t adcValThermistor = adc_thermistor_read();
        printf("\n %s %u", "Thermistor ADC value: ", adcValThermistor);   //"new line type(string) type(unsigned integer)", "ADC value: ", adc value
        float temp_celcius = find_temp(adcValThermistor);
        char tempStr[10];
        dtostrf(temp_celcius, 6, 3, tempStr); // Converts the value to string with 3 decimal places
        printf("\n %s %s", "Temperature in degrees C1: ", tempStr);  //
        printf("\n %s %f", "Temperature in degrees C2: ", temp_celcius);   
            
        
        uint16_t adcValInternal = adc_internal_read();
        printf("\n %s %u", "Internal voltage ADC value: ", adcValInternal); 
        float intVDD = voltage_calculation(adcValInternal);
        char intStr[10];
        dtostrf(intVDD, 6, 3, intStr); // Converts the value to string with 3 decimal places
        printf("\n %s %s", "Internal voltage: ", intStr);
        
        
        
        uint16_t adcValExternal = adc_external_read();
        printf("\n %s %u", "External voltage ADC value: ", adcValExternal); 
        float intVDD2 = voltage_calculation(adcValExternal);
        char intStr2[10];
        dtostrf(intVDD2, 6, 3, intStr2); // Converts the value to string with 3 decimal places
        printf("\n %s %s", "External voltage: ", intStr2);
        */
        
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
        
    }
}