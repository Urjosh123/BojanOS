#ifndef VGA_H
#define VGA_H
#include <stddef.h>
#include <stdint.h>
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
void vga_init(void);
void vga_clear(void);
void vga_putchar(char c);
void vga_write(const char* s, size_t n);
void vga_print(const char* s);
void vga_setcolor(uint8_t fg, uint8_t bg);
void vga_printf(const char* fmt, ...);
#endif
