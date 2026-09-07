#include "vfs.h"
#include "kheap.h"
#include "string.h"

vnode_t* vfs_root;
vnode_t* vfs_cwd;
char vfs_pwd[1024];

static vnode_t* make_node(int type, const char* name, vnode_t* parent) {
    vnode_t* n = kzalloc(sizeof(vnode_t));
    if (!n) return 0;
    n->type = type;
    n->parent = parent;
    strncpy(n->name, name, VFS_NAME_MAX - 1);
    n->children = 0;
    n->child_count = 0;
    n->child_cap = 0;
    n->data = 0;
    n->size = 0;
    n->cap = 0;
    return n;
}

static vnode_t* child_of(vnode_t* dir, const char* name) {
    if (!dir || dir->type != FS_TYPE_DIR) return 0;
    for (int i = 0; i < dir->child_count; i++) {
        if (strcmp(dir->children[i]->name, name) == 0) return dir->children[i];
    }
    return 0;
}

static int add_child(vnode_t* dir, vnode_t* child) {
    if (dir->child_count >= dir->child_cap) {
        int newcap = dir->child_cap ? dir->child_cap * 2 : 8;
        vnode_t** nc = kmalloc(newcap * sizeof(vnode_t*));
        if (!nc) return 0;
        for (int i = 0; i < dir->child_count; i++) nc[i] = dir->children[i];
        dir->children = nc;
        dir->child_cap = newcap;
    }
    dir->children[dir->child_count++] = child;
    return 1;
}

int vfs_is_abs(const char* p) { return p[0] == '/'; }

const char* vfs_basename(const char* path) {
    const char* b = path;
    for (const char* p = path; *p; p++)
        if (*p == '/') b = p + 1;
    return b;
}

vnode_t* vfs_resolve(const char* path) {
    if (!path || !path[0]) return vfs_cwd;
    vnode_t* cur = path[0] == '/' ? vfs_root : vfs_cwd;
    const char* p = path;
    while (*p) {
        while (*p == '/') p++;
        if (!*p) break;
        char name[VFS_NAME_MAX];
        int i = 0;
        while (*p && *p != '/') {
            if (i < VFS_NAME_MAX - 1) name[i++] = *p;
            p++;
        }
        name[i] = 0;
        if (strcmp(name, ".") == 0) continue;
        if (strcmp(name, "..") == 0) {
            if (cur->parent) cur = cur->parent;
            continue;
        }
        if (strcmp(name, "~") == 0) {
            vnode_t* home = child_of(vfs_root, "root");
            if (!home) return 0;
            cur = home;
            continue;
        }
        vnode_t* c = child_of(cur, name);
        if (!c) return 0;
        cur = c;
    }
    return cur;
}

