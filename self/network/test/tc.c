#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <poll.h>


/*
    How to simulate server recieves data slowly?
    How to check if send() is blocking?
    What the problem is with sending to WEB?
*/


#define BUFFER_SIZE 10240


int main(int argc, char *argv[]) 
{
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    // Get the send buffer size
    int send_buf_size;
    socklen_t send_buf_size_len = sizeof(send_buf_size);
    if (getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &send_buf_size, &send_buf_size_len) < 0) 
    {
        perror("getsockopt() failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket send buffer size: %d bytes\n", send_buf_size);


    // ---------------------------------------------------------------------------- //

    // Set the server address


    // local
    struct sockaddr_in server_addr =
    {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_LOOPBACK),
        .sin_port = htons(1234)
    };

    /*

    // web
    const char* server_ip = "142.250.188.46";       // google.com
    in_port_t server_port = 80;                     // http-server port

    struct sockaddr_in server_addr =
    {
        .sin_family = AF_INET,
        .sin_addr = { 0 },
        .sin_port = htons(server_port)
    };

    if (0 == inet_pton(AF_INET, server_ip, &server_addr.sin_addr.s_addr))
    {
        printf("Invalid server IP address: '%s'\n", server_ip);
        exit(EXIT_FAILURE);
    }

    */

    // ---------------------------------------------------------------------------- //


    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("connect() failed");
        exit(EXIT_FAILURE);
    }

    // Send data
    size_t data_size = send_buf_size * 100;
    char* buffer = (char*)calloc(1, data_size);
    memset(buffer, 'A', data_size);
    int bytes_sent = 0;
    int total_sent = 0;

    struct pollfd polls[1] = { {sockfd, POLLOUT, 0}  };
    while (true)
    {
        poll(polls, 1, -1);

        total_sent += bytes_sent;
        data_size -= bytes_sent;
        printf("TOTAL %d \n", total_sent);

        if (polls[0].revents & POLLOUT)
        {
            printf("POLLOUT \n");

            // bytes_sent = send(sockfd, buffer + total_sent, data_size, 0);
            bytes_sent = send(sockfd, buffer + total_sent, data_size, MSG_DONTWAIT);

            if (bytes_sent < 0) 
            {
                perror("send() failed");
                break;
            }
            else if (0 == bytes_sent)
            {
                printf(" ... \n");
                break;
            }
            else
            {
                // another part was recieved
                printf("  ... %d sent ... \n", bytes_sent);
            }
        }
    }

    total_sent += bytes_sent;
    printf("Total sent: %d bytes\n", total_sent);

    // Close the socket
    close(sockfd);

    return 0;
}
