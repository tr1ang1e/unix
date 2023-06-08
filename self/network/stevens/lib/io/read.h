#ifndef READ_H
#define READ_H


/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include "common/external.h"
#include "common/utils.h"


/* --------------------------------------------------------- */
/*                   H E L P   M A C R O S                   */
/* --------------------------------------------------------- */


/* --------------------------------------------------------- */
/*                         T Y P E S                         */
/* --------------------------------------------------------- */


/* --------------------------------------------------------- */
/*                      E X T E R N A L                      */
/* --------------------------------------------------------- */


/* --------------------------------------------------------- */
/*                    F U N C T I O N S                      */
/* --------------------------------------------------------- */

ssize_t Readn(int fd, char* dest, size_t reqCount);                         // read reqCount or less bytes, return -1 if no data read and error occur
ssize_t Readntime(int fd, char* dest, size_t reqCount, uint64_t maxTime);   // read as much data as might be read during ms time but not more than reqCount 
ssize_t Readnbuf(int fd, char* dest, size_t reqCount);                      // read reqCount or less bytes, buffered


#endif // READ_H