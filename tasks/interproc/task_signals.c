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

int sig1 = 0;
int sig2 = 0;
int answer = 0;

void sigusr1(int sigNumber) 
{
  ++sig1;
}

void sigusr2(int sigNumber)
{
  ++sig2;
}

void sigterm(int sigNumber)
{
  printf("%d %d\n", sig1, sig2);
}

void sigurg(int sigNumber)
{
  printf("%d\n", answer);
  exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
  pid_t answer = setsid();

  umask(0);    
  chdir("/");  

  signal(SIGUSR1, sigusr1);  
  signal(SIGUSR2, sigusr2);  
  signal(SIGTERM, sigterm);
  signal(SIGURG,  sigurg);

  while ( true )
  {
    pause();
  }

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  return 0;
}