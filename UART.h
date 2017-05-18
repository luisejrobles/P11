#include <inttypes.h>

char UART0_getchar(void);
unsigned int atoi(char *str);
void itoa(char *str, uint16_t number, uint8_t base);
void UART0_Init(uint16_t mode);
void UART0_gets(char *str);
void UART0_putchar(char data);
void UART0_puts(char *str);
void UART0_AutoBaudRate(void);
