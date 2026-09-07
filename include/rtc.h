#ifndef RTC_H
#define RTC_H
#include <stdint.h>
typedef struct {
    uint16_t year;
    uint8_t month, day, hour, minute, second;
} rtc_time_t;
void rtc_read(rtc_time_t* t);
uint64_t rtc_boot_ms(void);
#endif
