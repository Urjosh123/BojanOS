#include <stdint.h>
#include <stddef.h>

struct gdt_entry {
    uint16_t limit_lo;
    uint16_t base_lo;
    uint8_t base_mid;
    uint8_t access;
    uint8_t flags_limit_hi;
    uint8_t base_hi;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct gdt_entry gdt[5];

static void set_entry(int i, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[i].base_lo = base & 0xFFFF;
    gdt[i].base_mid = (base >> 16) & 0xFF;
    gdt[i].base_hi = (base >> 24) & 0xFF;
    gdt[i].limit_lo = limit & 0xFFFF;
    gdt[i].flags_limit_hi = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[i].access = access;
}

void gdt_flush(struct gdt_ptr* p);

void gdt_install(void) {
    set_entry(0, 0, 0, 0, 0);
    set_entry(1, 0, 0xFFFFF, 0x9A, 0xA0);
    set_entry(2, 0, 0xFFFFF, 0x92, 0xC0);
    set_entry(3, 0, 0xFFFFF, 0xFA, 0xA0);
    set_entry(4, 0, 0xFFFFF, 0xF2, 0xC0);
    struct gdt_ptr p;
    p.limit = sizeof(gdt) - 1;
    p.base = (uint64_t)&gdt;
    gdt_flush(&p);
}
