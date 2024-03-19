#include "Fan_monitor_TWI.h"

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
