# Scan for disks
DISKS_STR=$(sudo fdisk -l | grep "Disk /" | awk -F':' '{print $1}' | awk -F' ' '{print $2}')
DISKS=($(echo $DISKS_STR | tr ' ' '\n'))
COUNT=$(echo $DISKS_STR | tr -cd ' ' | wc -c)
COUNT=$(($COUNT + 1))

# Lists disks and await input
I=0
while [ $I -lt $COUNT ]
do
    echo "$(( $I + 1 )): ${DISKS[I]}"
    I=$(( $I + 1 ))
done
echo
echo "0: Quit"

# Get user input for disk num
RESPONSE=""
read -p "Choice (0-$COUNT): " RESPONSE
INDEX=$(( $RESPONSE )) # for a numeric value

# Condition for exit
if [ $INDEX == 0 ]
then
    exit
fi

# Condition for invalid response
if [[ $INDEX -lt 0 || $INDEX -gt $COUNT ]]
then
    echo "Error: invalid input"
    exit
fi

# Set index to actual list index value and get disk
INDEX=$(( $INDEX - 1 ))
SELECT="${DISKS[$INDEX]}"

# Run qemu
sudo -E qemu-system-x86_64 \
    -drive if=pflash,format=raw,unit=0,file=bios64.bin \
    -net none \
    -usb \
    -drive file=$SELECT,format=raw
