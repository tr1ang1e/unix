// scandir()
#define _DEFAULT_SOURCE
#include <dirent.h>

// clone()
#define _GNU_SOURCE
#include <sched.h>

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

#include "include/clean_code_utils.h"

void read_and_report(int fd)
{
  TTRACEMN("FD is ready to read :", fd);
  
  char buff[BUFSIZ];
  int bytes = read(fd, buff, BUFSIZ);
  buf[bytes] = 0;
  
  TTRACEMN("Bytes were read :", bytes);
}

int main(int argc, char* argv[])
{
  int f1 = open("./f1.fifo", O_RDWR);
  int f2 = open("./f2.fifo", O_RDWR);

  if (f1 == -1 || f2 == -1)
  {
    exit(EXIT_FAILURE);
  }
  else
  {
    TTRACEMN("file descriptor 1", f1);
    TTRACEMN("file descriptor 2", f2);
  }

  fd_set read_set;

  while(true)
  {
    // reset set
    FD_ZERO(&read_set);

    // set descriptors bits of set to tracked state
    FD_SET(f1, &read_set);
    FD_SET(f2, &read_set);

    // select() will block programm till f1 or f2 are not available for reading
    int result = select(f2 + 1, &read_set, NULL, NULL, NULL);
  }

  exit(EXIT_SUCCESS);
}