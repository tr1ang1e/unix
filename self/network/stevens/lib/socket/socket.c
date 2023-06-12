/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include "socket.h"


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

static char* sock_ntop(const struct sockaddr* addr, bool portRequired);
static int sock_bind_wild(int sockfd, int af);


/* --------------------------------------------------------- */
/*             P U B L I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

char* Sock_ntop(const struct sockaddr* addr, bool portRequired)
{
    char* result = sock_ntop(addr, portRequired);
    if (NULL == result)
    {
        err_sys("sock_ntop() error");
    }
    return result;
}

char* Sock_getsidename(int sockfd, GETSIDENAME callback, bool portRequired)
{
    /*
        - 'callback' might be getsockname() or getpeername()
        -  tested only for AF_INET and AF_INET6 
    */
    
    char* result = NULL;
    struct sockaddr sideName = { 0 };
    socklen_t sideNameLen = sizeof(sideName);

    int rc = callback(sockfd, &sideName, &sideNameLen);
    if (-1 != rc)
    {
        sa_family_t af = sideName.sa_family; 
        if ((AF_INET == af) || (AF_INET6 == af))
            result = Sock_ntop(&sideName, portRequired);
    }

    return result;
}

in_port_t Sock_get_port(const struct sockaddr* addr)
{
    in_port_t port = 0;

    switch (addr->sa_family)
    {
    case AF_INET:
        port = ((struct sockaddr_in*)addr)->sin_port;
        break;

    case AF_INET6:
        port = ((struct sockaddr_in6*)addr)->sin6_port;
        break;

    default:
        break;
    }

    return port;
}

int Sock_bind_wild(int sockfd, int af)
{
	int	port = sock_bind_wild(sockfd, af);
    if (port < 0)
    {
		err_sys("sock_bind_wild() error");
    }
	return port;
}

bool Sock_cmp_addr(const struct sockaddr* addr1, const struct sockaddr* addr2)
{
    bool result = false;

    if (addr1->sa_family != addr2->sa_family)
        return result;

    switch (addr1->sa_family)
    {
    case AF_INET:
    {
        struct in_addr in_addr1 = ((struct sockaddr_in*)addr1)->sin_addr;
        struct in_addr in_addr2 = ((struct sockaddr_in*)addr2)->sin_addr;
        result = !(bool)memcmp(&in_addr1, &in_addr2, sizeof(struct in_addr));
    }   
    break;

    case AF_INET6:
    {
        struct in6_addr in6_addr1 = ((struct sockaddr_in6*)addr1)->sin6_addr;
        struct in6_addr in6_addr2 = ((struct sockaddr_in6*)addr2)->sin6_addr;
        result = !(bool)memcmp(&in6_addr1, &in6_addr2, sizeof(struct in6_addr));
    } 
    break;

    case AF_UNIX:
    {
        char* pathname1 = ((struct sockaddr_un*)addr1)->sun_path;
        char* pathname2 = ((struct sockaddr_un*)addr2)->sun_path;
        result = !(bool)strcmp(pathname1, pathname2);
    }
    break;

    default:
        break;
    }

    return result;
}

bool Sock_cmp_port(const struct sockaddr* addr1, const struct sockaddr* addr2)
{
    bool result = false;

    if (addr1->sa_family != addr2->sa_family)
        return result;

    switch (addr1->sa_family)
    {
    case AF_INET:
    {
        in_port_t port1 = ((struct sockaddr_in*)addr1)->sin_port;
        in_port_t port2 = ((struct sockaddr_in*)addr2)->sin_port;
        result = (port1 == port2);
    }   
    break;

    case AF_INET6:
    {
        in_port_t port1 = ((struct sockaddr_in6*)addr1)->sin6_port;
        in_port_t port2 = ((struct sockaddr_in6*)addr2)->sin6_port;
        result = (port1 == port2);
    } 
    break;

    default:
        break;
    }  

    return result;
}

void Sock_set_addr(struct sockaddr* addr, const void* src)
{
	switch (addr->sa_family) 
    {
	case AF_INET: 
    {
		struct sockaddr_in* sin = (struct sockaddr_in*)addr;
		memcpy(&sin->sin_addr, src, sizeof(struct in_addr));
	}
    break;

	case AF_INET6: 
    {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6*)addr;
		memcpy(&sin6->sin6_addr, src, sizeof(struct in6_addr));
	}
    break;

    default:
        break;
	}

    return;
}

