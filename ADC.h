#include <inttypes.h>

/*Macro que retorna el valor normalizado de 0 a 100, tomando en cuenta la relacion de los
valores maximos y minimos.*/
#define ADC_Normalize( value ) (value*100)/(max-min)

static volatile uint16_t max;
static volatile uint16_t min;
static volatile uint16_t offsetADC;

void ADC_Ini( void );
void ADC_MinMax( uint8_t channel );
void ADC_Offset( );
void Timer2_Set_Volume( uint8_t volume );
uint8_t ADC_Read( uint8_t channel );