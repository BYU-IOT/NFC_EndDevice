#include <stdint.h>
//----------------Bits to enable-----------------
#define Bit0 0x01
#define Bit1 0x02
#define Bit2 0x04
#define Bit3 0x08
#define Bit4 0x10
#define Bit5 0x20
#define Bit6 0x40
#define Bit7 0x80


void NFC_init();
void NFC_write(uint16_t, uint16_t);
void NFC_continuousWrite(uint16_t, unsigned char*, uint16_t);
uint8_t NFC_read(uint16_t);
void NFC_reset();