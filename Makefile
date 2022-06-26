####I386####
CC=gcc
CFLAGS=-I src/crypto
####AVR####
ARDUINO_BIN = /Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin
ARDUINO_AVR = /Applications/Arduino.app/Contents/Java/hardware/arduino/avr
AVRDUDE_CONF = /Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf

AVRDUDE = $(ARDUINO_BIN)/avrdude
AVR_OBJCPY = $(ARDUINO_BIN)/avr-objcopy
AVR_SIZE = $(ARDUINO_BIN)/avr-size

CCAVR_FLAGS = -g -Os -w -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fuse-linker-plugin -Wl,--gc-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -MMD -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10813 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR
INCLUDE_AVR = -I$(ARDUINO_AVR)/cores/arduino -I$(ARDUINO_AVR)/variants/standard -Isrc/arduino
CCAVR = $(ARDUINO_BIN)/avr-g++
ARDUINO_LIBS = arduino_core.a core/wiring_digital.c.o core/WString.cpp.o core/wiring_pulse.c.o core/HardwareSerial.cpp.o core/HardwareSerial0.cpp.o core/Stream.cpp.o core/Print.cpp.o core/abi.cpp.o src/arduino/SPI.cpp src/arduino/rfid/MFRC522.cpp src/arduino/Ultrasonic.cpp src/arduino/SoftwareSerial.cpp
####


####RULES####
all: bench

bin:
	mkdir -p bin

###### Py tools

sparkle:
	${CC} ${CFLAGS} src/benchmark/encrypt_sparkle.c src/crypto/peer/*.c src/crypto/sparkle/*.c -o bin/encrypt_sparkle
	${CC} ${CFLAGS} src/benchmark/decrypt_sparkle.c src/crypto/peer/*.c src/crypto/sparkle/*.c -o bin/decrypt_sparkle

xoodyak:
	${CC} ${CFLAGS} -DXOODYAK src/benchmark/encrypt_sparkle.c src/crypto/peer/*.c src/crypto/xoodyak/*.c -o bin/encrypt_xoodyak
	${CC} ${CFLAGS} -DXOODYAK src/benchmark/decrypt_sparkle.c src/crypto/peer/*.c src/crypto/xoodyak/*.c -o bin/decrypt_xoodyak

###### Tests

test_sparkle:
	${CC} ${CFLAGS} -DDEBUG src/benchmark/test.c src/crypto/peer/*.c src/crypto/sparkle/*.c -o bin/test_sparkle

test_xoodyak:
	${CC} ${CFLAGS} -DDEBUG -DXOODYAK src/benchmark/test.c src/crypto/peer/*.c src/crypto/xoodyak/*.c -o bin/test_xoodyak

bench: bin
	${CC} ${CFLAGS} src/benchmark/client.c src/crypto/peer/*.c src/crypto/sparkle/*.c -o bin/client -DDEBUG
	${CC} ${CFLAGS} src/benchmark/server.c src/crypto/peer/*.c src/crypto/sparkle/*.c -o bin/server -DDEBUG

###### Demo

avr_bin: 
	${CCAVR} ${CCAVR_FLAGS} ${INCLUDE_AVR} ${CFLAGS} ${ARDUINO_LIBS} -DDEBUGARDUINO src/benchmark/atmega328.cpp src/crypto/peer/*.c src/crypto/sparkle/*.c -o bin/atmega
	${AVR_OBJCPY} -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 bin/atmega bin/atmega.eep
	${AVR_OBJCPY} -O ihex -R .eeprom bin/atmega bin/atmega.hex

avr_flash: avr_bin
	${AVRDUDE} -C${AVRDUDE_CONF} -v -patmega328p -carduino -P/dev/cu.usbmodem14101 -b115200 -D -Uflash:w:./bin/atmega.hex:i 

avr_bin_xoodyak: 
	${CCAVR} ${CCAVR_FLAGS} ${INCLUDE_AVR} ${CFLAGS} ${ARDUINO_LIBS} -DXOODYAK -DDEBUGARDUINO src/benchmark/atmega328.cpp src/crypto/peer/*.c src/crypto/xoodyak/*.c -o bin/atmega_xoodyak
	${AVR_OBJCPY} -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 bin/atmega_xoodyak bin/atmega_xoodyak.eep
	${AVR_OBJCPY} -O ihex -R .eeprom bin/atmega_xoodyak bin/atmega_xoodyak.hex

avr_flash_xoodyak: avr_bin_xoodyak
	${AVRDUDE} -C${AVRDUDE_CONF} -v -patmega328p -carduino -P/dev/cu.usbmodem14101 -b115200 -D -Uflash:w:./bin/atmega_xoodyak.hex:i 

###### Measurements

avr_measure:
	${CCAVR} ${CCAVR_FLAGS} ${INCLUDE_AVR} ${CFLAGS} ${ARDUINO_LIBS} src/benchmark/atmega328_measurements.cpp src/crypto/peer/*.c src/crypto/sparkle/*.c -o bin/atmega_measure
	${AVR_OBJCPY} -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 bin/atmega_measure bin/atmega_measure.eep
	${AVR_OBJCPY} -O ihex -R .eeprom bin/atmega_measure bin/atmega_measure.hex

avr_flash_measure: avr_measure
	${AVRDUDE} -C${AVRDUDE_CONF} -v -patmega328p -carduino -P/dev/cu.usbmodem14101 -b115200 -D -Uflash:w:./bin/atmega_measure.hex:i 

###### Misc

.PHONY: clean
clean:
	rm -rf bin