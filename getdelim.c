/* getdelim.c --- Implementation of replacement getdelim function (Gnulib).
   Copyright (C) 1994, 1996-1998, 2001, 2003, 2005-2011 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.  */

/* Ported from glibc by Simon Josefsson. */

#include <stdio.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <errno.h>

#include "getdelim.h"

#ifndef SIZE_MAX
# define SIZE_MAX UINT_MAX
#endif

#ifndef SSIZE_MAX
# define SSIZE_MAX INT_MAX /* To allow this file to be compiled as C++ */
#endif

/* Read up to (and including) a DELIMITER from FP into *LINEPTR (and
   NUL-terminate it).  *LINEPTR is a pointer returned from malloc (or
   NULL), pointing to *N characters of space.  It is realloc'ed as
   necessary.  Returns the number of characters read (not including
   the null terminator), or -1 on error or EOF.  */

ptrdiff_t
_mini_getdelim (char **lineptr, size_t *n, int delimiter, FILE *fp)
{
  ptrdiff_t result;
  size_t cur_len = 0;

  if (lineptr == NULL || n == NULL || fp == NULL)
    {
      errno = EINVAL;
      return -1;
    }

  if (*lineptr == NULL || *n == 0)
    {
      char *new_lineptr;
      *n = 120;
      new_lineptr = (char *) realloc (*lineptr, *n);
      if (new_lineptr == NULL)
        {
          return -1;
        }
      *lineptr = new_lineptr;
    }

  for (;;)
    {
      int i;

      i = getc (fp);
      if (i == EOF)
        {
          result = -1;
          break;
        }

      /* Make enough space for len+1 (for final NUL) bytes.  */
      if (cur_len + 1 >= *n)
        {
          size_t needed_max =
            SSIZE_MAX < SIZE_MAX ? (size_t) SSIZE_MAX + 1 : SIZE_MAX;
          size_t needed = 2 * *n + 1;   /* Be generous. */
          char *new_lineptr;

          if (needed_max < needed)
            needed = needed_max;
          if (cur_len + 1 >= needed)
            {
              /* errno = EOVERFLOW; (there is no EOVERFLOW on Windows */
              errno = ERANGE;
              return -1;
            }

          new_lineptr = (char *) realloc (*lineptr, needed);
          if (new_lineptr == NULL)
            {
              return -1;
            }

          *lineptr = new_lineptr;
          *n = needed;
        }

      (*lineptr)[cur_len] = i;
      cur_len++;

      if (i == delimiter)
        break;
    }
  (*lineptr)[cur_len] = '\0';
  result = cur_len ? (ptrdiff_t)cur_len : result;

  return result;
}

ptrdiff_t
_mini_getline (char **lineptr, size_t *n, FILE *stream)
{
  return _mini_getdelim (lineptr, n, '\n', stream);
}
