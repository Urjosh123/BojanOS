#ifndef KHEAP_H
#define KHEAP_H
#include <stddef.h>
void kheap_init(void);
void* kmalloc(size_t n);
void* kzalloc(size_t n);
void* krealloc(void* p, size_t n);
void kfree(void* p);
size_t kheap_used(void);
size_t kheap_total(void);
#endif
