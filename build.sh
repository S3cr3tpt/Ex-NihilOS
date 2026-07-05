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
    gcc -ffreestanding -mno-red-zone -m64 -mno-mmx -mno-sse -mno-sse2 -fno-pie -fno-pic -fno-asynchronous-unwind-tables -c $file -o $OBJ_FILE
done

echo "[*] Sweeping and Compiling Ring 0 Assembly..."
ASM_FILES=$(find src/cpu -name "*.asm")
for file in $ASM_FILES; do
    OBJ_FILE="$BUILD_DIR/$(basename $file .asm).o"
    nasm -f elf64 $file -o $OBJ_FILE
done

echo "[*] Linking Architecture..."
ALL_OBJS=$(find $BUILD_DIR -name "*.o" ! -name "kernel_entry.o")

# Wired to linker.ld. Fused as pure binary.
ld -m elf_x86_64 -T src/linker.ld --oformat binary -o $BUILD_DIR/kernel.bin $BUILD_DIR/kernel_entry.o $ALL_OBJS

echo "[*] Fusing Payload..."
cat $BUILD_DIR/boot.bin $BUILD_DIR/kernel.bin > $OS_IMAGE
dd if=/dev/zero bs=1048576 count=1 >> $OS_IMAGE 2>/dev/null

echo "[+] Built. Size: $(stat -c%s $OS_IMAGE) bytes."

if [ -n "$SSH_CLIENT" ]; then
    echo "[*] Remote Terminal Detected (SSH/Windows)."
    echo "[*] Booting Headless Mode. Broadcasting to VNC Port 5900..."
    qemu-system-x86_64 -drive format=raw,file=$OS_IMAGE -m 2G -vnc 0.0.0.0:0
else
    echo "[*] Local Hardware Detected."
    qemu-system-x86_64 -drive format=raw,file=$OS_IMAGE -m 2G
fi