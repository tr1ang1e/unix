#include <stdio.h>

int main(int argc, char** argv)
{
  if (!argc && argv[0] == NULL)
  {
    printf("no args \n");
  }
  else
  {
    printf("%s \n", argv[0]);
  }

  return 0;
}