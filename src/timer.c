#include "timer.h"

volatile uint32_t timer_ticks = 0;

void timer_install(uint32_t frequency) {
    (void)frequency;
}