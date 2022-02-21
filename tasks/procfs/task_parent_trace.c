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

#define fd FILE*

void find_parent(const char* child, char* parent)
{
  // set child process' directory
  char proc_dir[256] = "/proc/";
  int idx = 6;
  strcpy(&proc_dir[idx], child);
  
  // go to directory and open stat file
  chdir(proc_dir);
  fd stat = fopen("./stat", "r");

  // prepare variables to read from stat to
  int  pid;                       // unused
  char comm[] = { 0 };            // unused
  char state;                     // unused
  int  ppid;

  // parce all of fields to get ppid (fscanf work peculiarities)
  fscanf(stat, "%d %s %c %d", &pid, comm, &state, &ppid);    

  // write ppid value in got parent buffer
  sprintf(parent, "%d", ppid);  // sprintf() writes '\0' to the end
}

int main(int argc, char* argv[])
{ 
  // print process we start from 
  printf("%s\n", argv[1]);
  
  // generate first child
  char child[256] = { 0 };
  strcpy(child, argv[1]);

  // {  // debug (get pid from program / in task pid is given as an command line argument[1])
  // 
  //   pid_t pid = getpid();
  //   printf("%d\n", pid);
  //   sprintf(child, "%d", pid);
  // }

  const char* init = "1";         // the very parent process
  char parent[256] = { 0 };       // buffer for keeping parent

  while (strcmp(child, init))     // returns non-zero if not equal
  {
    find_parent(child, parent);   
    printf("%s\n", parent);
    strcpy(child, parent);        // got parent is new child
  }
  
  return 0;
}