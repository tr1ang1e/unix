// scandir()
#define _DEFAULT_SOURCE
#include <dirent.h>

// isdigit()
#include <ctype.h>

// open(), read()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include <stdio.h>
#include <string.h>

#define fd int

int filter (const struct dirent* entry)
{
  int result = 0;
  
  do
  {
    // filter non-digit name folders
    if (!isdigit(entry->d_name[0]))
    {
      break;
    };

    // cprepare to creating fuul path to process' "comm" file
    char filepath[256] = { 0 };
    strcpy(filepath, entry->d_name);
    
    // create path to process' "comm" file
    int idx = strlen(filepath);
    const char* comm_path = "/comm";
    strcpy(&filepath[idx], comm_path);

    // open "comm" file
    fd comm = open(filepath, O_RDONLY);
    if (comm == -1)
    {
      break;
    }

    // read process' name
    char process[256] = { 0 };
    idx = 0;
    while (read(comm, &process[idx], 1))
      ++idx;

    // debug info
    // printf("idx %d : len %ld : %s", idx, strlen(process), process);

    // remove '\n' symbol
    process[idx - 1] = '\0';

    // compare process' name
    const char* expected = "genenv";
    if (strcmp(process, expected) == 0)
    {
      // printf("%s\n", process);
      result = 1;
    }
      
  } while (0);

  return result;
}

int main(int argc, char* argv[])
{ 
  chdir("/proc/");

  struct dirent** namelist;
  int denum;       // directory elements number
  int count = 0;   // counter for appropriate processes

  // directory scan and filter 
  denum = scandir(".", &namelist, filter, alphasort);
  if (denum == -1) 
  {
      perror("scandir");
      exit(EXIT_FAILURE);
  }

  // count result number and free memory
  while (denum--) 
  {
    ++count;
    free(namelist[denum]);
  }
  free(namelist);

  // print answer
  printf("%d\n", count);

  return 0;
}