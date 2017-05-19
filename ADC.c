#include "ADC.h"
#include "UART0.h"
#include <avr/io.h>
#include <inttypes.h>

uint8_t ADC_Read(uint8_t channel)
{
	uint8_t data;
	uint8_t msb;
	DDRA   &= ~(1<<channel);			//Pin puerto A (ADC0) salida enable
	ADMUX  |= (channel<<MUX0);
	ADCSRA |= (1<<ADSC);				//ADC start conversion
	data = ADCL;						//Tomando dato
	msb  = ADCH;						//Leyendo ADCH para que ADC reciba nueva conversion
	UART0_puts("\n\n\rADC read exit");
	return data;
}
void ADC_Ini(void)
{
	/*  inicializa para 8 bits de resolución y habilita el ADC del microcontrolador de
	forma generica. Encontrar el desplazamiento (offset) de la medición y almacenarla.*/
	ADMUX  = (1<<REFS0);				//AVCC with external capacitor at AREF
	ADCSRA = (1<<ADEN)|(7<<ADPS0);		//ADC enable, ADC interrupt enable, 128 PS
	ADC_Offset();
}
void ADC_MinMax( uint8_t channel )
{	//unsigned char cadena[10];
	UART0_puts("\n\n\rPresione para capturar el minimo");
	UART0_getchar();
	UART0_puts("\n\rAntes de adc read");
	min = ADC_Read(channel);
	
	//itoa(cadena,min,10);
	//UART0_puts("\n\rMinimo capturado:");
	//UART0_puts(cadena);
	
	UART0_puts("\n\n\rPresione para capturar el maximo.");
	UART0_getchar();
	max = ADC_Read(channel);
	//itoa(cadena,max,10);
	//UART0_puts("\n\rMaximo capturado:");
	//UART0_puts(max);
}
void ADC_Offset()
{
	unsigned char offsetChar[10];
	uint8_t msb;
	ADMUX  |= (31<<MUX0);				//MUX para tomar tierra
	ADCSRA |= (1<<ADSC);				//ADC start conversion
	while(ADCSRA & 1<<ADSC);			//esperando el primer ciclo de conversion
	offsetADC = ADCL;					//tomando offset
	msb = ADCH;						//Tomando dato
	itoa(offsetChar,offsetADC,10);
	UART0_puts("\n\rOffset capturado:");
	UART0_puts(offsetChar);
	
}
void Timer2_Set_Volume(uint8_t volume)
{
	/*Ajusta el ancho de pulso que es producido sobre la terminal OC2B. El rango del valor de
	entrada sera de 0 a 100.*/
	unsigned char volumenChar[20];
	
	OCR2B = (OCR2A*volume)/100;
	itoa(volumenChar,volume,10);
	UART0_puts("\n\rVolumen: ");
	UART0_puts(volumenChar);
}