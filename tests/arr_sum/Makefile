all: main

main:
	nasm -f elf64 -F stabs main.asm -o main.o
	ld main.o -o main

clean:
	rm main{,.o}
