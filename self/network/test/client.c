#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <poll.h>


#define RESPONSE_BUFSIZ_KB 1048576  // 10 MB


int main(int argc, char** argv)
{
    int rc = 0;                     // temp var to keep ret code from calls
    char* HTTP_RESPONSE = NULL;     // pointer to be used for dynamic alloc
    
    /* create socket */

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock)
    {
        perror("socket()");
        goto exit;
    }

    /* 
        prepare server data and 
        associate socket with network data:
            server IP:port = explicitely 
            client IP:port = done by system 
    */
    
    const char* server_ip = "142.250.188.46";       // google.com
    in_port_t server_port = 80;                     // http-server port

    struct sockaddr_in server =
    {
        .sin_family = AF_INET,
        .sin_addr = { 0 },
        .sin_port = htons(server_port)
    };

    rc = inet_pton(AF_INET, server_ip, &server.sin_addr.s_addr);
    if (0 == rc)
    {
        printf("Invalid server IP address: '%s'\n", server_ip);
        goto exit;
    }

    rc = connect(sock, (struct sockaddr*)&server, sizeof(server));
    if (-1 == rc)
    {
        perror("connect()");
        goto exit;
    }

    /* get client network data auto assigned by system */

    struct sockaddr_in client = { 0 };
    int sockaddr_len = (int)sizeof(client);
    rc = getsockname(sock, (struct sockaddr*)&client, &sockaddr_len);
    if (-1 == rc)
    {
        perror("getsockname()");
        goto exit;
    }

    char ip_buff[INET_ADDRSTRLEN] = { 0 };
    const char* client_ip = inet_ntop(AF_INET, &client.sin_addr.s_addr, ip_buff, sizeof(ip_buff));
    if (NULL == client_ip)
    {
        perror("inet_ntop()");
        goto exit;
    }

    in_port_t client_port = ntohs(client.sin_port);

    /* printing connection results */

    printf("\nSocket is created and associated with:\n"  
           "  client = %s:%d\n"  
           "  server = %s:%d\n\n",
        client_ip, client_port,
        server_ip, server_port);

    /* main logic = HTTP communication */

    ssize_t bytes = 0;
    ssize_t total = 0;

    const char* HTTP_GET = 
        "GET / HTTP/1.1\r\n"
        "Host:www.google.com\r\n"
        "Connection:close\r\n"
        "\r\n";
    
    /*
        send() function guarantees that all data
        have to be sent will be actually sent,
        unless:

            non-blocking mode is activated
                - fcntl() with O_NONBLOCK flag = affect socket descriptor itself
                - send() with MSG_DONTWAIT flag = applied only to current call
            error occurs
        
          
    */

    bytes = send(sock, HTTP_GET, strlen(HTTP_GET), 0);
    if (-1 == rc)
    {
        perror("send()");
        goto exit;
    }
    printf("Sending data ... %ld bytes sent\n", bytes);

    /*
        recv() function doesn't guarantee that it will
        return after all data had sent by partner were
        actually recieved. In order to control socket
        state and be sure that all data were recieved
        socket reader should be used with poll() call.

        In following example, only POLLIN is handled - 
        in order to handle errors such as:
            POLLERR
            POLLHUP
            POLLNVAL
        
        The server will close the connection after the 
        entire response is sent because of "HTTP_GET",
        and therefore end of recieving might be detected.
    */

    HTTP_RESPONSE = (char*)calloc(1, RESPONSE_BUFSIZ_KB);
    char* destination = HTTP_RESPONSE;
    size_t available = RESPONSE_BUFSIZ_KB;
    total = 0;
    bytes = 0;

    struct pollfd polls[1] = { {sock, POLLIN, 0}  };
    while (true)
    {
        destination += bytes;
        available -= bytes;
        total += bytes;

        poll(polls, 1, -1);

        if (polls[0].revents & POLLIN)
        {
            bytes = recv(sock, destination, available, 0);
            
            if (-1 == bytes)
            {
                // error when recieving
                perror("recv()");
                goto exit;
            }
            else if (0 == bytes)
            {
                // EOF is recieved = connection closed
                break;
            }
            else
            {
                // another part was recieved
                printf("  ... %ld recieved ... \n", bytes);
            }
        }
    }

    printf("Recieving data is finished. Total %ld bytes are recieved\n", total);
    printf("Content:\n\n%s\n", HTTP_RESPONSE);

exit:

    /* free memory */

    if (NULL != HTTP_RESPONSE)
    {
        free(HTTP_RESPONSE);
    }

    /* close socket */

    if (-1 != sock)
    {
        rc = close(sock);
        if (-1 == rc)
        {
            perror("close()");
            goto exit;
        }
        else
        {
            printf("Socket is closed successfully\n");
        }
    }

    return 0;
}
