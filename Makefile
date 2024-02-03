all: flosp.iso

iso_dir:
	mkdir -p iso_dir

boot.bin: src/bootloader/bootloader.asm
	./bootloader_build.sh $< $@

iso_dir/flosp.bin: boot.bin kernel.bin
	cat $^ > $@

flosp.iso: iso_dir iso_dir/flosp.bin
	mkisofs -V 'flosp' \
		-o $@ \
		-b flosp.bin \
		-no-emul-boot \
		-boot-load-size 19 \
		iso_dir/

clean:
	rm -f *.bin *.o *.iso
	rm -rf iso_dir

kernel.bin: kernel_entry.o kernel.o
	ld -o $@ -Ttext 0x8400 $^ --oformat binary

kernel.o: src/kernel/kernel.c
	./kernel_build.sh $< $@

kernel_entry.o: src/kernel/kernel_entry.asm
	nasm $< -f elf64 -o $@
