#include <stdint.h>

#define SPI_ALL 0
#define SPI_XBEE 1
#define SPI_DEBUG 2
#define NFC 4

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C
#define SPI_MODE_MASK 0x0C

extern uint8_t spi_selected_device;

uint8_t SPI_init(int);

void SPI_deselect(uint8_t);
void SPI_select(uint8_t);
uint8_t SPI_adjustSpeed(int);
uint8_t SPI_send(uint8_t);
uint8_t SPI_receive(uint8_t);
char SPI_isData(void);
char SPI_needATTN(void);