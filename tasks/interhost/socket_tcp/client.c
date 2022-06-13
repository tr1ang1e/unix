// TCP socket client minimal example
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
 * 4. connect to server                       
 *    > connect                               // connect() = initiate a connection on a socket
 *    > get client socket net params          // getsockname() = local net params are bound with socket
 * 5. write to socket                         // send() = TCP valid send function
 * 6. read from socket                        // recv()
 * 7. close socket                            // close() 
 * 
 * */


#define CLIENT_PORT // not used = auto set by OS (see comments for '2. bind : sozket -<-->- params in code)
#define SERVER_PORT 1234

/*  struct sockaddr_in
    {
      sa_family_t     sin_family;         // unsigned short
      in_port_t       sin_port;           // uint16_t                  -<-- set with htons() (host to network short)
      struct in_addr  sin_addr;           // { unsigned long s_addr; } -<-- set with inet_aton() (ascii to number)
      char            sin_zero[8];        // to align with sizeof(struct sockaddr)
    }
*/
struct sockaddr_in client, server;  // client not used (see comments for '2. bind : sozket -<-->- params in code)


int main(int argc, char** argv)
{

  /* __1__ : create client socket (man socket)
   *
   * domain    :  AF_INET = IPv4
   * type      :  SOCK_STREAM = TCP (SOCK_DGRAM = UDP)
   * protocol  :  0 = default protocol 'type' 
   *  
   * */
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1)
  {
    perror("socket()");
    exit(EXIT_FAILURE);
  }
  printf(" :: __1__ : create client socket. Handler = %d \n", sock);


/* __2__ : bind client socket fd to net params
   * NOTE : this step is unnecessary for client side.
   *        
   * COMMENT 1 : 
   * If one of functions:
   *    sendto()   -  usually for UDP
   *    connect()  -  usually for TCP
   * is used before recfrom(), bind() is not necessary.
   * Both of them bind() implicitely with OS-given params. 
   * 
   * COMMENT 2 :
   * In addition, getsockname() will always return  0.0.0.0 IP
   * The same behavior will appear if explicitly set : 
   *    'client.sin_addr.s_addr = INADDR_ANY' despite of  if (sock == -1)
  {
    perror("socket()");
    exit(EXIT_FAILURE);
  }
  printf(" :: __1__ : create client socket. Handler = %d \n", sock);
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
 
  printf(" :: __2__ : bind socket fd to net params. Unnecessary for client side. Skipping... \n");


  /* __3__ : prepare server data
   * 
   * IP 127.0.0.1 = lockalhost = loopback adress  >>  IP adress, which means that server are on the same host as me 
   * Port might be any valid number  >>  a port that is listening by server side
   * 
   * */
  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);
  inet_aton("127.0.0.1", &server.sin_addr);                 
  printf(" :: __3__ : prepare server data. IP:port = 127.0.0.1:%d \n", SERVER_PORT);


  /* __4__ : connect to server
   * 
   * Effect:
   *   - initiate a connection on a socket
   *   - associate given client socket fd with given server data
   * 
   * Construction (struct sockaddr*)&(...) + sizeof(...) is used in network programming
   * to unify call of different functions (which use sockaddr argument) for different
   * network protocols (which use sockaddr_xx struct)
   *
   * Getting client socket net params is possible only if implicit binding succeded
   * 
   * */

  // connecting
  int result = connect(sock, (struct sockaddr*)&server, sizeof(server));
  if (result == -1)
  {
    perror("connect()");
    exit(EXIT_FAILURE);
  }
  printf(" :: __4__ : connect to server. Success if result is 0. Actual result = %d \n", result);

  // get client socket net params
  int sockaddr_length = sizeof(client);
  result = getsockname(sock, (struct sockaddr*)&client, &sockaddr_length);
  if (result == -1)
  {
    perror("getsockname()");
    exit(EXIT_FAILURE);
  }
  in_port_t client_port = ntohs(client.sin_port);
  const char* client_ip = inet_ntoa(client.sin_addr);
  printf("            My socket net params. IP:port = %s:%d \n", client_ip, client_port);


  /* __5__ : write to socket 
   *
   * */
  const char* message = "CLIENT MESSAGE";
  size_t messlen = strlen(message);
  int sent = send(sock, message, messlen, 0);
  if (sent == -1)
  {
    perror("send()");
    exit(EXIT_FAILURE);
  }
  else if (sent != messlen) printf(" :: __5__ : write to socket. Warning! Only %d of %ld bites are sent \n", sent, messlen);
  else printf(" :: __5__ : write to socket. Success: all of %d bytes are sent \n", sent);


  /* __6__ : read from socket
   *
   * */
  char buffer[BUFSIZ] = { 0 };
  int recieved = recv(sock, buffer, BUFSIZ, MSG_NOSIGNAL);
  if (recieved == -1)
  {
    perror("recv()");
    exit(EXIT_FAILURE);
  }
  printf(" :: __6__ : read from socket. Bytes recieved: %d. Message: %s \n", recieved, buffer);


  /* __7__ : close socket 
   * 
   * */
  result = close(sock);
  if(result == -1)
  {
    perror("close()");
    exit(EXIT_FAILURE);
  }
  printf(" :: __7__ : close socket. Success \n\n");


 exit(EXIT_SUCCESS); 
}