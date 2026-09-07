#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#include <stddef.h>
void* memset(void*, int, size_t);
void* memcpy(void*, const void*, size_t);
int memcmp(const void*, const void*, size_t);
size_t strlen(const char*);
char* strcpy(char*, const char*);
char* strncpy(char*, const char*, size_t);
char* strcat(char*, const char*);
int strcmp(const char*, const char*);
int strncmp(const char*, const char*, size_t);
char* strstr(const char*, const char*);
char* strchr(const char*, int);
char* strrchr(const char*, int);


#include "kheap.h"
#include "vfs.h"
#include "shell.h"
#include "multiboot.h"
#include "keyboard.h"
#include "rtc.h"

uint32_t total_mem_kb = 65536;
char cpu_vendor[13] = "TestCPU";



static char screen[1 << 16];
static size_t screen_len;

static char feed[1 << 16];
static size_t feed_pos, feed_len;
static int eof_input;

int keyboard_get(void) {
    if (feed_pos < feed_len) return (unsigned char)feed[feed_pos++];
    if (eof_input) return KB_CTRL_D;
    return KB_CTRL_D;
}

void vga_init(void) {}
void vga_clear(void) {}
void vga_setcolor(uint8_t a, uint8_t b) { (void)a; (void)b; }
void vga_putchar(char c) {
    if (screen_len < sizeof(screen) - 1) screen[screen_len++] = c;
}
void vga_write(const char* s, size_t n) {
    for (size_t i = 0; i < n && screen_len < sizeof(screen) - 1; i++)
        screen[screen_len++] = s[i];
}
void vga_print(const char* s) { while (*s) vga_putchar(*s++); }
void vga_printf(const char* fmt, ...) {
    va_list ap; va_start(ap, fmt);
    char b[2048]; vsnprintf(b, sizeof b, fmt, ap);
    vga_print(b);
    va_end(ap);
}
void serial_init(void) {}
void serial_putchar(char c) { (void)c; }
void serial_print(const char* s) { (void)s; }
void serial_printf(const char* fmt, ...) { (void)fmt; }
void idt_init(void) {}
void pic_init(void) {}
void pic_eoi(int i) { (void)i; }
void pic_unmask(int i) { (void)i; }
void pic_mask(int i) { (void)i; }
void timer_init(uint32_t h) { (void)h; }
void gdt_install(void) {}
void keyboard_init(void) {}
void keyboard_irq(void) {}
uint64_t timer_ticks(void) { return 0; }
uint64_t timer_uptime_ms(void) { return 0; }
void sleep_ms(uint64_t ms) { (void)ms; }
void rtc_read(rtc_time_t* t) {
    t->year = 2026; t->month = 9; t->day = 7; t->hour = 12; t->minute = 0; t->second = 0;
}
uint64_t rtc_boot_ms(void) { return 0; }
void reboot(void) {}
void poweroff(void) {}
void cpuid_vendor(char out[13]) { strcpy(out, "TestCPU"); }


static int failures;
#define CHECK(cond, ...) do { \
    if (cond) { printf("  [PASS] "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } \
    else { printf("  [FAIL] "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); failures++; } } while (0)

static void run(const char* cmd) {
    screen_len = 0;
    char line[2048];
    strncpy(line, cmd, sizeof line - 1);
    line[sizeof line - 1] = 0;
    shell_exec_line(line);
}

static void feed_file(const char* s) {
    strcpy(feed, s);
    feed_len = strlen(s);
    feed_pos = 0;
    eof_input = 1;
}

static int screen_has(const char* s) {
    screen[screen_len] = 0;
    return strstr(screen, s) != 0;
}

