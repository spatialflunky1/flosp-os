CC := clang
INC_DIR := src/inc
CFLAGS := \
		  -ffreestanding \
		  -fno-stack-protector \
		  -mno-red-zone \
		  -I${INC_DIR} \
		  -Wall \
		  -Wextra \
		  -Wmissing-prototypes \
		  -Wstrict-prototypes
LDFLAGS := \
		   -ffreestanding \
		   -no-pie \
		   -nostdlib \
		   -T src/kernel/kernel.ld
# 0: All messages
# 1: No debug messages
# 2: No info messages
# 3: No warning messages
# 4: No error messages
# 5: No critical messages
MESSAGE_FILTER := 0

C_SRC = $(wildcard src/kernel/*.c src/drivers/*.c src/drivers/filesystem/*.c src/cpu/*.c)
OBJ = $(C_SRC:src/%.c=obj/%.o)

all: obj bin kernel.elf

obj:
	mkdir -p obj/{kernel,drivers/filesystem,cpu}

bin:
	mkdir -p bin/kernel

kernel.elf: ${OBJ} \
	        obj/cpu/isr.o \
	        obj/cpu/gdt64.o \
	        obj/cpu/gdt64_init.o \
	        obj/cpu/cpu_asm.o \
	        obj/drivers/mem_asm.o
	${CC} -o $@ ${LDFLAGS} $^

obj/%.o: src/%.c
	 ${CC} -DMESSAGE_FILTER=${MESSAGE_FILTER} ${CFLAGS} -c $< -o $@

obj/kernel/kernel.o: src/kernel/kernel.c src/inc/kernel/kernel.h
	./kernel_build.sh "${CC}" "${CFLAGS}" "$<" "$@" "${MESSAGE_FILTER}"

obj/%.o: src/%.s
	nasm $< -f elf64 -o $@

clean:
	rm -rf obj bin *.elf
