#include "Fan_read_thermistor_and_voltage.h"


//--------------------------Temperature Reading functions---------------------------

void adc_init(void) {
    //we will use PD7 from analog input for the thermistor
    //as default set to measure internal voltage
    
    VREF.ADC0REF = VREF_REFSEL_2V048_gc;  // Select reference voltage (Vref) to be Analog VCC. Has to be done to initialize
    
    ADC0.CTRLA |= ADC_RESSEL_12BIT_gc;   //Resolution selection. we chose 12 bit meaning 0-4095 
    ADC0.CTRLC = ADC_PRESC_DIV2_gc; // Adjust prescaler as needed for the clock frequency 
    ADC0.MUXPOS = ADC_MUXPOS_VDDDIV10_gc;  // Select ADC input channel, here we use VDDDIV10 as standard which is the internal VDD divided by 10
    ADC0.CTRLA |= ADC_ENABLE_bm;     //enables ADC step 11
}

// Reads the analog voltage and returns it as uint
uint16_t adc_thermistor_read(void) {
    
    //select reference voltage
    VREF.ADC0REF = VREF_REFSEL_VDD_gc;  // Select reference voltage (Vref) to be VDD.
    
    //select multiplexer port
    ADC0.MUXPOS = ADC_MUXPOS_AIN7_gc;  // Select ADC input channel, here we use AIN7 which corresponds to PD7 on the curiosity nano.
   
    _delay_ms(1);   //needed delay to wait for change in VREF
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
	float meas_resistance;		//definerer m�lt motstand fra ADC
	const float vcc = 3.3;	//vcc spenning VTG (Voltage target) som er den internt genererte
	float temp;	//definerer utregnet temperatur
	meas_volt = adcVal*(vcc/4095);	//regner ut spenningen ved � dele p� antall sample punkt i 12-bit ADC
	meas_resistance = R0/((vcc/meas_volt)-1);	//regner ut motstanden 
	
	temp = 1/((1/T0)+(1/B)*log(meas_resistance/R0))-K0;	//utregning av temperatur etter Steinhart-Hart formelen. 
	return temp;	
}
//-----------------------End temperature reading functions-----------------------------



//-----------------------Voltage reading functions----------------------
uint16_t adc_internal_read(void){
    //change voltage reference
    VREF.ADC0REF = VREF_REFSEL_2V048_gc;  // Select reference voltage (Vref) to be a set internal voltage reference of 2.048 volts
    
    //change multiplexer selector
    ADC0.MUXPOS = ADC_MUXPOS_VDDDIV10_gc;  // Select ADC input channel, here VDDDiV10 which means the internal VDD divided by 10.

    _delay_us(10);   //needed delay to wait for change in VREF as specified in chapter 39.5.3 of the datasheet
    // Start conversion
    ADC0.COMMAND = ADC_STCONV_bm;

    // Wait for conversion to complete by checking a interupts flag
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm)) { ; };

    ADC0.INTFLAGS = ADC_RESRDY_bm;    
    // Return the ADC result
    
    return ADC0.RES;    //this clears the ADC0.INTFLAG also 
}

uint16_t adc_external_read(void){
    //change voltage reference
     VREF.ADC0REF = VREF_REFSEL_2V048_gc;  // Select reference voltage (Vref) to be a set internal voltage reference of 2.048 volts
    
    //change multiplexer selector
    ADC0.MUXPOS = ADC_MUXPOS_AIN4_gc; // Select ADC input channel equal to PD 4. 

    _delay_us(10);   //needed delay to wait for change in VREF as specified in chapter 39.5.3 of the datasheet
    // Start conversion
    ADC0.COMMAND = ADC_STCONV_bm;

    // Wait for conversion to complete by checking a interupts flag
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm)) { ; };

    ADC0.INTFLAGS = ADC_RESRDY_bm;    
    // Return the ADC result
    
    return ADC0.RES;    //this clears the ADC0.INTFLAG also 
}
  
float voltage_calculation(uint16_t adcVal){
    uint16_t samples = 4095;    //max value of 12-bit ADC
    float vMeas = 10*Voltage_ref_sel*adcVal/samples; //Divides the measured adc Value by the max number it can be which is the reference voltage.
    //Multiplied by 10 to account for 10 to one ratio
    return vMeas;
}   
// ----------------------End voltage reading functions------------------------------



//---------------------USART functions------------------------
//These functions are the same we used in the UART ooving in the course
void USART3_init(void)	//Setup function
{
 PORTB.DIR &= ~PIN1_bm;	//endret til port B fra C. Setter innput
 PORTB.DIR |= PIN0_bm;	//endret til port B fra C. Setter output 

 USART3.BAUD = (uint16_t)USART3_BAUD_RATE(9600);	//Setter en baudrate p� 9600 for utgang USART3 (m�tte endre fra USART1)
 USART3.CTRLB |= USART_TXEN_bm;	//Enabler Transmit Enable biten i USART Controll B registeret.
 USART3.CTRLB |= USART_RXEN_bm;	//enabler Return Enable biten.
/* This delay invalidates the initial noise on the TX line, after device reset. */
_delay_ms(10);
}

void USART3_sendChar(char c)	//funksjon for � sende en Karakter
{
	while (!(USART3.STATUS & USART_DREIF_bm))	//While som sjekker om det sendes noe p� USART3en
	{
		;	//n�r noe sendes gj�r den ingenting
	}
	USART3.TXDATAL = c;	//n�r ting ikke sendes karakteren c p� Usart TX pinnen.
}

static int USART3_printChar(char c, FILE *stream)
{
	USART3_sendChar(c);
	return 0;
}
//----------------------USART functions end--------------------------



void Draw_to_terminal(uint16_t adcValue, char Str[]){
    
    printf("\n %s %s %u", Str, " voltage ADC value: ", adcValue); 
        float intVDD = voltage_calculation(adcValue);
        char intStr[10];
        dtostrf(intVDD, 6, 3, intStr); // Converts the value to string with 3 decimal places
        printf("\n %s %s %s", Str, "External voltage: ", intStr);
}

