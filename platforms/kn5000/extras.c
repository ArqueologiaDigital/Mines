/* extras.c: Freestanding C runtime for KN5000
 *
 * Provides: calloc, free, memset, memcpy, rand, srand
 * No standard library available - all reimplemented for bare metal.
 */

#include <stdint.h>
#include <stddef.h>

/* ========================================================================
 * XorShift PRNG (16-bit)
 * ======================================================================== */

static unsigned int rand_seed = 1;

int rand(void)
{
    rand_seed ^= rand_seed << 7;
    rand_seed ^= rand_seed >> 9;
    rand_seed ^= rand_seed << 8;
    return rand_seed;
}

void srand(unsigned int v) { rand_seed = v ? v : 1; }

/* ========================================================================
 * Memory functions
 * ======================================================================== */

void *memset(void *s, int c, size_t n)
{
    uint8_t *p = (uint8_t *)s;
    while (n--) {
        *p++ = (uint8_t)c;
    }
    return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

/* ========================================================================
 * Heap allocator (bump allocator with static heap)
 * ======================================================================== */

static char heap[4096];
static char *heap_ptr = heap;

void *malloc(size_t v)
{
    if (sizeof(heap) - (size_t)(heap_ptr - heap) < v)
        return (void *)0;

    char *old_heap_ptr = heap_ptr;
    heap_ptr += v;
    return old_heap_ptr;
}

void *calloc(size_t elem_size, size_t num)
{
    size_t alloc_size = elem_size * num;
    char *ptr = malloc(alloc_size);

    if (ptr) {
        memset(ptr, 0, alloc_size);
    }
    return ptr;
}

void free(void *ptr)
{
    /* No-op: bump allocator does not support freeing */
    (void)ptr;
}
