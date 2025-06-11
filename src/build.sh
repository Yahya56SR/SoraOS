#!/bin/bash

set -e
sudo apt-get install -y g++ nasm qemu-system-x86 grub-pc-bin xorriso mtools

# Create build directory
mkdir -p build

# Compile assembly files with debug info
echo "Compiling assembly files..."
nasm -f elf32 -g -F dwarf boot.asm -o build/boot.o
nasm -f elf32 -g -F dwarf switch_task.asm -o build/switch_task.o
nasm -f elf32 -g -F dwarf interrupt_stubs.asm -o build/interrupt_stubs.o

# Compile C++ files with debug info
echo "Compiling C++ files..."
CXXFLAGS="-m32 -ffreestanding -fno-exceptions -fno-rtti -O0 -g3 -I./include"
g++ $CXXFLAGS -c kernel.cpp -o build/kernel.o
g++ $CXXFLAGS -c paging.cpp -o build/paging.o
g++ $CXXFLAGS -c tasking.cpp -o build/tasking.o
g++ $CXXFLAGS -c kernel_util.cpp -o build/kernel_util.o
g++ $CXXFLAGS -c process/process.cpp -o build/process.o
g++ $CXXFLAGS -c process/process_manager.cpp -o build/process_manager.o
g++ $CXXFLAGS -c interrupts.cpp -o build/interrupts.o

# Link with debug info
echo "Linking kernel..."
ld -m elf_i386 -T linker.ld -o build/kernel.bin \
   build/boot.o build/switch_task.o build/interrupt_stubs.o \
   build/kernel.o build/paging.o build/tasking.o \
   build/kernel_util.o build/process.o build/process_manager.o \
   build/interrupts.o

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