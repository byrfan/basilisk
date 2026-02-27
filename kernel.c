void delay() {
    for (volatile int i=0; i<1000000; i++);
}

void clear_screen() {
    volatile unsigned short* vga = (volatile unsigned short*)0xB8000;
    
    // 80 columns * 25 rows = 2000 total characters
    unsigned short blank = (0x0F << 8) | ' '; // Black background, White text, Space char

    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = blank;
    }
}

void run_game() {
    clear_screen();
    volatile unsigned short* vga = (volatile unsigned short*)0xB8000;
    int pos = 0;

    while(1) {
        vga[pos] = (0x0E << 8) | '*'; // yellow *
        delay();
        vga[pos] = (0x00 << 8) | ' '; // erase
        pos = (pos + 1) % 80;
    }
}
