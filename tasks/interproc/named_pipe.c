// open(), read()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "../../include/clean_code_utils.h"

int main(int argc, char* argv[])
{
  // named pipe would be created in thr current process directory
  char fifo[MAX_BUF_LENGTH] = { 0 };
  char* resultc = getcwd(fifo, MAX_BUF_LENGTH);

  // add name to path (path is without last '/' so we have to include it to the name)
  const char* name = "/fifo";
  strcpy(&fifo[strlen(fifo)], name);

  TTRACEM(fifo);

  // creating named pipe
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  int resulti = mkfifo(fifo, mode);
  
  if(resulti == -1)
  {
    perror("mkfifo()");
  }
  else  // read from fifo logic
  {
    size_t count = 0;
    char buffer[MAX_BUF_LENGTH] = { 0 };    // read to this buffer
    int fifo_fd = open(fifo, O_RDWR);       // read for read from buffer, write for prevent buffer closes after EOF of real writer

    while ( resulti = read(fifo_fd, buffer, MAX_BUF_LENGTH) )
    {
      if (resulti != -1)
      {
        // way to stop programm
        if(resulti == 1) break;

        // aim logic
        TTRACEMN("input length", resulti);
        memset(buffer, 0, resulti);
      }
      else
      {
        perror("read()");
        break;
      }
    }
  }
  
  unlink(fifo);    // file will be deleted after the last consumer leavs

  return 0;
}