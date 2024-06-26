/* 
 * File:   Fan_monitor_TWI.h
 * Author: steinar
 *
 * Created on 19 March 2024, 14:37
 * 
 *  Very application-specific interrupt driven I2C handler for the fan monitor.
 */

#ifndef FAN_MONITOR_TWI_H
#define	FAN_MONITOR_TWI_H

    #ifdef	__cplusplus
    extern "C" {
    #endif



#include <avr/io.h>
#include <avr/interrupt.h>

#define FAN_TWI_CLIENT_ADDRESS 58 //The I2C client has the address 58.


/* Defines for what the different bytes in the Fan_reg array represent: */
#define CURR_TEMP_H 0
#define CURR_TEMP_L 1
        
#define CURR_INT_VOLTAGE_H 2
#define CURR_INT_VOLTAGE_L 3
#define CURR_EXT_VOLTAGE_H 4
#define CURR_EXT_VOLTAGE_L 5

#define CURR_FAN_0_SPEED_H 6 //Dependent on the measurement/logging period.
#define CURR_FAN_0_SPEED_L 7 //RPM = 60*(this value)/measurement period.
#define CURR_FAN_1_SPEED_H 8
#define CURR_FAN_1_SPEED_L 9

#define ERROR_BYTE 10 //The different errors defined by the bits in the byte are defined below.
#define CLEAR_ERROR 11 //Writing to this byte will clear the written bits in ERROR_BYTE.

#define TEMP_ALARM_LEVEL_H 12  //thermistor ADC value that triggers error.
#define TEMP_ALARM_LEVEL_L 13
#define FAN_TIMEOUT 14 //How many measurement periods the fans need to be stopped to trigger the error.

#define MEASUREMENT_PERIOD_H 15 //How long the fan counter can count per measurement.
#define MEASUREMENT_PERIOD_L 16 //T_meas = (this value)*prescaler(=1024)/F_CPU(=4,000,000).

#define FAN_REG_LENGTH 17

volatile uint8_t Fan_reg[FAN_REG_LENGTH]; //Register bank, writable and readable by both I2C and this code. Volatile since it will be used in both interrupts and main.


/* Error byte bit positions: */

#define ERR_FAN0_bp 0 //Fan0 stopped error
#define ERR_FAN1_bp 1 //Fan1 stopped error
#define ERR_TEMP_bp 2 //Over temperature error
#define ERR_CNT_OVF_bp 3 //Fan counter overflowed
#define ERR_BOTH_FANS_bp 4 //Both fans stopped error
#define ERR_DEBUG_bp 7 //Just a general debug GPIO


void TWI0_client_init (void);



#ifdef	__cplusplus
}
#endif

#endif	/* FAN_MONITOR_TWI_H */
