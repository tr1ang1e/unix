/*
    :: TCP
    :: Get timestamp in string format from server  
    :: 2.01
        - socket connection
        - read from socket using 'read' in a simple loop

    $ ./__c --ip=<ip> --port=<port>
*/

#include "lib.h"


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */


/* --------------------------------------------------------- */
/*                         M A I N                           */
/* --------------------------------------------------------- */

int main(int argc, char** argv)
{
    init(SIDE_CLIENT, argc, argv);
    int rc = 0;  

    // create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock)
        err_sys("socket() error");

    // specify server
    struct sockaddr_in server =
    {
        .sin_family = AF_INET,
        .sin_addr = { 0 },
        .sin_port = htons(__port)
    };

    rc = inet_pton(AF_INET, __ip, &server.sin_addr.s_addr);
    if (0 == rc)
        err_quit("inet_pton() error, IP = %s", __ip);

    // connect
    rc = connect(sock, (struct sockaddr*)&server, sizeof(server));
    if (-1 == rc)
        err_sys("connect() error");

    // read
    char readBuff[MAXLINE + 1] = { 0 }; 
    while ( (rc = read(sock, readBuff, MAXLINE)) > 0 )
    {
        readBuff[rc] = '\0';
        if (EOF == fputs(readBuff, stdout))
            err_sys("fputs() error");
    }
    if (rc < 0) 
        err_sys("read() error");

    // close socket
    rc = close(sock);
    if (-1 == rc)
        err_sys("close() error");

    exit(EXIT_SUCCESS);
}


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

