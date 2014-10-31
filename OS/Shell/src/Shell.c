/*
 * shell.c
 *
 *  Created on: 31-Oct-2014
 *      Author: Harikrishnan.P
 */
/* ----------------------------------------------------------------- */
/* PROGRAM  shell.c                                                  */
/*    This program reads in an input line, parses the input line     */
/* into tokens, and use execvp() to execute the command.             */
/* ----------------------------------------------------------------- */

#include  <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parser.h"

int file_redirect(char *arg)
{
	int fd; /*file descriptor to the file we will redirect ls's output*/
	if(arg==NULL)
	{
		return 0;
	}
	if((fd = open(arg, O_RDWR | O_CREAT ,S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR))==-1){ /*open the file */
		perror("open");
		return 1;
	}

	dup2(fd,STDOUT_FILENO); /*copy the file descriptor fd into standard output*/
	dup2(fd,STDERR_FILENO); /* same, for the standard error */
	close(fd); /* close the file descriptor as we don't need it more  */

	return 0;
}
int spawn_proc (int in, int out, command *cmd)
{
  pid_t pid;

  if ((pid = fork ()) == 0)
    {
      if (in != 0)
        {
          dup2 (in, 0);
          close (in);
        }

      if (out != 1)
        {
          dup2 (out, 1);
          close (out);
        }
      file_redirect(cmd->outfile);
      return execvp (cmd->argv [0], (char * const *)cmd->argv);
    }

  return pid;
}
int fork_pipes (int n, command *cmd)
{
  int i;
  int in, fd [2];

  /* The first process should get its input from the original file descriptor 0.  */
  in = 0;

  /* Note the loop bound, we spawn here all, but the last stage of the pipeline.  */
  for (i = 0; i < n - 1; ++i)
    {
      pipe (fd);

      /* f [1] is the write end of the pipe, we carry `in` from the prev iteration.  */
      spawn_proc (in, fd [1], cmd + i);

      /* No need for the write and of the pipe, the child will write here.  */
      close (fd [1]);

      /* Keep the read end of the pipe, the next child will read from there.  */
      in = fd [0];
    }

  /* Last stage of the pipeline - set stdin be the read end of the previous pipe
     and output to the original file descriptor 1. */
  if (in != 0)
    dup2 (in, 0);

  file_redirect(cmd[i].outfile);
  /* Execute the last stage with the current process. */
  return execvp (cmd [i].argv [0], (char * const *)cmd [i].argv);
}
void  execute(int cmdc,command *cmd)
{
     pid_t  pid;
     int    status;

     if ((pid = fork()) < 0) {
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {
         fork_pipes(cmdc,cmd);
     }
     else {
          while (wait(&status) != pid)
               ;
     }
}
int  main(void)
{
     char  *line=(char *)malloc(1024*sizeof(char));
     char  *argv[64];
     while (1) {
          printf("Custom_Shell$");
          printf(" ");
          gets(line);
          //printf("\n");
          command commLine[6];
          int commands=parse_commands(line,commLine);
          execute(commands,commLine);
     }
}
