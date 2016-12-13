#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "avr.h"
#include "util.h"
#include "spi.h"
#include "nfc.h"
#include "xbee.h"
#include "uart.h"
#include "i2c.h"

uint8_t status = 0;
uint8_t NFC_MSG[30] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

ISR(USART1_RX_vect) //UART (XBEE)
{
    cli();
    xbee_buffer_head %= xbee_buffer_size; //if head = size, head = 0
    UART_buffer[xbee_buffer_head++] = UDR1;
    xbee_buffer_volume++;
    sei();
}

ISR(INT7_vect)
{
    cli();
    processNFC();
}

//*************************************************************************

int main(void)
{
    delay(20);
    AVR_init_mcu(); // initialize and setup AT90USB1286

    ClearPin(PORTB, XBEE_RST);  //reset xbee
    delay(100);
    SetPin(PORTB, XBEE_RST);  //turn off xbee reset

    SPI_init(128); //start SPI bus and set CLK at 7372800/128 = 57.6 KHz
    UARTInit(47); //3 ubrr-> 115200,  47 ubrr -> baud rate = 9600 when freq = 7.3728 Mhz
    delay(200);

    NFC_init();
    xbee_init();
    sei();
    Println("setup done");
    uint8_t current = 0;
    while (1)
    { 
        if (xbee_buffer_volume > 0)
        {
            current = read_buffer();
            if (current == 1)
            {
                SetPin(PORTF, DEVICE);
                status = 1;
            }
            else
            {
                ClearPin(PORTF, DEVICE);
                status = 0;
            }
        }
        XBEE_transmit(status);
        delay(2000);
    }
    return 0; //never reaches
}

//*************************************************************************

void printReg()
{
    int i;
    unsigned int base_address = 33; //sizeof(NDEF_BASE);
    for (i=0; i < 30; i++)
    {
        PrintHex(NFC_read(i+base_address));
        Print(",");
    }
    Println("");
}

//*************************************************************************

void processNFC()
{
    Println("processNFC");
    NFC_write(0xFFFE, (Bit2 | Bit4)); //disable RF
    unsigned int base_address = 33; //sizeof(NDEF_BASE);
    uint8_t i;
    for (i=0; i < 30; i++)
        NFC_MSG[i] = NFC_read(base_address++);

    xbee_configure(NFC_MSG); //Setup XBEE with NDEF configurations
    Println("Finished XBEE configuration");
    sei();
}

