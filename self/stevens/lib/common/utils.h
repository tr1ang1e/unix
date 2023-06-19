#ifndef UTILS_H
#define UTILS_H


/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/* --------------------------------------------------------- */
/*                   H E L P   M A C R O S                   */
/* --------------------------------------------------------- */

#define __unused            (void)
#define __console(...)      printf(__VA_ARGS__)

// numeric constants
#define MAXLINE             1024U
#define LISTENQ             1024U

// time constants
#define MS_PER_SEC          1000U
#define NS_PER_MS		    1000000U

// io constants
#define CONSOLE_LINE        100

// get string 
#define MSTR(macro)         #macro          // macro name to string
#define VSTR(macro)         MSTR(macro)     // macro value to string

// be sure 'va_start' and 'va_end' are called in pair
#define __va_init(args, prev)   { va_list args; va_start(args, prev);
#define __va_destroy(args)        va_end(args);  }

// console colors
#define ORIG	"\x1B[0m"
#define BLUE	"\x1B[38;5;69m"
#define GREEN 	"\x1B[32m"
#define GREY	"\x1B[38;5;242m"
#define RED 	"\x1B[31m"
#define YELL	"\x1B[38;5;227m"

/* log levels */

#ifdef DEBUG
    #define __debug_color(fmt, ...)     printf(BLUE "[ DEBUG ]   " ORIG fmt "\n", __VA_ARGS__)
    #define __debug(...)                __debug_color(__VA_ARGS__)
#else
    #define __debug(...)
#endif

#ifdef TRACE
    #define __trace_color(fmt, ...)     printf(GREY "[ TRACE ]   " fmt ORIG "\n", __VA_ARGS__)
    #define __trace(...)                __trace_color( __VA_ARGS__)
#else
    #define __trace(...)
#endif


/* --------------------------------------------------------- */
/*                         T Y P E S                         */
/* --------------------------------------------------------- */

typedef enum ByteOrder
{
    L_END = 0,
    B_END
} ByteOrder;

typedef void (*handle_proc_value)(const char*, void*);


/* --------------------------------------------------------- */
/*                    F U N C T I O N S                      */
/* --------------------------------------------------------- */

const char* Getenv(const char* name, const char* value);                    // get environmental variable or default value
int wait_for_enter(const char* prompt);                                     // ask for one symbol or new line character
void clear_n_chars(size_t n);                                               // remove n chars from stdout, clear line if n is 0
ByteOrder get_endiannes();                                                  // get host endiannes
uint64_t get_time_ms();                                                     // get monotonic clock time in milliseconds

int get_proc_value(const char* path, handle_proc_value, void*);             // get value from specified /proc/... file
void single_token_to_num(const char* token, void* result);                  // cast string to number


#endif // UTILS_H