#ifndef EXTERNAL_H
#define EXTERNAL_H


/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

/*
    All system headers which are:
        - necessary for either client or server side
        - or necessary for 'lib' source code modules
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
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>


#endif // EXTERNAL_H