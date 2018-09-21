SRC=string.c scanner.c main.c
CFLAGS=-std=c99

ALL: $(SRC)
	gcc -o ifj $^ $(CFLAGS)
