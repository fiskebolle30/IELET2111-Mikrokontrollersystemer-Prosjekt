/* 
 * File:   Fan_read_thermistor_and_voltage.h
 * Author: henri
 *
 * Created on 24. april 2024, 14:25
 */

#ifndef FAN_READ_THERMISTOR_AND_VOLTAGE_H
#define	FAN_READ_THERMISTOR_AND_VOLTAGE_H

    #ifdef	__cplusplus
    extern "C" {
    #endif

#ifndef F_CPU
    #define F_CPU 4000000UL
#endif

#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>  //library neccesary for printf
#include <stdlib.h> //library neccesary for dtostrf conversion function
   
//define all used functions.
void USART3_sendChar(char c);
void USART3_init(void);
int USART3_printChar(char c, FILE *stream);
void adc_init(void);
uint16_t adc_thermistor_read();
float find_temp(uint16_t adcVal);
uint16_t adc_internal_read(void);
uint16_t adc_external_read(void);
float voltage_calculation(uint16_t adcVal);
void Draw_to_terminal(uint16_t adcValue, char Str[]);

void check_temperature_error(uint16_t adcThermistorVal);  //Function to check if the temperature is above the set max point.



#define USART3_BAUD_RATE(BAUD_RATE)((float)(F_CPU * 64 / (16 *(float)BAUD_RATE)) + 0.5)	//calculates the baudrate used for USART transmision  
static FILE USART_stream = FDEV_SETUP_STREAM(USART3_printChar, NULL, _FDEV_SETUP_WRITE); 
#define Voltage_ref_sel 2.048f //internal 2,048 volt reference for ADC. "f" for float

        
#ifdef	__cplusplus
}
#endif

#endif	/* FAN_READ_THERMISTOR_AND_VOLTAGE_H */

