/*
    :: TCP
    :: Echo client. Replace select() with poll()
    :: 4.04

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
#define INFO

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
        Shutdown(sock, SHUT_WR);
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
    switch (actRead)
    {
    case -1:    // error
        break;             

    case  0:    // EOF
        rc = RC_SIG_BREAK;
        break;

    default:    // success
    {
        Coordinates coord;
        deserialize_coordinates(&coord, recvBuff);
        __console("Received: %d.%d.%d\n", coord.x, coord.y, coord.z);  
    }
    break;             
    }
    
    return rc;
}

RetCode data_exchange(int sock)
{
    RetCode rc = RC_SUCCESS;

    // poll() required arguments
    struct pollfd fds[] =
    {
        [0] = {  .fd = fileno(stdin),   .events = POLLIN  },
        [1] = {  .fd = sock,            .events = POLLIN  }
    };
    size_t nfds = sizeof(fds) / sizeof(struct pollfd);
    int timeout = -1;   // infinite
    int eventsNumber;

    // for readability purposes
    struct pollfd* inputPoll = &fds[0];
    struct pollfd* sockPoll = &fds[1];

    while (true)
    {
        eventsNumber = poll(fds, nfds, timeout);
        if (RC_ERROR == eventsNumber)   break;
        else if (0 == eventsNumber)   continue;

        // server data to be handled is ready
        if (sockPoll->revents & (POLLIN | POLLERR))
        {
            rc = receive_data(sock);
            if (RC_SIG_BREAK == rc)
            {
                // get FIN from server
                rc = RC_SUCCESS;
                break;
            }

            // reset revent bit
            sockPoll->revents &= ~POLLIN;
        }

        // user data to be sent is ready
        if (inputPoll->revents & POLLIN)
        {
            rc = send_data(sock);
            if (RC_SIG_BREAK == rc)
            {
                // no more input = fd not needed
                inputPoll->fd = -1;

                // restore rc defaul value
                rc = RC_SUCCESS;
                continue;
            }

            // reset revent bit
            inputPoll->revents &= ~POLLIN;
        }
    }

    return rc;
}
