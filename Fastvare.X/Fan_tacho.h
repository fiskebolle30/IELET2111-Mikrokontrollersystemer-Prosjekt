/* 
 * File:   Fan_tacho.h
 * Author: steinar
 *
 * Created on 24 April 2024, 14:49
 * 
 * Not much in the header file, as most of the tachometer is interrupt driven.
 */

#ifndef FAN_TACHO_H
#define	FAN_TACHO_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include "Fan_monitor_TWI.h" //To import the register bank.
void tacho_init();


#ifdef	__cplusplus
}
#endif

#endif	/* FAN_TACHO_H */

