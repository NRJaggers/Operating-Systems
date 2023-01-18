/*****************************************************
* Filename: pipeit.c
*
* Description:	write a program that launches the pipeline “ls | sort -r > outfile” and
*				waits for it to terminate. When it is finished, outfile should consist of a listing of the files in
*				the current directory in reverse alphabetical order
*
* Author: Nathan Jaggers
******************************************************/

#include <stdio.h>
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
		dup2(fd[1],STDOUT_FILENO);

		//close pipes if done
		//close(fd[1]);

		//list directories and have results sent through pipe
		if (execlp("ls","ls",(char*)NULL) == -1)
		{
			perror("Error in ls");
			return -1;
			//could also use exit?
		}

		//after exec program is no longer in control
		//only include error checking at this point
	}

	if (fork() == 0) 
	{
		//child process - sort
		close(fd[1]);

		//create outfile to store results and check for sucessful open
		int file = open("outfile", O_RDWR | O_CREAT | O_EXCL, 777); 
		
		//FILE *fptr = fopen("output","w+");
		//int file = fileno(fptr);

		//read in from pipe rather than standard input
		dup2(fd[0], STDIN_FILENO);
		//write out to file from pipe
		dup2(file, STDOUT_FILENO);

		execlp("sort","sort","-r","outfile",(char*)NULL);

		//after exec program is no longer in control
		//only include error checking at this point
		perror("Error in sort");
		return -1;

	}
	//parent process

	close(fd[0]);
	close(fd[1]);

	//wait for child
	wait(0);
	wait(0);

	return 0;
}

//not working, next idea -> error check system calls