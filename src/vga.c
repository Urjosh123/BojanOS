#include "vga.h"
#include "io.h"
static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;
static size_t row;
static size_t col;
static uint8_t color;
static inline uint8_t vga_color(uint8_t fg, uint8_t bg) {
    return fg | bg << 4;
}
static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t)c | (uint16_t)color << 8;
}
void vga_init(void) {
    row = 0;
    col = 0;
    color = vga_color(7, 0);
    vga_clear();
}
void vga_setcolor(uint8_t c) {
    color = c;
}
void vga_setcursor(size_t x, size_t y) {
    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x3D4, 14);
    outb(0x3D5, (pos >> 8) & 0xFF);
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0xFF);
}
void vga_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            VGA_MEMORY[y * VGA_WIDTH + x] = vga_entry(' ', color);
    row = 0;
    col = 0;
    vga_setcursor(0, 0);
}
void vga_scroll(void) {
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            VGA_MEMORY[y * VGA_WIDTH + x] = VGA_MEMORY[(y + 1) * VGA_WIDTH + x];
    for (size_t x = 0; x < VGA_WIDTH; x++)
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', color);
    row = VGA_HEIGHT - 1;
    col = 0;
}
void vga_backspace(void) {
    if (col > 0) {
        col--;
        VGA_MEMORY[row * VGA_WIDTH + col] = vga_entry(' ', color);
        vga_setcursor(col, row);
    }
}
void vga_putchar(char c) {
    if (c == '\n') {
        col = 0;
        row++;
    } else if (c == '\t') {
        size_t next = (col + 4) & ~3;
        while (col < next && col < VGA_WIDTH) {
            VGA_MEMORY[row * VGA_WIDTH + col] = vga_entry(' ', color);
            col++;
        }
    } else {
        VGA_MEMORY[row * VGA_WIDTH + col] = vga_entry((unsigned char)c, color);
        col++;
    }
    if (col >= VGA_WIDTH) {
        col = 0;
        row++;
    }
    if (row >= VGA_HEIGHT)
        vga_scroll();
    vga_setcursor(col, row);
}
void vga_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++)
        vga_putchar(data[i]);
}
void vga_print(const char* data) {
    size_t len = 0;
    while (data[len]) len++;
    vga_write(data, len);
}