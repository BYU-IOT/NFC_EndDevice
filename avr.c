#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include "avr.h"
#include "util.h"
#include "spi.h"

//*************************************************************************

void AVR_init_mcu()
{
    DDRB = 0;
    SetPin(PORTB, SS);
    
    DDRB = (1<<MASTER) | (1<<SS) | (1<<SCLK) | (1<<MOSI) | (0<<MISO) | (1<<XBEE_RST);
    
    SetPin(PORTB, MASTER);
    
    DDRD = (0<<RX) | (1<<TX) | (1<<SDA) | (1<<SCL);

    DDRE = (0<<IRQ_NFC);
    
    DDRC = (1<<XBEE_DTR);

    DDRF = (1<<DEVICE);
    ClearPin(PORTF, DEVICE);
    
    SPI_deselect(SPI_ALL);

    EICRB = 0xC0;
    EIMSK = 0x80;
    
    cli();
}

//*************************************************************************
