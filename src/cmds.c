#include "shell.h"
#include "vga.h"
#include "vfs.h"
#include <stdarg.h>
#include "string.h"
#include "kheap.h"
#include "timer.h"
#include "rtc.h"
#include "multiboot.h"
#include "keyboard.h"

typedef struct {
    char* data;
    size_t len;
    size_t cap;
} buffer_t;

static void buf_init(buffer_t* b) { b->data = 0; b->len = 0; b->cap = 0; }

static void buf_putc(buffer_t* b, char c) {
    if (b->len + 1 >= b->cap) {
        size_t nc = b->cap ? b->cap * 2 : 256;
        char* nb = kmalloc(nc);
        if (!nb) return;
        if (b->len) memcpy(nb, b->data, b->len);
        b->data = nb;
        b->cap = nc;
    }
    b->data[b->len++] = c;
    b->data[b->len] = 0;
}

static char* read_all_stdin(size_t* out_len) {
    buffer_t b;
    buf_init(&b);
    for (;;) {
        int c = sh_getc();
        if (c < 0) break;
        buf_putc(&b, (char)c);
    }
    if (out_len) *out_len = b.len;
    return b.data;
}

typedef struct {
    char* data;
    size_t size;
    int is_stdin;
} infile_t;

static int open_input(infile_t* f, const char* path) {
    if (!path) {
        f->data = read_all_stdin(&f->size);
        f->is_stdin = 1;
        return f->data != 0 || f->size == 0;
    }
    vnode_t* n = vfs_resolve(path);
    if (!n || n->type != FS_TYPE_FILE) {
        sh_printf("%s: no such file\n", path);
        return 0;
    }
    f->data = n->data;
    f->size = n->size;
    f->is_stdin = 0;
    return 1;
}

static int split_lines(char* data, size_t len, char*** out_lines, int* out_count) {
    if (!data || len == 0) {
        char** lines = kmalloc(sizeof(char*));
        if (lines) lines[0] = 0;
        *out_lines = lines;
        *out_count = 0;
        return 1;
    }
    int count = 0;
    for (size_t i = 0; i < len; i++) if (data[i] == '\n') count++;
    if (data[len - 1] != '\n') count++;
    char** lines = kmalloc(sizeof(char*) * (count + 1));
    int li = 0;
    size_t start = 0;
    for (size_t i = 0; i <= len; i++) {
        if (i == len || data[i] == '\n') {
            if (i > start || i < len) {
                size_t llen = i - start;
                char* l = kmalloc(llen + 1);
                memcpy(l, data + start, llen);
                l[llen] = 0;
                lines[li++] = l;
            }
            start = i + 1;
        }
    }
    *out_lines = lines;
    *out_count = li;
    return 1;
}

static int cmd_help(args_t* a);

static int cmd_echo(args_t* a) {
    int n = 1;
    int start = 1;
    if (a->argc > 1 && strcmp(a->argv[1], "-n") == 0) { n = 0; start = 2; }
    for (int i = start; i < a->argc; i++) {
        if (i > start) sh_putc(' ');
        sh_puts(a->argv[i]);
    }
    if (n) sh_putc('\n');
    return 0;
}

static int cmd_clear(args_t* a) { (void)a; vga_clear(); return 0; }

static int cmd_pwd(args_t* a) {
    (void)a;
    sh_puts(vfs_pwd);
    sh_putc('\n');
    return 0;
}

static int cmd_cd(args_t* a) {
    const char* target = (a->argc >= 2) ? a->argv[1] : "/root";
    if (a->argc >= 2 && (strcmp(target, "~") == 0)) target = "/root";
    vnode_t* n = vfs_resolve(target);
    if (!n || n->type != FS_TYPE_DIR) {
        sh_printf("cd: %s: no such directory\n", target);
        return 1;
    }
    vfs_cwd = n;
    vfs_update_pwd();
    return 0;
}

static void print_entry(vnode_t* n, int longfmt) {
    if (longfmt) {
        if (n->type == FS_TYPE_DIR) sh_puts("drwxr-xr-x");
        else sh_puts("-rw-r--r--");
        sh_printf(" 1 root root %8u ", (unsigned)n->size);
    }
    sh_puts(n->name);
    if (n->type == FS_TYPE_DIR) sh_putc('/');
    sh_putc('\n');
}

static int ls_dir(vnode_t* dir, int longfmt, int all) {
    for (int i = 0; i < dir->child_count; i++) {
        vnode_t* c = dir->children[i];
        if (!all && c->name[0] == '.') continue;
        print_entry(c, longfmt);
    }
    return 0;
}

static int cmd_ls(args_t* a) {
    int longfmt = 0, all = 0;
    int idx = 1;
    while (idx < a->argc && a->argv[idx][0] == '-' && a->argv[idx][1]) {
        for (char* f = a->argv[idx] + 1; *f; f++) {
            if (*f == 'l') longfmt = 1;
            else if (*f == 'a') all = 1;
        }
        idx++;
    }
    if (idx >= a->argc) return ls_dir(vfs_cwd, longfmt, all);
    int rc = 0;
    for (int i = idx; i < a->argc; i++) {
        vnode_t* n = vfs_resolve(a->argv[i]);
        if (!n) { sh_printf("ls: %s: no such file or directory\n", a->argv[i]); rc = 1; continue; }
        if (n->type == FS_TYPE_DIR) {
            if (i > idx) sh_printf("\n%s:\n", a->argv[i]);
            ls_dir(n, longfmt, all);
        } else {
            print_entry(n, longfmt);
        }
    }
    return rc;
}

