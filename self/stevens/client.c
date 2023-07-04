/*
    :: TCP
    :: Echo client. Add io multiplexing support
    :: 4.02

    $ ./__c --ip=<ip> --port=<port>
    Options values:
        <ip>    IP, domain name or one of constants: LOOPBACK, HOSTRY, RPI
        <port>  port value known from the server 

    # ------------------------------------------- #

    Application protocol description:
        - both client and server must know about exact binary data meaning
        - every meaningful data chunk should be followed by '\n' symbol
*/


#define DEBUG
#define TRACE

#include "lib.h"


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

static RetCode send_data(int sock);
static RetCode receive_data(int sock);
static RetCode data_exchange(int sock);


/* --------------------------------------------------------- */
/*                         M A I N                           */
/* --------------------------------------------------------- */

int main(int argc, char** argv)
{
    init(SIDE_CLIENT, argc, argv);
    RetCode rc = 0;  

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

    rc = data_exchange(sock);
    switch (rc)
    {
    case RC_SUCCESS: break;
    case RC_ERROR:
    case RC_SIG_BREAK:
        break;
    default: break;
    }

    Close(sock);
    exit(RET_EXIT(rc));
}


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

RetCode send_data(int sock)
{
    RetCode rc = RC_SUCCESS;

    /*
        buff size:
            - sizeof(Coordinates)       useful data
            - sizeof(char)              '\n' protocol implementation
    */
    char sendBuff[sizeof(Coordinates) + sizeof(char)] = { 0 };
    char* ptr = sendBuff;

    __unused fgets(sendBuff, sizeof(sendBuff), stdin);
    if ('\n' == sendBuff[0])
    {
        rc = RC_SIG_BREAK;
        return rc;
    }

    Coordinates coord =
    {
        .x = *ptr++ - 48,
        .y = *ptr++ - 48,
        .z = *ptr++ - 48
    };
    serialize_coordinates(sendBuff, &coord);
    __unused Writen(sock, sendBuff, sizeof(sendBuff));

    __console("Sent: %d.%d.%d\n", coord.x, coord.y, coord.z);
    return rc;
}

RetCode receive_data(int sock)
{
    RetCode rc = RC_SUCCESS;
    
    /*
        buff size:
            - sizeof(Coordinates)       useful data
            - sizeof(char)              '\n' protocol implementation
            - sizeof(char)              for Readline to add '\000'
    */
    char recvBuff[sizeof(Coordinates) + sizeof(char) + sizeof(char)] = { 0 };
    ssize_t actRead = Readline(sock, recvBuff, sizeof(recvBuff));
    
    Coordinates coord;
    deserialize_coordinates(&coord, recvBuff);
    __console("Received: %d.%d.%d\n", coord.x, coord.y, coord.z);  

    return rc;
}

RetCode data_exchange(int sock)
{
    RetCode rc = RC_SUCCESS;

    // select() required arguments
    int inputFd = fileno(stdin);
    int nfds = max(inputFd, sock) + 1;
    fd_set readSet; FD_ZERO(&readSet);

    while (true)
    {
        // restore fd bits
        FD_SET(inputFd, &readSet);
        FD_SET(sock, &readSet);

        rc = select(nfds, &readSet, NULL, NULL, NULL);
        if (RC_ERROR == rc)
            break;

        // server data to be handled is ready
        if (FD_ISSET(sock, &readSet))
        {
            rc = receive_data(sock);
        }

        // user data to be sent is ready
        if (FD_ISSET(inputFd, &readSet))
        {
            rc = send_data(sock);
            if (RC_SIG_BREAK == rc)
            {
                rc = RC_SUCCESS;
                break;
            }
        }
    }

    return rc;
}
