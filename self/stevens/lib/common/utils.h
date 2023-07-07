#ifndef UTILS_H
#define UTILS_H


/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#define CONSOLE_LINE        100             // must not contain 'U' like '100U'
#define OPEN_MAX            1024U

// get string 
#define MSTR(macro)         #macro          // macro name to string
#define VSTR(macro)         MSTR(macro)     // macro value to string

// compare values
#define max(a, b)           ((a > b) ? a : b)
#define min(a, b)           ((a < b) ? a : b)

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

// map RetCode into EXIT_* code
#define RET_EXIT(retCode)              ((retCode) ? EXIT_FAILURE : EXIT_SUCCESS)

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

#ifdef INFO
    #define __info_color(fmt, ...)     printf(YELL "[  INFO ]   " fmt ORIG "\n", __VA_ARGS__)
    #define __info(...)                __info_color( __VA_ARGS__)
#else
    #define __info(...)
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

typedef enum RetCode
{
    // appropriate for using with bool return type
    RC_ERROR        =  -1,
    RC_SUCCESS      =   0,

    // extended return codes
    RC_SIG_BREAK    = 100,
    RC_SIG_UNKN
} RetCode;

typedef struct Coordinates
{
    int32_t x;
    int32_t y;
    int32_t z;
} Coordinates;


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

void serialize_coordinates(void* dest, const Coordinates* src);
void deserialize_coordinates(Coordinates* dest, const void* src);


#endif // UTILS_H