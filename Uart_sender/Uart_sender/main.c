/*
 * Uart_sender.c
 *
 * Created: 07.11.2018 09:23:04
 * Author : armin
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "sender.h"




int main(void)
{
	CLKPR = 0x80;						//Clock prescaler 16MHz
	CLKPR = 0x00;

	MCUCR = MCUCR | (1<<JTD);			//JTD Schnittstelle ausschalten für PWM6 Mode
	MCUCR = MCUCR | (1<<JTD);
	
	DDRD = DDRD | (1<<DDD3);
	//PORTD = PORTD | (1<<PORTD3);
	
	
	DDRB = DDRB | (1<<DDB0);
	PORTB = PORTB | (1<<PORTB0);

	DDRB = DDRB | (1<<DDB1);
	PORTB = PORTB | (1<<PORTB1);
	
	init_usart();				//Initialisierung von Kommunikationsschnittstelle UART
	init_transmission_timer();	//Initaliesierung von Timer0 für UART
	
	sei();
	
    while (1) 
    {
    }
}

