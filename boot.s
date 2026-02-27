[org 0x7C00]
[bits 16]

KERNEL_OFFSET equ 0x1000

start:
    cli
    mov [BOOT_DRIVE], dl       ; store boot drive
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; print B to show bootloader runs
    mov ah, 0x0E
    mov al, 'B'
    int 0x10

    call load_kernel
    jmp switch_to_pm

; -------------------------
; Load kernel from disk
; -------------------------
load_kernel:
    xor ax, ax
    mov es, ax

    mov ah, 0x02        ; BIOS read sectors
    mov al, 2           ; read 20 sector (adjust as needed)
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [BOOT_DRIVE]
    
    mov bx, KERNEL_OFFSET
    int 0x13
    jc disk_error
    ret

disk_error:
    jmp $

; -------------------------
; GDT
; -------------------------
gdt_start:
gdt_null:
    dq 0x0000000000000000
gdt_code:
    dq 0x00CF9A000000FFFF
gdt_data:
    dq 0x00CF92000000FFFF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; -------------------------
; Enter Protected Mode
; -------------------------
switch_to_pm:
    cli
    lgdt [gdt_descriptor]

    in al, 0x92
    or al, 2
    out 0x92, al           ; enable A20

    mov eax, cr0
    or eax, 1
    mov cr0, eax           ; set PE bit

    jmp 0x08:protected_mode_start

; -------------------------
; 32-bit protected mode stub
; -------------------------
[bits 32]
protected_mode_start:
    mov ax, 0x10           ; data selector
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov esp, 0x90000       ; stack

    ; jump to kernel at 0x1000
    mov eax, KERNEL_OFFSET
    jmp eax

BOOT_DRIVE db 0

times 510-($-$$) db 0
dw 0xAA55
