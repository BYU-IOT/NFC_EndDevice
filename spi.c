#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include "spi.h"
#include "util.h"
#include "avr.h"

uint8_t spi_selected_device;

// init SPI bus, spifreq is in KHz
uint8_t SPI_init(int spifreq)
{  
  // Enable SPI, Master, set clock rate
  return(SPI_adjustSpeed(spifreq));
}

//******************************SPI FUCTIONS*********************************


// deselect all SPI devices and then select chosen device
void SPI_select(uint8_t device)
{
  SPI_deselect(SPI_ALL);
  switch(device) {
  case NFC:
    ClearPin(PORTB, SS);
    spi_selected_device = NFC;  // add a similar line to indicate last selected device
    break;
  }
}


void SPI_deselect(uint8_t device)
{
  switch(device) {
  case SPI_ALL:
    // add a line here for each device on SPI bus
    SetPin(PORTB, SS);
    break;
  case NFC:
    SetPin(PORTB, SS);
    break;
  }
}

// this routine sets the SPI bus clock freq
// CPU_FREQ is defined in avr.h 
// spifreq is in KHz
uint8_t SPI_adjustSpeed(int clkdiv)
{
  switch(clkdiv) {
    case 2:
      SPCR = (1<<SPE) | (1<<MSTR) | (0 << SPR1) | (0 << SPR0);
      SPSR = 1;
      break;
    case 4:
      SPCR = (1<<SPE) | (1<<MSTR) | (0 << SPR1) | (0 << SPR0);
      SPSR = 0;
      break;
    case 8:
      SPCR = (1<<SPE) | (1<<MSTR) | (0 << SPR1) | (1 << SPR0);
      SPSR = 1;
      break;
    case 16:
      SPCR = (1<<SPE) | (1<<MSTR) | (0 << SPR1) | (1 << SPR0);
      SPSR = 0;
      break;
    case 32:
      SPCR = (1<<SPE) | (1<<MSTR) | (1 << SPR1) | (0 << SPR0);
      SPSR = 1;
      break;
    case 64:
      SPCR = (1<<SPE) | (1<<MSTR) | (1 << SPR1) | (0 << SPR0);
      SPSR = 0;
      break;
    case 128:
      SPCR = (1<<SPE) | (1<<MSTR) | (1 << SPR1) | (1 << SPR0);
      SPSR = 0;
      break;
    default:
      // default to clk / 4
      SPCR = (1<<SPE) | (1<<MSTR) | (0 << SPR1) | (0 << SPR0);
      SPSR = 0;
      return(false);
      break;
  }  
  return(true);
}


// send a byte
unsigned char SPI_send(unsigned char data)
{  
  /* Start transmission */
  SPDR = data;
  /* Wait for transmission complete */ 
  while(!(SPSR & (1<<SPIF))) {
  }
  data = SPDR;
  return(data);
}


// receive a byte
unsigned char SPI_receive(unsigned char d)
{
  unsigned char data;

  SPDR = d;
  while(!(SPSR & (1<<SPIF)));
  data = SPDR;
  return(data);
}

char SPI_isData()
{
  if(SPSR & (1<<SPIF))
    return(true);
  else
    return(false);
}