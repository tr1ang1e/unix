// clone()
#define _GNU_SOURCE
#include <sched.h>

// umask()
#include <sys/stat.h>
#include <sys/types.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#include "../include/clean_code_utils.h"

static int daemon_logic()
{
  while (true)
  {
    sleep(1);
  }

  return 0;
}

int main(int argc, char* argv[])
{ 
  pid_t daemon_pid = fork();

  if (daemon_pid == -1) exit(EXIT_FAILURE);    // fork failed
  if (daemon_pid > 0)   exit(EXIT_SUCCESS);    // fork succeed, terminate parent process
  

  /**********************************************************************************
   *    frome now on we are in the process we would transform to the deamon         *
   **********************************************************************************/


  // the current process becomes session leader
  daemon_pid = setsid();
  if (daemon_pid == -1)  exit(EXIT_FAILURE);
  else                   TTRACEMN("setsid secceed, new sid/pid", daemon_pid);

  // set umask and change directory
  umask(0);      // all permissions
  chdir("/");    // root directory

  // signals handlers
  // TODO

  // close what should be closed
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  // call function which implements our daemon's logic
  return daemon_logic();
}