#include "ADC.h"
#include "UART0.h"
#include <avr/io.h>
#include <inttypes.h>
#define ADC_Normalize( value ) (value*100)/max		//regla de 3 para encontrar el valor

uint8_t ADC_Read(uint8_t channel)
{
	uint8_t data;
	uint8_t msb;
	
	ADMUX  |= (channel<<MUX0);
	ADCSRA |= (1<<ADSC);		//ADC enable, ADC start conversion
	data = ADCL;	//tomando dato
	msb  = ADCH;	//leyendo ADCH para que ADC reciba nueva conversion
	return data;
}
void ADC_Ini(void)
{
	/*  inicializa para 8 bits de resolución y habilita el ADC del microcontrolador de
	forma generica. Encontrar el desplazamiento (offset) de la medición y almacenarla.*/
	ADMUX  = (1<<REFS0);			//AVCC with external capacitor at AREF
	ADCSRA = (1<<ADEN)|(7<<ADPS0);	//ADC enable, ADC interrupt enable, 128 PS
	ADC_Offset(0);
}
void ADC_MinMax( uint8_t channel )
{	unsigned char cadena[10];
	UART0_puts("\n\rPresione para capturar el minimo.");
	UART0_getchar();
	min = ADC_Read(channel);
	itoa(cadena,min,10);
	UART0_puts("\n\rMinimo capturado:");
	UART0_puts(cadena);
	
	UART0_puts("\n\n\rPresione para capturar el maximo.");
	UART0_getchar();
	max = ADC_Read(channel);
	itoa(cadena,max,10);
	UART0_puts("\n\rMaximo capturado:");
	UART0_puts(max);
}
void ADC_Offset(uint8_t channel)
{
	uint8_t msb;	//Desbloquear siguiente conversion
	offsetADC = ADC_Read(channel);
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
