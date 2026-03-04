#ifndef UTIL_HEADER
#define UTIL_HEADER

typedef unsigned int size_t;
typedef unsigned char bool_t;
#define true 1
#define false 0

#define KEYBOARD_BUFFER_SIZE 256
#define SET_INTERUPT  __asm__ volatile ("sti")
extern void irq0_stub();
extern void irq1_stub();

static inline void outb(unsigned short port, unsigned char val);
static inline unsigned char inb(unsigned short port);
void keyboard_init();
void sleep_ms(unsigned long ms);
size_t strlen(const char* str);
void itoa(unsigned int num, char* str);
void kernel_init();
void clear_screen();
void draw_string(unsigned short x, unsigned short y, const char* str, char colour);
void draw_char(unsigned short x, unsigned short y, char c, char colour);
unsigned long get_ticks();
void init_entropy();
unsigned int rand_range(size_t max);
unsigned int rand_between(size_t min, size_t max);
#endif
