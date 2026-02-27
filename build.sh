#!/bin/bash

mkdir -p build/src

nasm -f bin boot.s -o build/src/boot.bin
nasm -f elf32 kernel_entry.s -o build/src/kernel_entry.o
gcc -m32 -ffreestanding -fno-pic -c kernel.c -o build/src/kernel.o
ld -m elf_i386 -T linker.ld build/src/kernel_entry.o build/src/kernel.o -o build/src/kernel.bin --oformat binary

# pad kernel to 512 bytes
KERNEL_SIZE=$(stat -c %s build/src/kernel.bin)
PAD=$((512 - KERNEL_SIZE % 512))
if [ $PAD -ne 512 ]; then
    dd if=/dev/zero bs=1 count=$PAD >> build/src/kernel.bin
fi

cat build/src/boot.bin build/src/kernel.bin > build/os.img

echo "Built os.img. Run with:"
echo "qemu-system-i386 -fda build/os.img"
