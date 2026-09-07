#include "multiboot.h"
#include "vga.h"
#include "serial.h"
#include "kheap.h"
#include "vfs.h"
#include "isr.h"
#include "pic.h"
#include "timer.h"
#include "keyboard.h"
#include "shell.h"
#include "rtc.h"
#include "string.h"
#include "io.h"
#include "gdt.h"

extern uint32_t boot_magic;
extern uint32_t boot_info;

static void banner(void) {
    vga_setcolor(0x0E, 0x00);
    vga_print("\n  BojanOS 1.0  -  64-bit from-scratch operating system\n");
    vga_setcolor(0x07, 0x00);
    vga_print("  bootloader: ");
    if (boot_magic == BOOT_MAGIC_BONJ) vga_print("BojanOS CD bootloader (El Torito)\n");
    else if (boot_magic == 0x2BADB002) vga_print("Multiboot1\n");
    else if (boot_magic == 0x36D76289) vga_print("Multiboot2\n");
    else vga_print("unknown\n");
    vga_printf("  CPU: %s   RAM: %u KB\n\n", cpu_vendor, total_mem_kb);
    serial_printf("BojanOS booted: magic=0x%x cpu=%s mem=%uKB\n", boot_magic, cpu_vendor, total_mem_kb);
}

static void tlog(const char* s) {
    serial_print(s);
    vga_print(s);
}

static int selftest(void) {
    int pass = 0, fail = 0;
    #define CHECK(cond, msg) do { \
        if (cond) { pass++; tlog("  [PASS] "); tlog(msg); tlog("\n"); } \
        else { fail++; tlog("  [FAIL] "); tlog(msg); tlog("\n"); } } while (0)

    tlog("== system self test ==\n");
    CHECK(strcmp("abc", "abc") == 0, "strcmp equal");
    CHECK(strcmp("abc", "abd") < 0, "strcmp order");
    CHECK(strlen("hello") == 5, "strlen");

    CHECK(vfs_mkdir("/tmp/testdir") == 1, "mkdir");
    CHECK(vfs_resolve("/tmp/testdir") != 0, "resolve dir");
    CHECK(vfs_mkfile("/tmp/testdir/a.txt") == 1, "mkfile");
    vnode_t* f = vfs_resolve("/tmp/testdir/a.txt");
    CHECK(f != 0, "resolve file");
    vfs_write_file(f, "line1\nline2\nline3\n", 18);
    CHECK(f->size == 18, "write size");

    char line[1024];
    char* lp;
    int rc;

    rc = shell_exec_line(lp = strcpy(line, "echo hello > /tmp/echo.txt"));
    (void)lp;
    vnode_t* e = vfs_resolve("/tmp/echo.txt");
    CHECK(e && e->size == 6 && memcmp(e->data, "hello\n", 6) == 0, "redirect write");

    shell_exec_line(strcpy(line, "cat /tmp/testdir/a.txt | wc -l > /tmp/wc.txt"));
    vnode_t* w = vfs_resolve("/tmp/wc.txt");
    CHECK(w && strncmp(w->data, "3", 1) == 0, "pipe cat|wc");

    shell_exec_line(strcpy(line, "echo -e '' >/dev/null; seq 1 5 > /tmp/seq.txt"));
    vnode_t* sq = vfs_resolve("/tmp/seq.txt");
    CHECK(sq && sq->size == 10, "seq output");

    shell_exec_line(strcpy(line, "sort /tmp/seq.txt -r > /tmp/sort.txt"));
    vnode_t* so = vfs_resolve("/tmp/sort.txt");
    CHECK(so && strncmp(so->data, "5", 1) == 0, "sort -r");

    CHECK(vfs_rm("/tmp/echo.txt") == 1, "rm file");
    CHECK(vfs_resolve("/tmp/echo.txt") == 0, "file gone");
    CHECK(vfs_rm("/tmp/testdir") == 0, "rmdir non-empty fails");

    rtc_time_t rt;
    rtc_read(&rt);
    CHECK(rt.year >= 2024, "rtc year");
    CHECK(total_mem_kb >= 16384, "memory detected");

    shell_exec_line(strcpy(line, "false"));
    CHECK(sh_last_exit() == 1, "false exit code");
    shell_exec_line(strcpy(line, "true"));
    CHECK(sh_last_exit() == 0, "true exit code");

    tlog("== self test: ");
    {
        char b[32];
        int i = 0;
        int p2 = pass, f2 = fail;
        if (!p2) b[i++] = '0'; else { char t[16]; int k = 0; while (p2) { t[k++] = '0' + p2 % 10; p2 /= 10; } while (k--) b[i++] = t[k]; }
        b[i++] = ' '; b[i++] = 'p'; b[i++] = 'a'; b[i++] = 's'; b[i++] = 's'; b[i++] = 'e'; b[i++] = 'd'; b[i++] = ','; b[i++] = ' ';
        if (!f2) b[i++] = '0'; else { char t[16]; int k = 0; while (f2) { t[k++] = '0' + f2 % 10; f2 /= 10; } while (k--) b[i++] = t[k]; }
        b[i++] = ' '; b[i++] = 'f'; b[i++] = 'a'; b[i++] = 'i'; b[i++] = 'l';
        b[i] = 0;
        tlog(b);
    }
    tlog(" ==\n");
    serial_printf("SELFTEST_RESULT %s\n", fail ? "FAIL" : "OK");
    return fail;
}

void kmain(uint64_t magic, uint64_t info) {
    boot_magic = (uint32_t)magic;
    boot_info = (uint32_t)info;

    serial_init();
    vga_init();
    boot_parse();
    kheap_init();
    vfs_init();

    gdt_install();
    idt_init();
    pic_init();
    timer_init(100);
    keyboard_init();
    pic_unmask(0);
    pic_unmask(1);
    sti();

    banner();

    const char* cmdline = 0;
    if (boot_magic == 0x2BADB002) {
        uint32_t* mb = (uint32_t*)(uintptr_t)boot_info;
        if (mb[0] & (1 << 2)) cmdline = (const char*)(uintptr_t)mb[8];
    } else if (boot_magic == 0x36D76289) {
        uint8_t* p = (uint8_t*)(uintptr_t)boot_info + 8;
        uint32_t total = *(uint32_t*)((uint8_t*)(uintptr_t)boot_info + 4);
        uint8_t* end = (uint8_t*)(uintptr_t)boot_info + total;
        while (p < end) {
            uint32_t type = *(uint32_t*)p;
            uint32_t size = *(uint32_t*)(p + 4);
            if (size == 0) break;
            if (type == 1) cmdline = (const char*)(p + 8);
            p += size;
        }
    }

    if (cmdline && strstr(cmdline, "selftest")) {
        int f = selftest();
        serial_printf("SELFTEST_RESULT %s\n", f ? "FAIL" : "OK");
        cli();
        while (1) hlt();
    }

    shell_init();
    shell_run();

    cli();
    while (1) hlt();
}
