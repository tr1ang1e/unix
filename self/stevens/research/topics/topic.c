/*
    :: Topic = SO_RCVBUF vs tcp_rmem
    :: Instruction:
        $ __topic                           # excpected and actual SO_RCVBUF values
        $ ss -aOmp | grep pid=<pid>         # get receive buffer size from outside the code
        $ sysctl net.ipv4.tcp_rmem          # check that changes have been applied only to the current process
*/

#include "lib.h"


int main(int argc, char** argv)
{
    int rc;
    
    /* create socket */

    int sock = Socket(AF_INET, SOCK_STREAM, 0);

    /* bind and listen socket so that statistics is available for 'man 8 ss' */

    struct sockaddr_in me =
    {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(__port)
    };

    Bind(sock, (struct sockaddr*)&me, sizeof(me));
    Listen(sock);

    /* set and get SO_RCVBUF */

    int expected = 32 * 1024;
    socklen_t optlen = sizeof(expected);
    rc = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &expected, optlen);
    if (-1 == rc)
        error("setsockopt() error");

    int actual = 0;
    rc = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &actual, &optlen);
    if (-1 == rc)
        error("getsockopt() error");

    /* print info */

    __console("Process ID = %d\n", getpid());
    __console("Listening socket, port = %d\n", __port);
    __console("SO_RCVBUF = set %d / got %d\n", expected, actual);

    int duration = 60;
    for (int i = 0; i < duration; ++i)
    {
        __console("\rsec %.3d/%.3d  ", i, duration);
        fflush(stdout);
        sleep(1);
    }

    /* free resources */

    Close(sock);


    return 0;
}
