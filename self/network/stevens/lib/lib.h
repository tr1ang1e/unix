#ifndef LIB_H
#define LIB_H


/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

/*
    All headers which are:
        - necessary for either client or server side
        - or necessary for 'lib' source code items
    To see which header is for which item: `man <item>` 
*/

#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>


/* --------------------------------------------------------- */
/*                   H E L P   M A C R O S                   */
/* --------------------------------------------------------- */

#define __unused            (void)
#define __console(...)      printf(__VA_ARGS__)

// numeric constants
#define MAXLINE             1024
#define LISTENQ             1024

// get string 
#define MSTR(macro)         #macro 
#define VSTR(macro)         MSTR(macro)

// be sure 'va_star't and 'va_end' are called in pair
#define __va_init(args, prev)   { va_list args; va_start(args, prev);
#define __va_destroy(args)        va_end(args);  }


/* --------------------------------------------------------- */
/*                         T Y P E S                         */
/* --------------------------------------------------------- */

typedef enum SideType
{
    SIDE_UNSPEC =  0,
    SIDE_CLIENT =  (1 << 30),
    SIDE_SERVER =  (1 << 31)
} SideType;


/* --------------------------------------------------------- */
/*                      E X T E R N A L                      */
/* --------------------------------------------------------- */

extern SideType __whoami;
extern bool __daemon_proc;

extern const char* __ip;
extern in_port_t __port;


/* --------------------------------------------------------- */
/*                    F U N C T I O N S                      */
/* --------------------------------------------------------- */

void init(SideType side, int argc, char** argv);

// wrappers
const char* Getenv(const char* name, const char* value);

// error handling
void err_msg(const char *fmt, ...);         // nonfatal error unrelated to a system call. Print a message and return
void err_quit(const char* fmt, ...);        // fatal error unrelated to a system call. Print a message and terminate 
void err_sys(const char *fmt, ...);         // fatal error related to a system call. Print a message and terminate


#endif