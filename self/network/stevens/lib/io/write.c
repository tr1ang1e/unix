/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include "write.h"


/* --------------------------------------------------------- */
/*             P U B L I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

ssize_t Writen(int fd, const char* src, size_t count)
{
	__trace("Writen(x x count=%zu)", count);
    
    ssize_t actWritten = 0;
    ssize_t	currentWritten;

	while (actWritten < count) 
    {
		currentWritten = write(fd, src, count);
        
        if (currentWritten <= 0)
        {
            if (EINTR == errno)
            {
                // interrupt before any data have been read
                continue;
            }
            else
            {
                // stop, return data were successfully written
                actWritten = (actWritten != 0) ? actWritten : -1;
                break;
            }
        }
        else
        {
            actWritten += currentWritten;
            src += currentWritten;
        }
	}

	return actWritten;
}
