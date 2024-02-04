version="\"$(cat ver.txt)\""
gcc -DKVER=$version -ffreestanding -c $2 -o $1
