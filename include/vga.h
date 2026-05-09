#ifndef VGA_H
#define VGA_H
#include <stdint.h>
#include <stddef.h>
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
void vga_init(void);
void vga_setcolor(uint8_t color);
void vga_putchar(char c);
void vga_write(const char* data, size_t size);
void vga_print(const char* data);
void vga_clear(void);
void vga_setcursor(size_t x, size_t y);
void vga_scroll(void);
void vga_backspace(void);
#endif
