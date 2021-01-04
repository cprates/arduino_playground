PORT=/dev/tty.usbmodem141401
TARGET=main
SRCS=main.c
INCLUDE_DIRS=-I. -I/usr/share/arduino/hardware/arduino/avr/cores/arduino

MCU=atmega328p
F_CPU=16000000
CFLAGS=-std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU}

all: clean build flash

build:
	avr-gcc ${CFLAGS} ${INCLUDE_DIRS} -o ${TARGET}.bin ${SRCS}
	avr-objcopy -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex
	avr-size -C ${TARGET}.bin

read:
	avrdude -c arduino -P ${PORT} -p ${MCU} -b 115200 -U flash:r:sdint.hex:r

flash:
	avrdude -p ${MCU} -c arduino -U flash:w:${TARGET}.hex:i -F -P ${PORT}

clean:
	rm -f *.bin *.hex *.d
