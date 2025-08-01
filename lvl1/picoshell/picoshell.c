#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int picoshell(char **cmds[])
{
    int status = 0;
    int i = -1;
    pid_t pid;
    int result = 0;
    int fd[2];
    int fd_stdin = 0;

    while(cmds[i++])
    {
        if(cmds[i + 1])
        {
            if(pipe(fd) == -1)
            {
                if(fd_stdin > 0)
                    close(fd_stdin);
                if(fd[0] >= 0)
                    close(fd[0]);
                if(fd[1] >= 0)
                    close(fd[1]);
                return 1;
            }
        }
        else
        {
            fd[0] = -1;
            fd[1] = -1;
        }
        pid = fork();
        if(pid == -1)
        {
            if(fd_stdin > 0)
                close(fd_stdin);
            if(fd[0] >= 0)
                close(fd[0]);
            if(fd[1] >= 0)
                close(fd[1]);
            return 1;
        }
        if(pid == 0)
        {
            if(fd_stdin)
            {
                if(dup2(fd_stdin, 0) == -1)
                    exit(1);
                close(fd_stdin);
            }
            if(fd[1] != -1)
            {
                if(dup2(fd[1], 1) == -1)
                    exit(1);
                close(fd[0]);
                close(fd[1]);
            }
            execvp(cmds[i][0], cmds[i]);
            exit(1);
        }
        if(fd_stdin != 0)
            close(fd_stdin);
        if(fd[1] != 0)
            close(fd[1]);
        fd_stdin = fd[0];
    }
    while(waitpid(pid,&status, 0) > 0 ) // dunno if it passed with > 0 or < 0
    {
        if(WIFEXITED(status) && WEXITSTATUS(status) != 0)
            result = 1;
        else if(!WIFEXITED(status))
            result = 1;
    }
    if(fd_stdin > 0)
        close(fd_stdin);
    if(fd[0] >= 0)
        close(fd[0]);
    if(fd[1] >= 0)
        close(fd[1]);
    return (result);
}