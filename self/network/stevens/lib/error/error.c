/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include "error.h"


/* --------------------------------------------------------- */
/*                      E X T E R N A L                      */
/* --------------------------------------------------------- */

bool __daemon_proc = false;             // not daemon by default


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

static void err_print_err(bool useErrno, const char* fmt, va_list args);    // format and flush error message into 'stderr'


/* --------------------------------------------------------- */
/*             P U B L I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

void err_msg(const char *fmt, ...)
{
    __va_init(args, fmt);
    err_print_err(false, fmt, args);
    __va_destroy(args);

    return;
}

void err_quit(const char* fmt, ...) 
{
    __va_init(args, fmt);
    err_print_err(false, fmt, args);
    __va_destroy(args);

    exit(EXIT_FAILURE);
}

void err_sys(const char *fmt, ...)
{
    __va_init(args, fmt);
    err_print_err(true, fmt, args);
    __va_destroy(args);

    exit(EXIT_FAILURE);
}


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

void err_print_err(bool useErrno, const char* fmt, va_list args)
{
    int errnoKeep = errno;
    char msg[MAXLINE] = { 0 };

    __unused vsnprintf(msg, sizeof(msg), fmt, args);
    if (useErrno)
    {
        size_t msgSize = strlen(msg);
        snprintf(msg + msgSize, sizeof(msg) - msgSize, ": %s", strerror(errnoKeep));
    }
    const char* msgEnd = "\n";
    __unused strncat(msg, msgEnd, sizeof(msg) - strlen(msg) - strlen(msgEnd));

    if (__daemon_proc)
    {
        // do smth
    }

    fflush(stdout);  // in case 'stderr' is redirected to 'stdout'
    fputs(msg, stderr);
    fflush(stderr);

    return;
}
