C_SRC = $(wildcard src/kernel/*.c src/drivers/*.c)
C_HEA = $(wildcard src/kernel/*.h src/drivers/*.h)
OBJ = $(C_SRC:src/%.c=obj/%.o)

all: obj bin flosp.iso

obj:
	mkdir -p obj/{bootloader,kernel,drivers}

bin:
	mkdir -p bin/{bootloader,kernel}

flosp.iso: iso_dir iso_dir/flosp.bin
	mkisofs -V 'flosp' \
		-o $@ \
		-b flosp.bin \
		-no-emul-boot \
		-boot-load-size 19 \
		iso_dir/

iso_dir:
	mkdir -p iso_dir

iso_dir/flosp.bin: bin/bootloader/boot.bin bin/kernel/kernel.bin
	cat $^ > $@

bin/bootloader/boot.bin: src/bootloader/bootloader.asm
	./bootloader_build.sh $^ $@

bin/kernel/kernel.bin: obj/bootloader/kernel_entry.o ${OBJ}
	ld -o $@ -Ttext 0x8400 $^ --oformat binary

$(OBJ): obj/%.o: src/%.c ${C_HEA}
	gcc -ffreestanding -c $< -o $@

obj/bootloader/kernel_entry.o: src/bootloader/kernel_entry.asm
	nasm $< -f elf64 -o $@

obj/kernel/kernel.o: src/kernel/kernel.c src/kernel/kernel.h
	./kernel_build.sh $@ $^

clean:
	rm -f *.iso
	rm -rf iso_dir obj bin
