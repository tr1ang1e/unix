/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include "read.h"


/* --------------------------------------------------------- */
/*             P U B L I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

ssize_t Readn(int fd, char* dest, size_t expRead)
{
    ssize_t actRead = 0;
    size_t currRead;
    char* currDest = dest;
    
    while (actRead < expRead)
    {
        currRead = read(fd, dest, expRead);

        if (currRead < 0)
        {
            if (EINTR == errno)
            {
                // interrupt before any data have been read
                continue;                           
            }
            else
            {
                // remove data which have been already read
                memset(dest, 0, actRead);
                actRead = -1;
                break;
            }
        }
        else if (0 == currRead)
        {
            // EOF, no more data can be read
            break;
        }
        else
        {
            // another chunk of data have been read
            actRead += currRead;
            currDest += currRead;
        }

    }

    return actRead;
}
