[bits 64]
global _start
extern kernel_main

; --- CONSTANTS (SAFE LOW MEMORY) ---
%define VBE_INFO_ADDR  0x8000
%define MODE_INFO_ADDR 0x9000

section .text
_start:
    [bits 16]
    ; DEBUG: Print 'A' (Alive)
    mov ah, 0x0E
    mov al, 'A'
    int 0x10

    ; 1. GET VESA INFO (Targeting 0x8000)
    mov ax, 0x4F00
    mov di, VBE_INFO_ADDR   ; FORCE DI TO 0x8000
    int 0x10
    cmp ax, 0x004F
    jne vbe_fail

    ; Get list pointer from the buffer we just filled
    mov si, [VBE_INFO_ADDR + 14]
    mov ax, [VBE_INFO_ADDR + 16]
    mov fs, ax

.find_mode:
    mov cx, [fs:si]
    add si, 2
    cmp cx, 0xFFFF
    je vbe_fail

    ; 2. GET MODE INFO (Targeting 0x9000)
    mov ax, 0x4F01
    mov di, MODE_INFO_ADDR  ; FORCE DI TO 0x9000
    int 0x10
    cmp ax, 0x004F
    jne .find_mode

    ; 3. CHECK FOR 1920x1080
    mov ax, [MODE_INFO_ADDR + 0x12]
    cmp ax, 1920        ; Width
    jne .find_mode
    
    mov ax, [MODE_INFO_ADDR + 0x14]
    cmp ax, 1080        ; Height
    jne .find_mode
    
    mov al, [MODE_INFO_ADDR + 0x19]
    cmp al, 32          ; Color Depth
    jne .find_mode

    ; --- FOUND IT ---
    mov eax, [MODE_INFO_ADDR + 0x28]
    mov [framebuffer_addr], eax

    ; SET MODE
    or cx, 0x4000
    mov bx, cx
    mov ax, 0x4F02
    int 0x10

    ; --- ENTER PROTECTED MODE ---
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:init_32bit

vbe_fail:
    mov ah, 0x0E
    mov al, 'V'
    int 0x10
    cli
    hlt
    jmp $

; --- 32-BIT MODE ---
[bits 32]
init_32bit:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x90000

    ; PAGING (Map 4GB)
    mov edi, 0x10000
    xor eax, eax
    mov ecx, 6144
    rep stosd

    mov dword [0x10000], 0x11003 
    mov dword [0x11000], 0x12003
    mov dword [0x11008], 0x13003
    mov dword [0x11010], 0x14003
    mov dword [0x11018], 0x15003

    mov edi, 0x12000
    mov eax, 0x83
    mov ecx, 2048
.huge_loop:
    mov [edi], eax
    add eax, 0x200000
    add edi, 8
    loop .huge_loop

    ; LONG MODE
    mov eax, 0x10000
    mov cr3, eax
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    jmp 0x18:init_64bit

; --- 64-BIT MODE ---
[bits 64]
init_64bit:
    mov edi, [framebuffer_addr] 
    mov rcx, 0
    mov ecx, edi
    mov rdi, rcx
    call kernel_main
    hlt
    jmp $

; --- DATA ---
section .data
framebuffer_addr: dd 0
; NOTE: Removed vbe_info_block and mode_info_block from here.
; We use the raw memory addresses 0x8000 and 0x9000 instead.

align 8
gdt_start:
    dq 0x0
    dq 0x00cf9a000000ffff
    dq 0x00cf92000000ffff
    dq 0x00209a0000000000
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start