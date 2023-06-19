#ifndef WRITE_H
#define WRITE_H


/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include "common/external.h"
#include "common/utils.h"


/* --------------------------------------------------------- */
/*                    F U N C T I O N S                      */
/* --------------------------------------------------------- */

ssize_t Writen(int fd, const char* src, size_t count);      // write count bytes, return -1 if no data sent


#endif // WRITE_H