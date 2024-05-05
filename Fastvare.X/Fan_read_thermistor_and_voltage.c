
#include "Fan_read_thermistor_and_voltage.h"
#include "Fan_monitor_TWI.h"


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
   
    _delay_us(10);   //needed delay to wait for change in VREF
    // Start conversion
    ADC0.COMMAND = ADC_STCONV_bm;

    // Wait for conversion to complete by checking a interups flag that 
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm)) { ; };

    ADC0.INTFLAGS = ADC_RESRDY_bm;    
    // Return the ADC result
    
    return ADC0.RES;    //this clears the ADC0.INTFLAG also 
}

float find_temp(uint16_t adcVal){
	//Function that calculated Thermistor temperature using Steinhart-Hart equation and 
	const float R0 = 10000.0;	//10k ohm resistor used in voltage divider and thermistor R0 value
	const float B = 3950.0;		//Thermistor B value from datasheet
	const float T0 = 298.15;	//25 deg C in kelvin. The value used to define thermistor resistance in datasheet
	const float K0 = 273.15;    //0 degrees kelvin
	float meas_volt;
	float meas_resistance;		
	const float vcc = 3.3;

	meas_volt = adcVal*(vcc/4095);	//calculates voltage by dividing by amount of samples with 12-bit ADC
    
	meas_resistance = R0/((vcc/meas_volt)-1);	//calculates resistance 
	
	float temp = 1/((1/T0)+(1/B)*log(meas_resistance/R0))-K0;	//calculated temperature in degrees C using  Steinhart-Hart equation. 
	return temp;	
}
//-----------------------End temperature reading functions-----------------------------





//-----------------------Voltage reading functions----------------------
uint16_t adc_internal_read(void){
    //change voltage reference
    VREF.ADC0REF = VREF_REFSEL_2V048_gc;  // Select reference voltage (Vref) to be a set internal voltage reference of 2.048 volts
    
    //change multiplexer selector
    ADC0.MUXPOS = ADC_MUXPOS_VDDDIV10_gc;  // Select ADC input channel, here VDDDIV10 which means the internal VDD divided by 10.

    _delay_us(10);   //needed delay to wait for change in VREF as specified in chapter 39.5.3 of the datasheet
    
    // Start conversion
    ADC0.COMMAND = ADC_STCONV_bm;

    // Wait for conversion to complete by checking a interupt flag
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm)) { ; };
    
    // Return the ADC result
    ADC0.INTFLAGS = ADC_RESRDY_bm;    
    
    return ADC0.RES;    //this also clears the ADC0.INTFLAG
}

uint16_t adc_external_read(void){
    //change voltage reference
     VREF.ADC0REF = VREF_REFSEL_2V048_gc;  // Select reference voltage (Vref) to be a set internal voltage reference of 2.048 volts
    
    //change multiplexer selector
    ADC0.MUXPOS = ADC_MUXPOS_AIN0_gc; // Select ADC input channel equal to PD 0. 

    _delay_us(10);   //needed delay to wait for change in VREF as specified in chapter 39.5.3 of the AVR128DB48 datasheet
    
    // Start conversion
    ADC0.COMMAND = ADC_STCONV_bm;

    // Wait for conversion to complete by checking a interupts flag
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm)) { ; };

    ADC0.INTFLAGS = ADC_RESRDY_bm;    
    // Return the ADC result
    
    return ADC0.RES;    //this also clears the ADC0.INTFLAG
}
  
float voltage_calculation(uint16_t adcVal){
    uint16_t samples = 4095;    //max value of 12-bit ADC
    float vMeas = 10*Voltage_ref_sel*adcVal/samples; //Divides the measured adc Value by the max number it can be which is the reference voltage.
    //Multiplied by 10 to account for 10 to one ratio
    return vMeas;
}   
// ----------------------End voltage reading functions------------------------------



//---------------------USART functions------------------------
//These functions are the same we used in the USART ooving in the course
void USART3_init(void)	//Setup function
{
 PORTB.DIR &= ~PIN1_bm;	//Sets input USART pin
 PORTB.DIR |= PIN0_bm;	//Sets output USART pin

 USART3.BAUD = (uint16_t)USART3_BAUD_RATE(9600);	//Sets 9600 baudrate
 USART3.CTRLB |= USART_TXEN_bm;	 //Enables Transmit bit in USART control B register
 USART3.CTRLB |= USART_RXEN_bm;	 //Enables Return Enable bit in USART control B register.
 
_delay_ms(10);  //delay for mitigating initial noise on the TX line after device reset
}

void USART3_sendChar(char c)	//USART function for sending a single character
{
	while (!(USART3.STATUS & USART_DREIF_bm))	//While loop checks if something is being sent over the USART line
	{
		;	//If something is being sent, do nothing
	}
	USART3.TXDATAL = c;	//if available, sends character
}
int USART3_printChar(char c, FILE *stream)  //function for parsing the sendChar function 
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
        printf("\n %s %s %s", Str, "Calculated voltage: ", intStr);
}



uint16_t temp_timeout = 0;  //counter that is used to check how long the thermistor has been over the set max point.
void check_temperature_error(uint16_t adcThermistorVal)  //Function to check if the temperature is above the set max point.
{
    uint16_t temperature_threshold = (Fan_reg[TEMP_ALARM_LEVEL_H] << 8) | Fan_reg[TEMP_ALARM_LEVEL_L];
    if(adcThermistorVal > temperature_threshold){ //increments the counter by one if above the threshold
        ++temp_timeout;
        if(temp_timeout > 10)   //has to be above threshold for a few loops to protect against erroneous measurements. 
        {
            Fan_reg[ERROR_BYTE] |= (1 << ERR_TEMP_bp);  //sets the error bit in the error byte register.
            //The error bit can be reset over the I2C line.
        }
    }
    else
    {
        temp_timeout = 0;   //resets the counter if erroneous measurement.
    }       
}
