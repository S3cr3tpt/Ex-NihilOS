[bits 64]

; --- THE MACRO TEMPLATES ---
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push 0      ; Push dummy error code for stack alignment
    push %1     ; Push the interrupt number
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    ; Hardware already pushed the error code
    push %1     ; Push the interrupt number
    jmp isr_common_stub
%endmacro

; --- THE DEPLOYMENT (First 32 CPU Panics) ---
ISR_NOERRCODE 0   ; Divide by Zero
ISR_NOERRCODE 1   ; Debug
ISR_NOERRCODE 2   ; NMI
ISR_NOERRCODE 3   ; Breakpoint
ISR_NOERRCODE 4   ; Overflow
ISR_NOERRCODE 5   ; Bound Range Exceeded
ISR_NOERRCODE 6   ; Invalid Opcode
ISR_NOERRCODE 7   ; Device Not Available
ISR_ERRCODE   8   ; Double Fault (Pushes Error Code)
ISR_NOERRCODE 9   ; Coprocessor Segment Overrun
ISR_ERRCODE   10  ; Invalid TSS (Pushes Error Code)
ISR_ERRCODE   11  ; Segment Not Present (Pushes Error Code)
ISR_ERRCODE   12  ; Stack-Segment Fault (Pushes Error Code)
ISR_ERRCODE   13  ; General Protection Fault (Pushes Error Code)
ISR_ERRCODE   14  ; Page Fault (Pushes Error Code)
ISR_NOERRCODE 15  ; Reserved
ISR_NOERRCODE 16  ; x87 Floating-Point Exception
ISR_ERRCODE   17  ; Alignment Check (Pushes Error Code)
ISR_NOERRCODE 18  ; Machine Check
ISR_NOERRCODE 19  ; SIMD Floating-Point Exception
ISR_NOERRCODE 20  ; Virtualization Exception
ISR_ERRCODE   21  ; Control Protection Exception (Pushes Error Code)
ISR_NOERRCODE 22  ; Reserved
ISR_NOERRCODE 23  ; Reserved
ISR_NOERRCODE 24  ; Reserved
ISR_NOERRCODE 25  ; Reserved
ISR_NOERRCODE 26  ; Reserved
ISR_NOERRCODE 27  ; Reserved
ISR_NOERRCODE 28  ; Hypervisor Injection Exception
ISR_ERRCODE   29  ; VMM Communication Exception (Pushes Error Code)
ISR_ERRCODE   30  ; Security Exception (Pushes Error Code)
ISR_NOERRCODE 31  ; Reserved

; --- THE COMMON BLAST DOOR ---
extern isr_handler ; The C function we will write next

isr_common_stub:
    ; 1. Preserve total CPU state
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; 2. Hand over to C logic
    mov rdi, rsp    ; Pass the current stack pointer to C as argument 1
    call isr_handler

    ; 3. Restore total CPU state
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax

    ; 4. Strip the interrupt number and error code (8 bytes each)
    add rsp, 16 

    ; 5. Hardware Return
    iretq