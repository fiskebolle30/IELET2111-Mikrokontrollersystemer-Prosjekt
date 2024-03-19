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

    #ifndef F_CPU
        #define F_CPU 4000000UL
    #endif

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


    void TWI0_client_init (void);

#ifdef	__cplusplus
}
#endif

#endif	/* FAN_MONITOR_TWI_H */

