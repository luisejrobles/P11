#include <avr/interrupt.h>
#include <avr/io.h>
#include <inttypes.h>
#include "UART0.h"
#include "Timer.h"

static volatile uint16_t max;
static volatile uint16_t min;
static volatile uint16_t offsetADC;

uint8_t ADC_Normalize(uint8_t valor)
{
	if (valor > max)
	{
		valor = max;
	}else if(valor < min)
	{
		valor = min;
	}
	return 100-(((valor-min)*100)/(max-min));
}
uint8_t ADC_Read(uint8_t channel)
{
	uint8_t data;
	DIDR0 = 0;
	ADMUX  = (1<<REFS0)|(channel<<MUX0)|(1<<ADLAR);
	ADCSRA = (1<<ADEN)|(1<<ADSC)|(7<<ADPS0);			//ADC start conversion, 128 PS
	while(ADCSRA&(1<<ADSC));				//Loop hasta que termine la conversion
	data  = ADCH;							//Leyendo dato
	return data - offsetADC;
}
void ADC_Ini(void)
{
	char offsetChar[10];
	/*  inicializa para 8 bits de resolución y habilita el ADC del microcontrolador de
	forma generica. Encontrar el desplazamiento (offset) de la medición y almacenarla.*/
	//OFFSET------------------------------
	ADMUX  = (1<<REFS0)|(31<<MUX0)|(1<<ADLAR);				//AVCC with external capacitor at AREF, y MUX a tierra p. offset
	ADCSRA = (1<<ADEN)|(1<<ADSC)|(7<<ADPS0);		//ADC enable, ADC interrupt enable, 128 PS
	ADCSRB &= (~(1<<MUX5));
	while(ADCSRA & 1<<ADSC);			//esperando el primer ciclo de conversion
	offsetADC = ADCH;					//tomando offset
	itoa(offsetChar,offsetADC,10);
	UART0_puts("\n\rOffset capturado:");
	UART0_puts(offsetChar);
}
void ADC_MinMax( uint8_t channel )
{
	char minChar[10], maxChar[10];

	UART0_getchar();
	UART0_getchar();
	UART0_getchar();
	UART0_puts("\n\n\rPresione para capturar el minimo");
	UART0_getchar();
	UART0_puts("\n\rMinimo: ");
	min = ADC_Read(channel);
	//min += 100;
	itoa(minChar,min,10);
	UART0_puts(minChar);
	
	UART0_puts("\n\n\rPresione para capturar el maximo.");
	UART0_getchar();
	max = ADC_Read(channel);
	//max -= 100;
	UART0_puts("\n\rMaximo: ");
	itoa(maxChar,max,10);
	UART0_puts(maxChar);
	UART0_getchar();
}
void Timer2_Set_Volume(uint8_t volume)
{
	/*Ajusta el ancho de pulso que es producido sobre la terminal OC2B. El rango del valor de
	entrada sera de 0 a 100.*/
	Timer2_Volume(volume);	
}