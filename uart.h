#include <stdint.h>

int xbee_buffer_volume; //definined in initialize() in functions.c
int xbee_buffer_tail;
int xbee_buffer_head;
int xbee_buffer_size; //size set in UART_init()
uint8_t UART_buffer[512];

void UARTInit(uint16_t);
char UART_ReadChar();
void UART_WriteChar(char);
void UART_WriteCharDelay(char);
uint8_t read_buffer();