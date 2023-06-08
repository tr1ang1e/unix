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

// get string 
#define MSTR(macro)         #macro          // macro name to string
#define VSTR(macro)         MSTR(macro)     // macro value to string

// be sure 'va_start' and 'va_end' are called in pair
#define __va_init(args, prev)   { va_list args; va_start(args, prev);
#define __va_destroy(args)        va_end(args);  }


/* --------------------------------------------------------- */
/*                         T Y P E S                         */
/* --------------------------------------------------------- */

typedef enum ByteOrder
{
    L_END = 0,
    B_END
} ByteOrder;


/* --------------------------------------------------------- */
/*                    F U N C T I O N S                      */
/* --------------------------------------------------------- */

const char* Getenv(const char* name, const char* value);        // get environmental variable or default value
void wait_for_enter();                                          // ask for any key to be pressed
ByteOrder get_endiannes();                                      // get host endiannes
uint64_t get_time_ms();                                         // get monotonic clock time in milliseconds


#endif // UTILS_H