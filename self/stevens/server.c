/*
    :: TCP
    :: Echo server. Test *rlimit() functions
    :: 4.04

    $ ./__s --port=<port>

    # ------------------------------------------- #

    In current server version all sockets (including
    listening one) are handled using multiplexing.
    The code listed below is a kind of draft just to
    demostrate the multiplexing principle. It is
    defenseless against DoS (denial-of-service) attack.
    
    The problem is in Readline() funcion which is used
    for client service: attack client might send some
    portion of bytes without final '\n' byte and
    therefore Readline() will never return and will
    block all server app. 
    
    To solve the problem:
        solution 1 = avoid using Readline(), fix protocol
        solution 2 = use nonblocking IO
        solution 3 = use fork() for managing every client
        solution 4 = set IO timeout
*/


#define DEBUG
#define TRACE
#define INFO


#include "lib.h"


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

RetCode handle_client(int sockfd);


/* --------------------------------------------------------- */
/*                         M A I N                           */
/* --------------------------------------------------------- */

int main(int argc, char** argv)
{
    init(SIDE_SERVER, argc, argv);
    RetCode rc = 0;

    /* maximize allowed number of open files */

    struct rlimit lim;
    rc = getrlimit(RLIMIT_NOFILE, &lim);
    if (RC_ERROR != rc)
    {
        __debug("Change RLIMIT_NOFILE: %d to %d", lim.rlim_cur, lim.rlim_max);
        lim.rlim_cur = lim.rlim_max;
        rc = setrlimit(RLIMIT_NOFILE, &lim);
    }
    if (RC_ERROR == rc) error("Changing RLIMIT_NOFILE error");

    /* start working with sockets */

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

    /*
        store all connected clients to be
        able to iterate over the array and
        perform any operations, particulary
        reading after poll() returns

        struct pollfd::fd value -1 means no 
        client stored in this array element
    */

    struct pollfd clients[OPEN_MAX];
    size_t clientsMax = sizeof(clients) / sizeof(struct pollfd);
    clients[0].fd = lsock;
    clients[0].events = POLLIN;
    for (size_t i = 1; i < clientsMax; ++i)
        clients[i].fd = -1;

    // helpers
    int client;
    int maxClientIdx = -1;
    const int timeout = -1;
    int nfds = clientsMax;
    struct pollfd* lsockPoll = &clients[0];

    while (true)
    {
        int ready = poll(clients, nfds, timeout);
        if (RC_ERROR == ready)
            break;
        
        /* 
            check clients for new data first because accepting 
            requires changing data related to the clients
        */

        struct pollfd* csockPoll;
        for (client = 1; client <= maxClientIdx; ++client)
        {    
            csockPoll = &clients[client];
            if (-1 == csockPoll->fd)
                continue;

            if (csockPoll->revents & (POLLIN | POLLERR))
            {
                rc = handle_client(csockPoll->fd);
                switch (rc)
                {
                case RC_SIG_BREAK:      // client has closed connection
                {
                    Close(csockPoll->fd);
                    csockPoll->fd = -1;
                    break;
                }

                default: break;         // error or success
                }

                /*
                    unnesessary iterations might be skipped
                    by checking the number of ready sockets:
                        
                        if (--ready <= 0) 
                            break;

                    but it must or must not be used depending
                    on particular circumstances: must if general
                    trafficis is low (a lot of clients, only few 
                    are communicating simultaneously) / must not
                    if all connected clients are tend to interact
                    with server at the same time
                */
            }
        }

        /* 
            new client is ready to be accepted
                - accept new client
                - update clients array
                - update all fd set
        */

        if (lsockPoll->revents & POLLIN)
        {
            int csock = Accept(lsock, NULL, NULL);
            
            // save new client descriptor
            for (client = 1; client < clientsMax; ++client)
            {
                if (-1 == clients[client].fd)
                {
                    clients[client].fd = csock;
                    clients[client].events = POLLIN;
                    break;
                }
            }

            if (client == clientsMax) error("Too many clients");
            if (client > maxClientIdx) maxClientIdx = client;
        }
    }

    Close(lsock);
    exit(EXIT_SUCCESS);
}


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

RetCode handle_client(int sockfd)
{
    RetCode rc = RC_SUCCESS;
    char buffer[BUFSIZ] = { 0 };

    ssize_t actRead = Readline(sockfd, buffer, sizeof(buffer));
    switch (actRead)
    {
    case -1:
        rc = RC_ERROR;
        break;

    case 0:
        rc = RC_SIG_BREAK;
        break;
    
    default:
    {            
        Coordinates coord;
        deserialize_coordinates(&coord, buffer);

        int32_t temp = coord.x;
        coord.x = coord.z;
        coord.z = temp;
        serialize_coordinates(buffer, &coord);

        size_t expSent = sizeof(Coordinates) + sizeof(char);            
        __unused Writen(sockfd, buffer, expSent);
        break;
    }  
    }

    return rc;
}
