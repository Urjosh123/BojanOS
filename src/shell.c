#include "shell.h"
#include "vga.h"
#include "serial.h"
#include "keyboard.h"
#include "string.h"
#include "kheap.h"
#include "vfs.h"
#include <stdarg.h>

char shell_hostname[32] = "bojanos";

static vnode_t* out_file;
static int out_append;
static vnode_t* in_file;
static size_t in_off;

static size_t pipe_in_off;

typedef struct {
    char* data;
    size_t len;
} pipe_storage_t;

static pipe_storage_t pipes[2];
static int pipe_writing;
static pipe_storage_t* pipe_reader;
static pipe_storage_t* pipe_writer;

#define PIPE_MAX (64 * 1024)
static void pipe_writer_add(char c) {
    if (!pipe_writer) return;
    if (pipe_writer->len >= PIPE_MAX) return;
    pipe_writer->data[pipe_writer->len++] = c;
}

static int last_exit;

#define HIST_MAX 32
static char* history[HIST_MAX];
static int hist_count;
static int hist_pos;

void sh_set_exit(int code) { last_exit = code & 0xFF; }
int sh_last_exit(void) { return last_exit; }

static void out_raw(char c) {
    if (out_file) {
        vfs_append_file(out_file, &c, 1);
    } else if (pipe_writing) {
        pipe_writer_add(c);
    } else {
        vga_putchar(c);
        serial_putchar(c);
    }
}

void sh_putc(char c) { out_raw(c); }

void sh_write(const char* s, size_t n) {
    for (size_t i = 0; i < n; i++) out_raw(s[i]);
}

void sh_puts(const char* s) {
    while (*s) out_raw(*s++);
}

static void sh_print_hex(uint64_t v) {
    const char* d = "0123456789abcdef";
    char b[17];
    int i = 0;
    if (!v) b[i++] = '0';
    while (v) { b[i++] = d[v & 0xF]; v >>= 4; }
    sh_puts("0x");
    while (i--) out_raw(b[i]);
}

void sh_printf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt != '%') { out_raw(*fmt++); continue; }
        fmt++;
        int lf = 0;
        int width = 0;
        int left = 0;
        if (*fmt == '-') { left = 1; fmt++; }
        while (*fmt >= '0' && *fmt <= '9') { width = width * 10 + (*fmt - '0'); fmt++; }
        while (*fmt == 'l') { lf++; fmt++; }
        char c = *fmt++;
        char numbuf[24];
        int nlen = 0;
        switch (c) {
        case 's': {
            const char* s = va_arg(ap, const char*);
            if (!s) s = "(null)";
            int slen = 0; while (s[slen]) slen++;
            if (left) { sh_puts(s); for (int k = slen; k < width; k++) out_raw(' '); }
            else { while (slen < width) { out_raw(' '); slen++; } sh_puts(s); }
            break;
        }
        case 'c': out_raw((char)va_arg(ap, int)); break;
        case 'd': case 'i': {
            int64_t v = lf >= 2 ? va_arg(ap, long long) : lf == 1 ? va_arg(ap, long) : va_arg(ap, int);
            int neg = v < 0;
            uint64_t u = neg ? (uint64_t)(-v) : (uint64_t)v;
            if (!u) numbuf[nlen++] = '0';
            while (u) { numbuf[nlen++] = '0' + u % 10; u /= 10; }
            if (neg) numbuf[nlen++] = '-';
            if (left) { while (nlen--) out_raw(numbuf[nlen]); for (int k = nlen + 1; k < width; k++) out_raw(' '); }
            else { while (nlen < width) numbuf[nlen++] = ' '; while (nlen--) out_raw(numbuf[nlen]); }
            break;
        }
        case 'u': {
            uint64_t v = lf >= 2 ? va_arg(ap, unsigned long long) : lf == 1 ? va_arg(ap, unsigned long) : va_arg(ap, unsigned int);
            if (!v) numbuf[nlen++] = '0';
            while (v) { numbuf[nlen++] = '0' + v % 10; v /= 10; }
            if (left) { while (nlen--) out_raw(numbuf[nlen]); for (int k = nlen + 1; k < width; k++) out_raw(' '); }
            else { while (nlen < width) numbuf[nlen++] = ' '; while (nlen--) out_raw(numbuf[nlen]); }
            break;
        }
        case 'x': {
            uint64_t v = lf >= 2 ? va_arg(ap, unsigned long long) : lf == 1 ? va_arg(ap, unsigned long) : va_arg(ap, unsigned int);
            sh_print_hex(v);
            break;
        }
        case '%': out_raw('%'); break;
        default: out_raw('%'); out_raw(c); break;
        }
    }
    va_end(ap);
}

