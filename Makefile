all: iso_dir boot.bin kernel.bin iso_dir/flosp.bin flosp.iso

iso_dir:
	mkdir -p iso_dir

boot.bin: src/bootloader/bootloader.asm
	./bootloader_build.sh

iso_dir/flosp.bin:
	cat boot.bin kernel.bin > iso_dir/flosp.bin

flosp.iso:
	mkisofs -V 'flosp' \
		-o flosp.iso \
		-b flosp.bin \
		-no-emul-boot \
		-boot-load-size 19 \
		iso_dir/

clean:
	rm -f kernel_entry.o 
	rm -f kernel.o
	rm -f boot.bin
	rm -f kernel.bin
	rm -rf iso_dir
	rm -f flosp.iso

kernel.bin: kernel.o kernel_entry.o
	ld -o kernel.bin -Ttext 0x8400 kernel_entry.o kernel.o --oformat binary

kernel.o: src/kernel/kernel.c
	./kernel_build.sh

kernel_entry.o: src/kernel/kernel_entry.asm
	nasm src/kernel/kernel_entry.asm -f elf64 -o kernel_entry.o
