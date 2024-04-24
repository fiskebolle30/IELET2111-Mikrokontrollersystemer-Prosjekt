#include "Fan_monitor_TWI.h"

uint8_t Fan_reg_pointer; //Pointer-ish (really offset) for the fan register array.

void TWI0_client_init ( void )
{
    // Pin configuration
    PORTA.DIRSET = PIN2_bm // SDA
    | PIN3_bm ; // SCL
    PORTA.PINCONFIG = PORT_PULLUPEN_bm ; // Enable pullups, in case the host doesn't.
    PORTA.PINCTRLUPD = PIN2_bm | PIN3_bm ;
    TWI0.SADDR = (FAN_TWI_CLIENT_ADDRESS << 1); //Set client address to 58, and disable General Call Address.
    TWI0.SCTRLA |= TWI_DIEN_bm | TWI_APIEN_bm | TWI_ENABLE_bm; //enable TWI
    sei();
}

uint8_t pointer_is_set;

inline void handle_read() //Transfer data to host
{
    //TODO: if pointer is at the second byte of the fan log pointer, dereference that pointer instead.
    TWI0.SDATA = Fan_reg[Fan_reg_pointer];
    ++Fan_reg_pointer;
    TWI0.SCTRLB = TWI_ACKACT_ACK_gc | TWI_SCMD_RESPONSE_gc;

}

inline void handle_write() //Transfer data from host
{
    if(!pointer_is_set)
    {
        Fan_reg_pointer = TWI0.SDATA;
        pointer_is_set = 1;
    }
    else
    {
        Fan_reg[Fan_reg_pointer] = TWI0.SDATA; //Set data to the byte pointed to by Fan_reg_pointer.
        ++Fan_reg_pointer;
    }
    
    if(Fan_reg_pointer > FAN_REG_LENGTH) //If the pointer has overflowed:
    {
        TWI0.SCTRLB = TWI_ACKACT_NACK_gc | TWI_SCMD_RESPONSE_gc;
    }
    else
    {
        TWI0.SCTRLB = TWI_ACKACT_ACK_gc | TWI_SCMD_RESPONSE_gc;
    }

}

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
    
    
    if(TWI0.SSTATUS & TWI_DIF_bm) //If this is a data interrupt:
    {
        if(TWI0.SSTATUS & TWI_DIR_bm) //If this is a read operation, data from client to host:
        {
            handle_read();
            asm("nop");
        }
        else //This is a write operation
        {
            handle_write();
        }
    }
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

