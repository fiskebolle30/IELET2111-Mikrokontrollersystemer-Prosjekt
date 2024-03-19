/* 
 * File:   main.c
 * Author: steinar
 *
 * Created on 12 March 2024, 14:06
 */

#include <avr/io.h>

#define F_CPU 4000000UL
#define TWI0_BAUD F_CPU/(2*100000)

#define TWI_IS_CLOCKHELD() TWI0.MSTATUS & TWI_CLKHOLD_bm
#define TWI_IS_BUSERR() TWI0.MSTATUS & TWI_BUSERR_bm
#define TWI_IS_ARBLOST() TWI0.MSTATUS & TWI_ARBLOST_bm
#define TWI_IS_BUSBUSY() ((TWI0.MSTATUS & TWI_BUSSTATE_BUSY_gc) == TWI_BUSSTATE_BUSY_gc )
// Waiting macro for ensuring that bus is OK before proceeding .
#define TWI_WAIT() while (!((TWI_IS_CLOCKHELD () ) || ( TWI_IS_BUSERR () ) || (TWI_IS_ARBLOST() ) || ( TWI_IS_BUSBUSY() ) ) )

typedef struct Fan_struct //Typedef for TWI register bank
{
    uint8_t curr_temp;
    uint8_t curr_fan_speed;
    uint8_t fan_log[200];
} Fan_monitor_registers_t;

volatile Fan_monitor_registers_t Fan_reg; //Register bank

/*
 * 
 */

void TWI0_client_init ( void )
{
    // Pin configuration
    PORTA.DIRSET = PIN2_bm // SDA
    | PIN3_bm ; // SCL
    PORTA.PINCONFIG = PORT_PULLUPEN_bm ; // Enable pullups, in case the host doesn't.
    PORTA.PINCTRLUPD = PIN2_bm | PIN3_bm ;
    TWI0.SADDR = (58 << 1); //Set client address to 58, and disable General Call Address.
    TWI0.SCTRLA |= TWI_ENABLE_bm; //enable TWI
}


int main()
{
    
}

