#include "uart.h"
#include "util.h"
#include "avr.h"
#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

//This function is used to initialize the USART
//at a given UBRR value
void UARTInit(uint16_t baud)
{
  xbee_buffer_size = 512;
  int i=0;
  while (i < xbee_buffer_size)
      UART_buffer[i++] = 0x00;

  xbee_buffer_volume = 0;
  xbee_buffer_head = 0;
  xbee_buffer_tail = 0;

  /* Set baud rate */
  UBRR1H = (unsigned char)(baud>>8);
  UBRR1L = (unsigned char)baud;

  // enable single speed UART
  UCSR1A = 0x00;
  // Enable receiver (RX) and transmitter (TX) and receive interrupt
  UCSR1B = 0x98; //0xF8
   // Enable no parity, 8 bit words, 1 stop bit
  UCSR1C = 0x06;
}

uint8_t read_buffer()
{
  xbee_buffer_tail %= xbee_buffer_size; //if tail = size, tail = 0
  xbee_buffer_volume--;
  return UART_buffer[xbee_buffer_tail++];
}

char UART_ReadChar()
{
  //Wait until a data byte is available
  while(!(UCSR1A & (1<<RXC1)));

  //Now USART has got data from host
  //and is available is buffer

  return UDR1;
}

void UART_WriteChar(char data)
{
  //Wait until the transmitter is ready
  while(!( UCSR1A & (1<<UDRE1)));

  UDR1 = data; //Now write the data to USART buffer
}

void UART_WriteCharDelay(char data)
{
   //Wait until the transmitter is ready

   while(!( UCSR1A & (1<<UDRE1)));

    UDR1 = data; //Now write the data to USART buffer
    delay(15);
}