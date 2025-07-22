#include <stdbool.h> //bool
#include <unistd.h> //fork, alarm
#include <sys/wait.h> //waitpid
#include <signal.h> //sigaction
#include <string.h> //strsignal
#include <errno.h> //errno
#include <stdio.h> //printf
#include <stdlib.h> //exit

void alarm_handler(int sig)
{
	(void)sig;
}

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	struct sigaction	sa; //struct to handle signals
	pid_t	pid; //to fork child
	int	status; //to verify exiting status
	int sig; //signal var

	sa.sa_handler = alarm_handler; //waitpid will be interrupted if EINTR
	sa.sa_flags = 0; //no flags
	if (sigaction(SIGALRM, &sa, NULL) < 0) //define what happens if an alarm fires (timeout)
		return (-1);
	pid = fork(); //create childprocess
	if (pid == -1) //if fork fails
		return (-1);
	if (pid == 0) //childprocess
	{
		f(); //run function f and exits successfully
		exit(0);
	}
	//handle timout being reached (process interruption)
	alarm(timeout); //alarm set in parent to make sure function doesn't take too long
	if (waitpid(pid, &status, 0) == -1) //wait for child to finish
	{
		if (errno == EINTR) //if interruption due to timeout reached (errno 4)
		{
			kill(pid, SIGKILL); //kill child process if it didn't finish in time
			waitpid(pid, NULL, 0); //make sure there are no zombies
			if (verbose) //print error bad function message
				printf("Bad function: timed out after %d seconds\n", timeout);
			return (0);
		}
		return (-1); //error
	}
	//handle normal exit
	if (WIFEXITED(status))
	{
		if (WEXITSTATUS(status) == 0) //if exit code is 0 (success)
		{
			if (verbose) //print success message
				printf("Nice function!\n");
			return (1);
		}
		else //if exit code is not 0 (failure)
		{
			if (verbose) //print message with exit code
				printf("Bad function: exited with code %d\n", WEXITSTATUS(status));
			return (0);
		}
	}
	//handle crash exit (with signal)
	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status); //get signal
		if (verbose) //print message with signal
			printf("Bad function: %s\n", strsignal(sig));
		return(0);
	}
	return (-1);
}


void	nice_function(void)
{
	//a function that exits normally with exit code 0
	return ;
}

void	bad_ft_exit_code(void)
{
	//a function that exits with a non-0 exit code
	exit(1);
}

void	bad_ft_segfault(void)
{
	//a function that causes a segfault
	char *str = NULL;
	int	i = 2;
	str[i] = 'a';
}

void	bad_ft_timout(void)
{
	//a function that runs indefinitly
	while(1)
	{

	}
}

void	bad_ft_sigkill(void)
{
	//a function that sleeps for longer that the timout and gets killed by the alarm handler
	sleep(5);
}

int	main(void)
{
	int res;

	printf("Test 1: Nice function\n");
	res = sandbox(nice_function, 5, true);
	printf("Result: %d\n", res);

	printf("Test 2: Bad function due to exit code\n");
	res = sandbox(bad_ft_exit_code, 5, true);
	printf("Result: %d\n", res);

	printf("Test 3: Bad function due to segfault\n");
	res = sandbox(bad_ft_segfault, 5, true);
	printf("Result: %d\n", res);

	printf("Test 4: Bad function due to timeout\n");
	res = sandbox(bad_ft_timout, 2, true);
	printf("Result: %d\n", res);

	printf("Test 5: Bad function killed by SIGKILL\n");
	res = sandbox(bad_ft_sigkill, 3, true);
	printf("Result: %d\n", res);

	return (0);
}
