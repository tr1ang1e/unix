/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/prctl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>


/* --------------------------------------------------------- */
/*                       D E F I N E S                       */
/* --------------------------------------------------------- */

#define PERROR_EXIT()        { perror("fatal error"); goto exit_failure; }

#define LOOP_SLEEP_S          1              // 1 sec
#define LOOP_SLEEP_NS         100000000      // 0.1 sec

#define SELECT_SLEEP_S        5              // 5 sec
#define SELECT_SLEEP_NS       100000000      // 0.1 sec

#define SIGPDEATH             SIGINT


/* --------------------------------------------------------- */
/*                        S T A T I C                        */
/* --------------------------------------------------------- */

struct timespec sleepForLoop =
{
    .tv_sec = LOOP_SLEEP_S,
    .tv_nsec = LOOP_SLEEP_NS
};

struct timespec sleepForSelect =
{
    .tv_sec = SELECT_SLEEP_S,
    .tv_nsec = SELECT_SLEEP_NS
};

// signal flags
sig_atomic_t sigalrmFlag;
sig_atomic_t sigpdeathFlag;


/* --------------------------------------------------------- */
/*                    P R O T O T Y P E S                    */
/* --------------------------------------------------------- */

static void child_main(pid_t ppid);
static void child_signals_handler(int signum);
static void parent_sigchld_handler(int signum);


/* --------------------------------------------------------- */
/*                          M A I N                          */
/* --------------------------------------------------------- */

int main()
{ 
    int rc;
    
    /* avoid race condition in chiild process */

    pid_t ppid = getpid();
    printf("Parent process PID: %d\n", ppid);

    /* handle child end */

    struct sigaction sigchldHandler = { 0 };
    sigemptyset(&sigchldHandler.sa_mask);
	sigchldHandler.sa_handler = parent_sigchld_handler;
	sigchldHandler.sa_flags = 0;

    rc = sigaction(SIGCHLD, &sigchldHandler, NULL);
    if (-1 == rc)
        PERROR_EXIT();

    /* start child */

    pid_t pid = fork();
    if (0 == pid)
    {        
        child_main(ppid);       // never returns
        abort();                // for safety
    }

    /* work loop */

    while (1)
    {
        nanosleep(&sleepForLoop, NULL);
    }

exit_failure:

    exit(EXIT_FAILURE);
}


/* --------------------------------------------------------- */
/*               I M P L E M E N T A T I O N S               */
/* --------------------------------------------------------- */

void child_main(pid_t ppid)
{
    int rc;
    
    printf("Child process PID: %d\n", getpid());

    /* form child signals mask */

    sigset_t set;
    sigemptyset(&set);

    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGPDEATH);

    rc = sigprocmask(SIG_BLOCK, &set, NULL);
    if (-1 == rc)
        PERROR_EXIT();

    /* set signals handlers */

    struct sigaction sigHandler = { 0 };
    sigemptyset(&sigHandler.sa_mask);
	sigHandler.sa_flags = 0;

	sigHandler.sa_handler = child_signals_handler;
    rc = sigaction(SIGALRM, &sigHandler, NULL);
    if (-1 == rc)
        PERROR_EXIT();

	sigHandler.sa_handler = child_signals_handler;
    rc = sigaction(SIGPDEATH, &sigHandler, NULL);
    if (-1 == rc)
        PERROR_EXIT();

    /* handle parents end */

    rc = prctl(PR_SET_PDEATHSIG, SIGPDEATH);
    if (-1 == rc)
        PERROR_EXIT();

    if (getppid() != ppid)
    {
        // avoid race condition in case if parent is already dead
        printf("Parent is already dead. Can't use prctl()\n");
        goto exit_failure;
    }

    /* work loop */

    sigemptyset(&set);

    while (1)
    {        
        pselect(0, NULL, NULL, NULL, &sleepForSelect, &set);

        /* 
            no need to check return value, because:
            - 0 and NULL args are always valid
            - struct timespec is correct (programmer responsibility) and never changes
            - EINTR checking is useless because getting signal is pselect() perpose
        */

        if (sigalrmFlag)
        {
            sigalrmFlag = 0;
            continue;
        }

        if (sigpdeathFlag)
        {
            const char* msgOnTerm = "Child is terminated by parent's termination\n";
            write(STDOUT_FILENO, msgOnTerm, strlen(msgOnTerm));
            abort();
        }

        /* 
            if we are here it means that
            pselect time expired instead 
            of getting any signals
            
            it's possible to place timer-based
            execution logic here
        */

        printf("pselect() time expired\n");
    }

exit_failure:

    exit(EXIT_FAILURE);
}

void child_signals_handler(int signum)
{
    switch (signum)
    {
    case SIGALRM:
        sigalrmFlag = 1;
        break;

    case SIGPDEATH:
        sigpdeathFlag = 1;
        break;

    default: 
        break;
    }

    return;
}

void parent_sigchld_handler(int signum)
{
    int waitStatus;
    pid_t pid; 
    
    while (1)
    {
        pid = waitpid(-1, &waitStatus, WNOHANG);
        if (0 >= pid)
            break;
    }

    return;
}
