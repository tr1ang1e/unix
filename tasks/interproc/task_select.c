// open(), read()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// umask()
#include <sys/stat.h>
#include <sys/types.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>

int main(int argc, char* argv[])
{
  // open pipes
  int f1 = open("./in1", O_RDONLY);
  int f2 = open("./in2", O_RDONLY);

  // prepare reading
  char buf1[2] = { 0 };
  char buf2[2] = { 0 };
  int answer = 0;

  // set descriptors bits of set to tracked state (alternative to FD_SET(...))
  fd_set read_set_template, read_set;
  *read_set_template.__fds_bits |= (1 << f1) | (1 << f2);

  while (f1 || f2)
  {
    // reset set
    read_set = read_set_template;

    // select() will block programm till f1 or f2 are not available for reading
    int result = select((f1 > f2 ? f1 : f2) + 1, &read_set, NULL, NULL, NULL);

    // logic if select not blocked
    if (result > 0)
    {
      // handle 1st descriptor if fifo is not empty
      if (f1 && FD_ISSET(f1, &read_set))
      {
        if (read(f1, buf1, 1))
        {
          answer += atoi(&buf1);
        }
        else
        {
          *read_set_template.__fds_bits &= ~(1 << f2);
          close(f1);
          f1 = 0;
        }
      }

      // handle 2nd descriptor if fifo is not empty
      if (f2 && FD_ISSET(f2, &read_set))
      {
        if (read(f2, buf2, 1))
        {
          answer += atoi(&buf2);
        }
        else
        {
          *read_set_template.__fds_bits &= ~(1 << f2);
          close(f2);
          f2 = 0;
        }
      }
    }
  }
  
  printf("%d\n", answer);

  exit(EXIT_SUCCESS);
}