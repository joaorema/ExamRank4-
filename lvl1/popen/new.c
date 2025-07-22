#include <unistd.h>
#include <stdlib.h>

int    ft_popen(const char *file, char *const av[], char type)
{
    int fd[2]; //pipe - fd[0] for input/read and fd[1] for output/write
    int pid; //to fork
 
    if (!file || !av || (type != 'r' && type != 'w')) //parse
        return (-1);
    if (pipe(fd) == -1) //perform pipe with fd[0] being read end and fd[1] being write end. if error...
        return (-1);
    pid = fork(); // fork child
    if (pid == -1) // if error...
    {
        close(fd[0]); 
        close(fd[1]);
        return (-1);
    }
    if (pid == 0) //childprocess
    {
        if (type == 'r') // child will write into pipe so parent can read
        {
            if (dup2(fd[1], 1) == -1) // stdout is now redirected to fd[1]
                exit (1); // if error
        }    
        if (type == 'w') // child will read from pipe (written from parent)
        {
            if (dup2(fd[0], 0) == -1) //stdin is now redirected to fd[0]
                exit (1); // if error
        }
        close (fd[1]); // close fd's so there aren't leaks
        close (fd[0]); // close fd's so there aren't leaks
        execvp(file, av); // execute file and av into/from fd[1]/fd[0]
        exit (1); // in case execvp fails, exit
    }  
    if (type == 'r') // we want to read from the child's output
    {
        close(fd[1]); // Close unused write end
        return (fd[0]); // Return read end to read from child output
    }
    else // we want to write to the child's input
    {
        close(fd[0]); // Close unused read end
        return (fd[1]); // Return write end to write to child's input
    }
} 
