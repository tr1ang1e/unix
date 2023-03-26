#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netinet/ip.h>
#include <arpa/inet.h>


int main(int argc, char** argv)
{
    int rc = 0;     // temp var to keep ret code from calls
    
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

    const char* HTTP_GET = 
        "GET / HTTP/1.1\r\n"
        "Host:google.com\r\n"
        "\r\n";
    
    bytes = send(sock, HTTP_GET, strlen(HTTP_GET), 0);
    if (-1 == rc)
    {
        perror("send()");
        goto exit;
    }
    printf("Sending data ... %ld bytes sent\n", bytes);

    char HTTP_RESPONSE[32768] = { 0 };
    bytes = recv(sock, HTTP_RESPONSE, sizeof(HTTP_RESPONSE), 0);
    if (-1 == bytes)
    {
        perror("recv()");
        goto exit;
    }
    printf("Recieving data ... %ld bytes recieved\n", bytes);
    printf("Content:\n\n%s\n", HTTP_RESPONSE);

    /* close socket */

    rc = close(sock);
    if (-1 == rc)
    {
        perror("close()");
        goto exit;
    }
    printf("Socket is closed successfully\n");


exit:
    return 0;
}
