// UDP socket server minimal example
// in general, server is just another client


#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "../../../include/clean_code_utils.h"

/* *** FLOW ***
 *  
 * 1. create server socket                // socket() = returns socket fd                     
 * 2. bind socket fd to net params        
 *    > set socket net params             // struct sockaddr_xx (xx = protocol family)
 *    > bind socket to params             // bind()
 * 3. read from socket                    
 *    > get message from client           // recvfrom()
 *    > check client socket net params    // getpeername() = remote net params are associated with socket
 * 4. write to socket                     // sendto()
 * 5. close socket                        // close() 
 * 
 * */

#define CLIENT_PORT // not used = recieve from socket connection (IP protocol header)
#define SERVER_PORT 1111

/*  struct sockaddr_in
    {
      sa_family_t     sin_family;         // unsigned short
      in_port_t       sin_port;           // uint16_t                  -<-- set with htons() (host to network short)
      struct in_addr  sin_addr;           // { unsigned long s_addr; } -<-- set with inet_aton() (ascii to number)
      char            sin_zero[8];        // to align with sizeof(struct sockaddr)
    }
*/
struct sockaddr_in client, server;

int main(int argc, char* argv[])
{
  /*** 1 ***/

  /* create client socket (man socket)
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
  printf("server socket handler = %d \n", sock);


  /*** 2 ***/

  /* bind socket fd to net params
   * NOTE : this step is necessary for UDP server
   *
   * COMMENT 1 :
   * pay attention to 'struct sockaddr' and 'socklen_t'
   * different socket families have different params structures:
   *    AF_INET : sockaddr_in
   *    AF_UNIX : sockaddr_un 
   *    ...
   * 
   * COMMENT 2 :
   * for purpose of compatibility, there is the universal structure
   * struct sockaddr 
   * {
   *   sa_family_t sa_family;         // necessary field
   *   char sa_data[14];              // additional size
   * } 
   * the family-specific structure should be casted to this universal one
   *  
   * */

  // set socket net params
  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);
  inet_aton("127.0.0.1", &server.sin_addr);  // 127.0.0.1 = lockalhost = loopback adress :
                                             // IP adress, which is recognized as "my own" without specifying real IP

  // bind socket to params
  int result = bind(sock, (struct sockaddr*)&server, sizeof(server));
  if (result == -1)
  {
    perror("bind()");
    exit(EXIT_FAILURE);
  }


  /*** 3 ***/

  // read from socket
  char buffer[BUFSIZ] = { 0 };
  int sockaddr_length = sizeof(client);
  int recieved =  recvfrom(sock, buffer, BUFSIZ, MSG_TRUNC, (struct sockaddr*)&client, &sockaddr_length);
  if (recieved == -1)
  {
    perror("recvfrom()");
    exit(EXIT_FAILURE);
  }
  printf("recieved message : %s \n", buffer);

  // // check client socket net params : these params are placed into '&client' by 'recvfrom()' function 
  // result = getpeername(sock, (struct sockaddr*)&client, &sockaddr_length);
  // if (result == -1)
  // {
  //   perror("getpeername()");
  //   exit(EXIT_FAILURE);
  // }
  in_port_t client_port = ntohs(client.sin_port);
  const char* client_ip = inet_ntoa(client.sin_addr);
  printf("client socket net params : IP = %s, port = %d \n", client_ip, client_port);


  /*** 4 ***/

  // write to socket
  const char* message = "SERVER MESSAGE";
  size_t messlen = strlen(message);
  int sent = sendto(sock, message, messlen, MSG_CONFIRM, (struct sockaddr*)&client, sizeof(client));
  if (sent == -1)
  {
    perror("sendto()");
    exit(EXIT_FAILURE);
  }
  else if (sent != messlen)
  {
    printf("warning! only %d of %ld bites are sent \n", sent, messlen);
  }


  /*** 5 ***/

  // close socket
  // use shutdown() to shut part or full-duplex socket connection down (man 2 shutdown)
  close(sock);


  exit(EXIT_SUCCESS);
}