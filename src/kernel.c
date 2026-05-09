#include <stdint.h>
#include "vga.h"
#include "shell.h"

char cpu_vendor[13];
uint32_t total_mem_kb;

static void cpuid(uint32_t code, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
    uint32_t bb;
    __asm__ volatile(
        "cpuid"
        : "=a"(*a), "=b"(bb), "=c"(*c), "=d"(*d)
        : "a"(code)
    );
    *b = bb;
}

void kmain(void) {
    uint32_t eax, ebx, ecx, edx;
    cpuid(0, &eax, &ebx, &ecx, &edx);
    *(uint32_t*)cpu_vendor = ebx;
    *(uint32_t*)(cpu_vendor + 4) = edx;
    *(uint32_t*)(cpu_vendor + 8) = ecx;
    cpu_vendor[12] = 0;
    total_mem_kb = *(uint16_t*)0x5000;
    if (total_mem_kb < 64) total_mem_kb = 32768;
    total_mem_kb += 1024;
    vga_init();
    shell_run();
}