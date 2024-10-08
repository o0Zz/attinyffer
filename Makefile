DEVICE     = attiny85           # it's what we're using
CLOCK      = 16500000           # 16.5Mhz, no external crystal needed
FUSES = -U lfuse:w:0xe1:m -U hfuse:w:0xdd:m -U efuse:w:0xff:m
PROGRAMMER = -c usbasp -P usb   # modify for different programmer
AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -I. -D $(MODULE)

_OBJECTS   := libs-device/osccal.o usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o uart/uart.o main.o 

all: gamepad

gamepad: OBJECTS = $(_OBJECTS) gamepad/gamepad.o
gamepad: MODULE = GAMEPAD
gamepad: gamepad/gamepad.o main.hex

flash:
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

install: flash fuse

upload:
	micronucleus --run main.hex

clean:
	rm -f main.hex main.elf $(_OBJECTS) gamepad/gamepad.o

# file targets:

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

main.elf: $(_OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=$(DEVICE) main.elf
