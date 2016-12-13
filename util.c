#include <util/delay.h>
#include "util.h"

/*
  delay(int x) busy waits for x ms before returning.
  The delay is only approximate.

  input: integer input parameter designates the number 
  of ms to delay.

  output: none
*/
void delay(int x)
{
  int i;

  // the library routine _delay_ms() has a maximum
  // delay of just over 32 ms at 8MHz
  if(x<33) {
    for(i=0;i<x;i++)
      _delay_ms(1);
  }
  else {
    for(i=0;i<(x/32);i++) {
      _delay_ms(32);
    }
    for(i=0;i<(x%32);i++)
    _delay_ms(1);
  }
}
