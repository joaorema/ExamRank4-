#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


int picoshell(char **cmds[])
{
    pid_t pid;
    int status = 0;
    int i = -1;
    int result;
    int fd[2];
    int fd_stdin;

    while(cmds[i++])
    {
        if(cmds[i +1]) //while there are more cmds
        {
            if(pipe(fd) < 0)
                return -1;
        }
        else  //lst cmd
        {
            fd[0] = -1;
            fd[1] = -1;
        }
        pid = fork();
        if(pid < 0)
        {
            close(fd[0]);
            close(fd[1]);
            return 1;
        }
        if(pid == 0)
        {
            if(fd_stdin != 0)
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
        close(fd[0]);
        close(fd[1]);
        fd_stdin = fd[0];
    }
    while(wait(&status) > 0)
    {
        if (!WIFEXITED(status) || (WIFEXITED(status) && (WEXITSTATUS(status) != 0)))			
			result = 1;
    }
    return result;
}