int main(void) {
    printf("== BojanOS host test harness ==\n");
    total_mem_kb = 65536;
    kheap_init();
    vfs_init();
    shell_init();

    CHECK(vfs_resolve("/") != 0, "root exists");
    CHECK(vfs_resolve("/bin") && vfs_resolve("/etc") && vfs_resolve("/root"), "initial dirs");
    CHECK(vfs_resolve("/etc/os-release") != 0, "os-release file");

    CHECK(vfs_mkdir("/home/user/docs") == 1, "mkdir nested");
    CHECK(vfs_mkdir("/home/user/docs") == 0, "mkdir dup fails");
    CHECK(vfs_resolve("/home/user/docs") != 0, "resolve nested");

    vnode_t* f = vfs_resolve("/home/user/docs/a.txt");
    CHECK(f == 0, "file absent before create");
    CHECK(vfs_mkfile("/home/user/docs/a.txt") == 1, "mkfile");
    f = vfs_resolve("/home/user/docs/a.txt");
    CHECK(f && f->size == 0, "file exists empty");
    vfs_write_file(f, "banana\napple\ncherry\n", 20);
    CHECK(f->size == 20, "write 20 bytes");
    vfs_append_file(f, "date\n", 5);
    CHECK(f->size == 25, "append 5 bytes");

    run("echo hello world");
    CHECK(screen_has("hello world"), "echo");

    run("echo -n nonewline");
    CHECK(screen_has("nonewline") && !screen_has("nonewline\n"), "echo -n");

    run("echo hello > /tmp/f1.txt");
    vnode_t* f1 = vfs_resolve("/tmp/f1.txt");
    CHECK(f1 && f1->size == 6 && memcmp(f1->data, "hello\n", 6) == 0, "redirect >");

    run("echo world >> /tmp/f1.txt");
    CHECK(f1->size == 12 && memcmp(f1->data, "hello\nworld\n", 12) == 0, "append >>");

    run("cat /tmp/f1.txt | wc -l");
    CHECK(screen_has("2"), "pipe cat|wc -l prints 2");

    run("cat /home/user/docs/a.txt | sort");
    CHECK(screen_has("apple") && screen_has("banana") && screen_has("cherry") && screen_has("date"), "sort via pipe");
    {
        char* apple = strstr(screen, "apple");
        char* banana = strstr(screen, "banana");
        CHECK(apple && banana && apple < banana, "sorted order apple before banana");
    }

    run("sort -r /home/user/docs/a.txt | head -2");
    CHECK(screen_has("date") && screen_has("cherry"), "sort -r | head -2");

    run("printf 'x\\nx\\ny\\nx\\n' | sort | uniq");
    CHECK(screen_has("x") && screen_has("y"), "uniq");

    run("grep app /home/user/docs/a.txt");
    CHECK(screen_has("apple") && !screen_has("banana"), "grep");

    run("cat /home/user/docs/a.txt | grep -i APP");
    CHECK(screen_has("apple"), "grep -i");

    run("cat /home/user/docs/a.txt | wc");
    CHECK(screen_has("4") && screen_has("4") && screen_has("25"), "wc lines/words/bytes");

    run("seq 1 10 > /tmp/seq.txt");
    vnode_t* sq = vfs_resolve("/tmp/seq.txt");
    CHECK(sq && sq->size == 21, "seq 1..10 -> 21 bytes");

    run("seq 5");
    CHECK(screen_has("1\n2\n3\n4\n5\n"), "seq 5");

    run("seq 10 5 30");
    CHECK(screen_has("10") && screen_has("25") && screen_has("30"), "seq step");

    run("factor 60");
    CHECK(screen_has("60: 2 2 3 5"), "factor 60");

    run("echo abc | rev");
    CHECK(screen_has("cba"), "rev");

    run("printf 'a\\nb\\nc\\n' | nl");
    CHECK(screen_has("1\t") && screen_has("2\t") && screen_has("3\t"), "nl");

    run("pwd");
    CHECK(screen_has("/root"), "pwd initial");
    run("cd /home/user/docs");
    run("pwd");
    CHECK(screen_has("/home/user/docs"), "cd + pwd");
    run("cd ..");
    run("pwd");
    CHECK(screen_has("/home/user"), "cd ..");
    run("cd /nope");
    CHECK(screen_has("no such directory"), "cd fail msg");
    run("cd ~");
    run("pwd");
    CHECK(screen_has("/root"), "cd ~");

    run("ls /etc");
    CHECK(screen_has("os-release") && screen_has("motd"), "ls dir");
    run("ls -l /etc");
    CHECK(screen_has("-rw-r--r--"), "ls -l perms");

    run("cp /tmp/f1.txt /tmp/f2.txt");
    vnode_t* f2 = vfs_resolve("/tmp/f2.txt");
    CHECK(f2 && f2->size == 12 && memcmp(f2->data, "hello\nworld\n", 12) == 0, "cp");

    run("mv /tmp/f2.txt /tmp/f3.txt");
    CHECK(vfs_resolve("/tmp/f2.txt") == 0 && vfs_resolve("/tmp/f3.txt") != 0, "mv");

    run("touch /tmp/newfile");
    CHECK(vfs_resolve("/tmp/newfile") != 0, "touch");

    run("rm /tmp/f3.txt");
    CHECK(vfs_resolve("/tmp/f3.txt") == 0, "rm file");
    run("rm /etc");
    CHECK(screen_has("is a directory"), "rm dir without -r fails");
    run("rm -r /home/user/docs");
    CHECK(vfs_resolve("/home/user/docs") == 0, "rm -r");

    run("mkdir /tmp/d1");
    run("rmdir /tmp/d1");
    CHECK(vfs_resolve("/tmp/d1") == 0, "rmdir empty");

    run("echo same > /tmp/a.txt");
    run("echo same > /tmp/b.txt");
    run("diff /tmp/a.txt /tmp/b.txt");
    CHECK(screen_has("identical"), "diff identical");
    run("echo different > /tmp/c.txt");
    run("diff /tmp/a.txt /tmp/c.txt");
    CHECK(screen_has("differ"), "diff different");

    fprintf(stderr, "BEFORE FIND\n"); fflush(stderr);
    run("find /etc -name motd");
    fprintf(stderr, "AFTER FIND\n"); fflush(stderr);
    CHECK(screen_has("motd"), "find -name");

    run("stat /tmp/a.txt");
    CHECK(screen_has("regular file"), "stat file");
    run("stat /etc");
    CHECK(screen_has("directory"), "stat dir");

    run("basename /home/user/docs/a.txt");
    CHECK(screen_has("a.txt"), "basename");
    run("dirname /home/user/docs/a.txt");
    CHECK(screen_has("/home/user/docs"), "dirname");

    run("echo line > /tmp/tee.txt | cat");
    run("echo teed | tee /tmp/tee.txt");
    {
        vnode_t* t = vfs_resolve("/tmp/tee.txt");
        CHECK(t && t->size == 5 && memcmp(t->data, "teed\n", 5) == 0, "tee writes file");
        CHECK(screen_has("teed"), "tee passes through");
    }

    run("cat /tmp/a.txt | cat | cat");
    CHECK(screen_has("same"), "multi-pipe");

    run("echo hello | wc -c > /tmp/bytes.txt");
    {
        vnode_t* b = vfs_resolve("/tmp/bytes.txt");
        CHECK(b && strncmp(b->data, "6", 1) == 0, "pipe+redirect together");
    }

    run("false");
    CHECK(sh_last_exit() == 1, "false -> exit 1");
    run("true");
    CHECK(sh_last_exit() == 0, "true -> exit 0");
    run("echo $?");
    CHECK(screen_has("0"), "$? expands");
    run("false");
    printf("LASTEXIT after false = %d\n", sh_last_exit()); fflush(stdout);
    run("echo $?");
    printf("screen after echo $?: [%s]\n", screen); fflush(stdout);
    CHECK(screen_has("1"), "$? after false");

    run("nonexistentcommand");
    CHECK(screen_has("command not found"), "unknown command");

    run("uname");
    CHECK(screen_has("BojanOS"), "uname");
    run("uname -a");
    CHECK(screen_has("x86_64"), "uname -a");
    run("whoami");
    CHECK(screen_has("root"), "whoami");
    run("id");
    CHECK(screen_has("uid=0"), "id");
    run("lscpu");
    CHECK(screen_has("x86_64"), "lscpu");
    run("df");
    CHECK(screen_has("ramfs"), "df");
    run("free");
    CHECK(screen_has("Mem:"), "free");
    run("date");
    CHECK(screen_has("2026"), "date");
    run("cal 9 2026");
    CHECK(screen_has("September") && screen_has("2026"), "cal");
    run("hostname testhost");
    run("hostname");
    CHECK(screen_has("testhost"), "hostname set/get");
    run("seq 1 100 | wc -l");
    CHECK(screen_has("100"), "100 line pipe");
    run("cat /tmp/a.txt");
    CHECK(screen_has("same"), "input redirect <");

    run("test -f /tmp/a.txt && echo yes");
    CHECK(screen_has("yes"), "test -f true");
    run("test -d /etc && echo isdir");
    CHECK(screen_has("isdir"), "test -d true");
    run("[ 3 -gt 2 ] && echo three");
    CHECK(screen_has("three"), "[ 3 -gt 2 ]");
    run("[ 1 -eq 2 ] || echo no");
    CHECK(screen_has("no"), "[ 1 -eq 2 ] false");

    feed_file("piped\nfrom\nstdin\n");
    run("cat");
    CHECK(screen_has("piped") && screen_has("stdin"), "cat from stdin");

    run("echo HELLO | tr x x 2>/dev/null || true");

    shell_add_history("test command one");
    shell_add_history("test command two");
    run("history");
    CHECK(screen_has("test command one") && screen_has("test command two"), "history runs");

    run("help");
    CHECK(screen_has("Built-in commands"), "help text");

    printf("\n== result: %d failures ==\n", failures);
    return failures ? 1 : 0;
}
