[org 0x7c00]

; --- 1. SETUP ---
xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7c00

; --- 2. MASSIVE KERNEL LOAD ---
mov ax, 0x1000  
mov es, ax      ; Set Extra Segment to 0x1000
mov bx, 0x0000  ; Target Address is ES:BX (0x10000)

mov ah, 0x02    ; Read Sectors Command
mov al, 120     ; [PATCH]: Read 120 Sectors (60KB Payload Limit!)
mov ch, 0
mov cl, 2       ; Start at Sector 2
mov dh, 0
mov dl, 0x80    ; Drive 0
int 0x13

jc disk_error

; --- 3. JUMP TO NEW KERNEL SPACE ---
jmp 0x1000:0000 ; Jump to CS=0x1000, IP=0000

disk_error:
    mov ah, 0x0E
    mov al, 'E'
    int 0x10
    hlt
    jmp $

times 510-($-$$) db 0
dw 0xaa55