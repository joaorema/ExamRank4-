#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int ft_popen(const char *file, char *const argv[], char type)
{
    int pid = 0;
    int fd[2];

    if(!file || !argv || (type != 'r' && type != 'w'))
        return -1;
    if(pipe(fd) == -1)
        return -1;
    pid = fork();
    if(pid == -1)
    {
        close(fd[0]);
        close(fd[1]);
        return -1;
    }
    if(pid == 0)
    {
        if(type == 'r')
        {
            if(dup2(fd[1], 1) == -1)
                exit(1);
        }
        if(type == 'w')
        {
            if(dup2(fd[0], 0) == -1)
                exit(1);
        }
        close(fd[0]);
        close(fd[1]);
        execvp(file, argv);
        exit(1);
    }
    if(type == 'r')
    {
        close(fd[1]);
        return(fd[0]);
    }
    else
    {
        close(fd[0]);
        return (fd[1]);
    }
    exit(1);

}