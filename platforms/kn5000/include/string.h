/* Freestanding string.h for KN5000 */
#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);

#endif /* _STRING_H */
