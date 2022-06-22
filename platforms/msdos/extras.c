#include <stdint.h>
#include <time.h>

static unsigned int rand_seed = 1;

int rand(void)
{
    /* 16-bit xorshift PRNG */
    rand_seed ^= rand_seed << 7;
    rand_seed ^= rand_seed >> 9;
    rand_seed ^= rand_seed << 8;
    return rand_seed;
}

void srand(unsigned int v) { rand_seed = v; }

time_t time(time_t *t)
{
    uint8_t high, low;

    asm __volatile("int $0x1a"
                   : "=c"(high), "=d"(low)
                   : "Rah"((char)0)
                   : "cc", "memory");

    time_t ret = (time_t)high << 16 | low;

    if (t)
        *t = ret;

    return ret;
}

void *malloc(size_t v)
{
    static char heap[4096];
    static char *heap_ptr = heap;
    char *old_heap_ptr;

    old_heap_ptr = heap_ptr;
    heap_ptr += v;

    if (heap_ptr > heap + sizeof(heap))
        return NULL;

    return old_heap_ptr;
}

void *calloc(size_t elem_size, size_t num)
{
    size_t alloc_size = elem_size * num;
    char *ptr = malloc(alloc_size);

    if (ptr) {
        for (size_t i = 0; i < alloc_size; i++) {
            ptr[i] = 0;
        }
    }

    return ptr;
}

void free(void *ptr) {
}

__asm__(".globl _start\n"
        "_start:\n"
        "   /* Clear the BSS */\n"
        "   xorw %ax, %ax\n"
        "   movw $__sbss, %di\n"
        "   movw $__lbss, %cx\n"
        "   cld\n"
        "   rep stosw\n"
        "   /* And we're ready! */\n"
        "   call main\n"
        ".globl exit\n"
        "exit:\n"
        "   movw $0x4c00, %ax\n"
        "   int $0x21\n");
