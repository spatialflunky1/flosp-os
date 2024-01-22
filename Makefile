all: bootloader kernel link floppy clear

bootloader:	
	nasm -f bin src/bootloader/bootloader.asm -i src/bootloader -o boot.bin 

kernel:
	gcc -ffreestanding -c src/kernel/kernel.c -o kernel.o
	nasm src/bootloader/kernel_entry.asm -f elf64 -o kernel_entry.o
	ld -o kernel.bin -Ttext 0x9000 kernel_entry.o kernel.o --oformat binary
	rm kernel_entry.o
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
