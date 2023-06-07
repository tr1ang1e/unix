#ifndef ERROR_H
#define ERROR_H


/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include "common/external.h"
#include "common/utils.h"


/* --------------------------------------------------------- */
/*                      E X T E R N A L                      */
/* --------------------------------------------------------- */

extern bool __daemon_proc;


/* --------------------------------------------------------- */
/*                    F U N C T I O N S                      */
/* --------------------------------------------------------- */

void err_msg(const char *fmt, ...);         // nonfatal error unrelated to a system call. Print a message and return
void err_quit(const char* fmt, ...);        // fatal error unrelated to a system call. Print a message and terminate 
void err_sys(const char *fmt, ...);         // fatal error related to a system call. Print a message and terminate


#endif // ERROR_H