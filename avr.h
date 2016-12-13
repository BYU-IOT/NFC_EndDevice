#include <stdint.h>

#define CPU_FREQ 7372800    //  7.3728 MHz


#define SetPin(Port, Bit) (Port |= (1 << Bit))
#define ClearPin(Port, Bit) (Port &= ~(1 << Bit))
#define ReadPin(Pin, Bit) ((Pin >> Bit) & 0x1)

#define MASTER PB0
#define SCLK PB1
#define MOSI PB2
#define MISO PB3
#define IRQ PB4
#define SS PB6

#define DEVICE PF3

#define SCL PD0
#define SDA PD1
#define RX PD2
#define TX PD3

#define IRQ_NFC PE7

#define XBEE_DTR PC0

#define XBEE_RST PB5

void AVR_init_mcu(void);