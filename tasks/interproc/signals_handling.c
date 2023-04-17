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

void sig_handler(int sigNumber)  // define signal handler
{
  TTRACEMN("\nsignal got", sigNumber);
  exit(0);
}


int main(int argc, char* argv[])
{
  // init signal handler
  // must be init before signal might be really got
  signal(SIGINT, sig_handler);  
  
  while ( true )
  {
    TTRACEM("work is doing");
    usleep(500000);
  }

  // signal initiated here would never be actualy initiated
  // signal(SIGINT, sig_handler);

  return 0;
}