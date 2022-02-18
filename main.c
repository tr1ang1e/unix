#include <stdbool.h>
#include <stdio.h>
#include <dlfcn.h>

#include "include/clean_code_utils.h"

static void (*funct)(const char*);     // create pointer to function

bool link_function(const char* libpath, const char* functname)
{
  bool result = false;

  do
  {
    void* lib = get_library_handle(libpath, RTLD_LAZY);   
    if (lib == NULL)
      break;

    funct = (void (*)(const char*))get_function_pointer(lib, functname);
    if (funct == NULL)
      break;

    result = true;

  } while (0);
  
  return result;
}

int main(int argc, char* argv[])
{ 
  if ( link_function("./lib/shared/libunit.so", "message") )
  {
    funct("linkage succeed");
  }
  else
  {
    printf("shared library or required function cannot be found\n");
  }
}