#include "Fan_monitor_TWI.h"

uint8_t Fan_reg_pointer; //Pointer-ish for the fan struct.

void TWI0_client_init ( void )
{
    // Pin configuration
    PORTA.DIRSET = PIN2_bm // SDA
    | PIN3_bm ; // SCL
    PORTA.PINCONFIG = PORT_PULLUPEN_bm ; // Enable pullups, in case the host doesn't.
    PORTA.PINCTRLUPD = PIN2_bm | PIN3_bm ;
    TWI0.SADDR = (FAN_TWI_CLIENT_ADDRESS << 1); //Set client address to 58, and disable General Call Address.
    TWI0.SCTRLA |= TWI_ENABLE_bm; //enable TWI
    sei();
}

uint8_t pointer_is_set;

ISR(TWI0_TWIS_vect)
{
    if(TWI0.SSTATUS & TWI_APIF_bm) //If this interrupt is for an address match:
    {
        if(!(TWI0.SSTATUS & TWI_DIR_bm)) //If the data direction is host write:
        {
            pointer_is_set = 0; //Pointer will get set at the first transaction of every host write.
        }
        TWI0.SCTRLB = TWI_ACKACT_ACK_gc | TWI_SCMD_RESPONSE_gc;
    }
    if(TWI0.SSTATUS & TWI_DIF_bm)
    {
        if(TWI0.SSTATUS & TWI_DIR_bm) //Read operation, data from client to host.
        {
            handle_read();
        }
        else //Write operation
        {
            handle_write();
        }
    }
}

inline void handle_read() //Transfer data to host
{
    
}

inline void handle_write() //Transfer data from host
{
    if(!pointer_is_set)
    {
        Fan_reg_pointer = TWI0.SDATA;
    }
    Fan_reg[2] = 2;
}


    /* Misc. plans:
     * - REMEMBER OFFSETOF()!!!!!!! and sizeof().
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

