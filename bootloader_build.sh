version="\"$(cat ver.txt)\""
nasm -dKVER=$version -f bin src/bootloader/bootloader.asm -i src/bootloader -o boot.bin 
