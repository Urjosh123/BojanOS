#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>
void timer_init(uint32_t hz);
void timer_tick(void);
uint64_t timer_ticks(void);
uint64_t timer_uptime_ms(void);
void sleep_ms(uint64_t ms);
#endif
