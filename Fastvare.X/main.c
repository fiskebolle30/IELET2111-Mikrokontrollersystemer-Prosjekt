/* 
 * File:   main.c
 * Author: steinar
 *
 * Created on 12 March 2024, 14:06
 */

/*
 * Henrik test branch
 */

//thermistor reading

#define F_CPU  4000000UL
#include <util/delay.h>
#include <avr/io.h>
//VREF_REFSEL_2V048_gc = (0x01<<0),  /* Internal 2.048V reference */

// Initialize ADC
void adc_init() {
    //we will use PD7 fro analog input for thermistor
    
    VREF.ADC0REF = VREF_REFSEL_VDD_gc;  // Select reference voltage (Vref) to be AVCC step 1
    
    ADC0.CTRLA |= ADC_RESSEL_0_bm   //Resolution selection bit 0 mask.  Step 3
    ADC0.CTRLB = ADC_PRESC_DIV2_gc; // Adjust prescaler as needed for your clock frequency 
    ADC0.CTRLC = 0x0    //Selecting the highest clock frequency of the ADC clock step 7 
    ADC0.MUXPOS = ADC_MUXPOS_AIN7_gc;  // Select ADC input channel, here we use ADC7 which corresponds to PD7 on the curiosity nano. step
    
    ADC0.CTRLA |= ADC_ENABLE_bm;     //enables ADC step 11
}


int main(void)
{
	// en "1" i registeret betyr innput
	
	PORTB.DIRSET |= PIN3_bm;    //output for LED
    PORTB.DIRSET & ~PIN2_bm;
    
	uint8_t buttonState = 0;	//variabel for knappetilstanden, kunne kanskje vært en binær variabel og.
	
	PORTB.PIN2CTRL = PORT_PULLUPEN_bm;
	//Setter opp den integrerte pullup motstanden til knappen
    
    
    
    
    
	while (1) 
    {	
        
		buttonState = (PORTB.IN & PIN2_bm);
		//kjører en bitvis AND med PIN2 bitmasken som er 0 for alle verdier utenom bit nummer 2 (inngangen fra knappen) Da får vi kun med inngangen vi vil ha.
		//denne masken ANDes med inngangsregisterer.
		PORTB.OUT = (buttonState << 1);
    }
}


