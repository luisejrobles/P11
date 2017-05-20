#include <inttypes.h>
#include <avr/io.h>

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
