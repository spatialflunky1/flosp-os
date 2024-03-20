CC := gcc
INC_DIR := src/inc
CFLAGS := \
		  -ffreestanding \
		  -fno-stack-protector \
		  -I${INC_DIR} \
		  -g
LDFLAGS := \
		   -ffreestanding \
		   -nostdlib \
		   -g3 \
		   -T src/kernel/kernel.ld

C_SRC = $(wildcard src/kernel/*.c src/drivers/*.c src/cpu/*.c)
OBJ = $(C_SRC:src/%.c=obj/%.o)

all: obj bin kernel.elf

obj:
	mkdir -p obj/{kernel,drivers,cpu}

bin:
	mkdir -p bin/kernel

kernel.elf: ${OBJ} obj/cpu/isr.o
	${CC} -o $@ ${LDFLAGS} $^

obj/%.o: src/%.c
	 ${CC} ${CFLAGS} -c $< -o $@

obj/kernel/kernel.o: src/kernel/kernel.c src/inc/kernel/kernel.h
	./kernel_build.sh "${CC}" "${CFLAGS}" "$<" "$@"

obj/cpu/isr.o: src/cpu/isr.s
	nasm $< -f elf64 -o $@

clean:
	rm -rf obj bin *.elf
