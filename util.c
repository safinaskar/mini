#define _POSIX_C_SOURCE 200809L /* Because I want to get all poll constants */

#include <stdio.h>

#include <poll.h>

struct int_str
{
  int i;
  const char *s;
};

static void
print_ored (struct int_str vals[], int val)
{
  int print_pipe = 0;
  int remains = val;

  for (int i = 0; vals[i].s != NULL; ++i)
    {
      if (vals[i].i == 0)
        {
          continue;
        }

      if (~(val | ~vals[i].i) == 0)
        {
          if (print_pipe)
            {
              printf (" | ");
            }
          else
            {
              print_pipe = 1;
            }

          printf ("%s", vals[i].s);

          remains &= ~vals[i].i;
        }
    }

  if (remains == 0)
    {
      if (!print_pipe)
        {
          printf ("0");
        }
    }
  else
    {
      if (print_pipe)
        {
          printf (" | ");
        }

      printf ("%d", remains);
    }
}

#define IS(i) {i, #i}

void
mini_poll (struct pollfd fds[], nfds_t nfds, int timeout)
{
  struct int_str poll_ored[] = {
    IS (POLLIN),
    IS (POLLRDNORM),
    IS (POLLRDBAND),
    IS (POLLPRI),
    IS (POLLOUT),
    IS (POLLWRNORM),
    IS (POLLWRBAND),
    IS (POLLERR),
    IS (POLLHUP),
    IS (POLLNVAL),
    {0}
  };

  int result = poll (fds, nfds, timeout);

  if (result == -1)
    {
      perror ("mini: mini_poll: poll (...) failed");
      return;
    }

  printf ("poll (...) = %d\n", result);

  for (nfds_t i = 0; i != nfds; ++i)
    {
      printf ("fds[%d] = {fd = %d, events = ", (int)i, fds[i].fd);
      print_ored (poll_ored, fds[i].events);
      printf (", revents = ");
      print_ored (poll_ored, fds[i].revents);
      printf ("}\n");
    }
}
