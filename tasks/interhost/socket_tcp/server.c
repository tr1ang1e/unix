// TCP socket server minimal example
// server is responsible for 
//   - listening socket and acknowleging connection
//   - answering through an established connection

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
 * 1. create server socket to listen      // socket() = returns socket fd                     
 * 2. bind socket fd to net params        
 *    > set socket net params             // struct sockaddr_xx (xx = protocol family)
 *    > bind socket to params             // bind()
 *    > check the binding is correct      // getsockname()
 * 3. start to listen socket              // listen()
 * 4. accept connection
 *    > accept                            // accept()
 *    > get current port number           // getsockname() = local net params are associated with socket
 *    > get client socket net params      // getpeername() = remote net params are associated with socket
 * 5. read from socket                    // recv()
 * 6. write to socket                     // send()
 * 7. close socket                        // close()
 * 
 * */


#define CLIENT_PORT // not used = recieve from socket connection (IP protocol header)
#define SERVER_PORT 1234

// defines for handling connections
#define CLIENT_QUEUE 4
#define MAX_CLIENTS 12

/*  struct sockaddr_in
    {
      sa_family_t     sin_family;         // unsigned short
      in_port_t       sin_port;           // uint16_t                  -<-- set with htons() (host to network short)
      struct in_addr  sin_addr;           // { unsigned long s_addr; } -<-- set with inet_aton() (ascii to number)
      char            sin_zero[8];        // to align with sizeof(struct sockaddr)
    }
*/
struct sockaddr_in client, server;


int main(int argc, char** argv)
{
  /* __1__ : create server socket to listen  
   *
   * */
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket == -1)
  {
    perror("socket()");
    exit(EXIT_FAILURE);
  }
  printf(" :: __1__ : create server socket to listen. Handler = %d \n", listenSocket);


  /* __2__ : bind socket fd to net params
   *
   * Explicit binding to the given port. If .sin_port equals zero then
   * the OS itself desides whish port should be provided to the socket 
   * 
   * */

  // set socket net params
  inet_aton("127.0.0.1", &server.sin_addr);
  server.sin_port = htons(SERVER_PORT);
  server.sin_family = AF_INET;
  
  // bind
  int result = bind(listenSocket, (struct sockaddr*)&server, sizeof(server));
  if(result == -1)
  {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  // check the binding is correct
  struct sockaddr_in temp = {0};
  int temp_length = sizeof(temp);
  result = getsockname(listenSocket, (struct sockaddr*)&temp, &temp_length);
  if(result == -1)
  {
    perror("getsockname()");
    exit(EXIT_FAILURE);
  }
  printf(" :: __2__ : bind socket fd to net params. IP:port = %s:%d \n", inet_ntoa(temp.sin_addr), ntohs(temp.sin_port));


  /* __3__ : start to listen socket 
   *
   * listen() have possibility to keep simultaneously connecting
   * clients in the special queue, which is called backlog. Corresponding
   * parameter takes the size of this backlog. If client doesn't fit,
   * it has special error code
   * 
   * */
  int clientSocket[MAX_CLIENTS] = { 0 };
  result = listen(listenSocket, CLIENT_QUEUE);
  if(result == -1)
  {
    perror("listen()");
    exit(EXIT_FAILURE);
  }
  printf(" :: __3__ : start to listen socket \n");


  // the following loop is useless in this file (given just for read-write example)
  // every client socket should be managed by thread/process or 
  // using multiplexing after it was accepted in the loop like this


  for(int num = 0; num < MAX_CLIENTS; ++num)
  {
    
    /* __4__ : accept connection
     *
     * Accept puts client data to given sockaddr structure if it is given. In the
     * following example this opportunity is skipped (replaced with NULL) to 
     * demonstrate working and differences between getsockname() and getpeername(). 
     * 
     * */
    
    // accept
    clientSocket[num] = accept(listenSocket, NULL, NULL);
    if(clientSocket[num] == -1)
    {
      perror("accept()");
      exit(EXIT_FAILURE);
    }
    printf(" :: __4__ : accept connection. Success. Info: \n");

    // get current port number
    int sockaddr_length = sizeof(client);
    result = getsockname(clientSocket[num], (struct sockaddr*)&client, &sockaddr_length);
    if(result == -1)
    {
      perror("getsockname()");
      exit(EXIT_FAILURE);
    }
    const char* local_ip = inet_ntoa(client.sin_addr);
    in_port_t local_port = ntohs(client.sin_port);
    printf("            Local data.   IP:port = %s:%d \n", local_ip, local_port);

    // get client socket net params
    result = getpeername(clientSocket[num], (struct sockaddr*)&client, &sockaddr_length);
    if(result == -1)
    {
      perror("getpeername()");
      exit(EXIT_FAILURE);
    }
    const char* client_ip = inet_ntoa(client.sin_addr);
    in_port_t client_port = ntohs(client.sin_port);
    printf("            Remote data.  IP:port = %s:%d \n", client_ip, client_port);


    /* __5__ : read from socket
     * 
     * */
    char buffer[BUFSIZ] = { 0 };
    int recieved = recv(clientSocket[num], buffer, BUFSIZ, 0);
    if (recieved == -1)
    {
      perror("recv()");
      exit(EXIT_FAILURE);
    }
    printf(" :: __5__ : read from socket. Client: %d. Bytes recieved: %d. Message: %s \n", num, recieved, buffer);
    

    /* __6__ : write to socket
     *
     * */
    const char* message = "SERVER MESSAGE";
    size_t messlen = strlen(message);
    int sent = send(clientSocket[num], message, messlen, MSG_NOSIGNAL);
    if (sent == -1)
    {
      perror("send()");
      exit(EXIT_FAILURE);
    }
    else if (sent != messlen) printf(" :: __6__ : write to socket. Warning! Only %d of %ld bites are sent \n", sent, messlen);
    else printf(" :: __6__ : write to socket. Success: all of %d bytes are sent \n", sent);    


    /* __7__ : close socket 
     * 
     * */
    result = close(clientSocket[num]);
    if(result == -1)
    {
      perror("close()");
      exit(EXIT_FAILURE);
    }
    printf(" :: __7__ : close socket. Success \n\n");


    // break example loop to end the program
    break;
  }

  close(listenSocket);
  exit(EXIT_SUCCESS);
}