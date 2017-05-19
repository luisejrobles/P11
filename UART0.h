#include <inttypes.h>
#include <avr/interrupt.h>

#define BUFFER_SIZE 64
#define BUFFER_EMPTY(buffer) (buffer.in_idx == buffer.out_idx)
#define BUFFER_FULL(buffer) (buffer.in_idx == MOD(buffer.out_idx - 1))
#define MOD(n) n&(BUFFER_SIZE-1)

char UART0_getchar( void );
unsigned int atoi(char *str);
uint8_t UART0_available( void );
void UART0_AutoBaudRate(void);
void BUFFER_INIT(void);
void itoa(char *str, uint16_t number, uint8_t base);
void UART0_gets(char *str);
void UART0_Ini(uint16_t mode);
void UART0_putchar(char data);
void UART0_puts(char *str);

typedef struct
{
	char buffer[BUFFER_SIZE];				/*	espacio reservado			*/
	volatile unsigned char in_idx :6;		/*	indice entrada (Head)		*/
	volatile unsigned char out_idx :6;		/*	indice entrada (tail)		*/
}ring_buffer_t;
