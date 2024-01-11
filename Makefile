all: boot

boot:	
	nasm -f bin src/bootloader/bootloader.asm -i src/bootloader -o boot.bin
	tools/floppy.sh # using virtualbox to test: using floppy image

clean:
	rm boot.bin
	rm boot.img
