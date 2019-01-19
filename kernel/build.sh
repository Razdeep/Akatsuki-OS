#!/bin/bash
./clearbuild.sh
i686-elf-as boot.s -o boot.o
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -T linker.ld -o Akatsuki.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
mkdir -p isodir/boot/grub
cp Akatsuki.bin isodir/boot/Akatsuki.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o Akatsuki.iso isodir