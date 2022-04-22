#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/* struct hostent             // man gethostbystring
   { 
      char  *h_name;          // official name of host
      char **h_aliases;       // alias list 
      int    h_addrtype;      // host address type
      int    h_length;        // length of address 
      char **h_addr_list;     // list of addresses            
*/
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

  printf("Canonical name  :  %s\n", host->h_name);
  printf("Adress type     :  %s\n", host->h_addrtype == AF_INET ? "IPv4" : "IPv6");
  printf("Adress length   :  %d bytes\n", host->h_length);
  printf("IP adresses     :  \n");

  int i = 0;
  while(host->h_addr_list[i] != NULL)
  {
    struct in_addr* ip = (struct in_addr*) host->h_addr_list[i];
    printf("[%d]  =  %s\n", i, inet_ntoa(*ip));
    ++i;
  }
  
  exit(EXIT_SUCCESS);
}