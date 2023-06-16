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

#define IP_PORT_DELIM   ":"


/* --------------------------------------------------------- */
/*                         T Y P E S                         */
/* --------------------------------------------------------- */

typedef int (*GETSIDENAME)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);


/* --------------------------------------------------------- */
/*                    F U N C T I O N S                      */
/* --------------------------------------------------------- */

// basic socket functions
int Socket(int domain, int type, int protocol);
void Bind(int sockfd, struct sockaddr* addr, socklen_t addrlen);
void Connect(int sockfd, struct sockaddr* addr, socklen_t addrlen);
int Accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
void Listen(int sockfd);
void Close(int sockfd);

// helper socket functions
void Sock_pton(int af, const char* restrict src, void* restrict dst);
char* Sock_ntop(const struct sockaddr* addr, bool portRequired);
char* Sock_getsidename(int sockfd, GETSIDENAME callback, bool portRequired);
in_port_t Sock_get_port(const struct sockaddr* addr);
int Sock_bind_wild(int sockfd, int af);
int Sock_get_backlog();
bool Sock_cmp_addr(const struct sockaddr* addr1, const struct sockaddr* addr2);
bool Sock_cmp_port(const struct sockaddr* addr1, const struct sockaddr* addr2);
void Sock_set_addr(struct sockaddr* addr, const void* src);
void Sock_set_wild(struct sockaddr* addr);
void Sock_set_port(struct sockaddr* addr, const in_port_t port);


#endif // SOCKET_H