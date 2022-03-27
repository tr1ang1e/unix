#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
  // prepare child programm command
  char child[BUFSIZ] = { 0 };
  int idx = 0;
  int i;
  for (i = 1; i < argc; ++i)
  {
    strcpy(&child[idx], argv[i]);
    idx += strlen(argv[i]);
    child[idx] = ' ';
    ++idx;
  }
  
  // preparing for pipe creation
  const char* start_pipe = child;
  const char* pipe_type = "r";

  FILE* read_pipe = popen(start_pipe, pipe_type);

  // read from output
  char ch;
  int counter = 0;
  while( fread(&ch, 1, 1, read_pipe) )
  {
    if (ch == '0') ++counter;    // EOF automatically closes pipe - no need to handle it
  }

  // anwer
  printf("%d\n", counter);
}