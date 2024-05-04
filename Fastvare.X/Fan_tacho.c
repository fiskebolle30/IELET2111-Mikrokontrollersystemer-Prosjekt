#include "Fan_tacho.h"

void tacho_init()
//Initialize IR tachometer. Fan blade passes are detected using the analog comparator, counting up on a timer.
{
    /* Setup analog comparators.*/
    
    AC0.MUXCTRL = AC_MUXNEG_AINN0_gc | AC_MUXPOS_AINP0_gc; //Set pinmux to PD2 and PD3. Don't care about initval or invert. (I think)
    AC0.CTRLA = AC_POWER_PROFILE0_gc //Maximum performance, the few extra tens of µA don't matter that much.
              | AC_HYSMODE_SMALL_gc //Small hysteresis for noise immunity.
              | AC_ENABLE_bm;
    EVSYS.CHANNEL0 = 0x20; //AC0_OUT. Couldn't find the proper define.
    
    AC1.MUXCTRL = AC_MUXNEG_AINN0_gc | AC_MUXPOS_AINP2_gc; //Set pinmux to PD5 and PD4.
    AC1.CTRLA = AC_POWER_PROFILE0_gc //Maximum performance, the few extra tens of µA don't matter that much.
              | AC_HYSMODE_SMALL_gc //Small hysteresis for noise immunity.
              | AC_ENABLE_bm;
    EVSYS.CHANNEL1 = 0x21; //AC1_OUT. Couldn't find the proper define.
    
    
    
    /* Setup TCB0 to count pulses from AC0 */
    
    TCB0.EVCTRL = TCB_CAPTEI_bm; //Enable capture event input, don't care about EDGE or FILTER.
    TCB0.CTRLB = TCB_CNTMODE_FRQ_gc; //Set the counter to frequency mode.
    TCB0.CTRLA = TCB_CLKSEL_EVENT_gc | TCB_ENABLE_bm; //Increment one per event, enable counter.
    
    EVSYS.USERTCB0COUNT = 1; //Set TCB0 to increment on events from channel (1-1) = ch.0.
    EVSYS.USERTCB0CAPT = 3; //Set TCB0 to capture and reset on events from channel (3-1) = ch.2.
    
    TCB0.INTCTRL = TCB_OVF_bm //Enable overflow interrupt to set an error bit,
                | TCB_CAPT_bm; //and enable capture interrupt on TCB0 to store captured values from TCB0 and TCB1 into the log,
    
    
    
    /* Setup TCB1 to count pulses from AC1 */
    
    TCB1.EVCTRL = TCB_CAPTEI_bm; //Enable capture event input, don't care about EDGE or FILTER.
    TCB1.CTRLB = TCB_CNTMODE_FRQ_gc; //Set the counter to frequency mode.
    TCB1.CTRLA = TCB_CLKSEL_EVENT_gc | TCB_ENABLE_bm; //Increment one per event, enable counter.
    
    EVSYS.USERTCB1COUNT = 2; //Set TCB1 to increment on events from channel (2-1) = ch.1.
    EVSYS.USERTCB1CAPT = 3; //Set TCB1 to capture and reset on events from channel (3-1) = ch.2.
    
    TCB1.INTCTRL = TCB_OVF_bm; //Enable overflow interrupt to set an error bit.
    
    
    
    /* Setup TCA0 to generate a periodic event on ch.2 to capture and then reset both TCB0 and TCB1: */
    
    TCA0.SINGLE.PERBUFH = Fan_reg[MEASUREMENT_PERIOD_H]; //Set the period of counting fan passes. 
    TCA0.SINGLE.PERBUFL = Fan_reg[MEASUREMENT_PERIOD_L];
    
    EVSYS.CHANNEL2 = 0x80; //Route the TCA0 overflow event onto event channel 2. Couldn't find a define, so the magic number from the datasheet is used.
    
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc | TCA_SINGLE_ENABLE_bm; //Clock source is clk_per divided by 1024, timer is enabled.
    sei(); //Set global interrupts.
}
uint16_t fan0_timeout = 0; //Counters to check if the fans have been stopped over time.
uint16_t fan1_timeout = 0;

inline void check_fan_errors() //Function to check if the fans are stopped.
{
    //Fan 0:
    if((Fan_reg[CURR_FAN_0_SPEED_H] == 0) && (Fan_reg[CURR_FAN_0_SPEED_L] == 0))
    {
        ++fan0_timeout;
        if(fan0_timeout > 1000) //If the fan has been idle for that long:
        {
            Fan_reg[ERROR_BYTE] |= (1 << ERR_FAN0_bp);
        }
    }
    else
    {
        fan0_timeout = 0;
    }
    
    //Fan 1:
    if((Fan_reg[CURR_FAN_0_SPEED_H] == 0) && (Fan_reg[CURR_FAN_0_SPEED_L] == 0))
    {
        ++fan1_timeout;
        if(fan1_timeout > 1000) //If the fan has been idle for that long:
        {
            Fan_reg[ERROR_BYTE] |= (1 << ERR_FAN0_bp);
        }
    }
    else
    {
        fan1_timeout = 0;
    }
    
    if((Fan_reg[ERROR_BYTE] & ((1 << ERR_FAN0_bp) | (1 << ERR_FAN1_bp))) == ((1 << ERR_FAN0_bp) | (1 << ERR_FAN1_bp))) //If both fans have an error:
    {
        Fan_reg[ERROR_BYTE] |= (1 << ERR_BOTH_FANS_bp); //activate both fans error
    }
}


ISR(TCB0_INT_vect)
{
    if(TCB0.INTFLAGS & TCB_OVF_bm)
    {
        Fan_reg[ERROR_BYTE] |= (1 << ERR_CNT_OVF_bp); //Set overflow error bit on overflow.
        TCB0.INTFLAGS = TCB_OVF_bm; //Clear interrupt flag.
    }
    if(TCB0.INTFLAGS & TCB_CAPT_bm) //Capture event from TCA0. Store capture registers from TCB0 and TCB1.
    {
        cli();
        Fan_reg[CURR_FAN_0_SPEED_H] = TCB0.CCMPH;
        Fan_reg[CURR_FAN_0_SPEED_L] = TCB0.CCMPL;
        
        Fan_reg[CURR_FAN_1_SPEED_H] = TCB1.CCMPH;
        Fan_reg[CURR_FAN_1_SPEED_L] = TCB1.CCMPL;
        sei();
        check_fan_errors();
    }
}

ISR(TCB1_INT_vect) //Overflow 
{
    if(TCB1.INTFLAGS & TCB_OVF_bm)
    {
        Fan_reg[ERROR_BYTE] |= (1 << ERR_CNT_OVF_bp); //Set overflow error bit on overflow.
        TCB1.INTFLAGS = TCB_OVF_bm; //Clear interrupt flag.
    }
}