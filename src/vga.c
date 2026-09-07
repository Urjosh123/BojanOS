#include "vga.h"
#include "io.h"
#include <stdint.h>
#include <stdarg.h>

static volatile uint16_t* const VGA = (uint16_t*)0xB8000;
static size_t row, col;
static uint8_t attr;

static uint16_t cell(unsigned char c) {
    return (uint16_t)c | ((uint16_t)attr << 8);
}

static void move_cursor(void) {
    uint16_t pos = row * VGA_WIDTH + col;
    outb(0x3D4, 14);
    outb(0x3D5, (pos >> 8) & 0xFF);
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0xFF);
}

void vga_clear(void) {
    for (int y = 0; y < VGA_HEIGHT; y++)
        for (int x = 0; x < VGA_WIDTH; x++)
            VGA[y * VGA_WIDTH + x] = cell(' ');
    row = 0;
    col = 0;
    move_cursor();
}

void vga_setcolor(uint8_t fg, uint8_t bg) {
    attr = (bg << 4) | (fg & 0x0F);
}

static void scroll(void) {
    for (int y = 1; y < VGA_HEIGHT; y++)
        for (int x = 0; x < VGA_WIDTH; x++)
            VGA[(y - 1) * VGA_WIDTH + x] = VGA[y * VGA_WIDTH + x];
    for (int x = 0; x < VGA_WIDTH; x++)
        VGA[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = cell(' ');
    row = VGA_HEIGHT - 1;
}

void vga_putchar(char c) {
    if (c == '\n') {
        col = 0;
        if (++row >= VGA_HEIGHT) scroll();
    } else if (c == '\t') {
        size_t n = 4 - (col % 4);
        while (n-- && col < VGA_WIDTH) {
            VGA[row * VGA_WIDTH + col] = cell(' ');
            col++;
        }
        if (col >= VGA_WIDTH) { col = 0; if (++row >= VGA_HEIGHT) scroll(); }
    } else if (c == '\r') {
        col = 0;
    } else if (c == '\b') {
        if (col > 0) {
            col--;
            VGA[row * VGA_WIDTH + col] = cell(' ');
        }
    } else {
        VGA[row * VGA_WIDTH + col] = cell((unsigned char)c);
        if (++col >= VGA_WIDTH) {
            col = 0;
            if (++row >= VGA_HEIGHT) scroll();
        }
    }
    move_cursor();
}

void vga_write(const char* s, size_t n) {
    for (size_t i = 0; i < n; i++) vga_putchar(s[i]);
}

void vga_print(const char* s) {
    while (*s) vga_putchar(*s++);
}

static void print_dec(uint64_t v) {
    char buf[24];
    int i = 0;
    if (v == 0) buf[i++] = '0';
    while (v) { buf[i++] = '0' + (v % 10); v /= 10; }
    while (i--) vga_putchar(buf[i]);
}

static void print_hex(uint64_t v, int width) {
    char buf[20];
    int i = 0;
    const char* d = "0123456789abcdef";
    if (v == 0) buf[i++] = '0';
    while (v) { buf[i++] = d[v & 0xF]; v >>= 4; }
    while (i < width) buf[i++] = '0';
    vga_putchar('0'); vga_putchar('x');
    while (i--) vga_putchar(buf[i]);
}

void vga_printf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt != '%') { vga_putchar(*fmt++); continue; }
        fmt++;
        int lflag = 0;
        if (*fmt == 'l') { lflag++; fmt++; if (*fmt == 'l') { lflag++; fmt++; } }
        char c = *fmt++;
        switch (c) {
        case 's': {
            const char* s = va_arg(ap, const char*);
            vga_print(s ? s : "(null)");
            break;
        }
        case 'c':
            vga_putchar((char)va_arg(ap, int));
            break;
        case 'd': case 'i': {
            int64_t v;
            if (lflag == 2) v = va_arg(ap, long long);
            else if (lflag == 1) v = va_arg(ap, long);
            else v = va_arg(ap, int);
            if (v < 0) { vga_putchar('-'); v = -v; }
            print_dec((uint64_t)v);
            break;
        }
        case 'u': case 'x': {
            uint64_t v;
            if (lflag == 2) v = va_arg(ap, unsigned long long);
            else if (lflag == 1) v = va_arg(ap, unsigned long);
            else v = va_arg(ap, unsigned int);
            if (c == 'x') print_hex(v, 0);
            else print_dec(v);
            break;
        }
        case '%':
            vga_putchar('%');
            break;
        default:
            vga_putchar('%');
            vga_putchar(c);
            break;
        }
    }
    va_end(ap);
}

void vga_init(void) {
    vga_setcolor(0x0A, 0x00);
    vga_clear();
}
