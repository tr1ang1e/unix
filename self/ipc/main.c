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

#define PERROR_EXIT()               { perror("fatal error"); goto exit_failure; }

#define FTOK_MAIN_PATNAME           __FILE__
#define FTOK_MAIN_PROJ_ID           'M'

#define PARENT_LOOP_SLEEP_S         0              // 0 sec
#define PARENT_LOOP_SLEEP_NS        100000000      // 0.1 sec
#define CHILD_LOOP_SLEEP_S          2              // 2 sec
#define CHILD_LOOP_SLEEP_NS         0              // 0 sec

#define MSGS_COUNT                  40


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

sig_atomic_t sigFlag;


/* --------------------------------------------------------- */
/*                    P R O T O T Y P E S                    */
/* --------------------------------------------------------- */

static void child_main(int msgQueue);
static void child_sigint_handler(int signum);
static void parent_sigchld_handler(int signum);


/* --------------------------------------------------------- */
/*                          M A I N                          */
/* --------------------------------------------------------- */

int main()
{ 
    int rc;
    
    /* prepare message queue */
    
    key_t queueKey = ftok(FTOK_MAIN_PATNAME, FTOK_MAIN_PROJ_ID);
    int msgQueue = msgget(queueKey, IPC_CREAT | 0660);
    if (-1 == msgQueue)
        PERROR_EXIT();
    
    /* handle child end */

    struct sigaction sigintHandler =
    {
        .sa_handler = parent_sigchld_handler,
        .sa_sigaction = NULL,
        .sa_mask = 0,
        .sa_flags = 0
    };

    rc = sigaction(SIGINT, &sigintHandler, NULL);
    if (-1 == rc)
        PERROR_EXIT();

    /* start child */

    pid_t pid = fork();
    if (0 == pid)
    {        
        child_main(msgQueue);       // never returns
        abort();                    // for safety
    }

    /* prepare messages sending */

    queueMessage msg = 
    {
        .mtype = 1,
        .mtext = { 0 }
    };

    sleepFor.tv_sec = PARENT_LOOP_SLEEP_S;
    sleepFor.tv_nsec = PARENT_LOOP_SLEEP_NS;

    /* work loop */

    for (int i = 0; i < MSGS_COUNT; ++i)
    {
        msg.mtext.value = i;

        rc = msgsnd(msgQueue, &msg, sizeof(msg.mtext), 0);
        if (rc == -1)
            perror("msgsnd");

        nanosleep(&sleepFor, NULL);

        // struct msqid_ds queueInfo;
        // msgctl(msgQueue, IPC_STAT, &queueInfo);
        // if (-1 == msgctl)
        //     PERROR_EXIT();
        // printf("Messages in queue = %ld\n", queueInfo.msg_qnum);
    }

    // debug
    sleepFor.tv_sec = 3 * CHILD_LOOP_SLEEP_S;
    nanosleep(&sleepFor, NULL);

exit:

    exit(EXIT_SUCCESS);

exit_failure:

    exit(EXIT_FAILURE);
}


/* --------------------------------------------------------- */
/*               I M P L E M E N T A T I O N S               */
/* --------------------------------------------------------- */

void child_main(int msgQueue)
{
    int rc;
    
    /* handle parents end */

    struct sigaction sigintHandler =
    {
        .sa_handler = child_sigint_handler,
        .sa_sigaction = NULL,
        .sa_mask = 0,
        .sa_flags = 0
    };

    rc = sigaction(SIGINT, &sigintHandler, NULL);
    if (-1 == rc)
        PERROR_EXIT();

    rc = prctl(PR_SET_PDEATHSIG, SIGINT);
    if (-1 == rc)
        PERROR_EXIT();

    /* pepare messages receiving */
    
    int msgsRcvLoop = 0;
    int msgsRcvTotal = 0;

    sleepFor.tv_sec = CHILD_LOOP_SLEEP_S;
    sleepFor.tv_nsec = CHILD_LOOP_SLEEP_NS;

    queueMessage msg = { 0 };
    int msgTyp = 0;  // 1st message in queue
    int msgRcvFlg = IPC_NOWAIT | MSG_NOERROR;

    /* work loop */

    while (1)
    {        
        msgsRcvLoop = 0;
       
        while (1)
        {            
            rc = msgrcv(msgQueue, &msg, sizeof(msg.mtext), msgTyp, msgRcvFlg);
            if (-1 == rc)
            {
                switch(errno)
                {
                case ENOMSG:  goto sleep;        // all messages were read
                default:      PERROR_EXIT();     // fatal error   
                }               
            }

            ++msgsRcvLoop;

            // debug
            printf("%d\n", msgsRcvLoop);
        }


    sleep:
        
        msgsRcvTotal += msgsRcvLoop;

        // debug
        printf("msgsRcvTotal = %d\n", msgsRcvTotal);

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
        pid = waitpid(-1, &waitStatus, WNOHANG);
        if (0 >= pid)
            break;

        // handle child exit status
        int rc = -1;
        if (WIFEXITED(waitStatus))
        {
            rc = WEXITSTATUS(waitStatus);

            // inform about child
            rc += 48;
            write(STDOUT_FILENO, 
                &rc, 
                sizeof(int));
        }

    }

    return;
}
