#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <libsh.h>
#include <libsh/cxx.hpp>

#include "mini.h"

using namespace std::literals;

static FILE *use(int fd, FILE *standard_stream, const char *mode)
{
  if (fd != -1 && fd == fileno(standard_stream)){
    return standard_stream;
  }else
    {
      FILE *result = fdopen(fd, mode);
      if (result == NULL){
        fprintf(stderr, "mini: %d: cannot use file descriptor: %s\n", fd, strerror(errno));
        exit (EXIT_FAILURE);
      }
      return result;
    }
}

int main(int, char *argv[])
{
  sh_init (argv[0]);
  sh_arg_parse (&argv, "Usage: "s + sh_get_program () + " [OPTION]... IN-FD OUT-FD ERR-FD\n"s, "");

  int in_fd = fileno(stdin);
  int out_fd = fileno(stdout);
  int err_fd = fileno(stderr);

  if (argv[0] != NULL)
    {
      in_fd = sh_long2int (sh_xx_strtol (argv[0], 0));
      ++argv;

      if (argv[0] != NULL)
        {
          out_fd = sh_long2int (sh_xx_strtol (argv[0], 0));
          ++argv;

          if (argv[0] != NULL)
            {
              err_fd = sh_long2int (sh_xx_strtol (argv[0], 0));
              ++argv;
            }
        }
    }

  sh_arg_end (argv);

  mini (use(in_fd, stdin, "r"), use(out_fd, stdout, "w"), use(err_fd, stderr, "w"));

  exit (EXIT_SUCCESS);
}
