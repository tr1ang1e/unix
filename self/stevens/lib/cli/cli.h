#ifndef CLI_H
#define CLI_H


/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include "common/external.h"
#include "common/utils.h"
#include "error/error.h"
#include "process/process.h"
#include "socket/socket.h"


/* --------------------------------------------------------- */
/*                         T Y P E S                         */
/* --------------------------------------------------------- */

typedef enum SideType
{
    SIDE_UNSPEC =  0,
    SIDE_CLIENT =  (1 << 30),
    SIDE_SERVER =  (1 << 31)
} SideType;

#define __SERVERS   \
    X(LOOPBACK)     \
    X(HOSTRY)       \
    X(RPI)

typedef enum ServerIP
{
    #define X(server) SERVER_##server,
        __SERVERS
        SERVER_MAX
    #undef X
} ServerIP;


/* --------------------------------------------------------- */
/*                      E X T E R N A L                      */
/* --------------------------------------------------------- */

extern SideType __whoami;

extern const char* __ip;
extern in_port_t __port;


/* --------------------------------------------------------- */
/*                    F U N C T I O N S                      */
/* --------------------------------------------------------- */

void init(SideType side, int argc, char** argv);


#endif // CLI_H