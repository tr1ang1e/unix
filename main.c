#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "include/clean_code_utils.h"

/* *** FLOW ***
 *  
 * 1. prepare socket
 *    > create socket                     // socket()
 *    > set socket params                 // struct sockaddr_xx (xx = protocol family)
 * 2. bind : socket -<-->- params         // bind()
 * 3. read to / write from socket         // 
 * 
 * */


/*  struct sockaddr_in
    {
      sa_family_t     sin_family;         // unsigned short
      in_port_t       sin_port;           // uint16_t                  -<-- set with htons() (host to network short)
      struct in_addr  sin_addr;           // { unsigned long s_addr; } -<-- set with inet_aton() (ascii to number)
      char            sin_zero[8];        // to align with sizeof(struct sockaddr)
    }
*/
struct sockaddr_in local;

int main(int argc, char* argv[])
{
  
  /* create socket (man socket)
   *
   * domain    :  AF_INET = IPv4
   * type      :  SOCK_DGRAM = UDP (SOCK_STREAM = TCP)
   * protocol  :  0 = default protocol 'type' 
   *  
   * */
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1)
  {
    perror("socket()");
    exit(EXIT_FAILURE);
  }
  printf("socket handler = %d \n", sock);

  // set socket params
  local.sin_family = AF_INET;
  local.sin_port = htons(1234);
  inet_aton("127.0.0.1", &local.sin_addr);

  /* bind : socket -<-->- params
   *
   * pay attention to 'struct sockaddr' and 'socklen_t'
   * different socket families have different params structures:
   *    AF_INET : sockaddr_in
   *    AF_UNIX : sockaddr_un 
   *    ...
   * 
   * for purpose of compatibility, there is the universal structure
   * struct sockaddr 
   * {
   *   sa_family_t sa_family;         // necessary field
   *   char sa_data[14];              // additional size
   * } 
   * 
   * the family-specific structure shoul be casted to this universal one
   *  
   * */
  int result = bind(sock, (struct sockaddr*)&local, sizeof(local));
  if (result == -1)
  {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}