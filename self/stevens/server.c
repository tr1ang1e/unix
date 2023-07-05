/*
    :: TCP
    :: Echo server. Add multiplexing support
    :: 4.02

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
        strore all connected clients to be
        able to iterate over the array and
        perform any operations, particulary
        reading after select() returns

        -1 value means no client stored in
        this array element
    */

    int clients[FD_SETSIZE];
    int client;  // helper
    int maxClientIdx = -1;
    for (client = 0; client < FD_SETSIZE; ++client)
        clients[client] = -1;

    // select() required arguments
    fd_set readSet, allSet;
    FD_ZERO(&allSet);
    FD_SET(lsock, &allSet);
    int nfds = lsock + 1;

    while (true)
    {
        readSet = allSet;
        int ready = select(nfds, &readSet, NULL, NULL, NULL);
        if (RC_ERROR == ready)
            break;
        
        /* 
            check clients for new data first because accepting 
            requires changing data related to the clients
        */

        for (client = 0; client <= maxClientIdx; ++client)
        {    
            int csock = clients[client];
            if (-1 == csock)
                continue;

            if (FD_ISSET(csock, &readSet))
            {
                rc = handle_client(csock);
                switch (rc)
                {
                case RC_SIG_BREAK:      // client has closed connection
                {
                    Close(csock);
                    FD_CLR(csock, &allSet);
                    clients[client] = -1;
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

        if (FD_ISSET(lsock, &readSet))
        {
            int csock = Accept(lsock, NULL, NULL);
            
            // save new client descriptor
            for (client = 0; client < FD_SETSIZE; ++client)
            {
                if (-1 == clients[client])
                {
                    clients[client] = csock;
                    break;
                }
            }
            if (FD_SETSIZE == client) error("Too many clients");
            
            // update select() required data
            FD_SET(csock, &allSet);
            if (csock >= nfds)  nfds = csock + 1;
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
