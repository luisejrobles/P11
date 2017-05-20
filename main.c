#include "UART0.h"
#include "Timer.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#define PHOTORESISTOR PORTA0
#define ADC_Normalize( value ) ((value-min)*100)/(max-min)

/*Macro que retorna el valor normalizado de 0 a 100, tomando en cuenta la relacion de los
valores maximos y minimos.*/
static volatile uint16_t max;
static volatile uint16_t min;
static volatile uint16_t offsetADC;

void ADC_Ini( void );
void ADC_MinMax( uint8_t channel );
void Timer2_Set_Volume( uint8_t volume );
uint8_t ADC_Read( uint8_t channel );

const struct note AngelesAzules[]={
	{bL, TEMPO_500*3/8},
	{d, TEMPO_500*3/8},
	{fS, TEMPO_500*3/8},
	{b, TEMPO_500*5/4},

	{a, TEMPO_500*3/8},
	{gS, TEMPO_500*3/4},
	{fS, TEMPO_500*3/8},
	{d, TEMPO_500*3/4},
	{bL, TEMPO_500*3/2},
	//first bit

	{bL, TEMPO_500*3/8},
	{d, TEMPO_500*3/8},
	{fS, TEMPO_500*3/8},
	{b, TEMPO_500*5/4},

	{a, TEMPO_500*3/8},
	{gS, TEMPO_500*3/4},
	{fS, TEMPO_500*3/8},
	{d, TEMPO_500*3/4},
	{bL, TEMPO_500*3/2},
	//first bit

	{d, TEMPO_500*3/8},
	{d, TEMPO_500*3/8},
	{d, TEMPO_500*3/8},
	{cS, TEMPO_500*3/8},
	{d, TEMPO_500*5/8},
	{aL, TEMPO_500*5/4},
	
	{d, TEMPO_500*3/8},
	{d, TEMPO_500*3/8},
	{cS, TEMPO_500*3/8},
	{d, TEMPO_500*5/8},
	{aL, TEMPO_500*5/8},

//****************

	{fS, TEMPO_500*3/8},
	{fS, TEMPO_500*3/8},
	{fS, TEMPO_500*3/8},
	{e, TEMPO_500*3/8},
	{fS, TEMPO_500*5/8},
	{cS, TEMPO_500*5/4},

	{fS, TEMPO_500*3/8},
	{fS, TEMPO_500*3/8},
	{e, TEMPO_500*3/8},
	{fS, TEMPO_500*5/8},
	{cS, TEMPO_500*5/8},

//*****************

	{g, TEMPO_500*1/2},
	{g, TEMPO_500*1/2},
	{g, TEMPO_500*1/2},
	{g, TEMPO_500*1/2},
	{e, TEMPO_500*1/2},
	{d, TEMPO_500*1/2},
	{cS, TEMPO_500*3},

	{g, TEMPO_500*1/2},
	{g, TEMPO_500*1/2},
	{g, TEMPO_500*1/2},
	{g, TEMPO_500*1/2},
	{e, TEMPO_500*1/2},
	{d, TEMPO_500*1/2},
	{cS, TEMPO_500*3},

//Se repite todo*****

	{d, TEMPO_500*3/8},
	{d, TEMPO_500*3/8},
	{d, TEMPO_500*3/8},
	{cS, TEMPO_500*3/8},
	{d, TEMPO_500*5/8},
	{aL, TEMPO_500*5/4},
	
	{d, TEMPO_500*3/8},
	{d, TEMPO_500*3/8},
	{cS, TEMPO_500*3/8},
	{d, TEMPO_500*5/8},
	{aL, TEMPO_500*5/8},

//****************

	{fS, TEMPO_500*3/8},
	{fS, TEMPO_500*3/8},
	{fS, TEMPO_500*3/8},
	{e, TEMPO_500*3/8},
	{fS, TEMPO_500*5/8},
	{cS, TEMPO_500*5/4},

	{fS, TEMPO_500*3/8},
	{fS, TEMPO_500*3/8},
	{e, TEMPO_500*3/8},
	{fS, TEMPO_500*5/8},
	{cS, TEMPO_500*5/8},

//*****************

	{g, TEMPO_500*1/2},
	{g, TEMPO_500*1/2},
	{g, TEMPO_500*1/2},
	{g, TEMPO_500*1/2},
	{e, TEMPO_500*1/2},
	{d, TEMPO_500*1/2},
	{cS, TEMPO_500*3},

	{g, TEMPO_500*1/2},
	{g, TEMPO_500*1/2},
	{g, TEMPO_500*1/2},
	{g, TEMPO_500*1/2},
	{e, TEMPO_500*1/2},
	{d, TEMPO_500*1/2},
	{cS, TEMPO_500*3}
	//and we're done
};

int main(void)
{
	UART0_Ini(0);
	UART0_AutoBaudRate();
	//UART0_puts("TEST");
	Timer0_Ini();
	ADC_Ini();
	ADC_MinMax(PHOTORESISTOR);
	while(1){
		if (UART0_available()){
			if(UART0_getchar() == 'p'){
				Timer2_Play(AngelesAzules,sizeof(AngelesAzules)/sizeof(struct note));
			}
		}
		Timer2_Set_Volume(ADC_Normalize(ADC_Read(PHOTORESISTOR)));
	}
	return 0;	
}

uint8_t ADC_Read(uint8_t channel)
{
	uint8_t data;
	uint8_t msb;
	DDRA &= ~(1<<channel);				//Pin puerto A (ADC0) salida enable
	ADMUX  = (1<<REFS0)|(channel<<MUX0);
	ADCSRA = (1<<ADEN)|(1<<ADSC)|(7<<ADPS0);			//ADC start conversion, 128 PS
	while(ADCSRA&(1<<ADSC));				//Loop hasta que termine la conversion
	data = ADCL;							//Tomando dato
	msb  = ADCH;							//Leyendo ADCH para que ADC reciba nueva conversion
	return data - offsetADC;
}
void ADC_Ini(void)
{
	char offsetChar[10];
	uint8_t msb;
	/*  inicializa para 8 bits de resolución y habilita el ADC del microcontrolador de
	forma generica. Encontrar el desplazamiento (offset) de la medición y almacenarla.*/
	//OFFSET------------------------------
	DIDR0 = (1<<ADC0D);							//Ahorrar energía
	ADMUX  = (1<<REFS0)|(31<<MUX0);				//AVCC with external capacitor at AREF, y MUX a tierra p. offset
	ADCSRA = (1<<ADEN)|(1<<ADSC)|(7<<ADPS0);		//ADC enable, ADC interrupt enable, 128 PS
	ADCSRB &= (~(1<<MUX5));
	while(ADCSRA & 1<<ADSC);			//esperando el primer ciclo de conversion
	offsetADC = ADCL;					//tomando offset
	msb = ADCH;							//Tomando dato
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
	itoa(minChar,min,10);
	UART0_puts(minChar);
	
	UART0_puts("\n\n\rPresione para capturar el maximo.");
	UART0_getchar();
	max = ADC_Read(channel);
	UART0_puts("\n\rMaximo: ");
	itoa(maxChar,max,10);
	UART0_puts(maxChar);
}
void Timer2_Set_Volume(uint8_t volume)
{
	/*Ajusta el ancho de pulso que es producido sobre la terminal OC2B. El rango del valor de
	entrada sera de 0 a 100.*/
	Timer2_Volume(volume);
}
