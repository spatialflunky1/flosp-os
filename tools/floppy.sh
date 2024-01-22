dd if=/dev/zero of=flosp.img bs=1024 count=1440
dd if=flosp.bin of=flosp.img conv=notrunc
