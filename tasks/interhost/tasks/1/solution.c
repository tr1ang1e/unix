#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct hostent hostent;

int main(int argc, char** argv)
{
  hostent* host;
  host = gethostbyname(argv[1]);

  if (host == NULL)
  {
    herror("gethostbyname()");
    exit(EXIT_FAILURE);
  }

  int i = 0;
  while(host->h_addr_list[i] != NULL)
  {
    struct in_addr* ip = (struct in_addr*) host->h_addr_list[i];
    printf("%s\n", inet_ntoa(*ip));
    ++i;
  }
  
  exit(EXIT_SUCCESS);
}