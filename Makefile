DISK_SIZE=2880

all: flosp.iso

BOOTX64.EFI:
	make -C src/bootloader
	mv src/bootloader/BOOTX64.EFI ./

kernel.elf:
	make -C src/os
	mv src/os/kernel.elf ./

flosp.iso: kernel.elf BOOTX64.EFI 
	dd if=/dev/zero of=./fat32.img bs=1024 count=${DISK_SIZE}
	# mkfs.fat -F 32 ./fat32.img
	mformat -i ./fat32.img -f ${DISK_SIZE} ::
	mkdir mnt
	sudo su -c "mount -t vfat -o exec,user,rw --source ./fat32.img --target ./mnt && \
		mkdir -p ./mnt/EFI/BOOT && \
		cp -v ./BOOTX64.EFI ./mnt/EFI/BOOT && \
		cp -v ./kernel.elf ./mnt && \
		umount ./mnt"
	mkdir iso
	mv fat32.img iso
	xorriso -as mkisofs -R -f -e fat32.img -no-emul-boot -o flosp.iso iso

clean:
	rm -rf \
		*.iso \
		*.EFI \
		*.elf \
		mnt/ \
		iso/
	make -C src/bootloader clean
	make -C src/os clean

qemu: all
	qemu-system-x86_64 \
		-drive if=pflash,format=raw,unit=0,file=bios64.bin \
		-net none \
		-cdrom flosp.iso

qemu-usb: all
	./qemu-usb.sh

wc:
	wc -l `find src`

usb: all
	./usb.sh
