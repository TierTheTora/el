AS := as
CC := gcc

all: assemble compile

assemble: read.S write.S openat.S close.S
	$(AS) read.S -o read.o
	$(AS) write.S -o write.o
	$(AS) openat.S -o openat.o
	$(AS) close.S -o close.o

compile: el.c write.o read.o openat.o close.o
	$(CC) el.c write.o read.o openat.o close.o -o elc

clean:
	rm write.o read.o openat.o close.o
