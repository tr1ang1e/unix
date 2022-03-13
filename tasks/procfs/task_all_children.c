#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define fd FILE*

void find_children(const char* parent, int* number)
{
  ++(*number);  // increment common counter

  // generate path to file with children enumeration
  char children_path[256] = { 0 };
  sprintf(children_path, "/proc/%s/task/%s/children", parent, parent);

  char children_buff[1024] = { 0 };    // buffer for read file
  int idx = 0;
  
  fd file = fopen(children_path, "r");              
  while(fread(&children_buff[idx], 1, 1, file))    // read bytes one by one
    ++idx;

  // printf("%d : %s : %s\n", *number, parent, children_buff);   // debug

  // split buffer into tokens (1 token = 1 pid)
  char tokens[256][8];
  idx = 0;
  char* token = strtok(children_buff, " ");
  while (token != NULL)
  {
    strcpy(tokens[idx], token);
    ++idx;
    token = strtok(NULL, " ");  
  }

  // recursive call for every pid
  int i = 0;
  for (i = 0; i < 64; ++i)
  {
    if(tokens[i][0] == '\0')
      break;

    find_children(&tokens[i][0], number);
  }

  fclose(file);
}

int main(int argc, char* argv[])
{ 
  int number = 0;    // arg process is included

  find_children(argv[1], &number);

  printf("%d\n", number);

  return 0;
}