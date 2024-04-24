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

#ifndef F_CPU
    #define F_CPU 4000000UL
#endif

#define TWI0_BAUD F_CPU/(2*100000)

#define FAN_TWI_CLIENT_ADDRESS 58


/* Having the register bank as a struct won't work with the way I was going to index it. Rip
typedef struct //Typedef for TWI register bank.
{
    uint8_t curr_temp;
    uint8_t curr_fan_speed;
    uint8_t ctrl_a;
    uint8_t logging_period_H; //High byte of how frequently to add values to the fan log.
    uint8_t logging_period_L; //Low byte
    uint8_t debug_LEDs;
    uint8_t fan_log[20000];
} Fan_monitor_registers_t; */

#define FAN_REG_LENGTH 20

volatile uint8_t Fan_reg[FAN_REG_LENGTH]; //Register bank
volatile uint8_t fan_log[2000];

#define CURR_TEMP 0 //Defines for what the different bytes in the Fan_reg array are.
#define CURR_FAN_SPEED 1
#define CTRL_A 2
#define LOGGING_PERIOD_H 3
#define LOGGING_PERIOD_L 4
#define FAN_LOG_PTR_H 5
#define FAN_LOG_PTR_L 6
//Remember to extend FAN_REG_LENGTH!!!!!

void TWI0_client_init (void);



#ifdef	__cplusplus
}
#endif

#endif	/* FAN_MONITOR_TWI_H */

