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
ARDUINO_LIBS = arduino_core.a core/wiring_digital.c.o core/wiring_pulse.c.o core/HardwareSerial.cpp.o core/HardwareSerial0.cpp.o core/Print.cpp.o core/abi.cpp.o src/arduino/SPI.cpp src/arduino/rfid/MFRC522.cpp src/arduino/Ultrasonic.cpp src/arduino/SoftwareSerial.cpp
####


####RULES####
all: bench

bin:
	mkdir -p bin

bench: bin
	${CC} ${CFLAGS} src/benchmark/client.c src/crypto/**/*.c -o bin/client -DDEBUG
	${CC} ${CFLAGS} src/benchmark/server.c src/crypto/**/*.c -o bin/server -DDEBUG

avr_bin: 
	${CCAVR} ${CCAVR_FLAGS} ${INCLUDE_AVR} ${CFLAGS} ${ARDUINO_LIBS} src/benchmark/atmega328.cpp src/crypto/**/*.c -o bin/atmega
	${AVR_OBJCPY} -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 bin/atmega bin/atmega.eep
	${AVR_OBJCPY} -O ihex -R .eeprom bin/atmega bin/atmega.hex

avr_flash: avr_bin
	${AVRDUDE} -C${AVRDUDE_CONF} -v -patmega328p -carduino -P/dev/cu.usbmodem14101 -b115200 -D -Uflash:w:./bin/atmega.hex:i 

.PHONY: clean
clean:
	rm -rf bin