/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include "read.h"


/* --------------------------------------------------------- */
/*                        G L O B A L                        */
/* --------------------------------------------------------- */

static bool ReadnbufBufferEmpty = true;


/* --------------------------------------------------------- */
/*             P U B L I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

ssize_t Readn(int fd, char* dest, size_t reqCount)
{
    __trace("Readn(x x reqCount=%zu)", reqCount);
    
    ssize_t actRead = 0;
    ssize_t currRead;
    
    while (actRead < reqCount)
    {
        currRead = read(fd, dest, reqCount);

        if (currRead < 0)
        {
            __info("read error: %d = %s", errno, strerror(errno));
            
            if (EINTR == errno)
            {
                // interrupt before any data have been read
                continue;                           
            }
            else
            {
                // stop, return data wich were successfully read if any
                actRead = (actRead != 0) ? actRead : -1;
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
            dest += currRead;
        }
    }

    return actRead;
}

ssize_t Readntime(int fd, char* dest, size_t reqCount, uint64_t maxTime)
{
    __trace("Readntime(x x reqCount=%zu maxTime=%ld)", reqCount, maxTime);

    uint64_t startTime = get_time_ms();
    ssize_t actRead = 0;
    ssize_t currRead = 0;

    while (true)
    {
        // time limit is reached
        if ((get_time_ms() - startTime) >= maxTime)
            break;

        // all requested data are sent
        if (actRead == reqCount)
            break;

        /* read another chunk of data */

        currRead = Readn(fd, dest, reqCount);

        if (-1 == currRead)
        {
            /*
                error occur while reading, return:
                    - error (-1) if no data were already sent to caller
                    - number of bytes were already sent to caller if any
            */
            
            actRead = (actRead != 0) ? actRead : -1;
            break;   
        }
        else
        {
            actRead += currRead;
            dest += currRead;
            reqCount -= currRead;
        }
    }

    return actRead;
}

ssize_t Readnbuf(int fd, char* dest, size_t reqCount)
{
    /*
        Function behaves in a fololowing way:
            1. try to send to caller all requested data from buffer
            2. if data in buffer is not enough:
                - sent to caller available data
                - try to read new data from fd
            3. call itself again (go to 1.)

        It returns error (-1) only if simultaneously:
            1. buffer is empty when function is called by caller (not by itself)
            2. and error returned from reading function          
    */
    
    __trace("Readnbuf(x x reqCount=%zu)", reqCount);

    /* static data */

    static char buffer[BUFSIZ] = { 0 };     
    static char* begin = buffer;     // first available element except (begin == end) case
    static char* end = buffer;       // first after last element   

    /* main logic */

    ssize_t actCount = 0;
    ssize_t availCount = end - begin;     
    
    if (availCount >= reqCount)
    {
        memcpy(dest, begin, reqCount);
        begin += reqCount;
        actCount = reqCount;
    }
    else
    {
        // copy available data first
        memcpy(dest, begin, availCount);
        actCount = availCount;
        dest += availCount;
        reqCount -= availCount;

        /* 
            if we are here, 2 variants are possible:
                - buffer is empty
                - all data are taken from buffer
            and in both variants we can use 
            buffer as if it doesn't store any data
        */

        ssize_t availCount;
        do
        {
            begin = buffer;
            availCount = read(fd, buffer, sizeof(buffer));
            __debug("read() result=%ld", availCount);

        } while((-1 == availCount) && (EINTR == errno));  // retry if interrupted by signal

        switch (availCount)
        {
        case -1:
        {            
            /*
                error occur while reading, return:
                    - error (-1) if no data were already sent to caller
                    - number of bytes were already sent to caller if any
            */
            __info("read error: %d = %s", errno, strerror(errno));

            actCount = (actCount != 0) ? actCount : -1;     
            end = buffer;
        }
        break;

        case 0:
        {
            // no more data to be read
            end = buffer;
        }
        break;

        default:
        {
            // actualize end iterator state            
            end = buffer + availCount;
            
            // recursion
            ssize_t recursionResult = Readnbuf(fd, dest, reqCount);
            recursionResult = (recursionResult == -1) ? 0 : recursionResult;
            actCount += recursionResult;
        }
        break;
        }
    }

    /*
        the way to decide if buffer is empty is to test 
        if begin iterator equals to end one. It migth be
        done once before return
    */
    ReadnbufBufferEmpty = (begin == end) ? true : false;

    return actCount;
}

ssize_t Readline(int fd, char* dest, size_t reqCount)
{   
    __trace("Readline(x x reqCount=%zu)", reqCount);
    
    ssize_t actRead;
    ssize_t rc;
    char current;

    for (actRead = 1; actRead < reqCount; ++actRead)
    {
        /*
            Count from 1 to reqCount because:
            - easier to count actual read bytes 
            - count always less than reqCount so null terminator 
                always might be placed to the destination buffer
        */

        rc = Readnbuf(fd, &current, 1);
       
        switch (rc)
        {
        case 1:  // symbol is read
        {            
            *dest++ = current;
            if ('\n' == current)
            {
                __debug("%s", "Line is read");
                *dest = '\0';
                goto exit;
            }
        }
        break;

        case 0:  // EOF
        {
            if (1 == actRead)  actRead = 0;
            else  *dest = '\0';
            goto exit;
        }
        break;

        default:  // error
        {
            actRead = -1;
            goto exit;
        }
        break;
        }
    }

exit:

    return actRead;
}

bool ReadnbufEmpty()
{
    return ReadnbufBufferEmpty;
}
