// use following command to test the programm (12345 is the port number, mught be any)
// ~ ./solution 12345
// echo "This is my data" > /dev/udp/127.0.0.1/12345

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define MAX_MESS_SIZE (5 * 1024)
#define SERVER_IP "127.0.0.1" 

static _Bool is_end(const char* message)
{
  const char* endMessage = "OFF\n";
  _Bool isEnd = !strcmp(message, endMessage);
  return isEnd;
}

struct sockaddr_in client, server;

int main(int argc, char** argv)
{
  // get port
  char* end = NULL;
  uint16_t port = (uint16_t)strtoul(argv[1], &end, 10);
  #define SERVER_PORT port

  // create socket
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1)
  {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  // set socket net params
  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);
  inet_aton(SERVER_IP, &server.sin_addr);

  // bind socket to params
  int result = bind(sock, (struct sockaddr*)&server, sizeof(server));
  if (result == -1)
  {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  // read from socket
  char buffer[MAX_MESS_SIZE] = { 0 };
  int sockaddr_length = sizeof(client);
  while(1)
  {
    size_t recieved =  recvfrom(sock, buffer, BUFSIZ, MSG_TRUNC, (struct sockaddr*)&client, &sockaddr_length);
    if (recieved == -1)
    {
      perror("recvfrom()");
      exit(EXIT_FAILURE);
    }

    // check if stop command recieved
    if(is_end(buffer))
    {
      break;
    }

    // print message and clean up buffer
    printf("%s\n", buffer);
    memset(buffer, 0, recieved);
  }

  // end programm
  close(sock);
  exit(EXIT_SUCCESS);
}