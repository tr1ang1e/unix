#include <stdbool.h>
#include <stdio.h>
#include <dlfcn.h>

#define TRACE(number) printf("%d\n", number)

void* get_library_handle(const char* libpath)
{
  return dlopen(libpath, RTLD_LAZY);       // dlopen()
}

void* get_function_pointer(void* libhandle, const char* functname)
{
  return dlsym(libhandle, functname);     // dlsym()
}

bool link_function(const char* libpath, const char* functname)
{
  bool result = false;
  void (*pFunct)(const char*);     // create pointer to function

  do
  {    
    void* library = get_library_handle(libpath);   
    if (library == NULL)
      break;

    pFunct = (void (*)(const char*))get_function_pointer(library, functname);
    if (pFunct == NULL)
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