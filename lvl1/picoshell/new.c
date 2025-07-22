#include <unistd.h> //for close, fork, execvp, dup2 and pipe
#include <stdlib.h> //for exit
#include <sys/wait.h> //for wait

int    picoshell(char **cmds[])
{
	int fd[2]; //pipe fd's - fd[0] for read end and fd[1] for write end
	pid_t pid; //childprocess pid through fork
	int	i = -1; //to iterate through cmds
	int res = 0; //standard res is 0
	int status = 0; 
	int fd_stdin = 0; //stdin to return to after each loop

	while (cmds[++i]) //looping over each cmd
	{
		if (cmds[i + 1]) //if not last cmd
		{
			if (pipe(fd) == -1) //perform pipe. 
				return (1); //if error return
		}
		else //identify last loop
		{
			fd[0] = -1;
			fd[1] = -1;
		}
		pid = fork(); //create new child process
		if (pid == -1) //if error, close fds and return
		{
			if (fd[0] != -1) //check if open
				close(fd[0]);
			if (fd[1] != -1) //check if open
				close(fd[1]);
			if (fd_stdin != 0) //check if changed
				close(fd_stdin);
			return (1);
		}
		if (pid == 0) //child process
		{
			if (fd_stdin != 0) //if there has been previous cmd
			{
				if (dup2(fd_stdin, 0) == -1) //stdin becomes previous cmd's output
					exit(1);
				close (fd_stdin);
			}
			if (fd[1] != -1) //if not last cmd
			{
				if (dup2(fd[1], 1) == -1) //stdout becomes pipe's write end (fd[1])
					exit(1);
				close(fd[1]);
				close(fd[0]);
			}
			execvp(cmds[i][0], cmds[i]); //exec cmd and args
			exit(1); //in case execvp fails
		}
		if (fd_stdin != 0) //if prev cmd
			close(fd_stdin); // we don't need stdin from prev cmd anymore
		if (fd[1] != -1) //if write end open
			close(fd[1]); //close it
		fd_stdin = fd[0]; //save the read end for the next cmd
	}
	while (wait(&status) > 0) //while all children are finishing we wait
	{
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) //if it exited with a non-0 status (error)
			res = 1;
		else if (!WIFEXITED(status)) //if it didn't exit normally
			res = 1;
	}
	return (res); //0 if success, 1 if error
}
