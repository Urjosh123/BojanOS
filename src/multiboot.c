#include "multiboot.h"
#include "io.h"
#include "string.h"

uint32_t boot_magic;
uint32_t boot_info;
uint32_t total_mem_kb = 32768;
char cpu_vendor[13] = "UnknownCPU";

void cpuid_vendor(char out[13]) {
    uint32_t a, b, c, d;
    __asm__ volatile("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(0));
    memcpy(out + 0, &b, 4);
    memcpy(out + 4, &d, 4);
    memcpy(out + 8, &c, 4);
    out[12] = 0;
}

static void parse_e820_cd(void) {
    uint16_t count = *(volatile uint16_t*)0x5000;
    uint64_t best = 0;
    uint8_t* p = (uint8_t*)0x5008;
    for (int i = 0; i < count && i < 64; i++) {
        uint64_t base = *(uint64_t*)(p + 0);
        uint64_t len = *(uint64_t*)(p + 8);
        uint32_t type = *(uint32_t*)(p + 16);
        if (type == 1 && base < 0x100000000ULL) {
            uint64_t top = base + len;
            if (top > best) best = top;
        }
        p += 24;
    }
    if (best > 16 * 1024 * 1024) total_mem_kb = (uint32_t)(best / 1024);
}

static void parse_mb1(void) {
    uint32_t* mb = (uint32_t*)(uintptr_t)boot_info;
    uint32_t flags = mb[0];
    if (flags & (1 << 0)) {
        uint32_t lower = mb[1];
        uint32_t upper = mb[2];
        total_mem_kb = lower + upper;
    }
    if (flags & (1 << 6)) {
        uint32_t mmap_len = mb[22];
        uint32_t mmap_addr = mb[23];
        uint64_t best = 0;
        uint32_t off = 0;
        while (off < mmap_len) {
            uint32_t* e = (uint32_t*)(uintptr_t)(mmap_addr + off);
            uint32_t esize = e[0];
            uint64_t base = *(uint64_t*)&e[3];
            uint64_t len = *(uint64_t*)&e[5];
            uint32_t type = e[7];
            if (type == 1 && base < 0x100000000ULL) {
                uint64_t top = base + len;
                if (top > best) best = top;
            }
            off += esize + 4;
        }
        if (best > 16 * 1024 * 1024) total_mem_kb = (uint32_t)(best / 1024);
    }
}

static void parse_mb2(void) {
    uint8_t* p = (uint8_t*)(uintptr_t)boot_info + 8;
    uint32_t total = *(uint32_t*)((uint8_t*)(uintptr_t)boot_info + 4);
    uint8_t* end = (uint8_t*)(uintptr_t)boot_info + total;
    while (p < end) {
        uint32_t type = *(uint32_t*)p;
        uint32_t size = *(uint32_t*)(p + 4);
        if (size == 0) break;
        if (type == 6) {
            uint32_t esize = *(uint32_t*)(p + 8);
            uint32_t elen = *(uint32_t*)(p + 12);
            uint8_t* e = p + 16;
            uint64_t best = 0;
            uint32_t off = 0;
            while (off + esize <= elen) {
                uint64_t base = *(uint64_t*)(e + off + 0);
                uint64_t len = *(uint64_t*)(e + off + 8);
                uint32_t etype = *(uint32_t*)(e + off + 16);
                if (etype == 1 && base < 0x100000000ULL) {
                    uint64_t top = base + len;
                    if (top > best) best = top;
                }
                off += esize;
            }
            if (best > 16 * 1024 * 1024) total_mem_kb = (uint32_t)(best / 1024);
        }
        p += size;
    }
}

void boot_parse(void) {
    cpuid_vendor(cpu_vendor);
    if (boot_magic == BOOT_MAGIC_BONJ) {
        parse_e820_cd();
    } else if (boot_magic == 0x2BADB002) {
        parse_mb1();
    } else if (boot_magic == 0x36D76289) {
        parse_mb2();
    }
}

void reboot(void) {
    uint8_t good;
    do {
        good = inb(0x64);
    } while (good & 0x02);
    outb(0x64, 0xFE);
    while (1) hlt();
}

void poweroff(void) {
    cli();
    while (1) hlt();
}
