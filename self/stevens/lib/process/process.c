/* --------------------------------------------------------- */
/*                     I N C L U D E S                       */
/* --------------------------------------------------------- */

#include "process.h"


/* --------------------------------------------------------- */
/*                      E X T E R N A L                      */
/* --------------------------------------------------------- */

struct sigaction __sigIgnorance = { .sa_handler = SIG_IGN, .sa_flags = 0 };


/* --------------------------------------------------------- */
/*             P U B L I C   F U N C T I O N S               */
/* --------------------------------------------------------- */

pid_t Fork()
{
	__trace("%s", "Fork()");
	
	pid_t pid = fork();
	if (-1 == pid)
		warning("fork() error");
	return pid;
}

SIGHANDLER Sigaction(int signum, SIGHANDLER sigHandler)
{
	__trace("Sigaction(signum=%d x)", signum);
	
	struct sigaction oldact;
	struct sigaction act = { 0 };

	if (NULL == sigHandler)
	{
		act = __sigIgnorance;
	}
	else
	{
		__unused sigemptyset(&act.sa_mask);
		act.sa_handler = sigHandler;
		act.sa_flags = 0;
		#ifdef SA_RESTART
		if (signum != SIGALRM)				// read [man 2 alarm]
			act.sa_flags |= SA_RESTART;		// read [man 2 signal], [man 7 signal]
		#endif
	}

	int rc = sigaction(signum, &act, &oldact);
	if (-1 == rc)
		error("sigaction() error");

	return oldact.sa_handler;
}

int get_max_proc_fd()
{
	/* 
		notice, that result might or might not
		include descriptor created by opendir()
		and removed by closedir() right before
		function returns
	*/

	int result = -1;
	int temp;
	struct dirent* dirEntry = NULL;

	char dirName[32] = { 0 };
	__unused snprintf(dirName, sizeof(dirName), "/proc/%i/fd/", getpid());

	DIR *dir = opendir(dirName);
	while (true) 
	{
		dirEntry = readdir(dir);
		if (NULL == dirEntry)
			break;

		// skip current and parent dirs
		if (dirEntry->d_name[0] == '.')
			continue;

		temp = atoi(dirEntry->d_name);
		result = (result > temp) ? result : temp;
	};

	__unused closedir(dir);

	return result;
}
