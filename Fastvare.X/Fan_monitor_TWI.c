#include "Fan_monitor_TWI.h"

uint8_t Fan_reg_pointer; //Pointer for indexing the fan register array. 

void TWI0_client_init ( void )
{
    //Load default values:
    Fan_reg[MEASUREMENT_PERIOD_H] = 0x03; //T_meas = (this value)*prescaler(1024)/F_CPU(4,000,000).
    Fan_reg[MEASUREMENT_PERIOD_L] = 0xD0; //This default value of 0x03D0 gives a period of 0.25 seconds.
    Fan_reg[FAN_TIMEOUT] = 10; //How many measurement periods of stopped fan before the error triggers. default approx. 2.5s
    Fan_reg[TEMP_ALARM_LEVEL_H] = (1825 >> 8) & 0xFF; //ADC reading threshold for triggering overtemp alaram. Default value of 1825 translates to 30 d. Celsius.
    Fan_reg[TEMP_ALARM_LEVEL_L] = (1825 & 0xFF);
    
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

inline void handle_read() //Transfer data to host. Inline is so the interrupts don't need to push all the working registers to the stack.
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
        
        Fan_reg[Fan_reg_pointer] = TWI0.SDATA; //Set data to the byte pointed to by Fan_reg_pointer.
        switch(Fan_reg_pointer)
        {
            case CLEAR_ERROR: { //If writing to CLEAR_ERROR, clear error bits.
                Fan_reg[ERROR_BYTE] &= ~Fan_reg[CLEAR_ERROR]; //Clear the error bits
                break;
            }
            case MEASUREMENT_PERIOD_L: { //If the host has written to the low and high byte of meas_per, write that value into the buffered period register.
                cli(); //Clear interrupts before writing 16 bit register, just in case.
                TCA0.SINGLE.PERBUFH = Fan_reg[MEASUREMENT_PERIOD_H];
                TCA0.SINGLE.PERBUFL = Fan_reg[MEASUREMENT_PERIOD_L];
                sei();
                break;
            }
        }
        ++Fan_reg_pointer; //Increment pointer to next byte.
    }
    
    if(Fan_reg_pointer > FAN_REG_LENGTH) //If the pointer has reached a value outside of the array:
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