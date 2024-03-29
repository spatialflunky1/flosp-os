#!/bin/sh

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

# Set disk GPT partition table
echo sudo parted "$SELECT" mklabel gpt
sudo parted "$SELECT" mklabel gpt

# Create 128 mb partition (type = EFI System)
echo sudo parted "$SELECT" mkpart primary fat32 1MB 128MB
sudo parted "$SELECT" mkpart primary fat32 1MB 128MB
echo sudo parted "$SELECT" set 1 esp on
sudo parted "$SELECT" set 1 esp on

# Get partition file location
EFI_PART=$(sudo fdisk -l $SELECT | tail -1 | awk -F' ' '{print $1}')

# Format partition fat32
sudo mkfs.fat -v -F 32 $EFI_PART

# mkdir temp
mkdir -v temp

# mount drive to temp
sudo mount -v $EFI_PART temp

# Create temp/EFI/BOOT folder (-pv)
sudo mkdir -pv temp/EFI/BOOT

# Copy {kernel.elf,BOOTX64.EFI} to temp/EFI/BOOT
sudo cp -v BOOTX64.EFI temp/EFI/BOOT
sudo cp -v kernel.elf temp

# umount temp
sudo umount temp

# Cleanup
rm -rfv temp
