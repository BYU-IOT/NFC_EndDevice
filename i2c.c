#include "i2c.h"
#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include "util.h"
#include "avr.h"

void transmit(char *DATA)
{
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); //send start condition
    delay(10);
    //while (!(TWCR & (1<<TWINT))); //wait for TWINT flag = start condition sent
    //if ((TWSR & 0xF8) != START) ERROR();
    
    TWDR = 0; //Load SLA_W into TWDR Register. Address
    TWCR = (1<<TWINT) | (1<<TWEN); //Clear TWINT bit in TWCR to start transmission of address
    delay(10);
    //while (!(TWCR & (1<<TWINT))); //Wait for TWINT Flag set. This indicates that the SLA+W has been transmitted
    //if ((TWSR & 0xF8) != MT_SLA_ACK)ERROR();
    
    int i=0;
    while (DATA[i] != 0)
    {
        TWDR = DATA[i++]; //Load DATA into TWDR Register.
        TWCR = (1<<TWINT) | (1<<TWEN); //Clear TWINT bit in TWCR to start transmission of data
        delay(10);
        //while (!(TWCR & (1<<TWINT))); //Wait for TWINT Flag set. This indicates that the DATA has been transmitted
        //if ((TWSR & 0xF8) != MT_DATA_ACK)ERROR();
    }
    
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO); //Transmit STOP condition
}

void Print(char *s)
{
    transmit(s);
}

void Println(char *s)
{
    delay(50);
    transmit(s);
    delay(50);
    transmit("\n");
    delay(50);
}

void PrintHex(uint8_t d)
{
  char str[3];

  sprintf(str, "%02x", d);
  str[2] = 0;
  Print(str);
}