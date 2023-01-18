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
	int restore_stdin = dup(STDIN_FILENO);
	int restore_stdout = dup(STDOUT_FILENO);
	
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

		//list directories and have results sent through pipe
		execlp("ls","ls",(char*)NULL);

		//close pipes if done
		close(fd[1]);

		//if no problems return zero
		return 0;
	}

	if (fork() == 0) 
	{
		//child process - sort
		close(fd[1]);

		//create outfile to store results and check for sucessful open
		int file = open("outfile", O_WRONLY | O_CREAT); 

		//read in from pipe rather than standard input
		dup2(fd[0], STDIN_FILENO);
		//write out to file from pipe
		dup2(file, STDOUT_FILENO);

		execlp("sort","sort","-r","outfile",(char*)NULL);

		//close pipes
		close(fd[0]);
		close(file);
	}
	//parent process

	//wait for child
	wait(0);
	wait(0);

	//restore stdin and stdout, close pipes and files
	dup2(restore_stdin,STDIN_FILENO);
	dup2(restore_stdout,STDOUT_FILENO);

	close(fd[0]);
	close(fd[1]);

	return 0;
}
