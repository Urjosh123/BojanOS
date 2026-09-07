#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stdint.h>
#define KB_KEY_UP    0x100
#define KB_KEY_DOWN  0x101
#define KB_KEY_LEFT  0x102
#define KB_KEY_RIGHT 0x103
#define KB_KEY_HOME  0x104
#define KB_KEY_END   0x105
#define KB_KEY_PGUP  0x106
#define KB_KEY_PGDN  0x107
#define KB_CTRL_D    0x108
#define KB_CTRL_C    0x109
void keyboard_init(void);
void keyboard_irq(void);
int keyboard_get(void);
#endif