int sh_getc(void) {
    if (in_file) {
        if (in_off >= in_file->size) return -1;
        return (unsigned char)in_file->data[in_off++];
    }
    if (pipe_reader) {
        if (pipe_in_off >= pipe_reader->len) return -1;
        return (unsigned char)pipe_reader->data[pipe_in_off++];
    }
    for (;;) {
        int c = keyboard_get();
        if (c == KB_CTRL_D || c < 0) return -1;
        if (c == KB_CTRL_C) return -1;
        return c;
    }
}

int sh_readline(char* buf, int max) {
    int pos = 0;
    for (;;) {
        int c = sh_getc();
        if (c < 0) { buf[pos] = 0; return -1; }
        if (c == '\n' || c == '\r') {
            buf[pos] = 0;
            sh_putc('\n');
            return pos;
        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
                sh_putc('\b'); sh_putc(' '); sh_putc('\b');
            }
        } else if (c == KB_CTRL_D) {
            buf[pos] = 0;
            return -1;
        } else if (c == KB_CTRL_C) {
            sh_puts("^C\n");
            sh_set_exit(130);
            buf[0] = 0;
            return 0;
        } else if (c == KB_KEY_UP) {
            if (hist_count && hist_pos > 0) {
                hist_pos--;
                while (pos > 0) { pos--; sh_putc('\b'); sh_putc(' '); sh_putc('\b'); }
                strncpy(buf, history[hist_pos], max - 1);
                buf[max - 1] = 0;
                pos = strlen(buf);
                sh_puts(buf);
            }
        } else if (c == KB_KEY_DOWN) {
            while (pos > 0) { pos--; sh_putc('\b'); sh_putc(' '); sh_putc('\b'); }
            if (hist_pos < hist_count - 1) {
                hist_pos++;
                strncpy(buf, history[hist_pos], max - 1);
                buf[max - 1] = 0;
            } else {
                hist_pos = hist_count;
                buf[0] = 0;
            }
            pos = strlen(buf);
            sh_puts(buf);
        } else if (c >= 32 && c < 127) {
            if (pos < max - 1) {
                buf[pos++] = (char)c;
                sh_putc((char)c);
            }
        }
    }
}

static char* expand_tilde(const char* tok) {
    if (tok[0] == '~' && (tok[1] == 0 || tok[1] == '/')) {
        char* out = kmalloc(strlen(tok) + 8);
        if (!out) return (char*)tok;
        strcpy(out, "/root");
        strcat(out, tok + 1);
        return out;
    }
    return (char*)tok;
}

typedef struct {
    char* items[64];
    int count;
    char out_file[256];
    int out_append;
    char in_file[256];
    int has_out;
    int has_in;
} cmdline_t;

static void append_str(char** out, const char* s) {
    while (*s) *(*out)++ = *s++;
}

