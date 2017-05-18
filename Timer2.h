#include <avr/interrupt.h>
#include <inttypes.h>
#include "Clock.h"

ISR (TIMER2_COMPA_vect);
uint8_t Timer2_Flag(void);
void Timer2_Init (uint8_t baseT);
