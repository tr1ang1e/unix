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

static int comparator(void* i, void* j)
{
  return -( *(char*)i - *(char*)j );
}

static void sort_string(char* buffer)
{
  qsort(buffer, strlen(buffer), sizeof(char), comparator);
}

struct sockaddr_in client, server;

int main(int argc, char** argv)
{
  // get port
  char* end = NULL;
  uint16_t port = (uint16_t)strtoul(argv[1], &end, 10);
  #define SERVER_PORT port

  // create socket
  int sockListen = socket(AF_INET, SOCK_STREAM, 0);
  if (sockListen == -1)
  {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  // set socket net params
  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);
  inet_aton(SERVER_IP, &server.sin_addr);

  // bind socket to params
  int result = bind(sockListen, (struct sockaddr*)&server, sizeof(server));
  if (result == -1)
  {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  // listen
  #define CLIENT_QUEUE 4
  result = listen(sockListen, CLIENT_QUEUE);
  if(result == -1)
  {
    perror("listen()");
    exit(EXIT_FAILURE);
  }

  // accept
  int sockClient = accept(sockListen, NULL, NULL);
  if(sockClient == -1)
  {
    perror("accept()");
    exit(EXIT_FAILURE);
  }

  // main logic
  char buffer[MAX_MESS_SIZE] = { 0 };
  while(1)
  {
    // read from socket
    size_t recieved =  recv(sockClient, buffer, BUFSIZ, MSG_NOSIGNAL);
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

    // sort string
    sort_string(buffer);

    // write to socket and clean up the buffer
    int sent = send(sockClient, buffer, recieved, 0);
    if (sent == -1)
    {
      perror("send()");
      exit(EXIT_FAILURE);
    }
    memset(buffer, 0, recieved);

  }

  // end programm
  close(sockClient);
  close(sockListen);
  exit(EXIT_SUCCESS);
}