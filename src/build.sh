#!/bin/bash

set -e
sudo apt-get install -y g++ nasm qemu-system-x86 grub-pc-bin xorriso mtools

# Create build directory
mkdir -p build

# Compile the assembly bootloader
echo "Compiling boot.asm..."
nasm -f elf32 boot.asm -o build/boot.o

# Compile the assembly task switcher
echo "Compiling switch_task.asm"
nasm -f elf32 switch_task.asm -o build/switch_task.o

# Compile the kernel
echo "Compiling kernel.cpp..."
g++ -m32 -ffreestanding -fno-exceptions -fno-rtti -O2 -c kernel.cpp -o build/kernel.o

# Compile the paging
echo "Compiling paging.cpp..."
g++ -m32 -ffreestanding -fno-exceptions -fno-rtti -O2 -c paging.cpp -o build/paging.o

# Compile the tasking
echo "Compiling tasking.cpp..."
g++ -m32 -ffreestanding -fno-exceptions -fno-rtti -O2 -c tasking.cpp -o build/tasking.o

# Link the kernel
echo "Linking kernel..."
ld -m elf_i386 -T linker.ld -o build/kernel.bin build/boot.o build/switch_task.o build/kernel.o build/paging.o build/tasking.o

# Check if kernel.bin exists and has size greater than 0
if [ ! -s build/kernel.bin ]; then
    echo "Error: kernel.bin is empty or does not exist."
    exit 1
fi

# Create a bootable ISO
echo "Creating bootable ISO..."
mkdir -p build/iso/boot/grub
cp build/kernel.bin build/iso/boot/
cat > build/iso/boot/grub/grub.cfg << EOF
set timeout=0
set default=0

menuentry "Sora OS" {
    multiboot /boot/kernel.bin
    boot
}
EOF

grub-mkrescue -o build/cos.iso build/iso

# Run the OS in QEMU
qemu-system-i386 -cdrom build/cos.iso -no-reboot -d int
