/* Freestanding stdlib.h for KN5000 */
#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>

void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void free(void *ptr);
void exit(int status);

int rand(void);
void srand(unsigned int seed);

#endif /* _STDLIB_H */
