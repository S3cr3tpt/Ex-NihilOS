[org 0x1000]

; --- 16-BIT REAL MODE ---
start:
    ; 1. SETUP STACK
    mov ax, 0
    mov ss, ax
    mov sp, 0xFFFC

    ; 2. GET VESA INFO (Scan for 1920x1080x32)
    mov ax, 0x4F00
    mov di, vbe_info_block
    int 0x10
    cmp ax, 0x004F
    jne vbe_fail

    mov si, [vbe_info_block + 14]
    mov ax, [vbe_info_block + 16]
    mov fs, ax

.find_mode:
    mov cx, [fs:si]
    add si, 2
    cmp cx, 0xFFFF
    je vbe_fail

    mov ax, 0x4F01
    mov di, mode_info_block
    int 0x10
    cmp ax, 0x004F
    jne .find_mode

    ; Check Resolution (1920x1080x32)
    mov ax, [mode_info_block + 0x12]
    cmp ax, 1920
    jne .find_mode
    mov ax, [mode_info_block + 0x14]
    cmp ax, 1080
    jne .find_mode
    mov al, [mode_info_block + 0x19]
    cmp al, 32
    jne .find_mode

    ; SAVE POINTER
    mov eax, [mode_info_block + 0x28]
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
    hlt
    jmp vbe_fail

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

    ; --- PAGING: MAP 4GB (Identity) ---
    ; We need:
    ; 1 PML4 at 0x10000
    ; 1 PDP  at 0x11000
    ; 4 PDs  at 0x12000, 0x13000, 0x14000, 0x15000 (To cover 4GB)
    
    ; 1. Clear 24KB of Memory (0x10000 - 0x16000)
    mov edi, 0x10000
    xor eax, eax
    mov ecx, 6144
    rep stosd

    ; 2. Link PML4 -> PDP
    mov dword [0x10000], 0x11003 

    ; 3. Link PDP -> 4 PDs
    mov dword [0x11000], 0x12003 ; PDP[0] -> PD0 (0-1GB)
    mov dword [0x11008], 0x13003 ; PDP[1] -> PD1 (1-2GB)
    mov dword [0x11010], 0x14003 ; PDP[2] -> PD2 (2-3GB)
    mov dword [0x11018], 0x15003 ; PDP[3] -> PD3 (3-4GB)

    ; 4. Fill all 4 PDs with 2MB Huge Pages
    ; We need to fill 512 entries * 4 Tables = 2048 entries total.
    mov edi, 0x12000       ; Start at PD0
    mov eax, 0x83          ; Phsyical 0 + Huge + Present + RW
    mov ecx, 2048          ; Fill 4GB worth of pages

.huge_loop:
    mov [edi], eax
    add eax, 0x200000      ; Add 2MB to physical address
    add edi, 8             ; Next entry
    loop .huge_loop

    ; --- ENABLE 64-BIT ---
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
    ; 1. GET FRAMEBUFFER
    mov edi, [framebuffer_addr] 
    
    ; 2. DRAW CENTER PIXEL (1920x1080)
    ; (1080/2 * 1920 + 1920/2) * 4 = 4151040
    mov dword [rdi + 4151040], 0x00FFFFFF ; White Pixel

    ; 3. DRAW A LINE (To be sure)
    mov ecx, 100
    mov rbx, 0
.line_loop:
    mov dword [rdi + rbx], 0x00FF0000 ; Blue Line at top left
    add rbx, 4
    loop .line_loop

    hlt
    jmp $

; --- DATA ---
align 4
framebuffer_addr dd 0
vbe_info_block: resb 512
mode_info_block: resb 256

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