all: bootloader kernel link floppy clear

bootloader:	
	nasm -f bin src/bootloader/bootloader.asm -i src/bootloader -o boot.bin 

kernel:
	gcc -ffreestanding -c src/kernel/kernel.c -o kernel.o
	ld -o kernel.bin -Ttext 0x9000 kernel.o --oformat binary
	rm kernel.o

link:
	cat boot.bin kernel.bin > flosp.bin

floppy:
	tools/floppy.sh # using virtualbox to test: using floppy image

clear:
	rm boot.bin
	rm kernel.bin
	rm flosp.bin

clean:
	rm flosp.img
