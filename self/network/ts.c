#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <poll.h>


#define SERVER_PORT 1234
#define BUFFER_SIZE 1024


int main(int argc, char *argv[]) 
{
    int sockfd, connfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    // Set the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);   // 127.0.0.1

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) < 0) {
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on 127.0.0.1:%d\n", SERVER_PORT);

    // Accept incoming connections and handle them
    while (true) 
    {
        int total_recieved = 0;

        // Accept a new connection
        client_addr_len = sizeof(client_addr);
        connfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (connfd < 0) 
        {
            perror("accept() failed");
            continue;
        }
        printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Receive data from the client
        struct pollfd polls[1] = { {connfd, POLLIN, 0} };
        while (true)
        {
            poll(polls, 1, -1);

            if (polls[0].revents & POLLIN)
            {
                bytes_received = recv(connfd, buffer, BUFFER_SIZE, 0);
                
                if (-1 == bytes_received)
                {
                    // Error when recieving
                    perror("recv()");
                    close(connfd);
                    close(sockfd);
                    exit(EXIT_FAILURE);
                }
                else if (0 == bytes_received)
                {
                    // EOF is recieved = connection closed
                    printf("Connection closed by client\n");
                    break;
                }
                else
                {
                    // Another part was recieved
                    total_recieved += bytes_received;
                }
            }
        }
        printf("Received %d bytes from client\n", total_recieved);

        // Close the connection
        close(connfd);
    }

    // Close the socket
    close(sockfd);

    return 0;
}
