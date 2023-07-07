#define DEBUG
#define TRACE
#define INFO


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

    int sock = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server =
    {
        .sin_family = AF_INET,
        .sin_addr = { 0 },
        .sin_port = htons(__port),
        .sin_zero = { 0 }
    };
    Sock_pton(AF_INET, __ip, &server.sin_addr.s_addr);
    Connect(sock, (struct sockaddr*)&server, sizeof(server));

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

        __unused Writen(sock, sendBuff, strlen(sendBuff));
        ssize_t actRead = Readline(sock, recvBuff, sizeof(recvBuff));
        if (actRead != strlen(sendBuff))
            warning("Some data were lost");

        __unused fputs(recvBuff, stdout);
        __unused fflush(stdout);
    }

    Close(sock);
    exit(EXIT_SUCCESS);
}


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */
