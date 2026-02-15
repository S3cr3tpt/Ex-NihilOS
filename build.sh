#!/bin/bash

# --- CONFIGURATION ---
SRC_BOOT="src/boot/boot.asm"
SRC_KERNEL_ASM="src/kernel/kernel.asm"
SRC_KERNEL_C="src/kernel/main.c"
BUILD_DIR="build"
OS_IMAGE="$BUILD_DIR/os-image.bin"

# 1. Clean
mkdir -p $BUILD_DIR
rm -f $BUILD_DIR/*.bin $BUILD_DIR/*.o

# 2. Assemble Bootloader
echo "[*] Assembling Bootloader..."
nasm -f bin $SRC_BOOT -o $BUILD_DIR/boot.bin

# 3. Assemble Kernel Entry
echo "[*] Assembling Kernel Entry..."
nasm -f elf64 $SRC_KERNEL_ASM -o $BUILD_DIR/kernel_entry.o

# 4. Compile C Kernel (THE FIX IS HERE)
echo "[*] Compiling C Kernel..."
# We add -mno-mmx -mno-sse -mno-sse2 so GCC doesn't use registers we haven't enabled.
gcc -ffreestanding -mno-red-zone -m64 -mno-mmx -mno-sse -mno-sse2 -c $SRC_KERNEL_C -o $BUILD_DIR/kernel_c.o

# 5. Link
echo "[*] Linking Kernel..."
# Important: kernel_entry.o MUST be first!
ld -o $BUILD_DIR/kernel.bin -Ttext 0x1000 $BUILD_DIR/kernel_entry.o $BUILD_DIR/kernel_c.o --oformat binary

# 6. Fuse and Pad
cat $BUILD_DIR/boot.bin $BUILD_DIR/kernel.bin > $OS_IMAGE
dd if=/dev/zero bs=1048576 count=1 >> $OS_IMAGE 2>/dev/null

echo "[+] Built. Size: $(wc -c < $OS_IMAGE) bytes."

# 7. Run
qemu-system-x86_64 -drive format=raw,file=$OS_IMAGE -vga std