static int cmd_mkdir(args_t* a) {
    if (a->argc < 2) { sh_puts("mkdir: missing operand\n"); return 1; }
    int rc = 0;
    for (int i = 1; i < a->argc; i++) {
        if (!vfs_mkdir(a->argv[i])) {
            sh_printf("mkdir: cannot create directory '%s'\n", a->argv[i]);
            rc = 1;
        }
    }
    return rc;
}

static int cmd_touch(args_t* a) {
    if (a->argc < 2) { sh_puts("touch: missing operand\n"); return 1; }
    int rc = 0;
    for (int i = 1; i < a->argc; i++) {
        vnode_t* n = vfs_resolve(a->argv[i]);
        if (!n) { if (!vfs_mkfile(a->argv[i])) { sh_printf("touch: %s: failed\n", a->argv[i]); rc = 1; } }
        else if (n->type != FS_TYPE_FILE) { sh_printf("touch: %s: is a directory\n", a->argv[i]); rc = 1; }
    }
    return rc;
}

static int cmd_rm(args_t* a) {
    int rf = 0;
    int idx = 1;
    while (idx < a->argc && a->argv[idx][0] == '-' && a->argv[idx][1]) {
        for (char* f = a->argv[idx] + 1; *f; f++) if (*f == 'r' || *f == 'R' || *f == 'f') rf = 1;
        idx++;
    }
    if (idx >= a->argc) { sh_puts("rm: missing operand\n"); return 1; }
    int rc = 0;
    for (int i = idx; i < a->argc; i++) {
        vnode_t* n = vfs_resolve(a->argv[i]);
        if (!n) { sh_printf("rm: %s: no such file or directory\n", a->argv[i]); rc = 1; continue; }
        if (n->type == FS_TYPE_DIR && !rf) { sh_printf("rm: %s: is a directory (use -r)\n", a->argv[i]); rc = 1; continue; }
        int ok = rf ? vfs_rm_recursive(a->argv[i]) : vfs_rm(a->argv[i]);
        if (!ok) { sh_printf("rm: cannot remove %s\n", a->argv[i]); rc = 1; }
    }
    return rc;
}

static int cmd_rmdir(args_t* a) {
    if (a->argc < 2) { sh_puts("rmdir: missing operand\n"); return 1; }
    int rc = 0;
    for (int i = 1; i < a->argc; i++) {
        vnode_t* n = vfs_resolve(a->argv[i]);
        if (!n || n->type != FS_TYPE_DIR) { sh_printf("rmdir: %s: no such directory\n", a->argv[i]); rc = 1; continue; }
        if (!vfs_rm(a->argv[i])) { sh_printf("rmdir: %s: directory not empty\n", a->argv[i]); rc = 1; }
    }
    return rc;
}

static int cmd_cat(args_t* a) {
    if (a->argc < 2) {
        size_t len;
        char* d = read_all_stdin(&len);
        if (d) sh_write(d, len);
        return 0;
    }
    int rc = 0;
    for (int i = 1; i < a->argc; i++) {
        infile_t f;
        if (!open_input(&f, a->argv[i])) { rc = 1; continue; }
        if (f.size) sh_write(f.data, f.size);
    }
    return rc;
}

static int cmd_head(args_t* a) {
    int n = 10;
    int idx = 1;
    if (a->argc >= 2 && a->argv[1][0] == '-' && isdigit((unsigned char)a->argv[1][1])) {
        n = atoi(a->argv[1] + 1);
        idx = 2;
    } else if (a->argc >= 3 && strcmp(a->argv[1], "-n") == 0) {
        n = atoi(a->argv[2]);
        idx = 3;
    }
    const char* path = (idx < a->argc) ? a->argv[idx] : 0;
    infile_t f;
    if (!open_input(&f, path)) return 1;
    size_t lines = 0;
    for (size_t i = 0; i < f.size && lines < (size_t)n; i++) {
        sh_putc(f.data[i]);
        if (f.data[i] == '\n') lines++;
    }
    return 0;
}

static int cmd_tail(args_t* a) {
    int n = 10;
    int idx = 1;
    if (a->argc >= 2 && a->argv[1][0] == '-' && isdigit((unsigned char)a->argv[1][1])) {
        n = atoi(a->argv[1] + 1);
        idx = 2;
    } else if (a->argc >= 3 && strcmp(a->argv[1], "-n") == 0) {
        n = atoi(a->argv[2]);
        idx = 3;
    }
    const char* path = (idx < a->argc) ? a->argv[idx] : 0;
    infile_t f;
    if (!open_input(&f, path)) return 1;
    int lines = 0;
    size_t start = 0;
    for (size_t i = f.size; i > 0; i--) {
        if (f.data[i - 1] == '\n') {
            lines++;
            if (lines == n) { start = i; break; }
        }
    }
    for (size_t i = start; i < f.size; i++) sh_putc(f.data[i]);
    return 0;
}

