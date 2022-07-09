####I386####
CC=g++
CFLAGS=-I src/crypto -g -Os -w -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -Wl,--gc-sections -fdata-sections -v
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

###### Set Flags
set_debug:
	$(eval CFLAGS=${CFLAGS} -DDEBUG)

set_xoodyak:
	$(eval CFLAGS=${CFLAGS} -DXOODYAK)
	$(eval RULE_NAME=xoodyak)
	$(eval ADD_FILES=src/crypto/${RULE_NAME}/*.c)

set_xoodyak_asm: set_xoodyak
	$(eval CFLAGS=${CFLAGS} -DDOASM)
	$(eval RULE_NAME=xoodyak_asm)
	$(eval ADD_FILES=${ADD_FILES} src/crypto/xoodyak/internal-xoodoo-avr.S)

set_sparkle:
	$(eval CFLAGS=${CFLAGS} -DSPARKLE)
	$(eval RULE_NAME=sparkle)
	$(eval ADD_FILES=src/crypto/${RULE_NAME}/*.c)

set_gcm:
	$(eval CFLAGS=${CFLAGS} -DGCM)
	$(eval RULE_NAME=gcm)
	$(eval ADD_FILES=src/crypto/${RULE_NAME}/*.c src/crypto/${RULE_NAME}/*.cpp)

set_empty:
	$(eval CFLAGS=${CFLAGS} -DEMPTY)
	$(eval RULE_NAME=empty)
	$(eval ADD_FILES=src/crypto/${RULE_NAME}/*.c)

###### Py tools

base_tools:
	${CC} ${CFLAGS} src/benchmark/encrypt.c src/crypto/peer/*.c ${ADD_FILES} -o bin/encrypt_${RULE_NAME}
	${CC} ${CFLAGS} src/benchmark/decrypt.c src/crypto/peer/*.c ${ADD_FILES} -o bin/decrypt_${RULE_NAME}

tools_sparkle: set_sparkle base_tools
tools_xoodyak: set_xoodyak base_tools
tools_gcm: set_gcm base_tools


###### Benchmark Linux
base_bench:
	${CC} ${CFLAGS} src/benchmark/measurements.c src/crypto/peer/*.c ${ADD_FILES} -o bin/measurements_${RULE_NAME}

measure_sparkle: set_sparkle base_bench
measure_xoodyak: set_xoodyak base_bench
measure_gcm: set_gcm base_bench
measure_empty: set_empty base_bench

###### Tests

test_base: set_debug
	${CC} ${CFLAGS} src/benchmark/test.c src/crypto/peer/*.c ${ADD_FILES} -o bin/test_${RULE_NAME}

test_gcm: set_gcm test_base
test_xoodyak: set_xoodyak test_base
test_sparkle: set_sparkle test_base

bench: bin
	${CC} ${CFLAGS} src/benchmark/client.c src/crypto/peer/*.c src/crypto/sparkle/*.c -o bin/client -DDEBUG
	${CC} ${CFLAGS} src/benchmark/server.c src/crypto/peer/*.c src/crypto/sparkle/*.c -o bin/server -DDEBUG

###### Demo

set_avr_demo:
	$(eval ADD_FILES=${ADD_FILES} src/benchmark/atmega328.cpp)

set_avr_measure:
	$(eval RULE_NAME=measure_${RULE_NAME})
	$(eval ADD_FILES=${ADD_FILES} src/benchmark/atmega328_measurements.cpp)

base_avr_bin:
	${CCAVR} ${CCAVR_FLAGS} ${INCLUDE_AVR} ${CFLAGS} ${ARDUINO_LIBS} -DDEBUGARDUINO src/crypto/peer/*.c ${ADD_FILES} -o bin/atmega_${RULE_NAME}
	${AVR_OBJCPY} -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 bin/atmega_${RULE_NAME} bin/atmega_${RULE_NAME}.eep
	${AVR_OBJCPY} -O ihex -R .eeprom bin/atmega_${RULE_NAME} bin/atmega_${RULE_NAME}.hex

base_avr_flash:
	${AVRDUDE} -C${AVRDUDE_CONF} -v -patmega328p -carduino -P/dev/cu.usbmodem14101 -b115200 -D -Uflash:w:./bin/atmega_${RULE_NAME}.hex:i 

base_avr_measure: set_avr_measure base_avr_bin
base_avr_demo: set_avr_demo base_avr_bin

# Empty
avr_bin_measure_empty: set_empty base_avr_measure

# Sparkle
avr_bin_demo_sparkle: set_sparkle base_avr_demo
avr_flash_demo_sparkle: avr_bin_demo_sparkle base_avr_flash
avr_bin_measure_sparkle: set_sparkle base_avr_measure
avr_flash_measure_sparkle: avr_bin_measure_sparkle base_avr_flash

# Xoodyak
avr_bin_demo_xoodyak: set_xoodyak base_avr_demo
avr_flash_demo_xoodyak: avr_bin_demo_xoodyak base_avr_flash
avr_bin_measure_xoodyak: set_xoodyak base_avr_measure
avr_flash_measure_xoodyak: avr_bin_measure_xoodyak base_avr_flash

# Xoodyak ASM
avr_bin_demo_xoodyak_asm: set_xoodyak_asm base_avr_demo
avr_flash_demo_xoodyak_asm: avr_bin_demo_xoodyak_asm base_avr_flash
avr_bin_measure_xoodyak_asm: set_xoodyak_asm base_avr_measure
avr_flash_measure_xoodyak_asm: avr_bin_measure_xoodyak_asm base_avr_flash

# GCM
avr_bin_demo_gcm: set_gcm base_avr_demo
avr_flash_demo_gcm: avr_bin_demo_gcm base_avr_flash
avr_bin_measure_gcm: set_gcm base_avr_measure
avr_flash_measure_gcm: avr_bin_measure_gcm base_avr_flash

###### Misc

.PHONY: clean
clean:
	rm -rf bin
