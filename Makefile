all: prepare bootloader kernel link iso2 clear

prepare:
	mkdir -p iso

bootloader:	
	nasm -f bin src/bootloader/bootloader.asm -i src/bootloader -o boot.bin 

kernel:
	gcc -ffreestanding -c src/kernel/kernel.c -o kernel.o
	nasm src/bootloader/kernel_entry.asm -f elf64 -o kernel_entry.o
	ld -o kernel.bin -Ttext 0x9000 kernel_entry.o kernel.o --oformat binary
	rm -f kernel_entry.o
	rm -f kernel.o

link:
	cat boot.bin kernel.bin > flosp.bin

iso2:
	mv flosp.bin iso
	mkisofs -V 'flosp' \
		-o flosp.iso \
		-b flosp.bin \
		-no-emul-boot \
		-boot-load-size 19 \
		iso/

clear:
	rm -f boot.bin
	rm -f kernel.bin

clean:
	rm -f flosp.iso
	rm -rf iso
