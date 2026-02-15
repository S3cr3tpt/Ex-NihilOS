#!/bin/bash

# --- CONFIGURATION ---
SRC_BOOT="src/boot/boot.asm"
SRC_KERNEL="src/kernel/kernel.asm"
BUILD_DIR="build"
OS_IMAGE="$BUILD_DIR/os-image.bin"

# 1. Clean
mkdir -p $BUILD_DIR
rm -f $BUILD_DIR/*.bin

# 2. Assemble
echo "[*] Assembling Bootloader..."
nasm -f bin $SRC_BOOT -o $BUILD_DIR/boot.bin
echo "[*] Assembling Kernel..."
nasm -f bin $SRC_KERNEL -o $BUILD_DIR/kernel.bin

# 3. FUSE AND PAD (The Fix)
# Combine boot + kernel
cat $BUILD_DIR/boot.bin $BUILD_DIR/kernel.bin > $OS_IMAGE

# Add "Zero Padding" to make the file 1MB.
# This ensures the BIOS can read 10 sectors without falling off the edge of the disk.
dd if=/dev/zero bs=1048576 count=1 >> $OS_IMAGE 2>/dev/null

echo "[+] Built. Size: $(wc -c < $OS_IMAGE) bytes."

# 4. Run
qemu-system-x86_64 -drive format=raw,file=$OS_IMAGE -vga std