#!/bin/bash

# 1. Clean previous build (delete old binary)
rm -f build/boot.bin

# 2. Assemble the Bootloader
# syntax: nasm [format] [source] -o [output]
nasm -f bin src/boot/boot.asm -o build/boot.bin

# 3. Check if it worked
if [ -f "build/boot.bin" ]; then
    echo "[+] Build Successful. Launching the Void..."
    # 4. Run Emulator 
    qemu-system-i386 -drive format=raw,file=build/boot.bin
else
    echo "[-] Build Failed. Check your Assembly."
fi