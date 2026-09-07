#ifndef STRING_H
#define STRING_H
#include <stddef.h>
size_t strlen(const char* s);
int strcmp(const char* a, const char* b);
int strncmp(const char* a, const char* b, size_t n);
int strcasecmp(const char* a, const char* b);
char* strcpy(char* dst, const char* src);
char* strncpy(char* dst, const char* src, size_t n);
char* strcat(char* dst, const char* src);
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
char* strstr(const char* haystack, const char* needle);
int strtol(const char* s, char** end, int base);
void* memset(void* dst, int c, size_t n);
void* memcpy(void* dst, const void* src, size_t n);
int memcmp(const void* a, const void* b, size_t n);
char* strndup(const char* s, size_t n);
char* strdup(const char* s);
int isdigit(int c);
int isalpha(int c);
int isspace(int c);
int tolower(int c);
int toupper(int c);
int atoi(const char* s);
#endif
