#include "util.h"
#include "snake.h"

volatile unsigned char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
volatile int keyboard_buffer_head = 0;
volatile int keyboard_buffer_tail = 0;

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

struct idt_entry {
    unsigned short offset_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short offset_high;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;

void idt_set_gate(int n, unsigned int handler) {
    idt[n].offset_low  = handler & 0xFFFF;
    idt[n].selector    = 0x08;   // your code segment
    idt[n].zero        = 0;
    idt[n].type_attr   = 0x8E;   // interrupt gate
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

volatile unsigned long ticks = 0;

void timer_handler() {
    ticks++;
    outb(0x20, 0x20); // EOI to PIC
}

unsigned long get_ticks() {
    return ticks;
}

void pic_remap() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}

void pit_init() {
    unsigned short divisor = 1193; // 1000 Hz

    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}

void handle_input(char scancode) {
    dir_t new_dir = current_direction; // default: no change

    switch (scancode) {
        case 0x11:  // W
            draw_char(78, 3, 'w', 0x0F);
            new_dir = DIR_UP;
            break;
        case 0x1F:  // S
            draw_char(78, 3, 's', 0x0F);
            new_dir = DIR_DOWN;
            break;
        case 0x1E:  // A
            draw_char(78, 3, 'a', 0x0F);
            new_dir = DIR_LEFT;
            break;
        case 0x20:  // D
            draw_char(78, 3, 'd', 0x0F);
            new_dir = DIR_RIGHT;
            break;
    }
    
    current_direction = new_dir;
}

void keyboard_handler() {
    unsigned char scancode = inb(0x60);  // Read scancode

    // Only handle key presses (ignore releases)
    if (scancode & 0x80) {
        outb(0x20, 0x20); // send EOI
        return;
    }
    
    handle_input(scancode);

    // Send EOI to PIC
    outb(0x20, 0x20);
}




unsigned char keyboard_get_scancode_irq() {
    if (keyboard_buffer_head == keyboard_buffer_tail) {
        return 0;  // Buffer empty
    }
    
    unsigned char scancode = keyboard_buffer[keyboard_buffer_tail];
    keyboard_buffer_tail = (keyboard_buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return scancode;
}

// Set up keyboard interrupt
void keyboard_init() {
    unsigned char mask = inb(0x21);
    mask &= ~0x02;  // Clear bit 1 (enable IRQ1)
    outb(0x21, mask);
    
    idt_set_gate(33, (unsigned int)irq1_stub);
}


void idt_init() {
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (unsigned int)&idt;

    idt_set_gate(32, (unsigned int)irq0_stub);
    idt_set_gate(33, (unsigned int)irq1_stub);  // Keyboard

    __asm__ volatile ("lidt %0" : : "m"(idtp));
}

void hide_cursor() {
    
    outb(0x3D4, 0x0A);      // Select cursor start register
    outb(0x3D5, 0x20);      // Set bit 5 to disable cursor
    
    outb(0x3D4, 0x0E);      // Cursor location high register
    outb(0x3D5, 0x0F);      // High byte = 15 (row 25 * 80 / 256)
    
    outb(0x3D4, 0x0F);      // Cursor location low register
    outb(0x3D5, 0xFF);      // Low byte = 255 (far off-screen)
}

void kernel_init() {
    idt_init();
    pic_remap();
    pit_init();
    SET_INTERUPT;

    clear_screen();
    hide_cursor();
}

void sleep_ms(unsigned long ms) {
    unsigned long target = ticks + ms;
    while (ticks < target) {
        __asm__ volatile("hlt"); 
    }
}

static unsigned int rng_seed = 12345678;

void init_entropy(void) {
    rng_seed = get_ticks() 
        ^ (unsigned long)&rng_seed;
}

// https://en.wikipedia.org/wiki/Xorshift
unsigned int rand(void) {
    unsigned int x = rng_seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng_seed = x;
    return x;
}

unsigned int rand_range(unsigned int max) {
    return rand() % max;  
}

unsigned int rand_between(size_t min, size_t max) {
    if (max <= min) return min;

    return min + rand_range(max - min + 1);
}

size_t strlen(const char* str) {
    const char* end = str;

    while (*end) end++;
    return end - str;
}

void itoa(unsigned int num, char* str) {
    int i = 0;
    
    // Special case for 0
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    
    while (num > 0) {
        str[i++] = '0' + (num % 10);  // Get last digit
        num /= 10;                     // Remove last digit
    }
    str[i] = '\0';  // Null terminate
    
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}



void clear_screen() {
    volatile unsigned short* vga = (volatile unsigned short*)0xB8000;
    
    unsigned short blank = (0x0F << 8) | ' '; // Black background, White text, Space char

    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = blank;
    }
}

void draw_char(unsigned short x, unsigned short y, char c, char colour) {
    volatile unsigned short* vga = (volatile unsigned short*)0xB8000;
    int index = y * 80 + x;
    vga[index] = (colour << 8) | (unsigned short)c;
}

void draw_string(unsigned short x, unsigned short y, const char* str, char colour) {
    size_t len = strlen(str);
    
    for (size_t i = 0; i < len; i++) {
        draw_char(x + i, y, str[i], colour);
    }
}
