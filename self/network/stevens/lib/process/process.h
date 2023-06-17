#ifndef PROCESS_H
#define PROCESS_H


/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include "common/external.h"
#include "common/utils.h"
#include "error/error.h"


/* --------------------------------------------------------- */
/*                         T Y P E S                         */
/* --------------------------------------------------------- */

typedef void (*SIGHANDLER)(int signum);


/* --------------------------------------------------------- */
/*                      E X T E R N A L                      */
/* --------------------------------------------------------- */

extern struct sigaction __sigIgnorance;


/* --------------------------------------------------------- */
/*                    F U N C T I O N S                      */
/* --------------------------------------------------------- */

pid_t Fork();
SIGHANDLER Sigaction(int signum, SIGHANDLER sigHandler);


#endif // PROCESS_H