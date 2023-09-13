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

#define LOOP_SLEEP_S          1              // 0 sec
#define LOOP_SLEEP_NS         100000000      // 0.1 sec

#define ALRM_INTERVAL         5


/* --------------------------------------------------------- */
/*                        S T A T I C                        */
/* --------------------------------------------------------- */

struct timespec sleepFor =
{
    .tv_sec = 0,
    .tv_nsec = 0
};

sig_atomic_t alarmFlag;     // guarantee that read and write operations are atomic


/* --------------------------------------------------------- */
/*                    P R O T O T Y P E S                    */
/* --------------------------------------------------------- */

static void child_main(pid_t ppid);
static void child_sigint_handler(int signum);
static void child_sigalrm_handler(int signum);
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

    /* prepare common sigaction */

    struct sigaction sigHandler = { 0 };
    sigemptyset(&sigHandler.sa_mask);
	sigHandler.sa_flags = 0;

    /* handle parents end */

	sigHandler.sa_handler = child_sigint_handler;

    rc = sigaction(SIGINT, &sigHandler, NULL);
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

    /* set alarm handler and start */

	sigHandler.sa_handler = child_sigalrm_handler;

    rc = sigaction(SIGALRM, &sigHandler, NULL);
    if (-1 == rc)
        PERROR_EXIT();

    sigset_t set;
    sigemptyset(&set);

    alarm(ALRM_INTERVAL);

    /* work loop */

    while (1)
    {        
        sigsuspend(&set);
        alarmFlag = 0;

        printf("alarm cycle\n");


        alarm(ALRM_INTERVAL);
    }

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
        pid = waitpid(-1, &waitStatus, WNOHANG);
        if (0 >= pid)
            break;
    }

    return;
}

void child_sigalrm_handler(int signum)
{
    // prevent race condition between
    // first alarm() call and
    // sigsuspend in work loop
    if (alarmFlag)
        alarm(ALRM_INTERVAL);

    // no actual work is required
    return;
}
