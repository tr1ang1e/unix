#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <dlfcn.h>

#include <sys/wait.h>
#include <errno.h>

#include "../../include/clean_code_utils.h"

int main(int argc, char* argv[])
{ 

  // fork() = create new process and check return codes in both
  pid_t pid = fork();                 
  /*        ^
   * execution of new process starts with assign command ( operator= )
   * in parent : pid = pid of child process
   * in child  : pid = 0
   * 
   */

  // each of processes print message 
  pid ? TTRACEM("Hi from parent") : TTRACEM("Hi from child");
  
  /* execve() = replace current process with new 
   * variants:
   *   - binary file
   *   - script (see script.py to know about requirements)
   */
  if (!pid)    
  {
    sleep(5);  // imitation of we need to wait child process
    
    // replace child process with script execution
    char* execArgs[] = { "./script.py", "execve() succeded", NULL };    // execArgs[0] is always replaced by scriptpath (make sence to write it explicit)
    char* execEnvi[] = { NULL };                                        // both of arrays must end with NULL pointer
    execve("./script.py", execArgs, execEnvi);                          // if execve() failed, errno will be set in apropriate value
    perror("execve() failed");                                          // this error massege causes we are still in child process (not in script)
  }
  else    // in parent process we might wait child is ended and get return code
  {
    int waitReturnValue;                    // return code of child process
    waitpid(pid, &waitReturnValue, 0);      // man 2 waitpid
    TTRACEN(waitReturnValue);
  }

  return 0;
}