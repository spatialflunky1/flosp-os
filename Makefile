all: boot

boot:	
	nasm -f bin src/boot.asm -i src -o boot.bin

clean:
	rm boot.bin
