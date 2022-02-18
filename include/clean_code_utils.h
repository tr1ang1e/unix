#ifndef CLEAN_CODE_H
#define CLEAN_CODE_H

// useful headers
#include <stdbool.h>

/* tracing 
 * 
 *  prefixes:
 *    - T = terminal
 *    - F = file
 *  
 *  suffixes:
 *    - N = number
 *    - M = message
 *
 * */
#define TTRACEN(number) printf("%d\n", number)
#define TTRACEM(message) printf("%s\n", message)


// dyn_runtime_lib.c
#define get_library_handle( ... ) dlopen( __VA_ARGS__ )       //  ( libpath [in] , flags [in] ) 
#define get_function_pointer( ... ) dlsym( __VA_ARGS__ )      //  ( functname [in] )





#endif // CLEAN_CODE_H