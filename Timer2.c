#include <avr/interrupt.h>
#include <inttypes.h>
#include "Clock.h"
#include "UART.h"

static uint8_t Flag;
static uint8_t base;

void Timer2_Init (uint8_t baseT)
{
	/*
	1. Disable the Timer/Counter2 interrupts by clearing OCIE2x and TOIE2.
	2. Select clock source by setting AS2 as appropriate.
	3. Write new values to TCNT2, OCR2x, and TCCR2x.
	4. To switch to asynchronous operation: Wait for TCN2UB, OCR2xUB, and TCR2xUB.
	5. Clear the Timer/Counter2 Interrupt Flags.
	6. Enable interrupts, if needed.
	*/
	
	TIMSK2 = 0;							//Clear a interrupciones timer2
	ASSR = (1<<AS2);					//Clock externo seleccionado
	OCR2A = 256-1;						//TOP
	TCNT2 = 0;							//Contador inicializado en 0
	TCCR2A = (1<<WGM21);					//CTC mode				
	TCCR2B = (5<<CS20);					// PS 128
	while( (ASSR&((1<<TCN2UB)|(1<<OCR2AUB)|(1<<OCR2BUB)|(1<<TCR2AUB)|(1<<TCR2BUB))) );
	TIFR2 = (7<<TOV2);					//Se borran en alto las banderas de interrupcion
	TIMSK2 = (1<<OCIE2A);				//Interrupt compare A enable
	sei();								//habilita interrupciones (global) */
	if((baseT>=1) && (baseT<=8))
	{
		Flag = baseT;						//segundos a los cuales llegar	
	}else
	{
		UART0_puts("\n\rSeg no permitidos, 1seg seleccionado default\n\r");
		Flag = 1;
	}
	base = 0;
}
uint8_t Timer2_Flag ( void )
{
	if( Flag == base ){
		base = 0;
		return 1;
	}
	else{
	
		return 0;
	}
}
ISR (TIMER2_COMPA_vect)
{ 
	base++;
	if(Timer2_Flag())
	{
		Clock_Update(Flag);
		Clock_Display();
	}
}