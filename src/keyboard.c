#include "keyboard.h"
#include "io.h"

static const char scancode_table[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

char keyboard_read(void) {
    while (1) {
        uint8_t status = inb(0x64);
        if ((status & 1) == 0 || (status & 0x20)) {
            __asm__ volatile("rep; nop");
            continue;
        }
        uint8_t scancode = inb(0x60);
        if (scancode < 128) {
            char c = scancode_table[scancode];
            if (c) return c;
        }
    }
}

void keyboard_install(void) {
}

int keyboard_available(void) {
    uint8_t status = inb(0x64);
    return (status & 1) && !(status & 0x20);
}