static int cmd_wc(args_t* a) {
    int lflag = 0, wflag = 0, cflag = 0;
    int idx = 1;
    while (idx < a->argc && a->argv[idx][0] == '-' && a->argv[idx][1]) {
        for (char* f = a->argv[idx] + 1; *f; f++) {
            if (*f == 'l') lflag = 1;
            else if (*f == 'w') wflag = 1;
            else if (*f == 'c') cflag = 1;
        }
        idx++;
    }
    if (!lflag && !wflag && !cflag) { lflag = wflag = cflag = 1; }
    const char* path = (idx < a->argc) ? a->argv[idx] : 0;
    infile_t f;
    if (!open_input(&f, path)) return 1;
    size_t lines = 0, words = 0, bytes = f.size, inw = 0;
    for (size_t i = 0; i < f.size; i++) {
        char c = f.data[i];
        if (c == '\n') lines++;
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') inw = 0;
        else if (!inw) { words++; inw = 1; }
    }
    if (lflag) sh_printf("%u ", (unsigned)lines);
    if (wflag) sh_printf("%u ", (unsigned)words);
    if (cflag) sh_printf("%u ", (unsigned)bytes);
    if (path) sh_puts(path);
    sh_putc('\n');
    return 0;
}

static int cmd_grep(args_t* a) {
    int ci = 0, inv = 0;
    int idx = 1;
    while (idx < a->argc && a->argv[idx][0] == '-' && a->argv[idx][1]) {
        for (char* f = a->argv[idx] + 1; *f; f++) {
            if (*f == 'i') ci = 1;
            else if (*f == 'v') inv = 1;
        }
        idx++;
    }
    if (a->argc - idx < 1) { sh_puts("grep: missing pattern\n"); return 2; }
    const char* pat = a->argv[idx++];
    size_t patlen = strlen(pat);
    const char* path = (idx < a->argc) ? a->argv[idx] : 0;
    infile_t f;
    if (!open_input(&f, path)) return 2;
    char** lines; int lc;
    split_lines(f.data, f.size, &lines, &lc);
    int found = 0;
    for (int i = 0; i < lc; i++) {
        int match = 0;
        size_t llen = strlen(lines[i]);
        if (patlen == 0) match = 1;
        else for (size_t j = 0; j + patlen <= llen; j++) {
            int ok = 1;
            for (size_t k = 0; k < patlen; k++) {
                char x = lines[i][j + k], y = pat[k];
                if (ci) { if (tolower((unsigned char)x) != tolower((unsigned char)y)) { ok = 0; break; } }
                else if (x != y) { ok = 0; break; }
            }
            if (ok) { match = 1; break; }
        }
        if (match ^ inv) {
            sh_puts(lines[i]);
            sh_putc('\n');
            found = 1;
        }
    }
    return found ? 0 : 1;
}

static int cmp_str(const void* x, const void* y) {
    return strcmp(*(const char* const*)x, *(const char* const*)y);
}

static int cmp_str_num(const void* x, const void* y) {
    int64_t a = atoi(*(const char* const*)x);
    int64_t b = atoi(*(const char* const*)y);
    if (a < b) return -1;
    if (a > b) return 1;
    return cmp_str(x, y);
}

static int cmd_sort(args_t* a) {
    int rev = 0, num = 0, uniq = 0;
    int idx = 1;
    while (idx < a->argc && a->argv[idx][0] == '-' && a->argv[idx][1]) {
        for (char* f = a->argv[idx] + 1; *f; f++) {
            if (*f == 'r') rev = 1;
            else if (*f == 'n') num = 1;
            else if (*f == 'u') uniq = 1;
        }
        idx++;
    }
    const char* path = (idx < a->argc) ? a->argv[idx] : 0;
    infile_t f;
    if (!open_input(&f, path)) return 2;
    char** lines; int lc;
    split_lines(f.data, f.size, &lines, &lc);
    for (int i = 1; i < lc; i++) {
        char* key = lines[i];
        int j = i - 1;
        while (j >= 0) {
            int c = num ? cmp_str_num(&key, &lines[j]) : cmp_str(&key, &lines[j]);
            if (rev) c = -c;
            if (c < 0) { lines[j + 1] = lines[j]; j--; }
            else break;
        }
        lines[j + 1] = key;
    }
    const char* prev = 0;
    for (int i = 0; i < lc; i++) {
        if (uniq && prev && strcmp(prev, lines[i]) == 0) continue;
        sh_puts(lines[i]);
        sh_putc('\n');
        prev = lines[i];
    }
    return 0;
}

static int cmd_uniq(args_t* a) {
    int count = 0;
    int idx = 1;
    while (idx < a->argc && a->argv[idx][0] == '-' && a->argv[idx][1]) {
        for (char* f = a->argv[idx] + 1; *f; f++) if (*f == 'c') count = 1;
        idx++;
    }
    const char* path = (idx < a->argc) ? a->argv[idx] : 0;
    infile_t f;
    if (!open_input(&f, path)) return 1;
    char** lines; int lc;
    split_lines(f.data, f.size, &lines, &lc);
    int run = 0;
    for (int i = 0; i < lc; i++) {
        run++;
        if (i + 1 < lc && strcmp(lines[i], lines[i + 1]) == 0) continue;
        if (count) sh_printf("%7d ", run);
        sh_puts(lines[i]);
        sh_putc('\n');
        run = 0;
    }
    return 0;
}

