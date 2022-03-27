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

#include "../../include/clean_code_utils.h"

void read_and_report(int fd)
{
  TTRACEMN("FD is ready to read", fd);
  
  char buff[BUFSIZ];
  int bytes = read(fd, buff, BUFSIZ - 1);
  buff[bytes] = 0;
  
  TTRACEMN("Bytes were read", bytes);
}

int main(int argc, char* argv[])
{
  // named pipes names
  const char* fifo_1 = "./f1.fifo";
  const char* fifo_2 = "./f2.fifo";

  // create named pipes
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  int result1 = mkfifo(fifo_1, mode);
  int result2 = mkfifo(fifo_2, mode);
  if (result1 == -1 || result2 == -1)
  {
    TTRACEM("mkfifo() error");
    exit (EXIT_FAILURE);
  }

  // open pipes
  int f1 = open(fifo_1, O_RDWR);
  int f2 = open(fifo_2, O_RDWR);
  if (f1 == -1 || f2 == -1)
  {
    TTRACEM("open() error");
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

    // logic if select not blocked
    if (result == -1 )
    {
      TTRACEM("select() error");
      exit(EXIT_FAILURE);
    }
    else
    {
      if (FD_ISSET(f1, &read_set)) read_and_report(f1);
      if (FD_ISSET(f2, &read_set)) read_and_report(f2);
    }
  }

  exit(EXIT_SUCCESS);
}