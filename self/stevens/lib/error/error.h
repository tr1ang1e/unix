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

void warning(const char *fmt, ...);         // nonfatal error. Print a message and return
void error(const char* fmt, ...);           // fatal error. Print a message and terminate 


#endif // ERROR_H