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
#include <stdio.h>  //library neccesary for printf
//VREF_REFSEL_2V048_gc = (0x01<<0),  /* Internal 2.048V reference */
uint16_t adcVal;
void USART3_sendChar(char c);
void USART3_init(void);
static int USART3_printChar(char c, FILE *stream);
void adc_init(void);

#define USART3_BAUD_RATE(BAUD_RATE)((float)(F_CPU * 64 / (16 *(float)BAUD_RATE)) + 0.5)	//lager baudraten. 
static FILE USART_stream = FDEV_SETUP_STREAM(USART3_printChar, NULL, _FDEV_SETUP_WRITE);


// Initialize ADC
void adc_init(void) {
    //we will use PD7 fro analog input for thermistor
    
    VREF.ADC0REF = VREF_REFSEL_VDD_gc;  // Select reference voltage (Vref) to be AVCC step 1
    
    ADC0.CTRLA |= ADC_RESSEL_12BIT_gc;   //Resolution selection bit 0 mask.  Step 3
    ADC0.CTRLC = ADC_PRESC_DIV2_gc; // Adjust prescaler as needed for your clock frequency  step 7 
    ADC0.MUXPOS = ADC_MUXPOS_AIN7_gc;  // Select ADC input channel, here we use ADC7 which corresponds to PD7 on the curiosity nano. step 8
    ADC0.CTRLA |= ADC_ENABLE_bm;     //enables ADC step 11
}

uint16_t adc_read() {
    // Start conversion
    ADC0.COMMAND = ADC_STCONV_bm;

    // Wait for conversion to complete
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));

    // Return the ADC result
    return ADC0.RES;
}



//---------------------USART functions------------------------ taken from UART ooving
void USART3_init(void)	//Setup funksjonen 
{
 PORTB.DIR &= ~PIN1_bm;	//endret til port B fra C. Setter innput
 PORTB.DIR |= PIN0_bm;	//endret til port B fra C. Setter output 
 PORTB.DIR |= PIN3_bm;	//setter internal LED som output

 USART3.BAUD = (uint16_t)USART3_BAUD_RATE(9600);	//Setter en baudrate på 9600 for utgang USART3 (måtte endre fra USART1)
 USART3.CTRLB |= USART_TXEN_bm;	//Enabler Transmit Enable biten i USART Controll B registeret.
 USART3.CTRLB |= USART_RXEN_bm;	//enabler Return Enable biten.
/* This delay invalidates the initial noise on the TX line, after device reset. */
_delay_ms(10);
}

void USART3_sendChar(char c)	//funksjon for å sende en Karakter
{
	while (!(USART3.STATUS & USART_DREIF_bm))	//While som sjekker om det sendes noe på USART3en
	{
		;	//når noe sendes gjør den ingenting
	}
	USART3.TXDATAL = c;	//når ting ikke sendes karakteren c på Usart TX pinnen.
}

static int USART3_printChar(char c, FILE *stream)
{
	USART3_sendChar(c);
	return 0;
}
//----------------------Usart functions end--------------------------



int main(void)
{
	// en "1" i registeret betyr innput
	adc_init();
    USART3_init();
	PORTB.DIRSET |= PIN3_bm;    //output for LED
    PORTB.DIRSET &= ~PIN2_bm;    //input for button
    
	uint8_t buttonState = 0;	//variabel for knappetilstanden, kunne kanskje vært en binær variabel og.
	
	PORTB.PIN2CTRL = PORT_PULLUPEN_bm;
	//Setter opp den integrerte pullup motstanden til knappen
    
    
    
    
    stdout = &USART_stream;
	while (1) 
    {	
        adcVal = adc_read();
        printf("\n%s %u", "ADC value: ", adcVal);   //"new line type(string) type(unsigned integer)", "ADC value: ", adc value
        
        
		buttonState = (PORTB.IN & PIN2_bm);
        
		//kjører en bitvis AND med PIN2 bitmasken som er 0 for alle verdier utenom bit nummer 2 (inngangen fra knappen) Da får vi kun med inngangen vi vil ha.
		//denne masken ANDes med inngangsregisterer.
		PORTB.OUT = (buttonState << 1);
    }
}


