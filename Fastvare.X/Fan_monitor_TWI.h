/* 
 * File:   Fan_monitor_TWI.h
 * Author: steinar
 *
 * Created on 19 March 2024, 14:37
 */

#ifndef FAN_MONITOR_TWI_H
#define	FAN_MONITOR_TWI_H

    #ifdef	__cplusplus
    extern "C" {
    #endif



#include <avr/io.h>
#include <avr/interrupt.h>

#define FAN_TWI_CLIENT_ADDRESS 58

#define FAN_REG_LENGTH 9

volatile uint8_t Fan_reg[FAN_REG_LENGTH]; //Register bank. Volatile since it will be used in interrupts.

/* Defines for what the different bytes in the Fan_reg array represent: */

#define CURR_TEMP 0 

#define CURR_FAN_0_SPEED_H 1 //Dependent on the measurement/logging period.
#define CURR_FAN_0_SPEED_L 2
#define CURR_FAN_1_SPEED_H 3
#define CURR_FAN_1_SPEED_L 4

#define ERROR_BYTE 5 //The different error bits in the byte are defined below.

#define TEMP_ALARM_LEVEL 6

#define LOGGING_PERIOD_H 7 //How often fan speed is written to the log, and how long the fan counter can count per measurement.
#define LOGGING_PERIOD_L 8
//Remember to extend FAN_REG_LENGTH!!!!! (max value + 1)


/* Error byte bit positions: */

#define ERR_FAN0_bp 0 //Fan stopped
#define ERR_FAN1_bp 1
#define ERR_TEMP_bp 2
#define ERR_CNT_OVF_bp 3 //Fan counter overflowed


void TWI0_client_init (void);



#ifdef	__cplusplus
}
#endif

#endif	/* FAN_MONITOR_TWI_H */

