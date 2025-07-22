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

int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
    struct sigaction    sa;
    pid_t pid;
    int status;
    int sig;

    sa.sa_handler = alarm_handler;
    sa.sa_flags = 0;

    if(sigaction(SIGALRM, &sa, NULL) < 0)
        return -1;
    pid = fork();
    if(pid == -1)
        return -1;
    if(pid == 0)
    {
        f();
        exit(0);
    }
    alarm(timeout);
    if(waitpid(pid, &status, 0) == -1)
    {
        if(errno == EINTR)
        {
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);
            if(verbose)
                printf("Bad fuction : timed out after %d seconds\n", timeout);
            return 0;
        }
        return -1;
    }
    if(WIFEXITED(status))
    {
        if(WEXISTSTATUS(status) == 0)
        {
            if(verbose)
                printf("Nice fuction!\n");
            return 1;
        }
        else
        {
            if(verbose)
                printf("Bad fuction: exited with code %d\n", WEXITSTATUS(status));
            return 0;
        }
    }
    if(WIFSIGNALED(status))
    {
        sig = WTERMSIG(status);
        if(verbose)
                printf("Bad fuction: %s\n", strsignal(sig));
        return 0;
    }
    return -1;
}

//man 3 wait 