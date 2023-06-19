/*
    :: TCP
    :: Echo server. Handle SIGCHLD signal
    :: 3.02

    $ ./__s --port=<port>
*/


#define DEBUG
#define TRACE

#include "lib.h"


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

void handle_client(int sockfd);


/* --------------------------------------------------------- */
/*                         M A I N                           */
/* --------------------------------------------------------- */

int main(int argc, char** argv)
{
    init(SIDE_SERVER, argc, argv);
    int rc = 0;
    int lsock = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server =
    {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(__port),
        .sin_zero = { 0 }
    };

    Bind(lsock, (struct sockaddr*)&server, sizeof(server));
    Listen(lsock);

    while (true)
    {
        int csock = Accept(lsock, NULL, NULL);
        if (-1 == csock)
            continue;
        __debug("%s", "Accepted");

        pid_t pid = Fork();
        if (0 == pid)
        {
            Close(lsock);
      
            handle_client(csock);
            Close(csock);

            exit(EXIT_SUCCESS);
        } 

        Close(csock);
    }

    Close(lsock);
    exit(EXIT_SUCCESS);
}


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

void handle_client(int sockfd)
{
    char buffer[BUFSIZ] = { 0 };

    while (true)
    {
        ssize_t actRead = read(sockfd, buffer, sizeof(buffer));
        switch (actRead)
        {
        case -1:
        {
            error("read() error");
            return;
        } 

        case 0:
            return;
        
        default:
        {
            __unused Writen(sockfd, buffer, actRead);
            break;
        }
        }       
    }
}
