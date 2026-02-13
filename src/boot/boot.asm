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

cli ; clear interrupts
lgdt [gdt_descriptor]

mov eax, cr0; load the value of the control register so i can change it from the real mode to the protected mode (32bit)
or eax, 0x01 ; it has to be or because i dont want to delete the data in eax i just want to change 1 bit
mov cr0, eax
;finnaly go to 32 bits
jmp 0x08:init_32bit

;Freeze in the 16 bit sector
stop:
    hlt       ; Sleep until interrupt
    jmp stop  ; Infinite loop

gdt_start:
    ; The CPU requires the first entry to be zero. It detects "null pointers" this way.
    dd 0x0                  ; 4 bytes of zeros
    dd 0x0                  ; 4 bytes of zeros

    ; 2. THE KERNEL CODE SEGMENT (Offset 0x08)
    ; Base=0, Limit=4GB, Access=0x9A, Flags=0xCF
    dw 0xffff           ; Limit (bits 0-15)
    dw 0x0000           ; Base (bits 0-15)
    db 0x00             ; Base (bits 16-23)
    db 10011010b        ; Access Byte (Present, Ring 0, Code, Readable)
    db 11001111b        ; Flags (4KB Granularity, 32-bit) + Limit (bits 16-19)
    db 0x00             ; Base (bits 24-31)

    ; 3. THE KERNEL DATA SEGMENT (Offset 0x10)
    ; Base=0, Limit=4GB, Access=0x92, Flags=0xCF
    dw 0xffff           ; Limit (bits 0-15)
    dw 0x0000           ; Base (bits 0-15)
    db 0x00             ; Base (bits 16-23)
    db 10010010b        ; Access Byte (Present, Ring 0, Data, Writable)
    db 11001111b        ; Flags (4KB Granularity, 32-bit) + Limit (bits 16-19)
    db 0x00             ; Base (bits 24-31)
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1   ; Size (16 bits)
    dd gdt_start   
    
    
                  ; Address (32 bits)
[bits 32]
init_32bit:
    ; 5. Set up Data Segments (Point to 0x10)
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ;as usual im going to use AI to write the comments to make it easier to read
   ; --- PHASE 1 & 2: SANITIZE PAGING MEMORY ---
    ; We will build tables at 0x1000, 0x2000, 0x3000, and 0x4000.
    mov edi, 0x1000    ; Set destination index to 0x1000 (PML4 base)
    mov cr3, edi       ; (Optional early load) We will point CR3 here later
    xor eax, eax       ; Set EAX to 0
    mov ecx, 4096      ; 4096 double-words (4 bytes) = 16384 bytes (16KB)
    rep stosd          ; Loop: Write EAX (0) to EDI, decrement ECX, repeat.

    ; --- PHASE 3: FORGE THE LINKS ---
    mov edi, 0x1000    ; Reset EDI back to PML4 base
    
    ; Link PML4 (0x1000) -> PDP (0x2000)
    ; We write a 32-bit dword. The upper 32-bits are already 0 from our sweep.
    mov dword [edi], 0x2003      ; Entry 0: Address 0x2000 + Flags 0x3 (Present | R/W)
    
    ; Link PDP (0x2000) -> PD (0x3000)
    mov dword [edi + 0x1000], 0x3003 
    
    ; Link PD (0x3000) -> PT (0x4000)
    mov dword [edi + 0x2000], 0x4003 

    ; --- PHASE 4: THE PAYLOAD (IDENTITY MAP 2MB) ---
    mov edi, 0x4000    ; Point EDI to the final Page Table (PT)
    mov eax, 0x0003    ; First physical frame (0x0) + Flags 0x3
    mov ecx, 512       ; We need to fill 512 entries

.build_page_table:
    mov dword [edi], eax    ; Write the physical address + flags into the table
    add eax, 0x1000         ; Increment the physical address by 4KB (next frame)
    add edi, 8              ; Move to the next 8-byte entry in the table
    loop .build_page_table  ; Repeat until ECX hits 0

    ; --- PHASE 5: THE ACTIVATION ---
    mov eax, 0x1000    ; Get the physical address of the Master Table (PML4)
    mov cr3, eax       ; Load it into the Memory Management Unit

     ; --- MEMORY RECONNAISSANCE (PAGING TEST) ---
    ; We are manually acting as the MMU to verify our data structure.

    ; Test 1: Did PML4 link to PDP correctly?
    ; Expected value at 0x1000 is 0x2003 (Address 0x2000 + R/W + Present)
    mov eax, dword [0x1000]
    cmp eax, 0x2003
    jne .skip_test_1
    mov byte [0xA0000 + 32175], 0x0A  ; Light Green Pixel (PML4 is Good)
.skip_test_1:

    ; Test 2: Did PDP link to PD correctly?
    ; Expected value at 0x2000 is 0x3003
    mov eax, dword [0x2000]
    cmp eax, 0x3003
    jne .skip_test_2
    mov byte [0xA0000 + 32180], 0x0E  ; Yellow Pixel (PDP is Good)
.skip_test_2:

    ; Test 3: Did the Payload Loop work?
    ; Let's check Entry 1 of the Page Table (Memory 0x4000 + 8 bytes).
    ; Expected value is 0x1003 (Physical Address 0x1000 + Flags 0x3).
    mov eax, dword [0x4008]
    cmp eax, 0x1003
    jne .skip_test_3
    mov byte [0xA0000 + 32185], 0x0D  ; Magenta/Purple Pixel (Payload is Good)
.skip_test_3:

    ; 6. Draw Red Pixel (Proof of Protected Mode)
    ; In 32-bit, we don't use 'es'. We write to 0xA0000 directly.
    mov byte [0xA0000 + 32165], 0x28 ; Red Pixel (Offset slightly to the right)

    ;load the ISR
    lidt [idt_descriptor]

    ;triger the interrup 0 (divide by zero)
    int 0x00

    ; 7. freeze again
    stop32bit:
        hlt       ; Sleep until interrupt
        jmp stop32bit  ; Infinite loop


;ISR
isr_handler:
    ;save all registers as we dont want to mess it up for the rest of the code
    pusha
    ;blue dot
    ;the 0xA0000 is the memory for the visual interface, the 32170 is the offset to write on the right of the red one
    mov byte [0xA0000 + 32170], 0x09; put the blue dot on right of the red one
    
    ;i forgot this and it broke everthing
    popa
    ;return to the main code
    iretd

;here i used ai to write the comments so it makes more sense in the future
idt_start:
    ; ENTRY 0: Divide by Zero Exception
    dw isr_handler      ; Low 16 bits of address
    dw 0x08             ; Code Segment Selector
    db 0x00             ; Always zero
    db 10001110b        ; Flags: Present, Ring 0, 32-bit Interrupt Gate
    dw 0x0000           ; High 16 bits (0 because we are in low memory)
idt_end:
idt_descriptor:
    dw idt_end - idt_start - 1 ; Size
    dd idt_start               ; Address

;give it the botting up signature
times 510 - ($-$$) db 0  ; Fill the rest with zeros
dw 0xaa55                ; The Magic Boot Signatures