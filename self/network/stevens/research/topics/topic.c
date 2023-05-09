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
    /* create socket */

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock)
        err_sys("socket() error");

    /* bind and listen socket so that statistics is available for 'man 8 ss' */

    struct sockaddr_in me =
    {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(__port)
    };

    int rc = bind(sock, (struct sockaddr*)&me, sizeof(me));
    if (-1 == rc)
        err_sys("bind() error");

    int backlog = 0;
    rc = listen(sock, backlog);
    if (-1 == rc)
        err_sys("listen() error");

    /* set and get SO_RCVBUF */

    int expected = 32 * 1024;
    socklen_t optlen = sizeof(expected);
    rc = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &expected, optlen);
    if (-1 == rc)
        err_sys("setsockopt() error");

    int actual = 0;
    rc = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &actual, &optlen);
    if (-1 == rc)
        err_sys("getsockopt() error");

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

    rc = close(sock);
    if (-1 == rc)
        err_sys("close() error");


    return 0;
}
