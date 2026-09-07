#include "isr.h"
#include "pic.h"
#include "io.h"
#include "vga.h"
#include "serial.h"
#include "string.h"
#include "timer.h"
#include "keyboard.h"

struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t ist;
    uint8_t flags;
    uint16_t base_mid;
    uint32_t base_hi;
    uint32_t zero;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[256];

static void set_gate(int n, void* handler) {
    uint64_t addr = (uint64_t)handler;
    idt[n].base_lo = addr & 0xFFFF;
    idt[n].sel = 0x08;
    idt[n].ist = 0;
    idt[n].flags = 0x8E;
    idt[n].base_mid = (addr >> 16) & 0xFFFF;
    idt[n].base_hi = (addr >> 32) & 0xFFFFFFFF;
    idt[n].zero = 0;
}

void idt_load(struct idt_ptr* p);

void idt_init(void) {
    memset(idt, 0, sizeof(idt));
    void* stubs[48];
    extern void isr0(void);
    stubs[0] = isr0;
#define S(N) extern void isr##N(void); stubs[N] = isr##N
    S(1); S(2); S(3); S(4); S(5); S(6); S(7); S(8); S(9);
    S(10); S(11); S(12); S(13); S(14); S(15); S(16); S(17); S(18); S(19);
    S(20); S(21); S(22); S(23); S(24); S(25); S(26); S(27); S(28); S(29);
    S(30); S(31); S(32); S(33); S(34); S(35); S(36); S(37); S(38); S(39);
    S(40); S(41); S(42); S(43); S(44); S(45); S(46); S(47);
#undef S
    for (int i = 0; i < 48; i++) set_gate(i, stubs[i]);
    struct idt_ptr p;
    p.limit = sizeof(idt) - 1;
    p.base = (uint64_t)idt;
    idt_load(&p);
}

static const char* exc_name(uint64_t v) {
    static const char* names[] = {
        "Divide by zero", "Debug", "NMI", "Breakpoint", "Overflow",
        "Bound range", "Invalid opcode", "Device not available",
        "Double fault", "Coprocessor overrun", "Invalid TSS",
        "Segment not present", "Stack fault", "General protection fault",
        "Page fault", "Reserved", "x87 FP exception", "Alignment check",
        "Machine check", "SIMD FP exception"
    };
    if (v < 20) return names[v];
    return "Exception";
}

void isr_handler(regs_t* r) {
    uint64_t v = r->vec;
    if (v == 32) {
        timer_tick();
        pic_eoi(0);
    } else if (v == 33) {
        keyboard_irq();
        pic_eoi(1);
    } else if (v >= 32 && v <= 47) {
        pic_eoi((int)(v - 32));
    } else if (v < 32) {
        serial_printf("\n*** %s (vec %u, err %u, rip 0x%x) ***\n",
                      exc_name(v), (unsigned)v, (unsigned)r->err, r->rip);
        vga_setcolor(0x04, 0x00);
        vga_printf("\nKernel exception: %s\nSystem halted.\n", exc_name(v));
        cli();
        while (1) hlt();
    }
}