static int cmd_diff(args_t* a) {
    if (a->argc < 3) { sh_puts("diff: two files required\n"); return 2; }
    infile_t fa, fb;
    if (!open_input(&fa, a->argv[1])) return 2;
    if (!open_input(&fb, a->argv[2])) return 2;
    char** la; int na;
    char** lb; int nb;
    split_lines(fa.data, fa.size, &la, &na);
    split_lines(fb.data, fb.size, &lb, &nb);
    uint16_t* d = kmalloc(sizeof(uint16_t) * (na + 1) * (nb + 1));
    if (!d) { sh_puts("diff: out of memory\n"); return 1; }
    for (int i = 0; i <= na; i++) d[i * (nb + 1) + nb] = na - i;
    for (int j = 0; j <= nb; j++) d[na * (nb + 1) + j] = nb - j;
    for (int i = na - 1; i >= 0; i--) {
        for (int j = nb - 1; j >= 0; j--) {
            int best = d[(i + 1) * (nb + 1) + (j + 1)] + (strcmp(la[i], lb[j]) ? 1 : 0);
            int del = d[(i + 1) * (nb + 1) + j] + 1;
            int ins = d[i * (nb + 1) + (j + 1)] + 1;
            if (del < best) best = del;
            if (ins < best) best = ins;
            d[i * (nb + 1) + j] = best;
        }
    }
    if (d[0] == 0) { sh_puts("Files are identical\n"); return 0; }
    int i = 0, j = 0;
    int diffs = 0;
    while (i < na || j < nb) {
        if (i < na && j < nb && strcmp(la[i], lb[j]) == 0) { i++; j++; continue; }
        int fromI = i, fromJ = j;
        int dels = 0, inss = 0;
        while (i < na && (j >= nb || d[i * (nb + 1) + j] != d[(i + 1) * (nb + 1) + j] - 1 ? 0 : 1)) {
            if (i < na && j < nb && strcmp(la[i], lb[j]) == 0) break;
            i++; dels++;
            if (i >= na) break;
            if (j < nb && d[i * (nb + 1) + j] == d[(i + 1) * (nb + 1) + (j)] - 1) continue;
            break;
        }
        i = fromI;
        while (j < nb) {
            if (i < na && strcmp(la[i], lb[j]) == 0) break;
            if (i < na && d[i * (nb + 1) + j] == d[i * (nb + 1) + (j + 1)] - 1) { j++; inss++; continue; }
            if (i < na && d[i * (nb + 1) + j] == d[(i + 1) * (nb + 1) + (j + 1)] + 0) { i++; j++; continue; }
            if (i < na && d[i * (nb + 1) + j] == d[(i + 1) * (nb + 1) + j] + 0) { i++; continue; }
            break;
        }
        (void)fromI; (void)fromJ; (void)dels; (void)inss;
        diffs++;
        if (diffs > 1000) break;
    }
    sh_printf("Files differ (%u edits)\n", (unsigned)d[0]);
    return 1;
}

static int cmd_cp(args_t* a) {
    if (a->argc < 3) { sh_puts("cp: missing operand\n"); return 1; }
    vnode_t* src = vfs_resolve(a->argv[1]);
    if (!src || src->type != FS_TYPE_FILE) { sh_printf("cp: %s: no such file\n", a->argv[1]); return 1; }
    vnode_t* dst = vfs_resolve(a->argv[2]);
    if (dst && dst->type == FS_TYPE_DIR) {
        char name[VFS_NAME_MAX];
        strcpy(name, vfs_basename(a->argv[1]));
        char full[512];
        strcpy(full, a->argv[2]);
        size_t l = strlen(full);
        if (full[l - 1] != '/') { full[l] = '/'; full[l + 1] = 0; }
        strcat(full, name);
        dst = vfs_resolve(full);
        if (!dst) { vfs_mkfile(full); dst = vfs_resolve(full); }
    } else if (!dst) {
        vfs_mkfile(a->argv[2]);
        dst = vfs_resolve(a->argv[2]);
    }
    if (!dst || dst->type != FS_TYPE_FILE) { sh_printf("cp: %s: cannot write\n", a->argv[2]); return 1; }
    vfs_write_file(dst, src->data ? src->data : "", src->size);
    return 0;
}

static int cmd_mv(args_t* a) {
    if (a->argc < 3) { sh_puts("mv: missing operand\n"); return 1; }
    int rc = cmd_cp(a);
    if (rc) return rc;
    if (!vfs_rm(a->argv[1])) { sh_printf("mv: cannot remove %s\n", a->argv[1]); return 1; }
    return 0;
}

static int cmd_write_file(args_t* a, int append) {
    if (a->argc < 2) { sh_puts("write: missing file\n"); return 1; }
    vnode_t* n = vfs_resolve(a->argv[1]);
    if (!n) { vfs_mkfile(a->argv[1]); n = vfs_resolve(a->argv[1]); }
    if (!n || n->type != FS_TYPE_FILE) { sh_printf("cannot open %s\n", a->argv[1]); return 1; }
    if (!append) vfs_truncate(n);
    size_t len;
    char* d = read_all_stdin(&len);
    if (d && len) vfs_append_file(n, d, len);
    return 0;
}

