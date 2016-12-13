#include "xbee.h"
#include "avr.h"
#include "spi.h"
#include "util.h"
#include "uart.h"
#include "i2c.h"
#include <avr/io.h>
#include <stdint.h>


void xbee_init()
{
    //----------------CONFIGURATION MODE COMMAND--------------------
    delay(1000); //must wait 1s before and after
    UART_WriteChar('+');
    UART_WriteChar('+');
    UART_WriteCharDelay('+');
    //---------------JOIN NOTIFICATION-----------------------------------
    char JN[3] = { 'J', 'N', '1' };
    XBEE_command(JN, 3);
    //---------------JOIN NOTIFICATION-----------------------------------
    char TT[3] = { 'I', 'D', '0' };
    XBEE_command(TT, 3);
    //----------------------NAME----------------------------
    char NAME[4] = { 'N', 'I', 'L', 'S'};
    XBEE_command(NAME, 4);
    //---------------SC-----------------------------------
    char SC[6] = { 'S', 'C', 'F', 'F', 'F', 'F' };
    XBEE_command(SC, 6);
    //---------------RO-----------------------------------
    char RO[3] = { 'R', 'O', 'A' };
    XBEE_command(RO, 3);
    //---------------WRITE TO NON VOLATILE-----------------------------------
    char WR[2] = {'W', 'R'};
    XBEE_command(WR, 2);
    //---------------EXIT COMMAND MODE-----------------------------------
    char EXIT[2] = { 'C', 'N' };
    XBEE_command(EXIT, 2);
    delay(1000); //must wait 1s before and after
}

//*************************************************************************

void xbee_configure(uint8_t* NFC_MSG)
{
    Println("Configuring XBEE now");

    char PAN_ID[18];
    PAN_ID[0] = ('I');
    PAN_ID[1] = ('D');
    int i;
    for (i=0; i < 16; i++)
        PAN_ID[i+2] = NFC_MSG[i];


    //=====ENCRYPTION=====
    int pos=0;
    int num;
    int top_nibble;
    int btm_nibble;
    int16_t merge[15] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    for (i=0; i < 30; i+=2)
    {
        top_nibble = NFC_MSG[i] - 48;
        if (top_nibble > 0xA)
            top_nibble -= 7;

        btm_nibble = NFC_MSG[i+1] - 48;
        if (btm_nibble > 0xA)
            btm_nibble -= 7;

        merge[pos++] = (top_nibble << 4) | btm_nibble;
    }

    int16_t new_key[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    pos=0;
    for (i=0; i < 15; i++)
    {
        if (merge[i] < 15)
            new_key[pos++] = merge[merge[i]];
    }

    for (i=pos; i < 16; i++)
    {
        if (new_key[i] == -1)
            new_key[i] = 255 - (merge[pos-(pos/2)] + merge[2] + merge[pos-(pos/3)])/3;
        pos++;
    }

    char KEY[34];
    KEY[0] = ('K');
    KEY[1] = ('Y');
    pos=0;
    for (i=0; i < 16; i++)
    {
        top_nibble = (new_key[i] & 0xF0) >> 4;
        if (top_nibble < 0xA)
            top_nibble += 48;
        else
            top_nibble += 55;

        KEY[2+pos++] = top_nibble;

        btm_nibble = new_key[i] & 0x0F;
        if (btm_nibble < 0xA)
            btm_nibble += 48;
        else
            btm_nibble += 55;

        KEY[2+pos++] = btm_nibble;
    }
    //----------------CONFIGURATION MODE COMMAND--------------------
    delay(250); //must wait 1s before and after
    UART_WriteChar('+');
    UART_WriteChar('+');
    UART_WriteCharDelay('+');
    //---------------JOIN NOTIFICATION-----------------------------------
    char JN[3] = { 'J', 'N', '1' };
    XBEE_command(JN, 3);
    //----------------------NAME----------------------------
    char NAME[3] = { 'M', 'S', 'N' };
    XBEE_command(NAME, 3);
    //---------------WRITE PAN ID-----------------------------------
    XBEE_command(PAN_ID, 18);
    //---------------ENCRYPTION ENABLE-----------------------------------
    char EE[3] = { 'E', 'E', '1' };
    XBEE_command(EE, 3);
    //---------------ENCRYPTION OPTIONS-----------------------------------
    char EO[3] = { 'E', 'O', '2' }; //use trust center
    XBEE_command(EO, 3);
    //---------------WRITE ENCRYPTION KEY-----------------------------------
    XBEE_command(KEY, 34);
    //---------------Sampling rate-----------------------------------
    char IR[5] = { 'I', 'R', 'B', 'B', '8' }; //Make sampling rate 3000 ms
    XBEE_command(IR, 5);
    //---------------WRITE TO NON VOLATILE-----------------------------------
    char WR[2] = {'W', 'R'};
    XBEE_command(WR, 2);
    //---------------EXIT COMMAND MODE-----------------------------------
    char EXIT[2] = { 'C', 'N' };
    XBEE_command(EXIT, 2);
}

//*************************************************************************
void XBEE_transmit(uint8_t sample)
{
    UART_WriteChar((char)sample); //send value to write to register
    UART_WriteChar(0x0A); //New Line
}

//*************************************************************************


void XBEE_command(char* command, int length)
{
    UART_WriteChar('A');
    UART_WriteChar('T');
    int i = 0;
    while (i < length)
       UART_WriteChar((char)command[i++]); //send value to write to register
    UART_WriteChar(0x0D); //Carriage return
    delay(15);
}