vnode_t* vfs_resolve_parent(const char* path, char* out_name) {
    char buf[1024];
    strncpy(buf, path, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = 0;
    char* slash = strrchr(buf, '/');
    if (!slash) {
        if (out_name) strcpy(out_name, buf);
        return vfs_cwd;
    }
    const char* name = slash + 1;
    if (out_name) strcpy(out_name, name[0] ? name : "/");
    if (slash == buf) return vfs_root;
    *slash = 0;
    return vfs_resolve(buf[0] ? buf : "/");
}

int vfs_write_file(vnode_t* n, const char* data, size_t len) {
    if (!n || n->type != FS_TYPE_FILE) return 0;
    if (len + 1 > n->cap) {
        size_t nc = n->cap ? n->cap : 64;
        while (nc <= len) nc *= 2;
        char* nd = kmalloc(nc);
        if (!nd) return 0;
        if (n->data && n->size) memcpy(nd, n->data, n->size);
        n->data = nd;
        n->cap = nc;
    }
    memcpy(n->data, data, len);
    n->size = len;
    n->data[len] = 0;
    return 1;
}

int vfs_append_file(vnode_t* n, const char* data, size_t len) {
    if (!n || n->type != FS_TYPE_FILE) return 0;
    size_t need = n->size + len + 1;
    if (need > n->cap) {
        size_t nc = n->cap ? n->cap : 64;
        while (nc < need) nc *= 2;
        char* nd = kmalloc(nc);
        if (!nd) return 0;
        if (n->data && n->size) memcpy(nd, n->data, n->size);
        n->data = nd;
        n->cap = nc;
    }
    memcpy(n->data + n->size, data, len);
    n->size += len;
    n->data[n->size] = 0;
    return 1;
}

void vfs_truncate(vnode_t* n) {
    if (n && n->type == FS_TYPE_FILE) {
        n->size = 0;
        if (n->data) n->data[0] = 0;
    }
}

int vfs_mkfile(const char* path) {
    char name[VFS_NAME_MAX];
    vnode_t* parent = vfs_resolve_parent(path, name);
    if (!parent || parent->type != FS_TYPE_DIR) return 0;
    if (!name[0] || child_of(parent, name)) return 0;
    vnode_t* n = make_node(FS_TYPE_FILE, name, parent);
    if (!n) return 0;
    return add_child(parent, n);
}

int vfs_mkdir(const char* path) {
    char name[VFS_NAME_MAX];
    vnode_t* parent = vfs_resolve_parent(path, name);
    if (!parent || parent->type != FS_TYPE_DIR) return 0;
    if (!name[0] || child_of(parent, name)) return 0;
    vnode_t* n = make_node(FS_TYPE_DIR, name, parent);
    if (!n) return 0;
    return add_child(parent, n);
}

int vfs_rm(const char* path) {
    vnode_t* n = vfs_resolve(path);
    if (!n || n == vfs_root || !n->parent) return 0;
    if (n->type == FS_TYPE_DIR && n->child_count > 0) return 0;
    return vfs_unlink_node(n);
}

int vfs_unlink_node(vnode_t* n) {
    if (!n || n == vfs_root || !n->parent) return 0;
    vnode_t* p = n->parent;
    for (int i = 0; i < p->child_count; i++) {
        if (p->children[i] == n) {
            for (int j = i; j < p->child_count - 1; j++)
                p->children[j] = p->children[j + 1];
            p->child_count--;
            return 1;
        }
    }
    return 0;
}

int vfs_rm_recursive_node(vnode_t* n) {
    if (!n || n == vfs_root) return 0;
    while (n->child_count > 0) {
        if (!vfs_rm_recursive_node(n->children[n->child_count - 1])) return 0;
    }
    return vfs_unlink_node(n);
}

int vfs_rm_recursive(const char* path) {
    vnode_t* n = vfs_resolve(path);
    if (!n || n == vfs_root) return 0;
    return vfs_rm_recursive_node(n);
}

static vnode_t* mkdir_simple(vnode_t* parent, const char* name) {
    vnode_t* n = make_node(FS_TYPE_DIR, name, parent);
    if (n) add_child(parent, n);
    return n;
}

static vnode_t* mkfile_simple(vnode_t* parent, const char* name, const char* content) {
    vnode_t* n = make_node(FS_TYPE_FILE, name, parent);
    if (!n) return 0;
    add_child(parent, n);
    if (content) vfs_write_file(n, content, strlen(content));
    return n;
}

void vfs_update_pwd(void) {
    if (vfs_cwd == vfs_root) {
        strcpy(vfs_pwd, "/");
        return;
    }
    char* names[64];
    int depth = 0;
    vnode_t* n = vfs_cwd;
    while (n && n != vfs_root && depth < 64) {
        names[depth++] = n->name;
        n = n->parent;
    }
    char buf[1024];
    buf[0] = 0;
    for (int i = depth - 1; i >= 0; i--) {
        strcat(buf, "/");
        strcat(buf, names[i]);
    }
    if (!buf[0]) strcpy(buf, "/");
    strcpy(vfs_pwd, buf);
}

void vfs_init(void) {
    vfs_root = make_node(FS_TYPE_DIR, "", 0);
    vfs_root->parent = vfs_root;
    vfs_cwd = vfs_root;

    vnode_t* bin = mkdir_simple(vfs_root, "bin");
    vnode_t* etc = mkdir_simple(vfs_root, "etc");
    vnode_t* home = mkdir_simple(vfs_root, "home");
    vnode_t* root = mkdir_simple(vfs_root, "root");
    vnode_t* tmp = mkdir_simple(vfs_root, "tmp");
    vnode_t* usr = mkdir_simple(vfs_root, "usr");
    mkdir_simple(usr, "bin");
    mkdir_simple(home, "user");

    mkfile_simple(etc, "motd", "Welcome to BojanOS - a from-scratch 64-bit operating system\n");
    mkfile_simple(etc, "hostname", "bojanos\n");
    mkfile_simple(etc, "os-release", "NAME=BojanOS\nVERSION=1.0\nID=bojanos\nPRETTY_NAME=\"BojanOS 1.0\"\n");
    mkfile_simple(root, "readme.txt",
        "BojanOS\n"
        "=======\n"
        "Everything you see was written from scratch for this OS:\n"
        " - 64-bit long mode kernel with paging, GDT, IDT, IRQs\n"
        " - PS/2 keyboard driver, PIT timer, CMOS RTC\n"
        " - in-RAM filesystem with directories and files\n"
        " - shell with pipes and redirections\n"
        "Type 'help' to list commands.\n");

    vfs_cwd = root;
    vfs_update_pwd();
    (void)bin; (void)tmp;
}
