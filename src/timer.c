#include "timer.h"
#include "io.h"
#include <stdint.h>

static volatile uint64_t ticks;

void timer_init(uint32_t hz) {
    ticks = 0;
    uint32_t div = 1193182 / hz;
    outb(0x43, 0x36);
    outb(0x40, div & 0xFF);
    outb(0x40, (div >> 8) & 0xFF);
}

void timer_tick(void) {
    ticks++;
}

uint64_t timer_ticks(void) { return ticks; }
uint64_t timer_uptime_ms(void) { return ticks * 10; }

void sleep_ms(uint64_t ms) {
    uint64_t target = timer_uptime_ms() + ms;
    sti();
    while (timer_uptime_ms() < target) {
        __asm__ volatile("hlt");
    }
}
