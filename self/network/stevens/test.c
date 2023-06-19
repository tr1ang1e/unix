/*
    Test if there is a blocked signals queue or not.
    To run testing:
        run ./__t from one terminal and get <pid>
        from another terminal send

            1st scenario:
                kill  -7 <pid>      // handler will be run
                kill -17 <pid>      // blocked signal ...
                kill -17 <pid>      // ... send several times ...
                kill -17 <pid>      // ... to ensure about queue
            Result: 
                after -7 signal handler finishes it's work,
                -17 signal handler will bw called only ONE time.

            2nd scenario:
                kill  -7 <pid>      // handler will be run
                kill -17 <pid>      // blocked signal ...
                kill  -7 <pid>      // ... and another blocked signal
            Result:
                after -7 signal handler finishes it's work,
                -17 signal handler will be skipped and
                -7 signal handler will be run again

    Conclusios: 
        - there is no default queue in Linux signals
        - only the last of blocked signals will be handled
*/


#include "lib.h"


void sighand(int signum)
{
    switch (signum)
    {
    case SIGBUS:  // 7
    {
        __console("start SUGBUS\n");
        sleep(20);
        __console("end SUGBUS\n");
    }
    break;

    case SIGCHLD:  // 17
    {
        __console("SIGCHLD\n");
    }
    break;

    default:
        break;
    }
}


int main()
{
	struct sigaction act = { 0 };
	__unused sigemptyset(&act.sa_mask);
	act.sa_handler = sighand;
	act.sa_flags = 0;
	__unused sigaction(SIGCHLD, &act, NULL);

    __unused sigaddset(&act.sa_mask, SIGCHLD);
	__unused sigaction(SIGBUS, &act, NULL);
    
    __console("%d\n", getpid());
    sleep(120);
    return 0;
}
