version="\"$(cat ver.txt)\""
nasm -dKVER=$version -f bin $1 -i src/bootloader -o $2 
