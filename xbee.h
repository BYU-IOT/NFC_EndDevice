#include "avr.h"
#include "spi.h"
#include "util.h"

void xbee_init();
void xbee_configure();
void XBEE_command(char*, int);
void XBEE_read_setting(char*);
char convertChar(unsigned int);