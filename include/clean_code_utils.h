#ifndef CLEAN_CODE_H
#define CLEAN_CODE_H


// logging
#define PTRACEN(number) printf("%d\n", number)


// dyn_runtime_lib.c
#define get_library_handle( ... ) dlopen( __VA_ARGS__ )       //  ( libpath [in] , flags [in] ) 
#define get_function_pointer( ... ) dlsym( __VA_ARGS__ )      //  ( functname [in] )





#endif // CLEAN_CODE_H