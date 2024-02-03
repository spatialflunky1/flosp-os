version="\"$(cat ver.txt)\""
gcc -DKVER=$version -ffreestanding -c $1 -o $2
