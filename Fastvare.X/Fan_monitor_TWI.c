#include "Fan_monitor_TWI.h"

void TWI0_client_init ( void )
{
    // Pin configuration
    PORTA.DIRSET = PIN2_bm // SDA
    | PIN3_bm ; // SCL
    PORTA.PINCONFIG = PORT_PULLUPEN_bm ; // Enable pullups, in case the host doesn't.
    PORTA.PINCTRLUPD = PIN2_bm | PIN3_bm ;
    TWI0.SADDR = (TWI_CLIENT_ADDRESS << 1); //Set client address to 58, and disable General Call Address.
    TWI0.SCTRLA |= TWI_ENABLE_bm; //enable TWI
    sei();
}

ISR(TWI0_TWIS_vect)
{
    if(TWI0.SSTATUS & TWI_APIF_bm) //If this interrupt is for an address match:
    {
        
    }
}


    /* Misc. plans:
     * - REMEMBER OFFSETOF()!!!!!!!
     * - NACK if register pointer is set out of bounds
     * - NACK if register pointer increments out of bounds
     * - pseudocode:
     * if(ptr < offsetof(1st array)) {
     *  struct[ptr] OR MAYBE *(struct + ptr)
     * }
     * else if(ptr within 1st array) {
     *  handle looping array
     * }
     * else if(second array....
     * 
     * //default:
     * NACK();
     */

