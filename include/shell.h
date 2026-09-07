#ifndef SHELL_H
#define SHELL_H
#include <stddef.h>
typedef struct {
    int argc;
    char* argv[32];
} args_t;

typedef int (*cmd_fn_t)(args_t* a);

typedef struct {
    const char* name;
    cmd_fn_t fn;
    const char* help;
} cmd_entry_t;

extern const cmd_entry_t commands[];
const cmd_entry_t* cmd_lookup(const char* name);

void shell_init(void);
void shell_run(void);
int shell_exec_line(char* line);
int shell_run_builtin(args_t* a);

void sh_putc(char c);
void sh_puts(const char* s);
void sh_write(const char* s, size_t n);
void sh_printf(const char* fmt, ...);
int sh_getc(void);
int sh_readline(char* buf, int max);
int sh_redirect(void);
void sh_restore(void);
char* shell_get_history(int i);
int shell_history_count(void);
void shell_add_history(const char* s);

void sh_set_exit(int code);
int sh_last_exit(void);

extern char shell_hostname[32];
#endif
