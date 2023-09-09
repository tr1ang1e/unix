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
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>


/* --------------------------------------------------------- */
/*                       D E F I N E S                       */
/* --------------------------------------------------------- */

#define PERROR_EXIT()        { perror("fatal error"); goto exit_failure; }

#define FTOK_MAIN_PATNAME    __FILE__
#define FTOK_MAIN_PROJ_ID    'M'

#define LOOP_SLEEP_S          0              // 0 sec
#define LOOP_SLEEP_NS         100000000      // 0.1 sec

#define MSGS_COUNT            40


/* --------------------------------------------------------- */
/*                         T Y P E S                         */
/* --------------------------------------------------------- */

typedef struct 
{
    int value;
} messageText;

typedef struct 
{
    long mtype;
    messageText mtext;
} queueMessage;


/* --------------------------------------------------------- */
/*                        S T A T I C                        */
/* --------------------------------------------------------- */

struct timespec sleepFor =
{
    .tv_sec = 0,
    .tv_nsec = 0
};


/* --------------------------------------------------------- */
/*                    P R O T O T Y P E S                    */
/* --------------------------------------------------------- */

static void child_main(pid_t ppid);
static void child_sigint_handler(int signum);
static void parent_sigchld_handler(int signum);


/* --------------------------------------------------------- */
/*                          M A I N                          */
/* --------------------------------------------------------- */

int main()
{ 
    int rc;
    
    /* avoid race condition in slavchilde process */

    pid_t ppid = getpid();
    printf("Parent process PID: %d\n", ppid);

    /* prepare sleep structure */

    sleepFor.tv_sec = LOOP_SLEEP_S;
    sleepFor.tv_nsec = LOOP_SLEEP_NS;

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
        nanosleep(&sleepFor, NULL);
    }

exit:

    exit(EXIT_SUCCESS);

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

    /* handle parents end */

    struct sigaction sigintHandler = { 0 };
    sigemptyset(&sigintHandler.sa_mask);
	sigintHandler.sa_handler = child_sigint_handler;
	sigintHandler.sa_flags = 0;

    rc = sigaction(SIGINT, &sigintHandler, NULL);
    if (-1 == rc)
        PERROR_EXIT();

    rc = prctl(PR_SET_PDEATHSIG, SIGINT);
    if (-1 == rc)
        PERROR_EXIT();

    /* avoid race condition in case if parent is already dead */

    if (getppid() != ppid)
    {
        printf("Parent is already dead. Can't use prctl()\n");
        goto exit_failure;
    }

    /* work loop */

    while (1)
    {        
        nanosleep(&sleepFor, NULL);
    }

exit:

    exit(EXIT_SUCCESS);

exit_failure:

    exit(EXIT_FAILURE);
}

void child_sigint_handler(int signum)
{
    const char* msgOnTerm = "Child is terminated by parent's termination\n";
    write(STDOUT_FILENO, 
          msgOnTerm, 
          strlen(msgOnTerm));

    abort();
}

void parent_sigchld_handler(int signum)
{
    int waitStatus;
    pid_t pid; 
    
    while (1)
    {
        // printf("1\n");       // order!
    
        pid = waitpid(-1, &waitStatus, WNOHANG);
        if (0 >= pid)
            break;
    
        // printf("2\n");       // order!

        // handle child exit status
        int rc = -1;
        
        if (WIFEXITED(waitStatus))
        {
            rc = WEXITSTATUS(waitStatus);
            rc += 48;
            write(STDOUT_FILENO, 
                  &rc, 
                  sizeof(int));

            const char* state = " <- exit\n";
            write(STDOUT_FILENO, 
                  state, 
                  strlen(state));
        }

        if (WIFSIGNALED(waitStatus))
        {
            rc = WTERMSIG(waitStatus);
            rc += 48;
            write(STDOUT_FILENO, 
                  &rc, 
                  sizeof(int));

            const char* state = " <- signal\n";
            write(STDOUT_FILENO, 
                  state, 
                  strlen(state));
        }
    }

    return;
}
