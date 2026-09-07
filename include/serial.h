#ifndef SERIAL_H
#define SERIAL_H
void serial_init(void);
void serial_putchar(char c);
void serial_print(const char* s);
void serial_printf(const char* fmt, ...);
#endif
