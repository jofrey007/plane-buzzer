# Makefile pre plane-buzzer (ATtiny402)

MCU = attiny402
F_CPU = 3333333
PORT = /dev/ttyUSB0

# Arduino toolchain cesta
TOOLCHAIN = /home/attila/.arduino15/packages/DxCore/tools/avr-gcc/7.3.0-atmel3.6.1-azduino8a/bin

CC = $(TOOLCHAIN)/avr-gcc
OBJCOPY = $(TOOLCHAIN)/avr-objcopy
SIZE = $(TOOLCHAIN)/avr-size

CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=c99
TARGET = plane-buzzer

all: $(TARGET).hex size

$(TARGET).elf: $(TARGET).c
	$(CC) $(CFLAGS) -o $@ $<

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

size: $(TARGET).elf
	$(SIZE) --mcu=$(MCU) -C $<

flash: $(TARGET).hex
	pyupdi -d $(MCU) -c $(PORT) -f $<

clean:
	rm -f $(TARGET).elf $(TARGET).hex

.PHONY: all size flash clean
