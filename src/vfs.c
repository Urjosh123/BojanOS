#include "vfs.h"
#include "string.h"
#include "vga.h"

fs_dir fs_root;
fs_dir* fs_current;
char fs_path[256];

static fs_file* find_file_in_dir(fs_dir* dir, const char* name) {
    for (int i = 0; i < dir->file_count; i++) {
        if (dir->files[i].used && strcmp(dir->files[i].name, name) == 0)
            return &dir->files[i];
    }
    return 0;
}

static fs_dir* find_dir_in_dir(fs_dir* dir, const char* name) {
    for (int i = 0; i < dir->subdir_count; i++) {
        if (dir->subdirs[i] && dir->subdirs[i]->used && strcmp(dir->subdirs[i]->name, name) == 0)
            return dir->subdirs[i];
    }
    return 0;
}

void fs_init(void) {
    memset(&fs_root, 0, sizeof(fs_root));
    strcpy(fs_root.name, "");
    fs_root.used = 1;
    fs_root.parent = &fs_root;
    fs_current = &fs_root;
    strcpy(fs_path, "/");
}

int fs_mkfile(fs_dir* dir, const char* name) {
    if (find_file_in_dir(dir, name) || find_dir_in_dir(dir, name)) return 0;
    if (dir->file_count >= FS_MAX_FILES) return 0;
    fs_file* f = &dir->files[dir->file_count++];
    f->used = 1;
    strcpy(f->name, name);
    f->size = 0;
    f->content[0] = 0;
    return 1;
}

int fs_rmfile(fs_dir* dir, const char* name) {
    fs_file* f = find_file_in_dir(dir, name);
    if (!f) return 0;
    f->used = 0;
    return 1;
}

char* fs_cat(fs_dir* dir, const char* name) {
    fs_file* f = find_file_in_dir(dir, name);
    if (!f) return 0;
    return f->content;
}

int fs_write(fs_dir* dir, const char* name, const char* data) {
    fs_file* f = find_file_in_dir(dir, name);
    if (!f) {
        if (!fs_mkfile(dir, name)) return 0;
        f = find_file_in_dir(dir, name);
    }
    int len = 0;
    while (data[len] && len < FS_MAX_CONTENT - 1) len++;
    if (len >= FS_MAX_CONTENT) len = FS_MAX_CONTENT - 1;
    for (int i = 0; i < len; i++) f->content[i] = data[i];
    f->content[len] = 0;
    f->size = len;
    return 1;
}

int fs_append(fs_dir* dir, const char* name, const char* data) {
    fs_file* f = find_file_in_dir(dir, name);
    if (!f) return 0;
    int dlen = 0;
    while (data[dlen]) dlen++;
    if (f->size + dlen >= FS_MAX_CONTENT - 1)
        dlen = FS_MAX_CONTENT - 1 - f->size;
    for (int i = 0; i < dlen; i++)
        f->content[f->size + i] = data[i];
    f->content[f->size + dlen] = 0;
    f->size += dlen;
    return 1;
}

fs_file* fs_find_file(fs_dir* dir, const char* name) {
    return find_file_in_dir(dir, name);
}

fs_dir* fs_find_dir(fs_dir* dir, const char* name) {
    return find_dir_in_dir(dir, name);
}

int fs_mkdir(fs_dir* dir, const char* name) {
    if (find_file_in_dir(dir, name) || find_dir_in_dir(dir, name)) return 0;
    if (dir->subdir_count >= FS_MAX_DIRS) return 0;
    static fs_dir pool[FS_MAX_DIRS];
    static int pool_idx = 0;
    if (pool_idx >= FS_MAX_DIRS) return 0;
    fs_dir* d = &pool[pool_idx++];
    memset(d, 0, sizeof(fs_dir));
    d->used = 1;
    strcpy(d->name, name);
    d->parent = dir;
    dir->subdirs[dir->subdir_count++] = d;
    return 1;
}

int fs_rmdir(fs_dir* dir, const char* name) {
    fs_dir* d = find_dir_in_dir(dir, name);
    if (!d || d->file_count > 0 || d->subdir_count > 0) return 0;
    d->used = 0;
    return 1;
}

void fs_pwd(void) {
    vga_print(fs_path);
}

int fs_cd(const char* path) {
    if (strcmp(path, "/") == 0 || strcmp(path, "~") == 0) {
        fs_current = &fs_root;
        strcpy(fs_path, "/");
        return 1;
    }
    if (strcmp(path, "..") == 0) {
        if (fs_current != &fs_root) {
            fs_current = fs_current->parent;
            int len = 0;
            while (fs_path[len]) len++;
            if (len > 1) {
                len--;
                while (len > 0 && fs_path[len] != '/') len--;
                if (len == 0) fs_path[1] = 0;
                else fs_path[len] = 0;
            }
        }
        return 1;
    }
    fs_dir* d = find_dir_in_dir(fs_current, path);
    if (!d) return 0;
    fs_current = d;
    int plen = 0;
    while (fs_path[plen]) plen++;
    if (plen > 1) {
        fs_path[plen] = '/';
        plen++;
    }
    int nlen = 0;
    while (path[nlen] && plen < 255) {
        fs_path[plen] = path[nlen];
        plen++;
        nlen++;
    }
    fs_path[plen] = 0;
    return 1;
}

void fs_ls(fs_dir* dir) {
    for (int i = 0; i < dir->subdir_count; i++) {
        if (dir->subdirs[i] && dir->subdirs[i]->used) {
            vga_print(dir->subdirs[i]->name);
            vga_print("/\n");
        }
    }
    for (int i = 0; i < dir->file_count; i++) {
        if (dir->files[i].used) {
            vga_print(dir->files[i].name);
            vga_putchar('\n');
        }
    }
}

void fs_ll(fs_dir* dir) {
    for (int i = 0; i < dir->subdir_count; i++) {
        if (dir->subdirs[i] && dir->subdirs[i]->used) {
            vga_print("drwxr-xr-x root root 4096 09/05/2026 ");
            vga_print(dir->subdirs[i]->name);
            vga_print("/\n");
        }
    }
    for (int i = 0; i < dir->file_count; i++) {
        if (dir->files[i].used) {
            vga_print("-rw-r--r-- root root ");
            char buf[16];
            int n = dir->files[i].size;
            int j = 0;
            if (n == 0) {
                buf[j++] = '0';
            } else {
                char t[16];
                int k = 0;
                while (n > 0) { t[k++] = '0' + (n % 10); n /= 10; }
                while (k-- > 0) buf[j++] = t[k];
            }
            buf[j] = 0;
            vga_print(buf);
            vga_print(" 09/05/2026 ");
            vga_print(dir->files[i].name);
            vga_putchar('\n');
        }
    }
}

int fs_cp(fs_dir* src_dir, const char* src, fs_dir* dst_dir, const char* dst) {
    fs_file* sf = find_file_in_dir(src_dir, src);
    if (!sf) return 0;
    if (!fs_mkfile(dst_dir, dst)) return 0;
    fs_file* df = find_file_in_dir(dst_dir, dst);
    if (!df) return 0;
    for (int i = 0; i <= sf->size && i < FS_MAX_CONTENT; i++)
        df->content[i] = sf->content[i];
    df->size = sf->size;
    return 1;
}

int fs_mv(fs_dir* src_dir, const char* src, fs_dir* dst_dir, const char* dst) {
    if (!fs_cp(src_dir, src, dst_dir, dst)) return 0;
    return fs_rmfile(src_dir, src);
}