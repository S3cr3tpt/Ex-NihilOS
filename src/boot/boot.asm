[org 0x7c00]

; --- 1. SETUP ---
xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7c00

; --- 2. LOAD KERNEL ---
mov ah, 0x02    ; Read Sectors
mov al, 10      ; Read 10 Sectors
mov ch, 0
mov cl, 2       ; Start Sector 2
mov dh, 0
mov dl, 0x80    ; Drive 0
mov bx, 0x1000  ; Target Address
int 0x13

jc disk_error   ; <--- SAFETY CHECK

; --- 3. JUMP ---
jmp 0x1000

disk_error:
    mov ah, 0x0E
    mov al, 'E' ; Print 'E' if read fails
    int 0x10
    hlt
    jmp $

times 510-($-$$) db 0
dw 0xaa55