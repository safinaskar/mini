#ifndef _MINI_GETDELIM_H
#define _MINI_GETDELIM_H

#include <stdio.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* I use ptrdiff_t and not ssize_t, because Windows doesn't support ssize_t */
ptrdiff_t _mini_getdelim (char **lineptr, size_t *n, int delimiter, FILE *fp);
ptrdiff_t _mini_getline (char **lineptr, size_t *n, FILE *stream);

#ifdef __cplusplus
}
#endif

#endif
