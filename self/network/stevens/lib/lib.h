#ifndef LIB_H
#define LIB_H


/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

/*
    All headers which are:
        - necessary for both client and server side
        - or necessary for 'lib' source code items
    To see which header is for which item: `man <item>` 
*/

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


/* --------------------------------------------------------- */
/*                      E X T E R N A L                      */
/* --------------------------------------------------------- */

extern const char* ip;
extern int port;


/* --------------------------------------------------------- */
/*                   H E L P   M A C R O S                   */
/* --------------------------------------------------------- */

#define CONSOLE(...)  printf(__VA_ARGS__)


/* --------------------------------------------------------- */
/*                         T Y P E S                         */
/* --------------------------------------------------------- */

typedef enum SideType
{
    SIDE_CLIENT = 0,
    SIDE_SERVER = 1
} PeerType;


/* --------------------------------------------------------- */
/*                    F U N C T I O N S                      */
/* --------------------------------------------------------- */

void parse_cli_args(int argc, char** argv);


#endif