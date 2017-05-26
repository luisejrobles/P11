#include <inttypes.h>

#ifndef ADC_H_
#define ADC_H_

uint8_t ADC_Normalize( uint8_t valor );
uint8_t ADC_Read( uint8_t channel );
void ADC_Ini( void );
void ADC_MinMax( uint8_t channel );
void Timer2_Set_Volume( uint8_t volume );



#endif /* ADC_H_ */