/* 
 * File:   main.c
 * Author: Henrik
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

#define USART3_BAUD_RATE(BAUD_RATE)((float)(F_CPU * 64 / (16 *(float)BAUD_RATE)) + 0.5)	//lager baudraten til UART overføring 
static FILE USART_stream = FDEV_SETUP_STREAM(USART3_printChar, NULL, _FDEV_SETUP_WRITE); 



//--------------------------Temperature Reading functions---------------------------
// Initialize ADC
void adc_init(void) {
    //we will use PD7 from analog input for the thermistor
    
    VREF.ADC0REF = VREF_REFSEL_VDD_gc;  // Select reference voltage (Vref) to be AVCC step 1
    
    ADC0.CTRLA |= ADC_RESSEL_12BIT_gc;   //Resolution selection bit 0 mask.  Step 3
    ADC0.CTRLC = ADC_PRESC_DIV2_gc; // Adjust prescaler as needed for your clock frequency  step 7 
    ADC0.MUXPOS = ADC_MUXPOS_AIN7_gc;  // Select ADC input channel, here we use ADC7 which corresponds to PD7 on the curiosity nano. step 8
    ADC0.CTRLA |= ADC_ENABLE_bm;     //enables ADC step 11
}

// Reads the analog voltage and returns it as uint
uint16_t adc_read() {
    // Start conversion
    ADC0.COMMAND = ADC_STCONV_bm;

    // Wait for conversion to complete by checking a interups flag that 
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm)) { ; };

    ADC0.INTFLAGS = ADC_RESRDY_bm;    
    // Return the ADC result
    return ADC0.RES;    //this clears the ADC0.INTFLAG also 
}


float find_temp(uint16_t adcVal){
	//kode Srevet av Henrik For ooving 3 oppgave 2b
	const float R0 = 10000.0;	//10k ohm resistor constant
	const float B = 3950.0;		//B verdien til thermistoren min. funnet i databladet dens.
	const float T0 = 298.15;			//romtemp i kelvin, deffinert ut ifra databblad til thermistoren
	const float K0 = 273.15; //0 grader kelvin
	float meas_volt;
	float meas_resistance;		//definerer målt motstand fra ADC
	const float vcc = 3.3;	//vcc spenning VTG (Voltage target) som er den internt genererte
	float temp;	//definerer utregnet temperatur
	meas_volt = adcVal*(vcc/4095);	//regner ut spenningen ved å dele på antall sample punkt i 12-bit ADC
	meas_resistance = R0/((vcc/meas_volt)-1);	//regner ut motstanden 
	
	temp = 1/((1/T0)+(1/B)*log(meas_resistance/R0))-K0;	//utregning av temperatur etter Steinhart-Hart formelen. 
	return temp;	
}

// uint16_t internal_voltage_calculation(uint16_t){}

//-----------------------End temperature reading functions-----------------------------


//---------------------USART functions------------------------
//These functions are the same we used in the UART ooving in the course
void USART3_init(void)	//Setup function
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
        _delay_ms(1000);
        adcVal = adc_read();
        float temp_celcius = find_temp(adcVal);
        printf("\n %s %u", "ADC value: ", adcVal);   //"new line type(string) type(unsigned integer)", "ADC value: ", adc value
        char tempStr[10];
        dtostrf(temp_celcius, 6, 2, tempStr); // Converts the value to string with 2 decimal places
        printf("\n %s", "Temperature in degrees C: ", tempStr);  //
        printf("\n %s %f", "Temperature in degrees C: ", temp_celcius); //"new line type(string) type(float)", temperature
        
		buttonState = (PORTB.IN & PIN2_bm);
        
		//kjører en bitvis AND med PIN2 bitmasken som er 0 for alle verdier utenom bit nummer 2 (inngangen fra knappen) Da får vi kun med inngangen vi vil ha.
		//denne masken ANDes med inngangsregisterer.
		PORTB.OUT = (buttonState << 1);
    }
}


