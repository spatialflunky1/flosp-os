CC=$1
CFLAGS=$2
C_SRC=$3
C_OBJ=$4
FILTER=$5

version="\"$(cat ../../ver.txt)\""

echo "$CC $CFLAGS -DKVER=$version -DMESSAGE_FILTER=0 -c $C_SRC -o $C_OBJ" 

$CC $CFLAGS -DKVER=$version -DMESSAGE_FILTER=$FILTER -c $C_SRC -o $C_OBJ
