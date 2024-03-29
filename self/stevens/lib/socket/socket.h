#ifndef SOCKET_H
#define SOCKET_H


/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include "common/external.h"
#include "common/utils.h"
#include "error/error.h"


/* --------------------------------------------------------- */
/*                   H E L P   M A C R O S                   */
/* --------------------------------------------------------- */

/*
    for IPv4/6 see [man 3 inet_ntop] macros
    the actual biggest is 108 [man 7 unix] 
    result common is unix but rounded to the power of 2
*/
#define INET_COMMON_ADDRSTRLEN   128   
     
#define IP_PORT_DELIM   ":"                 // just for string representation when passing to output


/* --------------------------------------------------------- */
/*                         T Y P E S                         */
/* --------------------------------------------------------- */

typedef int (*GETSIDENAME)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);


/* --------------------------------------------------------- */
/*                    F U N C T I O N S                      */
/* --------------------------------------------------------- */

// basic
int Socket(int domain, int type, int protocol);
void Bind(int sockfd, struct sockaddr* addr, socklen_t addrlen);
void Connect(int sockfd, struct sockaddr* addr, socklen_t addrlen);
int Accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
void Listen(int sockfd);
void Close(int sockfd);
void Shutdown(int sockfd, int how);

// utility
void Sock_pton(int af, const char* restrict src, void* restrict dst);
char* Sock_ntop(const struct sockaddr* addr, bool portRequired);
char* Sock_getsidename(int sockfd, GETSIDENAME callback, bool portRequired);
bool Sock_getaddrinfo(int af, const char* asciiName, char* ipRepr);

// helper
int Sock_get_backlog();
int Sock_send_rst(int sock);
int Sock_set_lowat(int sockfd, int sndLowat, int rcvLowat);

// unused
int Sock_bind_wild(int sockfd, int af);
in_port_t Sock_get_port(const struct sockaddr* addr);
bool Sock_cmp_addr(const struct sockaddr* addr1, const struct sockaddr* addr2);
bool Sock_cmp_port(const struct sockaddr* addr1, const struct sockaddr* addr2);
void Sock_set_addr(struct sockaddr* addr, const void* src);
void Sock_set_wild(struct sockaddr* addr);
void Sock_set_port(struct sockaddr* addr, const in_port_t port);


#endif // SOCKET_H