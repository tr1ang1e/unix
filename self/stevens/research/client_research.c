#include "lib.h"


int main(int argc, char** argv)
{
    init(SIDE_CLIENT, argc, argv);
    int rc = 0;  

    // create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock)
        err_sys("socket() error");

    // specify server
    struct sockaddr_in server =
    {
        .sin_family = AF_INET,
        .sin_addr = { 0 },
        .sin_port = htons(__port),
        .sin_zero = { 0 }
    };

    rc = inet_pton(AF_INET, __ip, &server.sin_addr.s_addr);
    if (0 == rc)
        err_quit("inet_pton() error, IP = %s", __ip);

    // connect
    rc = connect(sock, (struct sockaddr*)&server, sizeof(server));
    if (-1 == rc)
        err_sys("connect() error");

    // address and port in net byte order
    __console("    hex = %08X:%04X \n", server.sin_addr.s_addr, __port);

    char writeBuff[MAXLINE + 1] = { 0 };
    writeBuff[0] = 'A';

    while (true)
    {
        wait_for_enter(":: Send byte to server? "); 

        // write
        ssize_t written = write(sock, writeBuff, 1);
        if (1 != written)
            err_sys("write() error"); 
    };

    wait_for_enter(":: Data sent. Close connection? ");

    // read
    // char readBuff[MAXLINE + 1] = { 0 }; 
    // while ( (rc = read(sock, readBuff, MAXLINE)) > 0 )
    // {
    //     readBuff[rc] = '\0';
    //     if (EOF == fputs(readBuff, stdout))
    //         err_sys("fputs() error");
    // }
    // if (rc < 0) 
    //     err_sys("read() error");

    // close socket
    rc = close(sock);
    if (-1 == rc)
        err_sys("close() error");


    wait_for_enter(":: Connection closed. Finish execution? ");

    exit(EXIT_SUCCESS);
}
