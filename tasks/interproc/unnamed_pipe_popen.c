#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "../../include/clean_code_utils.h"

int main(int argc, char* argv[])
{
  // preparing for pipe creation
  const char* start_pipe = "ls -la";
  const char* pipe_type = "r";

  /* open unnamed pipe
   * 
   * popen is high-level function, which combines:
   * > pipe() -->- creating unnamed pipe
   * > fork() -->- child process
   * > exec() -->- replace child with shell
   * 
   * return value is FILE* = pointer to stream, owned by pipe (not pipe identificator as is)
   *
   * */
  file_d read_pipe = popen(start_pipe, pipe_type);

  // prepare for reading
  char* buffer = NULL;
  int result = 0;
  size_t line_size = 0;
  int entries_number = -1;  // first string of 'ls -la' output is 'total ...'

  while( result = getline(&buffer, &line_size, read_pipe) != -1 )
  {
    printf("%s", buffer);  // unchaneged output = just the example
    ++entries_number;
  }

  if ( result ) perror("getline()");
  else TTRACEMN("entries_number", entries_number);
 
  free(buffer);
  return 0;
}