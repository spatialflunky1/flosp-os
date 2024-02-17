version="\"$(cat ver.txt)\""
gcc -DKVER=$version -DDEBUG_FILTER=0 -Iinc/ -ffreestanding -c $2 -o $1
