#ifndef VFS_H
#define VFS_H
#include <stddef.h>
#define VFS_NAME_MAX 64
#define FS_TYPE_FILE 1
#define FS_TYPE_DIR  2

typedef struct vnode vnode_t;
struct vnode {
    int type;
    char name[VFS_NAME_MAX];
    vnode_t* parent;
    vnode_t** children;
    int child_count;
    int child_cap;
    char* data;
    size_t size;
    size_t cap;
};

extern vnode_t* vfs_root;
extern vnode_t* vfs_cwd;
extern char vfs_pwd[1024];

void vfs_init(void);
vnode_t* vfs_resolve(const char* path);
vnode_t* vfs_resolve_parent(const char* path, char* out_name);
int vfs_mkfile(const char* path);
int vfs_mkdir(const char* path);
int vfs_rm(const char* path);
int vfs_rm_recursive(const char* path);
int vfs_unlink_node(vnode_t* n);
int vfs_rm_recursive_node(vnode_t* n);
int vfs_write_file(vnode_t* n, const char* data, size_t len);
int vfs_append_file(vnode_t* n, const char* data, size_t len);
void vfs_truncate(vnode_t* n);
const char* vfs_basename(const char* path);
void vfs_update_pwd(void);
int vfs_is_abs(const char* p);
char* vfs_join(const char* dir, const char* name);
#endif
