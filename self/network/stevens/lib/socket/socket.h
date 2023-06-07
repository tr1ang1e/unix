#ifndef SOCKET_H
#define SOCKET_H


/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include "common/external.h"
#include "common/utils.h"
#include "error/error.h"


/* --------------------------------------------------------- */
/*                    F U N C T I O N S                      */
/* --------------------------------------------------------- */

char* Sock_ntop(const struct sockaddr* addr, bool portRequired);
in_port_t Sock_get_port(const struct sockaddr* addr);
int Sock_bind_wild(int sockfd, int af);
bool Sock_cmp_addr(const struct sockaddr* addr1, const struct sockaddr* addr2);
bool Sock_cmp_port(const struct sockaddr* addr1, const struct sockaddr* addr2);
void Sock_set_addr(struct sockaddr* addr, const void* src);
void Sock_set_wild(struct sockaddr* addr);
void Sock_set_port(struct sockaddr* addr, const in_port_t port);


#endif // SOCKET_H