// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>

#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "../../include/clean_code_utils.h"

int main()
{
  // create pipe to send shm_id from one process to another
  pipe_d pipe_descriptors[2] = { 0 };
  pipe_d* pipe_r = &pipe_descriptors[0];
  pipe_d* pipe_w = &pipe_descriptors[1];

  // prepare fork() and communication
  pid_t pid;
  shm_d shm;
  char buff[BUFSIZ];

  int status = EXIT_FAILURE;
  do
  {
    // create unnamed pipe
    if ( pipe(pipe_descriptors) == -1 )
    {
      perror("pipe()");
      break;
    }

    // create child process
    pid = fork();
    if (pid == -1)
    {
      perror("fork()");
      break;
    }

    if (pid > 0)    // parent
    {
      close(*pipe_r);

      // prepare shmget() args
      key_t key = ftok("sharedmemory", 1);    // ftok() get key = not necessary 
      int pageSize = getpagesize();
      flags_t flags = IPC_CREAT | IPC_EXCL;
      int permissions = 384;                  // 110 000 000 or '600' = 9 least significant bits of flags 

      // create shared memory
      shm = shmget(key, pageSize, flags | permissions);
      if (shm == -1)
      {
        perror("shmget()");
        break;
      }

      // attach to shared memory
      void* shm_ptr = shmat(shm, NULL, 0);
      if ( (long long int)shm_ptr == -1 )
      {
        perror("parent, shmat()");
        break;
      }

      *(int*)shm_ptr = 42;    // use shared memory

      write(*pipe_w, &shm, sizeof(shm_d));    // send shared memory descriptor
      close(*pipe_w); 

      // wait for child process 
      int waitReturnValue;
      waitpid(pid, &waitReturnValue, 0);
    }

    
  /********************************************************
   *            child process from here ...               *
   ********************************************************/

    else // child process
    {
      int result;
      do
      {
        close(*pipe_w);
        
        // get shm identificator
        result = read(*pipe_r, &shm, sizeof(shm_d));
        if (result == -1)
        {
          perror("read()");
          break;
        }
        close(*pipe_r);

        void* shm_ptr = shmat(shm, NULL, 0);
        if ( (long long int)shm_ptr == -1 )
        {
          perror("child, shmat()");
          break;
        }

        result = *(int*)shm_ptr;
        if (result == 42)
        {
          TTRACEM("shared memory usage succeeded");
        }

        status = EXIT_SUCCESS;
      } while (0);

      exit(status);
    }

   /********************************************************
    *                   ... to here                        *
    ********************************************************/

    status = EXIT_SUCCESS;
  } while(0);

  // release shared memory
  if ( shmctl(shm, IPC_RMID, NULL) == -1 )
  {
    perror("shmctl()");
    status = EXIT_FAILURE;
  }

  exit(status);
}