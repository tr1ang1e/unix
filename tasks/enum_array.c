#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <dlfcn.h>

#include "../include/clean_code_utils.h"

typedef enum INDECIES
{
  ZERO = 0 ,
  FIRST    ,
  SECOND   ,
  THIRD    ,
  AMOUNT   
} INDECIES;

int main(int argc, char* argv[])
{ 

  int numbers[] = 
  {
    [ZERO]  = 1,
    [FIRST] = 1,
    [THIRD] = 1,
  };

  TTRACEN(numbers[SECOND]);
  TTRACEN(numbers[THIRD]);

  const char* strings[] =
  {
    [SECOND] = "half " "one more half" "\n",
  };

  TTRACEM(strings[SECOND]);

  return 0;
}