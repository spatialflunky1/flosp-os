CC := clang
LD := lld

C_SRC := $(wildcard src/*.c)
C_HEA := $(wildcard src/*.h)
OBJ := $(C_SRC:src/%.c=obj/%.o)

INC_DIR := src/inc

CFLAGS := -ffreestanding \
	      -mno-red-zone \
	      -I${INC_DIR} \
	      -target x86_64-unknown-windows \
		  -DDEBUG \
		  -DINFO \
		  -Wall \
		  -Wextra \
		  -Wmissing-prototypes \
		  -Wstrict-prototypes
LDFLAGS := -flavor link \
		   -subsystem:efi_application \
		   -entry:efi_main

all: obj BOOTX64.EFI

obj:
	mkdir obj

BOOTX64.EFI: ${OBJ}
	${LD} ${LDFLAGS} $^ -out:BOOTX64.EFI

$(OBJ): obj/%.o: src/%.c ${C_HEA}
	${CC} ${CFLAGS} -c $< -o $@

clean:
	rm -rf \
		*.EFI \
		obj/