void Sock_set_wild(struct sockaddr* addr)
{
  	const void* wildptr;

	switch (addr->sa_family) 
    {
	case AF_INET: 
    {
		static struct in_addr in4addr_any;
		in4addr_any.s_addr = htonl(INADDR_ANY);
		wildptr = &in4addr_any;
	}
	break;

	case AF_INET6: 
		wildptr = &in6addr_any;
	    break;

    default:
        return;
	}

	Sock_set_addr(addr, wildptr);
    return;  
}

void Sock_set_port(struct sockaddr* addr, const in_port_t port)
{
    switch (addr->sa_family)
    {
    case AF_INET:
        ((struct sockaddr_in*)addr)->sin_port = port;
        break;

    case AF_INET6:
        ((struct sockaddr_in6*)addr)->sin6_port = port;
        break;

    default:
        break;
    }

    return;
}


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

char* sock_ntop(const struct sockaddr* addr, bool portRequired)
{
    // common used data
    char* result = NULL;
    static char buffer[108 + 1 + 5] = { 0 };    // max Unix domain pathname length [man 7 unix] + IP_PORT_DELIM + max port length
    int err = 0;

    switch (addr->sa_family)
    {
    case AF_INET:
    {
        struct sockaddr_in* sin = (struct sockaddr_in*) addr;

        const char* presentation = inet_ntop(AF_INET, &sin->sin_addr, buffer, sizeof(buffer));
        if (NULL == presentation)
            break;

        if (portRequired)
        {
            in_port_t port = ntohs(sin->sin_port);
            if (0 == port)
                break;

            char portstr[7] = { 0 };
            snprintf(portstr, sizeof(portstr), IP_PORT_DELIM "%d", port);
            __unused strcat(buffer, portstr);
        }

        result = buffer;
    }
    break;

    case AF_INET6:
    {
        struct sockaddr_in6* sin6 = (struct sockaddr_in6*) addr;

        const char* presentation = inet_ntop(AF_INET, &sin6->sin6_addr, buffer, sizeof(buffer));
        if (NULL == presentation)
            err = errno;

        if (portRequired)
        {
            in_port_t port = ntohs(sin6->sin6_port);
            if (0 == port)
                break;

            char portstr[7] = { 0 };
            snprintf(portstr, sizeof(portstr), ".%d", port);
            __unused strcat(buffer, portstr);
        }

        result = buffer;
    }
    break;

    case AF_UNIX:
    {
        // TODO
    }
    break;

    case AF_PACKET:  
    {
        // AF_LINK in BSD
        // TODO
    }
    break;

    default:
        break;
    }

    return result;
}

int sock_bind_wild(int sockfd, int af)
{
	int result = -1;
    int rc;
    socklen_t len;

	switch (af) 
    {
	case AF_INET: 
    {
		struct sockaddr_in sin = 
        { 
            .sin_family = AF_INET,
	        .sin_port = htons(0),                    // ephemeral port
		    .sin_addr.s_addr = htonl(INADDR_ANY),    // universal address
            .sin_zero = { 0 }
        };

        rc = bind(sockfd, (struct sockaddr*)&sin, sizeof(sin));
        if (rc < 0)
            break;

		len = sizeof(sin);
		rc = getsockname(sockfd, (struct sockaddr*)&sin, &len);
        if (rc < 0)
            break;
		
		result = sin.sin_port;
	}
    break;

	case AF_INET6: 
    {
		struct sockaddr_in6	sin6 =
        {
		    .sin6_family = AF_INET6,
		    .sin6_port = htons(0),          // ephemeral port
		    .sin6_addr = in6addr_any,       // universal address
            .sin6_flowinfo = 0, 
            .sin6_scope_id = 0
        };

        rc = bind(sockfd, (struct sockaddr*)&sin6, sizeof(sin6));
        if (rc < 0)
            break;

		len = sizeof(sin6);
		rc = getsockname(sockfd, (struct sockaddr*)&sin6, &len);
        if (rc < 0)
            break;

        result = sin6.sin6_port;
	}

    default:
        break;
    }

	return result;
}
