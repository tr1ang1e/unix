# note that current dependencies might not exist
# because this is just an example to source file 'dyn_runtime_lib.c'

a.out: main.c libunit.so ./include/clean_code_utils.h
	gcc main.c -ldl

libunit.so: ./source/c/unit.c ./include/c/header.h
	gcc -shared -fPIC ./source/c/unit.c -o ./lib/shared/libunit.so