static int glob_match(const char* pat, const char* s) {
    if (*pat == 0) return *s == 0;
    if (*pat == '*') return glob_match(pat + 1, s) || (*s && glob_match(pat, s + 1));
    if (*pat == '?') return *s && glob_match(pat + 1, s + 1);
    return *s == *pat && glob_match(pat + 1, s + 1);
}

static void find_rec(vnode_t* dir, const char* prefix, const char* namepat) {
    for (int i = 0; i < dir->child_count; i++) {
        vnode_t* c = dir->children[i];
        if (!namepat || glob_match(namepat, c->name)) {
            sh_puts(prefix);
            sh_puts(c->name);
            if (c->type == FS_TYPE_DIR) sh_putc('/');
            sh_putc('\n');
        }
        if (c->type == FS_TYPE_DIR) {
            char np[1024];
            strcpy(np, prefix);
            strcat(np, c->name);
            strcat(np, "/");
            find_rec(c, np, namepat);
        }
    }
}

static int cmd_find(args_t* a) {
    const char* start = ".";
    const char* namepat = 0;
    for (int i = 1; i < a->argc; i++) {
        if (strcmp(a->argv[i], "-name") == 0 && i + 1 < a->argc) { namepat = a->argv[++i]; }
        else if (a->argv[i][0] != '-') start = a->argv[i];
    }
    vnode_t* n = vfs_resolve(start);
    if (!n || n->type != FS_TYPE_DIR) { sh_printf("find: %s: no such directory\n", start); return 1; }
    const char* base = vfs_pwd;
    (void)base;
    sh_puts(".");
    if (n->type == FS_TYPE_DIR) sh_putc('/');
    sh_putc('\n');
    find_rec(n, "./", namepat);
    return 0;
}

static void tree_rec(vnode_t* dir, int depth) {
    for (int i = 0; i < dir->child_count; i++) {
        vnode_t* c = dir->children[i];
        for (int k = 0; k < depth; k++) sh_puts("    ");
        sh_puts("|-- ");
        sh_puts(c->name);
        if (c->type == FS_TYPE_DIR) sh_putc('/');
        sh_putc('\n');
        if (c->type == FS_TYPE_DIR) tree_rec(c, depth + 1);
    }
}

static int cmd_tree(args_t* a) {
    vnode_t* n = (a->argc >= 2) ? vfs_resolve(a->argv[1]) : vfs_cwd;
    if (!n || n->type != FS_TYPE_DIR) { sh_puts("tree: no such directory\n"); return 1; }
    sh_puts(n->name[0] ? n->name : "/");
    sh_putc('\n');
    tree_rec(n, 0);
    return 0;
}

static int cmd_stat(args_t* a) {
    if (a->argc < 2) { sh_puts("stat: missing operand\n"); return 1; }
    vnode_t* n = vfs_resolve(a->argv[1]);
    if (!n) { sh_printf("stat: %s: no such file or directory\n", a->argv[1]); return 1; }
    sh_printf("  File: %s\n", a->argv[1]);
    sh_printf("  Size: %-10u Type: %s\n", (unsigned)n->size, n->type == FS_TYPE_DIR ? "directory" : "regular file");
    sh_printf("  Access: (%s)  Uid: 0   Gid: 0\n", n->type == FS_TYPE_DIR ? "0755/drwxr-xr-x" : "0644/-rw-r--r--");
    return 0;
}

static int cmd_basename(args_t* a) {
    if (a->argc < 2) return 1;
    const char* b = vfs_basename(a->argv[1]);
    sh_puts(b);
    sh_putc('\n');
    return 0;
}

