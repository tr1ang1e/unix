/*
    :: TCP
    :: Echo client. Write initial data and read modified
    :: 4.01

    $ ./__c --ip=<ip> --port=<port>
    Options values:
        <ip>    IP, domain name or one of constants: LOOPBACK, HOSTRY, RPI
        <port>  port value known from the server 
*/


#define DEBUG
#define TRACE

#include "lib.h"


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

static size_t send_data(int sock);
static size_t receive_data(int sock);


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

    __debug("%s", "Connected");

    while (true)
    {
        size_t sent = send_data(sock);
        if (0 == sent)
            break;
        
        size_t received = receive_data(sock);
        if (received != sent)
            warning("Some data were lost");
        
        break;  // send_data currently doesn't have interactivity
    }

    Close(sock);
    exit(EXIT_SUCCESS);
}

/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

size_t send_data(int sock)
{
    char sendBuff[BUFSIZ] = { 0 };
    Coordinates coord = 
    {
        .x = -1,
        .y =  0,
        .z =  1,
    };
    serialize_coordinates(sendBuff, &coord);
    __console("Before: %d.%d.%d\n", coord.x, coord.y, coord.z);

    size_t expSent = sizeof(Coordinates) + sizeof(char);
    size_t actSent = Writen(sock, sendBuff, expSent);

    return actSent;
}

size_t receive_data(int sock)
{
    char recvBuff[BUFSIZ] = { 0 };
    Coordinates coord = { 0 };

    ssize_t actRead = Readline(sock, recvBuff, sizeof(recvBuff));
    deserialize_coordinates(&coord, recvBuff);

    __console("After: %d.%d.%d\n", coord.x, coord.y, coord.z);
    return actRead;
}
