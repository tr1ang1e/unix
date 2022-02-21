// clone()
#define _GNU_SOURCE
#include <sched.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#include "../include/clean_code_utils.h"


#define CHILD_STACK_SIZE (10000)            // define stack size
char CHILD_STACK[CHILD_STACK_SIZE + 1];     // alloc memory for child stack


typedef struct CLONE_ARGS_S 
{
  int number;
  const char* message;

} CLONE_ARGS;

int function(void* args)
{
  int number = ((CLONE_ARGS*)args)->number;
  const char* message = ((CLONE_ARGS*)args)->message;
  
  TTRACEMN("start clone process, pid", getpid());

  for (int i = 0; i < number; ++i)
  {
    TTRACEM(message);
    sleep(1);
  }

  return 0;
}

int main(int argc, char* argv[])
{ 
  CLONE_ARGS args = {10, "clone loop"};

/* example, use different flags
   * flags type are necessary unsigned int
   *
   * SIGCHILD  =  if [off] , wait() function doesn't make sence
   * to check effect:
   *   > comment CLONE_PARENT flag
   *   > comment loop in main
   * 
   * CLONE_PARENT  =  if [on] , child would have parent's parent as its own
   * to check effect:
   *   > uncomment CLONE_PARENT flag
   *   > uncomment loop in main
   *   ~ bash $$ (in terminal which processe are run from, <pid>)
   *   ~ pstree <pid> -p     
   * 
   */ 
  flags_t flags = SIGCHLD;  // | CLONE_PARENT;
  pid_t pid = clone(function, CHILD_STACK + CHILD_STACK_SIZE, flags, (void*)&args);

  // for (int i = 0; i < args.number; ++i)
  // {
  //   TTRACEM("parent");
  //   usleep(500000);
  // }

  if (pid)
  {
    TTRACEMN("wait for clone process, pid", pid);
    int waitReturnValue = 1;
    pid_t r = waitpid(pid, &waitReturnValue, 0);
    TTRACEMN("clone process return value", waitReturnValue);
  }

  return 0;
}