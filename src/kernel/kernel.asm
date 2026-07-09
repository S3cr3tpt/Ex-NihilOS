[bits 64]
global _start
extern kernel_main

; --- CONSTANTS (SAFE LOW MEMORY) ---
%define VBE_INFO_ADDR  0x8000
%define MODE_INFO_ADDR 0x9000

section .text
_start:
    [bits 16]
    ; Zero out data segments to ensure clean physical addressing
    xor ax, ax
    mov ds, ax
    mov es, ax

    ; DEBUG: Print 'A' (Alive)
    mov ah, 0x0E
    mov al, 'A'
    int 0x10
    
; --- 1. GET MEMORY MAP (E820) ---
    mov di, 0x5004      
    xor ebx, ebx        
    mov dword [0x5000], 0 

.e820_loop:
    mov eax, 0xe820
    mov ecx, 24         
    mov edx, 0x534D4150 
    int 0x15
    jc .e820_done       
    cmp eax, 0x534D4150 
    jne .e820_done
    add di, 24          
    inc dword [0x5000]  
    test ebx, ebx       
    je .e820_done
    jmp .e820_loop
.e820_done:
    
    ; 1. GET VESA INFO 
    mov ax, 0x4F00
    mov di, VBE_INFO_ADDR
    int 0x10
    cmp ax, 0x004F
    jne vbe_fail

    mov si, [VBE_INFO_ADDR + 14]
    mov ax, [VBE_INFO_ADDR + 16]
    mov fs, ax

.find_mode:
    mov cx, [fs:si]
    add si, 2
    cmp cx, 0xFFFF
    je vbe_fail

    ; 2. GET MODE INFO
    mov ax, 0x4F01
    mov di, MODE_INFO_ADDR 
    int 0x10
    cmp ax, 0x004F
    jne .find_mode

    ; 3. CHECK FOR 1920x1080
    mov ax, [MODE_INFO_ADDR + 0x12]
    cmp ax, 1920        
    jne .find_mode
    
    mov ax, [MODE_INFO_ADDR + 0x14]
    cmp ax, 1080        
    jne .find_mode
    
    mov al, [MODE_INFO_ADDR + 0x19]
    cmp al, 32          
    jne .find_mode

    ; --- FOUND IT ---
    mov eax, [MODE_INFO_ADDR + 0x28]
    ; [PATCH]: Force 32-bit displacement for new high memory address
    mov [dword framebuffer_addr], eax

    ; SET MODE
    or cx, 0x4000
    mov bx, cx
    mov ax, 0x4F02
    int 0x10

    ; --- ENTER PROTECTED MODE ---
    cli
    ; [PATCH]: Force 32-bit displacement to reach the new 0x10000 offset
    lgdt [dword gdt_descriptor]  
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp dword 0x08:init_32bit

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

    ; PAGING [PATCH: Evacuated to 0x70000 to make room for Kernel]
    mov edi, 0x70000
    xor eax, eax
    mov ecx, 6144
    rep stosd

    mov dword [0x70000], 0x71003 
    mov dword [0x71000], 0x72003
    mov dword [0x71008], 0x73003
    mov dword [0x71010], 0x74003
    mov dword [0x71018], 0x75003

    mov edi, 0x72000
    mov eax, 0x83
    mov ecx, 2048
.huge_loop:
    mov [edi], eax
    add eax, 0x200000
    add edi, 8
    loop .huge_loop

    ; LONG MODE
    mov eax, 0x70000    ; Route CR3 to new Page Tables
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