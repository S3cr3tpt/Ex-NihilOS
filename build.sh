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

# 4. Dynamic Compilation (Sweep and Compile)
echo "[*] Sweeping and Compiling C Matrix..."
C_FILES=$(find src -name "*.c")
for file in $C_FILES; do
    OBJ_FILE="$BUILD_DIR/$(basename $file .c).o"
    gcc -ffreestanding -mno-red-zone -m64 -mno-mmx -mno-sse -mno-sse2 -fno-pie -fno-pic -fno-asynchronous-unwind-tables -c $file -o $OBJ_FILE
done

# 4.5 Dynamic Assembly Compilation (For Ring 0 files like interrupts.asm)
echo "[*] Sweeping and Compiling Ring 0 Assembly..."
ASM_FILES=$(find src/cpu -name "*.asm")
for file in $ASM_FILES; do
    OBJ_FILE="$BUILD_DIR/$(basename $file .asm).o"
    nasm -f elf64 $file -o $OBJ_FILE
done

# 5. Dynamic Linker
echo "[*] Linking Architecture..."
# Collect all generated object files, excluding kernel_entry.o to prevent duplicate linking
ALL_OBJS=$(find $BUILD_DIR -name "*.o" ! -name "kernel_entry.o")

# kernel_entry.o MUST remain first to guarantee it sits exactly at 0x1000
ld -m elf_x86_64 -o $BUILD_DIR/kernel.bin -Ttext 0x1000 $BUILD_DIR/kernel_entry.o $ALL_OBJS --oformat binary

# 6. Fuse and Pad
cat $BUILD_DIR/boot.bin $BUILD_DIR/kernel.bin > $OS_IMAGE
dd if=/dev/zero bs=1048576 count=1 >> $OS_IMAGE 2>/dev/null

echo "[+] Built. Size: $(wc -c < $OS_IMAGE) bytes."

# 7. Run
qemu-system-x86_64 -drive format=raw,file=$OS_IMAGE -vga std