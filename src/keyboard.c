#include "keyboard.h"
#include "io.h"

#define BUFSZ 128
static int buf[BUFSZ];
static int buf_head, buf_tail;

static const unsigned char keymap[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'', '`',
    0, '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' ', 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static const unsigned char keymap_shift[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0, 'A','S','D','F','G','H','J','K','L',':','"', '~',
    0, '|','Z','X','C','V','B','N','M','<','>','?', 0,
    '*', 0, ' ', 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static int shift_down;
static int ctrl_down;

static void buf_push(int c) {
    int next = (buf_head + 1) % BUFSZ;
    if (next == buf_tail) return;
    buf[buf_head] = c;
    buf_head = next;
}

void keyboard_irq(void) {
    uint8_t sc = inb(0x60);
    uint8_t code = sc & 0x7F;
    int released = (sc & 0x80) != 0;

    if (code == 0x2A || code == 0x36) {
        shift_down = !released;
        return;
    }
    if (code == 0x1D) {
        ctrl_down = !released;
        return;
    }
    if (released) return;

    if (code == 0xE0) return;

    if (ctrl_down) {
        if (code == 0x20) { buf_push(KB_CTRL_D); return; }
        if (code == 0x2E) { buf_push(KB_CTRL_C); return; }
    }

    switch (code) {
    case 0x48: buf_push(KB_KEY_UP); return;
    case 0x50: buf_push(KB_KEY_DOWN); return;
    case 0x4B: buf_push(KB_KEY_LEFT); return;
    case 0x4D: buf_push(KB_KEY_RIGHT); return;
    case 0x47: buf_push(KB_KEY_HOME); return;
    case 0x4F: buf_push(KB_KEY_END); return;
    case 0x49: buf_push(KB_KEY_PGUP); return;
    case 0x51: buf_push(KB_KEY_PGDN); return;
    default: break;
    }

    unsigned char c = shift_down ? keymap_shift[code] : keymap[code];
    if (c) buf_push(c);
}

int keyboard_get(void) {
    while (buf_head == buf_tail) {
        __asm__ volatile("sti; hlt; cli");
    }
    int c = buf[buf_tail];
    buf_tail = (buf_tail + 1) % BUFSZ;
    return c;
}

void keyboard_init(void) {
    buf_head = buf_tail = 0;
    shift_down = ctrl_down = 0;
}