static int cmd_dirname(args_t* a) {
    if (a->argc < 2) return 1;
    char buf[1024];
    strncpy(buf, a->argv[1], sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = 0;
    char* slash = strrchr(buf, '/');
    if (!slash) { sh_puts(".\n"); return 0; }
    if (slash == buf) { sh_puts("/\n"); return 0; }
    *slash = 0;
    sh_puts(buf[0] ? buf : "/");
    sh_putc('\n');
    return 0;
}

static int cmd_rev(args_t* a) {
    const char* path = (a->argc >= 2) ? a->argv[1] : 0;
    infile_t f;
    if (!open_input(&f, path)) return 1;
    char** lines; int lc;
    split_lines(f.data, f.size, &lines, &lc);
    for (int i = 0; i < lc; i++) {
        int l = (int)strlen(lines[i]);
        while (l--) sh_putc(lines[i][l]);
        sh_putc('\n');
    }
    return 0;
}

static int cmd_nl(args_t* a) {
    const char* path = (a->argc >= 2) ? a->argv[1] : 0;
    infile_t f;
    if (!open_input(&f, path)) return 1;
    char** lines; int lc;
    split_lines(f.data, f.size, &lines, &lc);
    for (int i = 0; i < lc; i++) {
        sh_printf("%6d\t%s\n", i + 1, lines[i]);
    }
    return 0;
}

static int cmd_tee(args_t* a) {
    if (a->argc < 2) { sh_puts("tee: missing file\n"); return 1; }
    int append = 0;
    int idx = 1;
    if (a->argc > 1 && strcmp(a->argv[1], "-a") == 0) { append = 1; idx = 2; }
    vnode_t* n = vfs_resolve(a->argv[idx]);
    if (!n) { vfs_mkfile(a->argv[idx]); n = vfs_resolve(a->argv[idx]); }
    if (!n || n->type != FS_TYPE_FILE) return 1;
    if (!append) vfs_truncate(n);
    size_t len;
    char* d = read_all_stdin(&len);
    if (d && len) {
        sh_write(d, len);
        vfs_append_file(n, d, len);
    }
    return 0;
}

static int cmd_df(args_t* a) {
    (void)a;
    sh_puts("Filesystem     Size    Used   Avail Use% Mounted on\n");
    sh_printf("ramfs       %6uK %6uK %6uK  %u%% /\n",
        (unsigned)(kheap_total() / 1024),
        (unsigned)(kheap_used() / 1024),
        (unsigned)((kheap_total() - kheap_used()) / 1024),
        (unsigned)(kheap_total() ? kheap_used() * 100 / kheap_total() : 0));
    return 0;
}

static void count_rec(vnode_t* n, int* files, int* dirs, size_t* bytes) {
    if (n->type == FS_TYPE_DIR) {
        (*dirs)++;
        for (int i = 0; i < n->child_count; i++) count_rec(n->children[i], files, dirs, bytes);
    } else {
        (*files)++;
        *bytes += n->size;
    }
}

static int cmd_du(args_t* a) {
    vnode_t* n = (a->argc >= 2 && a->argv[1][0] != '-') ? vfs_resolve(a->argv[1]) : vfs_cwd;
    if (!n) { sh_puts("du: no such file or directory\n"); return 1; }
    int files = 0, dirs = 0; size_t bytes = 0;
    count_rec(n, &files, &dirs, &bytes);
    sh_printf("%u\t%s\n", (unsigned)((bytes + 1023) / 1024), a->argc >= 2 ? a->argv[1] : ".");
    return 0;
}

static int cmd_free(args_t* a) {
    (void)a;
    sh_puts("              total        used        free\n");
    sh_printf("Mem:       %8uK   %8uK   %8uK\n",
        total_mem_kb, (unsigned)(kheap_used() / 1024),
        total_mem_kb > (unsigned)(kheap_used() / 1024) ? total_mem_kb - (unsigned)(kheap_used() / 1024) : 0);
    sh_puts("Swap:             0            0            0\n");
    return 0;
}

static int cmd_uname(args_t* a) {
    int all = 0;
    for (int i = 1; i < a->argc; i++)
        if (a->argv[i][0] == '-') for (char* f = a->argv[i] + 1; *f; f++) if (*f == 'a') all = 1;
    if (all) sh_puts("BojanOS 1.0 bojanos x86_64 GNU/BojanOS\n");
    else sh_puts("BojanOS\n");
    return 0;
}

static int cmd_hostname(args_t* a) {
    if (a->argc >= 2) {
        strncpy(shell_hostname, a->argv[1], 31);
        shell_hostname[31] = 0;
        return 0;
    }
    sh_puts(shell_hostname);
    sh_putc('\n');
    return 0;
}

static int cmd_whoami(args_t* a) { (void)a; sh_puts("root\n"); return 0; }
static int cmd_id(args_t* a) { (void)a; sh_puts("uid=0(root) gid=0(root) groups=0(root)\n"); return 0; }

static int cmd_date(args_t* a) {
    (void)a;
    rtc_time_t t;
    rtc_read(&t);
    static const char* months[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    static const char* days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    int dow = (t.day + t.month + t.year + t.year / 4) % 7;
    sh_printf("%s %s %2u %02u:%02u:%02u UTC %u\n",
        days[dow & 7],
        (t.month >= 1 && t.month <= 12) ? months[t.month - 1] : "???",
        t.day, t.hour, t.minute, t.second, t.year);
    return 0;
}

static int zeller(int y, int m, int d) {
    if (m < 3) { m += 12; y--; }
    int k = y % 100, j = y / 100;
    int h = (d + 13 * (m + 1) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
    return (h + 6) % 7;
}

static int days_in_month(int y, int m) {
    static const int dm[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) return 29;
    return dm[m - 1];
}

static int cmd_cal(args_t* a) {
    rtc_time_t now;
    rtc_read(&now);
    int month = now.month, year = now.year;
    if (a->argc >= 3) { month = atoi(a->argv[1]); year = atoi(a->argv[2]); }
    else if (a->argc >= 2) { month = atoi(a->argv[1]); }
    if (month < 1 || month > 12) month = now.month;
    static const char* mn[] = {"January","February","March","April","May","June","July",
                               "August","September","October","November","December"};
    sh_printf("    %s %u\n", mn[month - 1], year);
    sh_puts("Su Mo Tu We Th Fr Sa\n");
    int first = zeller(year, month, 1);
    int dim = days_in_month(year, month);
    for (int i = 0; i < first; i++) sh_puts("   ");
    for (int d = 1; d <= dim; d++) {
        sh_printf("%2d ", d);
        if ((first + d) % 7 == 0) sh_putc('\n');
    }
    if ((first + dim) % 7 != 0) sh_putc('\n');
    return 0;
}

static int cmd_uptime(args_t* a) {
    (void)a;
    uint64_t ms = timer_uptime_ms();
    uint64_t s = ms / 1000;
    sh_printf(" %02u:%02u:%02u up %u:%02u,  1 user,  load average: 0.00, 0.00, 0.00\n",
        (unsigned)((s / 3600) % 24), (unsigned)((s / 60) % 60), (unsigned)(s % 60),
        (unsigned)(s / 3600), (unsigned)((s / 60) % 60));
    return 0;
}

static int cmd_sleep(args_t* a) {
    if (a->argc < 2) return 1;
    int secs = atoi(a->argv[1]);
    sleep_ms((uint64_t)secs * 1000);
    return 0;
}

static int cmd_seq(args_t* a) {
    long start = 1, step = 1, end = 1;
    if (a->argc == 2) { end = atoi(a->argv[1]); }
    else if (a->argc == 3) { start = atoi(a->argv[1]); end = atoi(a->argv[2]); }
    else if (a->argc >= 4) { start = atoi(a->argv[1]); step = atoi(a->argv[2]); end = atoi(a->argv[3]); }
    if (step == 0) return 1;
    if (step > 0) for (long v = start; v <= end; v += step) sh_printf("%d\n", (int)v);
    else for (long v = start; v >= end; v += step) sh_printf("%d\n", (int)v);
    return 0;
}

static int cmd_factor(args_t* a) {
    if (a->argc < 2) { sh_puts("factor: missing number\n"); return 1; }
    long n = atoi(a->argv[1]);
    sh_printf("%d:", (int)n);
    if (n < 2) { sh_putc('\n'); return 0; }
    for (long d = 2; d * d <= n; d++) {
        while (n % d == 0) { sh_printf(" %d", (int)d); n /= d; }
    }
    if (n > 1) sh_printf(" %d", (int)n);
    sh_putc('\n');
    return 0;
}

static int cmd_true(args_t* a) { (void)a; return 0; }
static int cmd_false(args_t* a) { (void)a; return 1; }

static int cmd_exit(args_t* a) {
    (void)a;
    sh_puts("logout\n");
    poweroff();
    return 0;
}

static int cmd_reboot(args_t* a) {
    (void)a;
    sh_puts("rebooting...\n");
    reboot();
    return 0;
}

static int cmd_poweroff(args_t* a) {
    (void)a;
    sh_puts("System halted. You can power off the VM.\n");
    poweroff();
    return 0;
}

static int cmd_history(args_t* a) {
    (void)a;
    for (int i = 0; i < shell_history_count(); i++) {
        sh_printf("%4d  %s\n", i + 1, shell_get_history(i));
    }
    return 0;
}

static int cmd_lscpu(args_t* a) {
    (void)a;
    sh_puts("Architecture:        x86_64\n");
    sh_printf("CPU(s):              1\n");
    sh_printf("Model name:          %s\n", cpu_vendor);
    sh_puts("CPU mode:            64-bit\n");
    return 0;
}

static int cmd_about(args_t* a) {
    (void)a;
    vga_setcolor(0x0E, 0x00);
    sh_puts("  ____  ___      _           ___  ____\n");
    sh_puts(" | __ )/ _ \\ __ | | __ _ _ _/ _ \\/ ___|\n");
    sh_puts(" |  _ \\ | | |  \\| |/ _` | '_| | | \\___ \\\n");
    sh_puts(" | |_) | |_| | |  | (_| | | | |_| |___) |\n");
    sh_puts(" |____/ \\___/|_|\\_|\\__,_|_|  \\___/|____/\n\n");
    vga_setcolor(0x07, 0x00);
    sh_puts("BojanOS 1.0 - a 64-bit operating system written from scratch\n");
    sh_puts("  own bootloader, long-mode kernel, drivers, filesystem and shell\n");
    sh_printf("  CPU: %s   Memory: %u KB\n", cpu_vendor, total_mem_kb);
    sh_puts("Type 'help' for commands.\n");
    return 0;
}

static int cmd_help(args_t* a) {
    (void)a;
    sh_puts(
        "Built-in commands:\n"
        "  help about ls ll dir cd pwd mkdir rmdir rm touch cp mv\n"
        "  cat head tail wc sort uniq diff grep find tree stat\n"
        "  echo printf rev nl tee basename dirname df du free\n"
        "  date cal uptime sleep seq factor history\n"
        "  uname hostname whoami id lscpu clear\n"
        "  true false test [ exit reboot poweroff\n"
        "Operators: >  >>  <  |  $?  ~\n");
    return 0;
}

static int cmd_ll(args_t* a) {
    char* fake[32];
    args_t b;
    b.argc = a->argc + 1;
    b.argv[0] = "ls";
    b.argv[1] = "-l";
    for (int i = 1; i < a->argc && i < 30; i++) b.argv[i + 1] = a->argv[i];
    b.argv[b.argc] = 0;
    (void)fake;
    return cmd_ls(&b);
}

static int cmd_dir(args_t* a) { return cmd_ls(a); }

static int cmd_printf(args_t* a) {
    for (int i = 1; i < a->argc; i++) {
        const char* s = a->argv[i];
        while (*s) {
            if (*s == '\\' && s[1]) {
                s++;
                switch (*s) {
                case 'n': sh_putc('\n'); break;
                case 't': sh_putc('\t'); break;
                case '\\': sh_putc('\\'); break;
                default: sh_putc(*s); break;
                }
                s++;
            } else {
                sh_putc(*s++);
            }
        }
    }
    return 0;
}

static int cmd_test(args_t* a) {
    int bracket = strcmp(a->argv[0], "[") == 0;
    int argc = a->argc;
    if (bracket) {
        if (argc < 2 || strcmp(a->argv[argc - 1], "]") != 0) {
            sh_puts("[: missing ]\n");
            return 2;
        }
        argc--;
    }
    if (argc == 2) {
        return a->argv[1][0] == 0 ? 0 : 1;
    }
    if (argc == 3) {
        const char* op = a->argv[1];
        const char* target = a->argv[2];
        vnode_t* n = vfs_resolve(target);
        if (strcmp(op, "-f") == 0) return (n && n->type == FS_TYPE_FILE) ? 0 : 1;
        if (strcmp(op, "-d") == 0) return (n && n->type == FS_TYPE_DIR) ? 0 : 1;
        if (strcmp(op, "-e") == 0) return n ? 0 : 1;
        return 1;
    }
    if (argc == 4) {
        const char* lhs = a->argv[1];
        const char* op = a->argv[2];
        const char* rhs = a->argv[3];
        int l = atoi(lhs), r = atoi(rhs);
        if (strcmp(op, "=") == 0) return strcmp(lhs, rhs) == 0 ? 0 : 1;
        if (strcmp(op, "!=") == 0) return strcmp(lhs, rhs) != 0 ? 0 : 1;
        if (strcmp(op, "-eq") == 0) return l == r ? 0 : 1;
        if (strcmp(op, "-ne") == 0) return l != r ? 0 : 1;
        if (strcmp(op, "-lt") == 0) return l < r ? 0 : 1;
        if (strcmp(op, "-gt") == 0) return l > r ? 0 : 1;
        if (strcmp(op, "-le") == 0) return l <= r ? 0 : 1;
        if (strcmp(op, "-ge") == 0) return l >= r ? 0 : 1;
        return 1;
    }
    return 1;
}

#define C(n, f, h) { n, f, h }
const cmd_entry_t commands[] = {
    C("help", cmd_help, "show help"),
    C("about", cmd_about, "about BojanOS"),
    C("echo", cmd_echo, "print text"),
    C("printf", cmd_printf, "formatted print"),
    C("clear", cmd_clear, "clear screen"),
    C("cls", cmd_clear, "clear screen"),
    C("pwd", cmd_pwd, "print working directory"),
    C("cd", cmd_cd, "change directory"),
    C("ls", cmd_ls, "list directory"),
    C("ll", cmd_ll, "long listing"),
    C("dir", cmd_dir, "list directory"),
    C("mkdir", cmd_mkdir, "make directory"),
    C("rmdir", cmd_rmdir, "remove empty directory"),
    C("rm", cmd_rm, "remove file"),
    C("touch", cmd_touch, "create empty file"),
    C("cat", cmd_cat, "print file"),
    C("head", cmd_head, "first lines"),
    C("tail", cmd_tail, "last lines"),
    C("wc", cmd_wc, "count lines words bytes"),
    C("grep", cmd_grep, "search text"),
    C("sort", cmd_sort, "sort lines"),
    C("uniq", cmd_uniq, "unique lines"),
    C("diff", cmd_diff, "compare files"),
    C("cp", cmd_cp, "copy file"),
    C("mv", cmd_mv, "move file"),
    C("find", cmd_find, "find files"),
    C("tree", cmd_tree, "directory tree"),
    C("stat", cmd_stat, "file status"),
    C("basename", cmd_basename, "file name part"),
    C("dirname", cmd_dirname, "directory part"),
    C("rev", cmd_rev, "reverse lines"),
    C("nl", cmd_nl, "number lines"),
    C("tee", cmd_tee, "write to file and stdout"),
    C("df", cmd_df, "filesystem usage"),
    C("du", cmd_du, "disk usage"),
    C("free", cmd_free, "memory usage"),
    C("uname", cmd_uname, "system name"),
    C("hostname", cmd_hostname, "show/set hostname"),
    C("whoami", cmd_whoami, "current user"),
    C("id", cmd_id, "user identity"),
    C("date", cmd_date, "current date/time"),
    C("cal", cmd_cal, "calendar"),
    C("uptime", cmd_uptime, "system uptime"),
    C("sleep", cmd_sleep, "delay seconds"),
    C("seq", cmd_seq, "number sequence"),
    C("factor", cmd_factor, "factor numbers"),
    C("history", cmd_history, "command history"),
    C("lscpu", cmd_lscpu, "cpu info"),
    C("true", cmd_true, "return success"),
    C("false", cmd_false, "return failure"),
    C("test", cmd_test, "test expression"),
    C("[", cmd_test, "test expression"),
    C("exit", cmd_exit, "halt system"),
    C("logout", cmd_exit, "halt system"),
    C("halt", cmd_poweroff, "halt system"),
    C("poweroff", cmd_poweroff, "halt system"),
    C("shutdown", cmd_poweroff, "halt system"),
    C("reboot", cmd_reboot, "restart system"),
    {0, 0, 0}
};
#undef C

const cmd_entry_t* cmd_lookup(const char* name) {
    for (const cmd_entry_t* e = commands; e->name; e++) {
        if (strcmp(e->name, name) == 0) return e;
    }
    return 0;
}
