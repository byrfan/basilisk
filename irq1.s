; IRQ1 keyboard interrupt stub
global irq1_stub
extern keyboard_handler

irq1_stub:
    pusha           ; Save all registers
    push ds
    push es
    push fs
    push gs
    
    call keyboard_handler   ; Call C handler
    
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iret            ; Return from interrupt
