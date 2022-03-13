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

#include "../../include/clean_code_utils.h"

int main(int argc, char* argv[])
{
  /* array of 2 pipe descriptors 
   * see `man 2 pipe`

   * [0] = output (read)
   * [1] = input (write)
   *
   * > pipe = one-directional
   * > to establish two-directional communication,
   * necessary to create 2 [in] + 2 [out] descriptors 
   * 
   **/
  pipe_d  pipe_descriptors[2] = { 0 };
  pipe_d* pipe_r = &pipe_descriptors[0];
  pipe_d* pipe_w = &pipe_descriptors[1];

  pid_t pid; 
  char buff[BUFSIZ];

  // create unnamed pipe
  int result = pipe(pipe_descriptors);
  if (result == -1)
  {
    perror("pipe()");
    exit(EXIT_FAILURE);
  }

  pid = fork();
  if(pid == -1)
  {
    perror("fork()");
    exit(EXIT_FAILURE);
  } 

  if (pid > 0)    // parent = write to pipe
  {
    close(*pipe_r);

    for (int i = 0; i < 3; ++i)
    {
      scanf("%s", buff);
      write(*pipe_w, buff, strlen(buff));
    }

    close(*pipe_w);  

    int waitReturnValue;
    waitpid(pid, &waitReturnValue, 0);
    exit(EXIT_SUCCESS);
  }

  else    // child = read from pipe
  {
    close(*pipe_w);

    int idx = 0;
    while ( read(*pipe_r, &buff[idx], 1) > 0 )    // read is blocked, if pipe is empty
    {
      ++idx;

      if (buff[idx - 1] == '.')
      {
        // use write() not printf() just for training
        write(STDOUT_FILENO, buff, strlen(buff) - 1);    
        write(STDOUT_FILENO, "\n", 1); 

        // reset buffer
        memset(buff, 0, idx);
        idx = 0;
      }
    }

    close(*pipe_r);
    exit(EXIT_SUCCESS);
  }
}