#include "idt.h"
#include "string.h"
struct idt_entry idt_entries[256];
struct idt_ptr idtp;

void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_mid = (base >> 16) & 0xFFFF;
    idt_entries[num].base_hi = (base >> 32) & 0xFFFFFFFF;
    idt_entries[num].sel = sel;
    idt_entries[num].ist = 0;
    idt_entries[num].flags = flags;
}
void idt_install(void) {
    idtp.limit = sizeof(struct idt_entry) * 256 - 1;
    idtp.base = (uint64_t)&idt_entries;
    memset(&idt_entries, 0, sizeof(struct idt_entry) * 256);
    __asm__ volatile("lidt %0" : : "m"(idtp));
}
