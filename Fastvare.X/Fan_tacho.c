#include "Fan_tacho.h"

tacho_init()
{
    AC0.MUXCTRL = AC_MUXNEG_AINN0_gc | AC_MUXPOS_AINP0_gc; //Set pinmux. Don't care about initval or invert. (i think)
    AC0.CTRLA = AC_POWER_PROFILE0_gc //Maximum performance, the few extra tens of µA don't matter that much.
              | AC_HYSMODE_SMALL_gc //Small hysteresis for noise immunity.
              | AC_ENABLE_bm
              | AC_OUTEN_bm; //DEBUG ONLY
    EVSYS.CHANNEL0 = 0x20; //AC0_OUT. Couldn't find the proper define.
    
    AC1.MUXCTRL = AC_MUXNEG_AINN0_gc | AC_MUXPOS_AINP2_gc;
    AC1.CTRLA = AC_POWER_PROFILE0_gc //Maximum performance, the few extra tens of µA don't matter that much.
              | AC_HYSMODE_SMALL_gc //Small hysteresis for noise immunity.
              | AC_ENABLE_bm;
    EVSYS.CHANNEL1 = 0x21; //AC1_OUT. Couldn't find the proper define.
    
    EVSYS.USERTCB0CAPT = 1; //Set TCB0 to accept events from channel (1-1) into the capture event user.
}