static void tokenize(char* line, cmdline_t* cl) {
    cl->count = 0;
    cl->has_out = 0;
    cl->has_in = 0;
    cl->out_file[0] = 0;
    cl->in_file[0] = 0;
    char* p = line;
    while (*p && cl->count < 63) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;
        if (*p == '>' || *p == '<') {
            int is_out = *p == '>';
            int app = 0;
            if (is_out && p[1] == '>') { app = 1; p++; }
            p++;
            while (*p == ' ' || *p == '\t') p++;
            char* start = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '>' && *p != '<') p++;
            char saved = *p;
            *p = 0;
            if (is_out) {
                strncpy(cl->out_file, start, 255);
                cl->out_append = app;
                cl->has_out = 1;
            } else {
                strncpy(cl->in_file, start, 255);
                cl->has_in = 1;
            }
            if (saved) *p = saved, p++;
            continue;
        }
        char* start = p;
        char* out = p;
        int quote = 0;
        while (*p) {
            if (*p == '\'' && quote != 2) { quote = quote == 1 ? 0 : 1; p++; continue; }
            if (*p == '"' && quote != 1) { quote = quote == 2 ? 0 : 2; p++; continue; }
            if (!quote && (*p == ' ' || *p == '\t' || *p == '>' || *p == '<')) break;
            if (!quote && *p == '$' && p[1] == '?') {
                char num[8];
                int code = last_exit;
                int l = 0;
                if (code == 0) num[l++] = '0';
                else { char t[8]; int k = 0; while (code) { t[k++] = '0' + code % 10; code /= 10; } while (k--) num[l++] = t[k]; }
                num[l] = 0;
                append_str(&out, num);
                p += 2;
                continue;
            }
            *out++ = *p++;
        }
        {
            char delim = *p;
            if (delim) {
                *p = 0;
                p++;
            }
            *out = 0;
        }
        cl->items[cl->count++] = expand_tilde(start);
    }
    cl->items[cl->count] = 0;
}

static int apply_redirs(cmdline_t* cl) {
    out_file = 0;
    in_file = 0;
    in_off = 0;
    if (cl->has_out) {
        vnode_t* f = vfs_resolve(cl->out_file);
        if (!f) {
            if (!vfs_mkfile(cl->out_file)) {
                sh_printf("shell: cannot create %s\n", cl->out_file);
                return 0;
            }
            f = vfs_resolve(cl->out_file);
        }
        if (!f || f->type != FS_TYPE_FILE) {
            sh_printf("shell: %s: not a file\n", cl->out_file);
            return 0;
        }
        out_file = f;
        out_append = cl->out_append;
        if (!out_append) vfs_truncate(f);
    }
    if (cl->has_in) {
        vnode_t* f = vfs_resolve(cl->in_file);
        if (!f || f->type != FS_TYPE_FILE) {
            sh_printf("shell: %s: no such file\n", cl->in_file);
            return 0;
        }
        in_file = f;
        in_off = 0;
    }
    return 1;
}

static void restore_redirs(void) {
    out_file = 0;
    in_file = 0;
}

int shell_run_builtin(args_t* a) {
    const cmd_entry_t* e = cmd_lookup(a->argv[0]);
    if (!e) {
        sh_printf("%s: command not found\n", a->argv[0]);
        return 127;
    }
    return e->fn(a);
}

static int run_segment(char* segment, int do_pipe) {
    cmdline_t cl;
    tokenize(segment, &cl);
    if (cl.count == 0) return 0;
    pipe_writing = do_pipe;
    if (do_pipe && pipe_writer) {
        pipe_writer->len = 0;
    }
    if (!apply_redirs(&cl)) {
        restore_redirs();
        pipe_writing = 0;
        return 1;
    }
    args_t a;
    a.argc = cl.count;
    for (int i = 0; i < cl.count; i++) a.argv[i] = cl.items[i];
    a.argv[cl.count] = 0;
    int rc = shell_run_builtin(&a);
    restore_redirs();
    pipe_writing = 0;
    return rc;
}

