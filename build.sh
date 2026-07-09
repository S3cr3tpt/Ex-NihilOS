#!/bin/bash

SRC_BOOT="src/boot/boot.asm"
SRC_KERNEL_ASM="src/kernel/kernel.asm"
BUILD_DIR="build"
OS_IMAGE="$BUILD_DIR/os-image.bin"

mkdir -p $BUILD_DIR
rm -f $BUILD_DIR/*.bin $BUILD_DIR/*.o

echo "[*] Assembling Bootloader..."
nasm -f bin $SRC_BOOT -o $BUILD_DIR/boot.bin

echo "[*] Assembling Kernel Entry..."
nasm -f elf64 $SRC_KERNEL_ASM -o $BUILD_DIR/kernel_entry.o

echo "[*] Sweeping and Compiling C Matrix..."
C_FILES=$(find src -name "*.c")
for file in $C_FILES; do
    OBJ_FILE="$BUILD_DIR/$(basename $file .c).o"
    gcc -Os -ffreestanding -mno-red-zone -m64 -mno-mmx -mno-sse -mno-sse2 -fno-pie -fno-pic -fno-asynchronous-unwind-tables -c $file -o $OBJ_FILE
done

echo "[*] Sweeping and Compiling Ring 0 Assembly..."
ASM_FILES=$(find src/cpu -name "*.asm")
for file in $ASM_FILES; do
    OBJ_FILE="$BUILD_DIR/$(basename $file .asm).o"
    nasm -f elf64 $file -o $OBJ_FILE
done

echo "[*] Linking Architecture..."
ALL_OBJS=$(find $BUILD_DIR -name "*.o" ! -name "kernel_entry.o")
ld -m elf_x86_64 -T src/linker.ld --oformat binary -o $BUILD_DIR/kernel.bin $BUILD_DIR/kernel_entry.o $ALL_OBJS

echo "[*] Fusing Payload..."
# 1. Forge a mathematically perfect 2MB magnetic platter (4096 sectors)
dd if=/dev/zero of=$OS_IMAGE bs=512 count=4096 2>/dev/null
# 2. Inject Bootloader at Sector 0
dd if=$BUILD_DIR/boot.bin of=$OS_IMAGE bs=512 seek=0 conv=notrunc 2>/dev/null
# 3. Inject Kernel Matrix at Sector 1
dd if=$BUILD_DIR/kernel.bin of=$OS_IMAGE bs=512 seek=1 conv=notrunc 2>/dev/null

echo "[+] Built. Size: $(stat -c%s $OS_IMAGE) bytes."

if [ -n "$SSH_CLIENT" ]; then
    qemu-system-x86_64 -drive format=raw,file=$OS_IMAGE -m 2G -vnc 0.0.0.0:0
else
    qemu-system-x86_64 -drive format=raw,file=$OS_IMAGE -m 2G
fi