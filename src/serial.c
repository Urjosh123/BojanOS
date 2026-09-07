#include "serial.h"
#include "io.h"
#include <stdarg.h>
#include <stdint.h>

#define COM1 0x3F8

static int tx_ready(void) {
    return inb(COM1 + 5) & 0x20;
}

void serial_putchar(char c) {
    if (c == '\n') {
        while (!tx_ready()) __asm__ volatile("rep; nop");
        outb(COM1, '\r');
    }
    while (!tx_ready()) __asm__ volatile("rep; nop");
    outb(COM1, (uint8_t)c);
}

void serial_print(const char* s) {
    while (*s) serial_putchar(*s++);
}

static void sdec(uint64_t v) {
    char b[24];
    int i = 0;
    if (!v) b[i++] = '0';
    while (v) { b[i++] = '0' + v % 10; v /= 10; }
    while (i--) serial_putchar(b[i]);
}

void serial_printf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt != '%') { serial_putchar(*fmt++); continue; }
        fmt++;
        int lf = 0;
        if (*fmt == 'l') { lf++; fmt++; if (*fmt == 'l') { lf++; fmt++; } }
        char c = *fmt++;
        switch (c) {
        case 's': { const char* s = va_arg(ap, const char*); serial_print(s ? s : "(null)"); break; }
        case 'c': serial_putchar((char)va_arg(ap, int)); break;
        case 'd': case 'i': {
            int64_t v = lf == 2 ? va_arg(ap, long long) : lf == 1 ? va_arg(ap, long) : va_arg(ap, int);
            if (v < 0) { serial_putchar('-'); v = -v; }
            sdec((uint64_t)v);
            break;
        }
        case 'u': {
            uint64_t v = lf == 2 ? va_arg(ap, unsigned long long) : lf == 1 ? va_arg(ap, unsigned long) : va_arg(ap, unsigned int);
            sdec(v);
            break;
        }
        case 'x': {
            uint64_t v = lf == 2 ? va_arg(ap, unsigned long long) : lf == 1 ? va_arg(ap, unsigned long) : va_arg(ap, unsigned int);
            const char* d = "0123456789abcdef";
            char b[17]; int i = 0;
            if (!v) b[i++] = '0';
            while (v) { b[i++] = d[v & 0xF]; v >>= 4; }
            serial_print("0x");
            while (i--) serial_putchar(b[i]);
            break;
        }
        case '%': serial_putchar('%'); break;
        default: serial_putchar('%'); serial_putchar(c); break;
        }
    }
    va_end(ap);
}

void serial_init(void) {
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
}
