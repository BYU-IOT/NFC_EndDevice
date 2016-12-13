#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include "avr.h"
#include "util.h"
#include "spi.h"
#include "nfc.h"
#include "xbee.h"
#include "uart.h"
#include "i2c.h"

//===============NFC NDEF BASE=================
uint8_t NDEF_BASE[33] = {
	//------------------------NDEF Application---------------------------
	0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01,
	0xE1, 0x03, //Capability Container ID
	//-----------------------Capability Container-----------------------
	0x00, 0x0F, //CCLen 15 bytes
	0x20, //Mapping version 2.0
	0x00, 0x3B, //MLe (49 bytes); Maximum R-APDU data size
	0x00, 0x34, //MLc (52 bytes); Maximum C-APDU data size
	0x04, //Tag, File Control TLV (4 = NDEF file)
	0x06, //Length File Control TLV (6 bytes of data for this tag)
	0xE1, 0x04, //Type 4 Tag File Identifier
	0x0B, 0xB8, //Max NDEF size (3k bytes of RF430CL330H useable memory)
	0x00, //NDEF file read access condition; read access without any security
	0x00, //NDEF file write access condition; write access without any security
	//--------------------------NDEF File-------------------------------
	0xE1, 0x04, //NDEF File ID
	0x00, 0x07, //NDEF Length 7 bytes


	//-------------------------NDEF start-------------------------------
	0xD1, //NDEF Header MB=1, ME=1, CF=0, SR=1, IL=0, TNF=001
	0x01, //Type Length 1 byte
	0x01, //Payload length 3 bytes
	0x54, //Type T (text)........0x55 = Type U (URI/URL)
	//------------Payload start-----------------------
	0x00, //Plain text.........0x01 = URI Record Type : http://www.
	//33 bytes total right now
};


//******************************NFC FUCTIONS*********************************

void NFC_init()
{
    // NFC_write(0xFFFA, (Bit1 | Bit2)); //enable End of Read and End of Write INTO signals
    // NFC_write(0xFFFE, (Bit1 | Bit3 | Bit5 | Bit6)); //enable NFC INTO Signal
    
    NFC_write(0xFFFA, (Bit1 | Bit2)); //enable EOR and EOW interrupts, BIT5 is NDEF error
    delay(15);
    
    NFC_write(0xFFFE, (Bit1 | Bit2 | Bit3 | Bit4)); //configure IRQ pin for active high, enable RF
    delay(15);

    NFC_write(0xFFFA, 0x06); //enable End of Read and End of Write INTO signals
    NFC_write(0xFFFE, 0x6A); //enable NFC INTO Signal
}

//***************************************************************************

void NFC_write(uint16_t regAddress, uint16_t data)
{
    uint8_t TxAddr[2];
    uint8_t TxData[2];
    TxAddr[0] = regAddress >> 8;
    TxAddr[1] = regAddress & 0x00FF;
	TxData[0] = data >> 8;
	TxData[1] = data & 0x00FF;
    // get everyone off the SPI bus
    SPI_deselect(SPI_ALL);
    // get the debug engine on the bus
    ClearPin(PORTB, SS);
    delay(15);

    SPI_send(0x02); //write command
    delay(2);
    SPI_send(TxAddr[0]);
    delay(2);
    SPI_send(TxAddr[1]);
    delay(2);
    SPI_send(TxData[1]);
    delay(2);
    SPI_send(TxData[0]);
    delay(2);

    SetPin(PORTB, SS);
    SPI_select(spi_selected_device);
}

void NFC_continuousWrite(uint16_t regAddress, unsigned char* data_buffer, uint16_t data_length)
{ 
    uint8_t TxAddr[2];
    TxAddr[0] = regAddress >> 8;
    TxAddr[1] = regAddress & 0x00FF;
    // get everyone off the SPI bus
    SPI_deselect(SPI_ALL);
    // get the debug engine on the bus
    ClearPin(PORTB, SS);
    delay(15);

    SPI_send(0x02); //write command
    delay(2);
    SPI_send(TxAddr[0]);
    delay(2);
    SPI_send(TxAddr[1]);
    delay(2);
    
    int index = 0;
    while (index < data_length)
    {
        SPI_send(data_buffer[index]);
        delay(2);
        index++;
    }

    SetPin(PORTB, SS);
    SPI_select(spi_selected_device);
}

uint8_t NFC_read(uint16_t regAddress)
{
    uint8_t TxAddr[2];
    TxAddr[0] = regAddress >> 8;
    TxAddr[1] = regAddress & 0x00FF;
    
    uint8_t mybyte;

    // get everyone off the SPI bus
    SPI_deselect(SPI_ALL);
    // get the debug engine on the bus
    ClearPin(PORTB, SS);
    delay(5);
    
    SPI_send(0x03); //read Command
    delay(2);
    SPI_send(TxAddr[0]);
    delay(2);
    SPI_send(TxAddr[1]);
    delay(2);
    SPI_send(0x00); //dummy byte
    
    mybyte = SPI_receive(0x00);
    
    delay(5);
    
    SetPin(PORTB, SS);
    SPI_select(spi_selected_device);
    
    return(mybyte);
}

//*************************************************************************

void NFC_reset()
{
    NFC_write(0xFFF8, (Bit1 | Bit2)); //ACKnowledge registers and resets them
    delay(20);
    NFC_write(0xFFFA, (Bit1 | Bit2)); //enable EOR and EOW interrupts
    delay(20);
    NFC_write(0xFFFE, (Bit1 | Bit2 | Bit3 | Bit4)); //configure IRQ pin for active high, enable RF
    //t_ready = 20ms
    NFC_write(0xFFFE, Bit0);
    ClearPin(PORTB, XBEE_RST);  //reset xbee
    delay(100);
    SetPin(PORTB, XBEE_RST);  //turn off xbee reset
    delay(400);
    NFC_init();
}
