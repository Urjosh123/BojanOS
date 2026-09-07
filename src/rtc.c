#include "rtc.h"
#include "io.h"
#include "timer.h"

static uint8_t cmos_read(uint8_t reg) {
    outb(0x70, reg);
    return inb(0x71);
}

void rtc_read(rtc_time_t* t) {
    uint8_t sec, min, hour, day, mon, year, statusB;
    do {
        while (cmos_read(0x0A) & 0x80) ;
        sec = cmos_read(0x00);
        min = cmos_read(0x02);
        hour = cmos_read(0x04);
        day = cmos_read(0x07);
        mon = cmos_read(0x08);
        year = cmos_read(0x09);
        statusB = cmos_read(0x0B);
    } while (sec != cmos_read(0x00));

    if (!(statusB & 0x04)) {
        sec = (sec & 0x0F) + ((sec >> 4) & 0x07) * 10;
        min = (min & 0x0F) + ((min >> 4) & 0x07) * 10;
        day = (day & 0x0F) + ((day >> 4) & 0x03) * 10;
        mon = (mon & 0x0F) + ((mon >> 4) & 0x01) * 10;
        year = (year & 0x0F) + ((year >> 4) * 10);
        if (!(statusB & 0x02)) {
            hour = ((hour & 0x0F) + (((hour & 0x70) >> 4) * 10)) | (hour & 0x80);
        }
    }
    t->second = sec;
    t->minute = min;
    t->hour = hour;
    t->day = day;
    t->month = mon;
    t->year = 2000 + year;
}

uint64_t rtc_boot_ms(void) {
    return timer_uptime_ms();
}
