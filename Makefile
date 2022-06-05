CC=gcc
CFLAGS=-I src/crypto -DDEBUG

all: bench

bin:
	mkdir -p bin

bench: bin
	${CC} ${CFLAGS} src/benchmark/client.c src/crypto/**/*.c -o bin/client
	${CC} ${CFLAGS} src/benchmark/server.c src/crypto/**/*.c -o bin/server

.PHONY: clean
clean:
	rm -rf bin