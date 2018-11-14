
#define F_CPU  16000000UL   //Takt
#define BAUD  9600UL		//gewünschte Baudrate
#define UBRR_CALC (F_CPU/16UL/BAUD-1)  //Baudrtate aus Takt berechnen


char i=0;									//laufvariable für empfangdaten[i]
volatile unsigned char sende_daten[3];			//dynamischer Speicher der Akkudaten
volatile uint16_t voltage=0b0011001100001111;	//Akku Spannung	0-3650mV
volatile uint8_t temperatur=0b01010101;			//Temperatur	0-120C
char overflow_counter=0;	//Zählt Overflows für Pause

void daten_senden(void);

void init_usart (void)
{
	UBRR1H = (unsigned char)(UBRR_CALC>>8); //Baudrate einstellen
	UBRR1L = (unsigned char)(UBRR_CALC);
	UCSR1B = UCSR1B | (1<<TXEN1);	//Sender ein
	UCSR1B = UCSR1B | (1<<RXEN1);	//empfang ein
	UCSR1B = UCSR1B | (1<<TXCIE1);	//Sende-Interruput ein
	//UCSR1B = UCSR1B | (1<<UDRIE1);	//Buffer-Interrupt ein
	//UCSR1A = UCSR1A | (1<<UDRE1);
	
	UCSR1C = UCSR1C &~ (1<<UMSEL10);	//asynchr.
	UCSR1C = UCSR1C &~ (1<<UMSEL11);
	
	UCSR1C = UCSR1C &~ (1<<UPM10);		//even parity
	UCSR1C = UCSR1C | (1<<UPM11);
	
	UCSR1C = UCSR1C &~ (1<<USBS1);		//1-Stop-Bit
	
	UCSR1C = UCSR1C | (1<<UCSZ10);		//8-Bit data
	UCSR1C = UCSR1C | (1<<UCSZ11);
	UCSR1B = UCSR1B &~ (1<<UCSZ12);
	
	//UCSR1C = UCSR1C &~ (UCPOL1);		//muss low sein im asynchron mode
	
	
}
void init_transmission_timer(void)
{
	TCCR0A = TCCR0A &~ (1<<COM0A0);		//Normal Port Operations
	TCCR0A = TCCR0A &~ (1<<COM0A1);
	
	TCCR0A = TCCR0A &~ (1<<WGM00);		//Normal Mode
	TCCR0A = TCCR0A &~ (1<<WGM01);
	TCCR0B = TCCR0B &~ (1<<WGM02);
	
	TCCR0B = TCCR0B &~ (1<<CS00);		//Teiler 256 (16MHz / 256 = 16µs)
	TCCR0B = TCCR0B &~ (1<<CS01);
	TCCR0B = TCCR0B | (1<<CS02);
	
	OCR0A = 100;	//Compare bei 1,6ms	(16µs * 100 = 1,6ms)
	
	TIMSK0 = TIMSK0 | (1<<OCIE0A);		//Interrupt nach 1,6ms
	//TIMSK0 = TIMSK0 | (1<<TOIE0);		//Overflow Interrupt nach 4ms
	
}
void daten_senden(void)
{
	overflow_counter=0;
	
	sende_daten[0] = temperatur;
	sende_daten[1] = sende_daten[1] | voltage;			//LOW-Byte
	sende_daten[2] = sende_daten[2] | (voltage>>8);		//HIGH-Byte
	
	//Funktioniert nicht mit For schleife ???

	while( !(UCSR1A & (1<<UDRE1)) );	//warten transmission buffer leer
	UDR1 = sende_daten[0];					//Zeichen in Variable ablegen	//UDR1 -> 8 Bit daten 9.Bit wäre in UCSR1B
	while( !(UCSR1A & (1<<UDRE1)) );	//warten transmission buffer leer
	UDR1 = sende_daten[1];					//Zeichen in Variable ablegen	//UDR1 -> 8 Bit daten 9.Bit wäre in UCSR1B
	while( !(UCSR1A & (1<<UDRE1)) );	//warten transmission buffer leer
	UDR1 = sende_daten[2];					//Zeichen in Variable ablegen	//UDR1 -> 8 Bit daten 9.Bit wäre in UCSR1B
	
	
	
	
	PORTB = PORTB ^ (1<<PORTB0);
	
		
} 
ISR(USART1_TX_vect)     //Interrupt für sender //alle  11,75ms -> 8ms pause + 3*1,25ms (Data)
{	
	TCNT0 = 0;					//nötig um nicht in de overflow zu geraten
	overflow_counter = 0;		//Counter wird auf 0 gesetzt
}

ISR(TIMER0_COMPA_vect)
{
	PORTB = PORTB ^ (1<<PORTB1);
	
	TCNT0 = 0;
	overflow_counter++;
	
	if(overflow_counter >= 5)
	{
		daten_senden();
	}
	
}
ISR(USART1_UDRE_vect)
{
	
}