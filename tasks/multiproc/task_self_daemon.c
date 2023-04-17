// scandir()
#define _DEFAULT_SOURCE
#include <dirent.h>

// clone()
#define _GNU_SOURCE
#include <sched.h>

// isdigit()
#include <ctype.h>

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

int main(int argc, char* argv[])
{ 
  // the current process becomes session leader
  pid_t new_pid = setsid();
  if (new_pid == -1)  exit(EXIT_FAILURE);
  else printf("%d\n", new_pid);

  // set umask and change directory
  umask(0);      // all permissions
  chdir("/");    // root directory

  // close what should be closed
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  sleep(100);   // time for daemon to work (task succeed requirement)

  exit(EXIT_SUCCESS);
}