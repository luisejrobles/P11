#include <avr/interrupt.h>
#include <avr/io.h>
#include "UART0.h"
#include "Timer.h"
#define PHOTORESISTOR PORTA0

/*Macro que retorna el valor normalizado de 0 a 100, tomando en cuenta la relacion de los
valores maximos y minimos.*/
static volatile uint16_t max;
static volatile uint16_t min;
static volatile uint16_t offsetADC;

uint8_t ADC_Normalize(uint8_t valor);
void ADC_Ini( void );
void ADC_MinMax( uint8_t channel );
void Timer2_Set_Volume( uint8_t volume );
uint8_t ADC_Read( uint8_t channel );

const struct note ImperialMarch[]={
	//for the sheet music see:
	//http://www.musicnotes.com/sheetmusic/mtd.asp?ppn=MN0016254
	//this is just a translation of said sheet music to frequencies / time in ms
	//used TEMPO ms for a quart note
	
	{a, TEMPO},
	{a, TEMPO},
	{a, TEMPO},
	{f, TEMPO*3/4},
	{cH, TEMPO*1/4},

	{a, TEMPO},
	{f, TEMPO*3/4},
	{cH, TEMPO*1/4},
	{a, TEMPO*2},
	//first bit

	{eH, TEMPO},
	{eH, TEMPO},
	{eH, TEMPO},
	{fH, TEMPO*3/4},
	{cH, TEMPO*1/4},

	{gS, TEMPO},
	{f, TEMPO*3/4},
	{cH, TEMPO*1/4},
	{a, TEMPO*2},
	//second bit...

	{aH, TEMPO},
	{a, TEMPO*3/4},
	{a, TEMPO*1/4},
	{aH, TEMPO},
	{gSH, TEMPO/2},
	{gH, TEMPO/2},

	{fSH, TEMPO*1/4},
	{fH, TEMPO*1/4},
	{fSH, TEMPO/2},
	{0,TEMPO/2},
	{aS, TEMPO/2},
	{dSH, TEMPO},
	{dH, TEMPO/2},
	{cSH, TEMPO/2},
	//start of the interesting bit

	{cH, TEMPO*1/4},
	{b, TEMPO*1/4},
	{cH, TEMPO/2},
	{0,TEMPO/2},
	{f, TEMPO*1/4},
	{gS, TEMPO},
	{f, TEMPO*3/4},
	{a, TEMPO*1/4},

	{cH, TEMPO},
	{a, TEMPO*3/4},
	{cH, TEMPO*1/4},
	{eH, TEMPO*2},
	//more interesting stuff (this doesn't quite get it right somehow)

	{aH, TEMPO},
	{a, TEMPO*3/4},
	{a, TEMPO*1/4},
	{aH, TEMPO},
	{gSH, TEMPO/2},
	{gH, TEMPO/2},

	{fSH, TEMPO*1/4},
	{fH, TEMPO*1/4},
	{fSH, TEMPO/2},
	{0,TEMPO/2},
	{aS, TEMPO/2},
	{dSH, TEMPO},
	{dH, TEMPO/2},
	{cSH, TEMPO/2},
	//repeat... repeat

	{cH, TEMPO*1/4},
	{b, TEMPO*1/4},
	{cH, TEMPO/2},
	{0,TEMPO/2},
	{f, TEMPO/2},
	{gS, TEMPO},
	{f, TEMPO*3/4},
	{cH, TEMPO*1/4},

	{a, TEMPO},
	{f, TEMPO*3/4},
	{c, TEMPO*1/4},
	{a, TEMPO*2}
	//and we're done
};
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
	Timer0_Ini();
	ADC_Ini();
	ADC_MinMax(PHOTORESISTOR);
	while(1){
		if (UART0_available()){
			if(UART0_getchar() == 'p'){
				Timer2_Play(ImperialMarch,sizeof(ImperialMarch)/sizeof(struct note));
			}
		}
		Timer2_Set_Volume(ADC_Normalize(ADC_Read(PHOTORESISTOR)));
	}
	return 0;	
}

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
