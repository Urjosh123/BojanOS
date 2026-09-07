#ifndef MULTIBOOT_H
#define MULTIBOOT_H
#include <stdint.h>
typedef struct {
    uint64_t base;
    uint64_t len;
    uint32_t type;
} e820_entry_t;

#define BOOT_MAGIC_BONJ 0x4A4E4F42
extern uint32_t boot_magic;
extern uint32_t boot_info;
extern uint32_t total_mem_kb;
extern char cpu_vendor[13];
void boot_parse(void);
void cpuid_vendor(char out[13]);
void reboot(void);
void poweroff(void);
#endif
