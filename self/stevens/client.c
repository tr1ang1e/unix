/*
    :: TCP
    :: Echo client. Create several sockets to test server SIGCHLD handling
    :: 3.02

    $ ./__c --ip=<ip> --port=<port>
    Options values:
        <ip>    IP, domain name or one of constants: LOOPBACK, HOSTRY, RPI
        <port>  port value known from the server 
*/


#define DEBUG
#define TRACE

#include "lib.h"


/* --------------------------------------------------------- */
/*                         M A I N                           */
/* --------------------------------------------------------- */

int main(int argc, char** argv)
{
    init(SIDE_CLIENT, argc, argv);
    int rc = 0;  

    /* 
        create and connect multiple sockets in purpose of 
        testing of server handles simultaneous SIGCHLD signals
    */
    
    #define SOCKNUMBER 5
    int sock[SOCKNUMBER] = { 0 };
    
    for (int s = 0; s < SOCKNUMBER; ++s) 
    {
        sock[s] = Socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in server =
        {
            .sin_family = AF_INET,
            .sin_addr = { 0 },
            .sin_port = htons(__port),
            .sin_zero = { 0 }
        };
        Sock_pton(AF_INET, __ip, &server.sin_addr.s_addr);
        Connect(sock[s], (struct sockaddr*)&server, sizeof(server));
    }

    __debug("%s", "Connected");

    /*
        echo logic is implemented only for one socket,
        others are just to be closed together with the first
    */

    int echoSock = sock[0];
    char sendBuff[BUFSIZ] = { 0 };
    char recvBuff[BUFSIZ] = { 0 };
    while (true)
    {
        recvBuff[0] = '\0';
        
        __unused fgets(sendBuff, sizeof(sendBuff), stdin);
        if ('\n' == sendBuff[0])
        {
            __console("Stopping client\n");
            break;
        }
        __debug("fgets() result, len=%zu: %s", strlen(sendBuff), sendBuff);

        __unused Writen(echoSock, sendBuff, strlen(sendBuff));
        ssize_t actRead = Readline(echoSock, recvBuff, sizeof(recvBuff));
        if (actRead != strlen(sendBuff))
            warning("Some data were lost");

        __unused fputs(recvBuff, stdout);
        __unused fflush(stdout);
    }

    // close all opened and connected sockets
    for (int s = 0; s < SOCKNUMBER; ++s)
        Close(sock[s]);
    
    exit(EXIT_SUCCESS);
}
