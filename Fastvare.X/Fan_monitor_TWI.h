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

#define FAN_REG_LENGTH 7

volatile uint8_t Fan_reg[FAN_REG_LENGTH]; //Register bank
volatile uint8_t fan_log[2000];

#define CURR_TEMP 0 //Defines for what the different bytes in the Fan_reg array are.
#define CURR_FAN_SPEED 1
#define CTRL_A 2
#define LOGGING_PERIOD_H 3
#define LOGGING_PERIOD_L 4
#define FAN_LOG_PTR_H 5
#define FAN_LOG_PTR_L 6
//Remember to extend FAN_REG_LENGTH!!!!! (max value + 1)

void TWI0_client_init (void);



#ifdef	__cplusplus
}
#endif

#endif	/* FAN_MONITOR_TWI_H */

