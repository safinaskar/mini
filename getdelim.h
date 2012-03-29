#ifndef _MINI_STRTOK_R
#define _MINI_STRTOK_R

#include <stdio.h>
#include <stddef.h>

/* I use ptrdiff_t and not ssize_t, because Windows doesn't support ssize_t */
ptrdiff_t _mini_getdelim (char **lineptr, size_t *n, int delimiter, FILE *fp);
ptrdiff_t _mini_getline (char **lineptr, size_t *n, FILE *stream);

#endif
