#ifndef VFS_H
#define VFS_H

#define FS_MAX_FILES 16
#define FS_MAX_DIRS 4
#define FS_MAX_NAME 48
#define FS_MAX_CONTENT 512

typedef struct fs_file {
    char name[FS_MAX_NAME];
    char content[FS_MAX_CONTENT];
    int size;
    int used;
} fs_file;

typedef struct fs_dir {
    char name[FS_MAX_NAME];
    fs_file files[FS_MAX_FILES];
    int file_count;
    struct fs_dir* parent;
    struct fs_dir* subdirs[FS_MAX_DIRS];
    int subdir_count;
    int used;
} fs_dir;

extern fs_dir fs_root;
extern fs_dir* fs_current;
extern char fs_path[256];

void fs_init(void);
int fs_mkfile(fs_dir* dir, const char* name);
int fs_rmfile(fs_dir* dir, const char* name);
char* fs_cat(fs_dir* dir, const char* name);
int fs_write(fs_dir* dir, const char* name, const char* data);
int fs_append(fs_dir* dir, const char* name, const char* data);
fs_file* fs_find_file(fs_dir* dir, const char* name);
fs_dir* fs_find_dir(fs_dir* dir, const char* name);
int fs_mkdir(fs_dir* dir, const char* name);
int fs_rmdir(fs_dir* dir, const char* name);
void fs_pwd(void);
int fs_cd(const char* path);
void fs_ls(fs_dir* dir);
void fs_ll(fs_dir* dir);
int fs_cp(fs_dir* src_dir, const char* src, fs_dir* dst_dir, const char* dst);
int fs_mv(fs_dir* src_dir, const char* src, fs_dir* dst_dir, const char* dst);

#endif
