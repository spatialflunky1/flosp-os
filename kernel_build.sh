version="\"$(cat ver.txt)\""
gcc -DKVER=$version -DMESSAGE_FILTER=0 -Iinc/ -ffreestanding -c $2 -o $1
