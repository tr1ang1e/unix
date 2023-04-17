// number of system calls 'read' and 'write'

#include <stdio.h>  
  
int main()
{
  char name[100];
  printf("What is your name? __");
  gets(name);
  return printf("Hello %s\n",name);
}

// answer = 4
// gcc task_strace.c
// strace -o answer.txt -e trace=read,write ./a.out
// cat answer.txt