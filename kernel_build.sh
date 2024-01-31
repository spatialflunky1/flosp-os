version="\"$(cat ver.txt)\""
gcc -DKVER=$version -ffreestanding -c src/kernel/kernel.c -o kernel.o
