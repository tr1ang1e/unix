#include "lib.h"


int main(int argc, char** argv)
{
    init(SIDE_SERVER, argc, argv);
    int rc = 0;

    // create listen socket
    int lsock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == lsock)
        err_sys("socket() error");

    // configure server
    struct sockaddr_in server = 
    {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),   // specify local interface IP to associate with socket
        .sin_port = htons(__port),
        .sin_zero = { 0 }
    };

    // bind socket
    rc = bind(lsock, (struct sockaddr*)&server, sizeof(server));
    if (-1 == rc)
        err_sys("bind() error");

    // print interface IP
    char* sideName = Sock_getsidename(lsock, getsockname, false);
    if (NULL == sideName)  err_msg("Sock_getsidename() error");
    else  __console("  Interface IP = %s\n\n", sideName);
    
    // listen socket
    int backlog = atoi(Getenv("LISTENQ", VSTR(LISTENQ)));
    rc = listen(lsock, backlog);
    if (-1 == rc)
        err_sys("listen() error");

    int csock = 0;
    char writeBuff[MAXLINE] = { 0 };
    while (true)
    {
        // accept connection
        csock = accept(lsock, NULL, NULL);
        if (-1 == csock)
        {
            if (errno == EPROTO || errno == ECONNABORTED)  continue;
            else  err_sys("accept() error");
        }

        // get info about client
        char* partnerSide = Sock_getsidename(csock, getpeername, true);
        __console("Client %s accepted ", partnerSide);

        // get info about me
        char* mySide = Sock_getsidename(csock, getsockname, false);
        __console("on interface %s\n", mySide);
       
        wait_for_enter(":: Read data from buffer? ");

        // read
        char readBuff[MAXLINE + 1] = { 0 }; 
        while ( (rc = read(csock, readBuff, MAXLINE)) > 0 )
        {
            readBuff[rc] = '\0';
            if (EOF == fputs(readBuff, stdout))
                err_sys("fputs() error");
            fflush(stdout);

            // loop management
            int input = wait_for_enter(" :: [B]reak or read another chunk? ");
            if ((input == 'B') || (input == 'b'))
                break;
        }
        if (rc < 0) 
            err_sys("read() error"); 

        wait_for_enter(":: Data read. Close connection? ");

        // close connection
        rc = close(csock);
        if (-1 == rc)
            err_sys("close() client socket error");
    }

    // close socket
    rc = close(lsock);
    if (-1 == rc)
        err_sys("close() listen socket error");

    exit(EXIT_SUCCESS);
}
