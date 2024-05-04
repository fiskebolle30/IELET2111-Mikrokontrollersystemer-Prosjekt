#include "Fan_monitor_TWI.h"

uint8_t Fan_reg_pointer; //Pointer-ish (really offset) for the fan register array.

void TWI0_client_init ( void )
{
    //Load previous settings from EEPROM? No time to implement, load default values instead:
    Fan_reg[MEASUREMENT_PERIOD_H] = 0x04;
    Fan_reg[MEASUREMENT_PERIOD_L] = 0x00;
    Fan_reg[FAN_TIMEOUT] = 20;
    
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
        if(Fan_reg_pointer == CLEAR_ERROR)
        {
            Fan_reg[ERROR_BYTE] &= ~TWI0.SDATA; //Clear the error bits
        }
        Fan_reg[Fan_reg_pointer] = TWI0.SDATA; //Set data to the byte pointed to by Fan_reg_pointer.
        ++Fan_reg_pointer;
    }
    
    if(Fan_reg_pointer > FAN_REG_LENGTH) //If the pointer has "overflowed":
    {
        TWI0.SCTRLB = TWI_ACKACT_NACK_gc | TWI_SCMD_RESPONSE_gc;
    }
    else
    {
        TWI0.SCTRLB = TWI_ACKACT_ACK_gc | TWI_SCMD_RESPONSE_gc;
    }

}

ISR(TWI0_TWIS_vect) //There's only one interrupt for TWI client mode. Using if statements to determine why the interrupt was called.
{
    if(TWI0.SSTATUS & TWI_APIF_bm) //If this interrupt is for an address match:
    {
        if(!(TWI0.SSTATUS & TWI_DIR_bm)) //And if the data direction is host write:
        {
            pointer_is_set = 0; //Pointer will get set at the first transaction of every host write.
        }
        
        TWI0.SCTRLB = TWI_ACKACT_ACK_gc | TWI_SCMD_RESPONSE_gc; //Send ACK on address match.
    }
    
    
    if(TWI0.SSTATUS & TWI_DIF_bm) //If this is a data interrupt:
    {
        if(TWI0.SSTATUS & TWI_DIR_bm) //If this is a read operation, data from client to host:
        {
            handle_read();
        }
        else //This is a write operation:
        {
            handle_write();
        }
    }
}