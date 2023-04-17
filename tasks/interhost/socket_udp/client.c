// UDP socket client minimal example
// client is responsible for initiationg connection with server
// in non-specific cases client is also responsible for closing the connection

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
 * 1. create client socket                    // socket() = returns socket fd                     
 * 2. bind client socket fd to net params     // unnecessary for client (see comments in code)
 *    > set socket net params                 // struct sockaddr_xx (xx = protocol family)
 *    > bind socket to params                 // bind()
 * 3. prepare server data                     // struct sockaddr_xx (xx = protocol family)
 * 4. write to socket                     
 *    > send message to server                // sendto() = UDP valid send function
 *    > get client socket net params          // getsockname() = local net params are bound with socket
 * 5. read from socket                        // recvfrom()
 * 6. close socket                            // close() 
 * 
 * */

#define CLIENT_PORT // not used = auto set by OS (see comments for '2. bind : sozket -<-->- params in code)
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
  /* __1__ : create client socket (man socket)
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
  printf("client socket handler = %d \n", sock);


  /* __2__ : bind client socket fd to net params
   * NOTE : this step is unnecessary for client side.
   *        
   * COMMENT 1 : 
   * If sendto() or connect() function is used before recfrom(),
   * both of them bind() implicitely with OS-given params. 
   * 
   * COMMENT 2 :
   * In addition, getsockname() will always return  0.0.0.0 IP
   * The same behavior will appear if explicitly set : 
   *    'client.sin_addr.s_addr = INADDR_ANY' despite of
   *    'inet_aton("127.0.0.1", &client.sin_addr)'
   * 
   * */

  // set socket net params
  // client.sin_family = AF_INET;
  // client.sin_port = htons(1234);
  // inet_aton("127.0.0.1", &client.sin_addr);  // 127.0.0.1 = lockalhost = loopback adress :
                                                // IP adress, which is recognized as "my own" without specifying real IP
  // bind socket to params
  // int result = bind(...);                    // see 'server.c' source code for bind() example  


  /* __3__ : prepare server data
   * 
   * IP 127.0.0.1 = lockalhost = loopback adress  >>  IP adress, which means that server are on the same host as me 
   * Port might be any valid number  >>  a port that is listening by server side
   * 
   * */
  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);
  inet_aton("127.0.0.1", &server.sin_addr);    
  

  /* __4__ : write to socket 
   *
   *    About connect:
   * 
   * connect() does not result anything like it does in TCP.
   * It just checks for any immediate errors and store the peer’s IP address and port number, 
   * so no need to pass server address and server address length arguments in sendto():
   * 
   *    int result = connect(...)
   *    int result = sendto(..., (struct sockaddr*)NULL, sizeof(server)); 
   *
   * For UDP it's possible to call connect() multiple times because
   * UDP protocol is connectionless (e.g. TCP = connection based).
   * 
   *  
   *    Write to socket ( see sockaddr_in* cast to sockaddr* essence in 'server.c' : 2. bind socket fd to net params ):
   * 
   * */
  const char* message = "CLIENT MESSAGE";
  size_t messlen = strlen(message);
  int sent = sendto(sock, message, messlen, MSG_CONFIRM, (struct sockaddr*)&server, sizeof(server));
  if (sent == -1)
  {
    perror("sendto()");
    exit(EXIT_FAILURE);
  }
  else if (sent != messlen)
  {
    printf("warning! only %d of %ld bites are sent \n", sent, messlen);
  }

  // get client socket net params = possible only if implicit binding succeded
  int sockaddr_length = sizeof(client);
  int result = getsockname(sock, (struct sockaddr*)&client, &sockaddr_length);
  if (result == -1)
  {
    perror("getsockname()");
    exit(EXIT_FAILURE);
  }
  in_port_t client_port = ntohs(client.sin_port);
  const char* client_ip = inet_ntoa(client.sin_addr);
  printf("my socket net params : IP = %s, port = %d \n", client_ip, client_port);

  /* __ 5__ : read from socket 
   * 
   * 
   * */
  char buffer[BUFSIZ] = { 0 };
  int recieved =  recvfrom(sock, buffer, BUFSIZ, MSG_TRUNC, (struct sockaddr*)&server, &sockaddr_length);
  if (recieved == -1)
  {
    perror("recvfrom()");
    exit(EXIT_FAILURE);
  }
  printf("recieved message : %s \n", buffer); 


  /* __6__ : close socket
   * 
   *  use shutdown() to shut part or full-duplex socket connection down (man 2 shutsown)
   *
   * */
  result = close(sock);
  if(result == -1)
  {
    perror("close()");
    exit(EXIT_FAILURE);
  }


  exit(EXIT_SUCCESS);
}