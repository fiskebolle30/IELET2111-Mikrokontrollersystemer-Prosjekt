/* 
 * File:   main.c
 * Author: steinar
 *
 * Created on 12 March 2024, 14:06
 */

/*
 * henrik test branch
 */



#define F_CPU  4000000UL
#include <util/delay.h>
#include <avr/io.h>


int main(void)
{
	// en "1" i registeret betyr innput
	
	PORTB.DIRSET = 0b00001000; // PB3 as output , other PB pins as inputs . Tatt fra oppg1 setter pin PB3 som utgang. blir utgang da bit nummer 3 er in
			//Denne linjen setter alle port B pinnenent til definerte inn/utganger. og siden vi vil at B2 som er knappen skal være inngang er det greit da den allerede er deffinert som det
			
	uint8_t buttonState = 0;	//variabel for knappetilstanden, kunne kanskje vært en binær variabel og.
	
	PORTB.PIN2CTRL = PORT_PULLUPEN_bm;
	//Setter opp den integrerte pullup motstanden
	
	while (1) 
    {	
		buttonState = (PORTB.IN & PIN2_bm);
		//kjører en bitvis AND med PIN2 bitmasken som er 0 for alle verdier utenom bit nummer 2 (inngangen fra knappen) Da får vi kun med inngangen vi vil ha.
		//denne masken ANDes med inngangsregisterer.
		
		//PORTB.OUT = ((buttonState << 3) |= 0b11110111);
		//PORTB.OUT |= (buttonState << 1) | 0b11110111;
		PORTB.OUT = (buttonState << 1);
		// Fikk hjelp av en i klassen til at det måtte være den nederste av de tre alternativene over. Jeg tenkte først at den øverste er den eneste som ga mening. 
		//ser ærlig ikke hvorfor den ikke er riktig. Får spørre neste time.
		
		/*
		if (buttonState)	// sjekker om Buttonstate er 1
		{
			PORTB.OUT = 0b11110111;	//skrur på led
			//PORTB.OUT = ~PIN3_bm;
		} 
		
		else
		{
			PORTB.OUT = 0b11111111;	//skrur av led
		}
		*/
	
		//looper så til toppen av while(1) loopen.
    }
}


