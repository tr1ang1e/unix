#include <stdbool.h>
#include <stdio.h>
#include <dlfcn.h>

#define TRACE(number) printf("%d\n", number)

void (*funct)(const char*);     // create pointer to function

bool link_function(const char* libpath, const char* functname)
{
  bool result = false;

  do
  {
    // get library handle
    void* lib = dlopen(libpath, RTLD_LAZY);   
    if (lib == NULL)
    {
      break;
    }

    // get function pointer
    funct = (void (*)(const char*))dlsym(lib, functname);
    if (funct == NULL)
    {
      break;
    }

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