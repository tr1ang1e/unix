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
        .sin_addr.s_addr = htonl(INADDR_ANY),   // specify interface to accept FROM
        .sin_port = htons(__port)
    };

    // bind socket
    rc = bind(lsock, (struct sockaddr*)&server, sizeof(server));
    if (-1 == rc)
        err_sys("bind() error");

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
            if (errno == EPROTO || errno == ECONNABORTED) continue;
            else
                err_sys("accept() error");
        }

        // debug
        __console("Client accepted\n");

        // deal with client untill user input
        wait_for_enter();

        // write
        // __unused snprintf(writeBuff, sizeof(writeBuff), <fmt>, <arg>);
        // size_t written = write(csock, writeBuff, strlen(writeBuff));
        // if (strlen(writeBuff) != written)
        //    err_sys("write() error"); 

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
