;initial offset
[org 0x7c00]
;initialize the video and call it
mov AH, 0x00
mov AL, 0x13
int 0x10

;setup for the video
mov ax, 0xA000
mov es, ax

;"calculate the midle" 
mov di, 32160
; finnaly writing the white pixel
mov byte [es:di], 0x0F

stop:
    hlt       ; Sleep until interrupt
    jmp stop  ; Infinite loop

;give it the botting up signature
times 510 - ($-$$) db 0  ; Fill the rest with zeros
dw 0xaa55                ; The Magic Boot Signatures