static int exec_pipeline(char* line, int len) {
    line[len] = 0;
    if (!pipes[0].data) pipes[0].data = kmalloc(PIPE_MAX);
    if (!pipes[1].data) pipes[1].data = kmalloc(PIPE_MAX);
    pipes[0].len = 0;
    pipes[1].len = 0;
    pipe_reader = 0;
    pipe_writer = 0;
    pipe_writing = 0;
    pipe_in_off = 0;

    char* seg = line;
    char* p = line;
    int rc = 0;
    int which = 0;

    for (;;) {
        if (*p == '|' || *p == 0) {
            int last = (*p == 0);
            if (*p == '|') *p = 0;

            pipe_writer = last ? 0 : &pipes[which];
            rc = run_segment(seg, !last);

            if (last) break;
            pipe_reader = &pipes[which];
            pipe_in_off = 0;
            which ^= 1;
            pipes[which].len = 0;
            p++;
            while (*p == ' ' || *p == '\t') p++;
            seg = p;
        } else {
            p++;
        }
    }
    pipe_reader = 0;
    pipe_writer = 0;
    pipe_writing = 0;
    out_file = 0;
    in_file = 0;
    return rc;
}

static int is_separator(char* p, int* kind) {
    if (p[0] == ';') { *kind = ';'; return 1; }
    if (p[0] == '&' && p[1] == '&') { *kind = '&'; return 2; }
    if (p[0] == '|' && p[1] == '|') { *kind = 'o'; return 2; }
    return 0;
}

int shell_exec_line(char* line) {
    while (*line == ' ' || *line == '\t') line++;
    if (!*line || *line == '#') return 0;

    int rc = 0;
    char* start = line;
    char* p = line;
    for (;;) {
        int kind = 0, skip = 0;
        if (*p == 0) { skip = 0; }
        else { int k; int n = is_separator(p, &k); if (n) { skip = n; kind = k; } }

        if (*p == 0 || skip) {
            int len = (int)(p - start);
            while (len > 0 && (start[len - 1] == ' ' || start[len - 1] == '\t')) len--;
            if (len > 0) {
                char* segment = start;
                char saved = segment[len];
                int run = 1;
                if (kind == '&' && rc == 0) run = 1;
                else if (kind == '&' && rc != 0) run = 0;
                else if (kind == 'o' && rc != 0) run = 1;
                else if (kind == 'o' && rc == 0) run = 0;
                if (run) rc = exec_pipeline(segment, len);
                else segment[len] = saved;
                sh_set_exit(rc);
            }
            if (*p == 0) break;
            p += skip;
            while (*p == ' ' || *p == '\t') p++;
            start = p;
        } else {
            if (*p == '#') {
                *p = 0;
                continue;
            }
            p++;
        }
    }
    return rc;
}

static void prompt(void) {
    vga_setcolor(0x0A, 0x00);
    sh_printf("root@%s", shell_hostname);
    vga_setcolor(0x07, 0x00);
    sh_puts(":");
    vga_setcolor(0x0B, 0x00);
    sh_puts(vfs_pwd);
    vga_setcolor(0x07, 0x00);
    sh_puts("# ");
    vga_setcolor(0x07, 0x00);
}

void shell_init(void) {
    hist_count = 0;
    hist_pos = 0;
    last_exit = 0;
}

void shell_run(void) {
    char line[1024];
    vnode_t* motd = vfs_resolve("/etc/motd");
    if (motd && motd->type == FS_TYPE_FILE) {
        vga_setcolor(0x0E, 0x00);
        sh_write(motd->data, motd->size);
        vga_setcolor(0x07, 0x00);
        sh_puts("Type 'help' for commands, 'about' for system info.\n\n");
    }
    for (;;) {
        prompt();
        hist_pos = hist_count;
        int n = sh_readline(line, sizeof(line));
        if (n < 0) {
            sh_puts("\n");
            break;
        }
        if (n == 0 || line[0] == 0) continue;
        if (hist_count < HIST_MAX) {
            history[hist_count++] = strdup(line);
        } else {
            for (int i = 1; i < HIST_MAX; i++) history[i - 1] = history[i];
            history[HIST_MAX - 1] = strdup(line);
        }
        shell_exec_line(line);
    }
}

int sh_redirect(void) { return out_file != 0; }
void sh_restore(void) { restore_redirs(); }

char* shell_get_history(int i) {
    if (i < 0 || i >= hist_count) return 0;
    return history[i];
}

int shell_history_count(void) { return hist_count; }

void shell_add_history(const char* s) {
    if (hist_count < HIST_MAX) history[hist_count++] = strdup(s);
}
