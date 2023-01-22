/*****************************************************
* Filename: pipeit.c
*
* Description:	write a program that launches the pipeline 
*               “ls | sort -r > outfile” and waits for it to terminate. When it
*               is finished, outfile should consist of a listing of the files 
*               in the current directory in reverse alphabetical order
*
* Author: Nathan Jaggers
******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include<fcntl.h>

int main(int argc, char** argv)
{
    //create file descriptior and set up pipe
    int fd[2];
    
    if(pipe(fd) != 0)
    {
        printf("Unable to make pipe\n");
    };

    //fork process and do ls and sort separately
    if (fork() == 0) 
    {
        //child process - ls

        //close unused pipes
        close(fd[0]);

        //read out to pipe rather than standard out
        if (dup2(fd[1],STDOUT_FILENO) == -1)
        {
            perror("Error creating write pipe");
            exit(EXIT_FAILURE);
        }

        //close pipes if done
        close(fd[1]);

        //list directories and have results sent through pipe
        if (execlp("ls","ls",(char*)NULL) == -1)
        {
            //after exec program is no longer in control
            //only include error checking at this point
            perror("Error in ls");
            exit(EXIT_FAILURE);
        }

    }

    if (fork() == 0) 
    {
        //child process - sort
        close(fd[1]);

        //create outfile to store results and check for sucessful open
        int file = open("outfile", O_RDWR | O_CREAT | O_TRUNC, 0777);

        if(file == -1)
        {
            perror("Error creating file");
            exit(EXIT_FAILURE);
        }

        //read in from pipe rather than standard input
        if(dup2(fd[0], STDIN_FILENO) == -1)
        {
            perror("Error creating read pipe");
            exit(EXIT_FAILURE);
        }

        //write out to file from pipe
        if (dup2(file, STDOUT_FILENO) == -1)
        {
            perror("Error connecting pipe to file");
            exit(EXIT_FAILURE);
        }

        if (execlp("sort","sort","-r",(char*)NULL) == -1)
        {
            //after exec program is no longer in control
            //only include error checking at this point
            perror("Error in sort");
            exit(EXIT_FAILURE);
        }

    }
    //parent process
    close(fd[0]);
    close(fd[1]);

    //wait for children
    wait(NULL);
    wait(NULL);

    exit(EXIT_SUCCESS);
}