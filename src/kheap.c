#include "kheap.h"
#include "multiboot.h"
#include "string.h"

#define HEAP_START 0x1000000ULL
#define HEAP_MAX   0x2000000ULL

#ifdef UNIT_TEST
static unsigned char heap_storage[4 * 1024 * 1024];
#endif

static unsigned char* heap_ptr;
static size_t used;
static size_t total;

void kheap_init(void) {
#ifndef UNIT_TEST
    heap_ptr = (unsigned char*)HEAP_START;
#else
    heap_ptr = heap_storage;
#endif
    used = 0;
    total = HEAP_MAX - HEAP_START;
    if (total_mem_kb) {
        uint64_t mem = (uint64_t)total_mem_kb * 1024;
        if (mem > HEAP_START + total) {
        } else if (mem > HEAP_START + 0x100000) {
            total = mem - HEAP_START - 0x100000;
        } else {
            total = 0x100000;
        }
    }
}

static size_t align_up(size_t x, size_t a) {
    return (x + a - 1) & ~(a - 1);
}

void* kmalloc(size_t n) {
    n = align_up(n, 16);
    if (used + n > total) return 0;
    void* p = heap_ptr + used;
    used += n;
    return p;
}

void* kzalloc(size_t n) {
    void* p = kmalloc(n);
    if (p) memset(p, 0, n);
    return p;
}

void kfree(void* p) {
    (void)p;
}

void* krealloc(void* p, size_t n) {
    void* np = kmalloc(n);
    if (!np) return 0;
    if (p) {
        size_t copy = n;
        unsigned char* a = p;
        unsigned char* b = np;
        for (size_t i = 0; i < copy; i++) b[i] = a[i];
    }
    return np;
}

size_t kheap_used(void) { return used; }
size_t kheap_total(void) { return total; }
