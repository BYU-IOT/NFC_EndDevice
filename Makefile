# Name: Makefile
# Author: J. Kelly Flanagan
# Copyright: Copyright (c) 2013
# License: no unauthorized use is permitted

DEVICE     = at90usb1286
CLOCK      = 7322807
PROGRAMMER = -c avrispmkII -P usb -B 1
OBJECTS    = main.o avr.o util.o spi.o nfc.o xbee.o uart.o i2c.o
FUSES      = -U lfuse:w:0xec:m -U hfuse:w:0xd1:m -U efuse:w:0xff:m 

# AT90USB1286 fuse bits used above
# Example for 8 MHz external crystal oscillator found on STK525
# Fuse high byte:
# 0xd9 = 1 1 0 1   0 0 0 1 <-- BOOTRST (boot reset vector at 0x0000)
#        ^ ^ ^ ^   ^ ^ ^------ BOOTSZ0
#        | | | |   | +-------- BOOTSZ1
#        | | | |   +---------- EESAVE (set 0 to preserve EEPROM over chip erase)
#        | | | +-------------- WDTON (set to 0 to turn watchdog on)
#        | | +---------------- SPIEN (set to 1 to disbale serial programming)
#        | +------------------ JTAGEN (set to 1 to disable JTAG programming)
#        +-------------------- OCDEN (if set to 0, on chip debug enabled)
# Fuse low byte:
# 0xff = 1 1 1 1   1 1 1 1
#        ^ ^ \ /   \--+--/
#        | |  |       +------- CKSEL 3..0 (8M internal RC)
#        | |  +--------------- SUT 1..0 (slowly rising power)
#        | +------------------ CKOUT
#        +-------------------- CKDIV8 (set to 1 to divide clock by 8)
#
# For computing fuse byte values for other devices and options see
# the fuse bit calculator at http://www.engbedded.com/fusecalc/

# Tune the lines below only if you know what you are doing:

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

# symbolic targets:
all:	main.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

readeeprom:
	$(AVRDUDE) -U eeprom:r:eeprom.hex:i

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID main.hex

clean:
	rm -f main.hex main.elf $(OBJECTS)

# file targets:
main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=$(DEVICE) main.elf
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	main.elf
	avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c
