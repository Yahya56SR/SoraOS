#!/bin/bash

set -e

# Check if dependencies are installed (Debian/Ubuntu)
if ! command -v gcc &> /dev/null || ! command -v nasm &> /dev/null || \
   ! command -v qemu-system-x86 &> /dev/null || ! command -v grub-mkrescue &> /dev/null || \
   ! command -v xorriso &> /dev/null || ! command -v mtools &> /dev/null; then
    echo "Installing dependencies (Debian/Ubuntu)..."
    sudo apt-get update
    sudo apt-get install -y gcc nasm qemu-system-x86 grub-pc-bin xorriso mtools
fi

# Create build directory
mkdir -p build

# Compile assembly files with debug info
echo "Compiling assembly files..."
nasm -f elf32 -g -F dwarf boot.asm -o build/boot.o
nasm -f elf32 -g -F dwarf switch_task.asm -o build/switch_task.o
nasm -f elf32 -g -F dwarf interrupt_stubs.asm -o build/interrupt_stubs.o

# Compile C files with debug info
echo "Compiling C files..."
CFLAGS="-m32 -ffreestanding -O0 -g3 -I./include"
gcc $CFLAGS -c kernel.c -o build/kernel.o
gcc $CFLAGS -c paging.c -o build/paging.o
gcc $CFLAGS -c tasking.c -o build/tasking.o
gcc $CFLAGS -c io.c -o build/io.o
gcc $CFLAGS -c process/process.c -o build/process/process.o
gcc $CFLAGS -c process/process_manager.c -o build/process/process_manager.o
gcc $CFLAGS -c interrupts.c -o build/interrupts.o
gcc $CFLAGS -c liballoc.c -o build/liballoc.o
gcc $CFLAGS -c memorys.c -o build/memorys.o

# Link with debug info
echo "Linking kernel..."
ld -m elf_i386 -T linker.ld -o build/kernel.bin \
   build/boot.o build/switch_task.o build/interrupt_stubs.o \
   build/kernel.o build/paging.o build/tasking.o \
   build/io.o build/process/process.o build/process/process_manager.o \
   build/interrupts.o build/liballoc.o build/memorys.o

# Create ISO
echo "Creating bootable ISO..."
mkdir -p build/iso/boot/grub
cp build/kernel.bin build/iso/boot/

cat > build/iso/boot/grub/grub.cfg << EOF
set timeout=0
set default=0

menuentry "SoraOS" {
    multiboot /boot/kernel.bin
    boot
}
EOF

grub-mkrescue -o build/cos.iso build/iso

# Run with debug output
qemu-system-i386 -cdrom build/cos.iso -no-reboot -d int,cpu_reset -monitor stdio