// UCLA CS 111 Lab 1 command execution

#define UNUSED(x) (void)(x)
#include "command.h"
#include "command-internals.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <error.h>
#include <string.h>

void exe_cmd(command_t c);

int
command_status(command_t c)
{
  return c->status;
}

void exe_redi_cmd(command_t c)
{
  if (c->input != NULL)
    {
      int fd = open(c->input, O_RDONLY);
      if (fd < 0)
	{
	  error(1, 0, "Cannot open file");
	}
      int redi = dup2(fd, 0);
      if (redi < 0)
	{
	  error(127, 0, "Redirect input error");
	}
      close(fd);
    }
  if (c->output != NULL)
    {
      int fd = open(c->output, O_CREAT | O_TRUNC | O_WRONLY, 0646);
      if (fd < 0)
	{
	  error(1, 0, "Cannot open file");
	}
      int redi = dup2(fd, 1);
      if (redi < 0)
	{
	  error(127, 0, "Redirect output error");
	}
      close(fd);
    }
}


//deal with exec command, but not exit the whole process and terminate the shell
//not sure if need to be fixed
void exe_simple_cmd(command_t c)
{
  char str[] = "exec";
  if (strcmp(c->u.word[0], str) == 0)
    {
      execvp(c->u.word[1], &(c->u.word[1]));
      error(127, 0, "Command does not found");
    }
  else
    {
      int pid = fork();

      //cannot fork
      if (pid < 0)
	{
	  error(127, 0, "Forking error");
	}

      //child process
      else if (pid == 0)
	{
	  exe_redi_cmd(c);
	  execvp(c->u.word[0], c->u.word);
	  error(127, 0, "Command does not found");
	}
      //parent process
      else
	{
	  int status;
	  if (waitpid(pid, &status, 0) < 0)
	    {
	      error(127, 0, "waitpid failed");
	    }
	  else
	    {
	      int exitstatus = WEXITSTATUS(status);
	      c->status = exitstatus;
	    }
	}
    }
}

void exe_and_cmd(command_t c)
{
  //execute left part
  exe_cmd(c->u.command[0]);
  //if success, then execute right part
  if (c->u.command[0]->status == 0)
    {
      exe_cmd(c->u.command[1]);
      c->status = c->u.command[1]->status;
    }
  //if not success, do not have to do the right part
  else
    {
      c->status = c->u.command[0]->status;
    }
}

void exe_or_cmd(command_t c)
{
  //execute the left part
  exe_cmd(c->u.command[0]);
  //if success, do not have to execute the right part
  if (c->u.command[0]->status == 0)
    {
      c->status = c->u.command[0]->status;
    }
  //if not success, execute right part
  else
    {
      exe_cmd(c->u.command[1]);
      c->status = c->u.command[1]->status;
    }
}

void exe_sequence_cmd(command_t c)
{
  exe_cmd(c->u.command[0]);
  exe_cmd(c->u.command[1]);
  c->status = c->u.command[1]->status;
}

void exe_pipe_cmd(command_t c)
{
  int fd[2];
  pipe(fd);
  pid_t firstpid = fork();
  if (firstpid == 0)
    {
      close(fd[0]);
      dup2(fd[1], 1);
      exe_cmd(c->u.command[0]);
      exit(c->u.command[0]->status);
    }
  else if (firstpid < 0) 
    error (127,0,"Forking error");
  else
    {
      pid_t secondpid = fork();
      if (secondpid == 0)
	{
	  close(fd[1]);
	  dup2(fd[0], 0);
	  exe_cmd(c->u.command[1]);
	  exit(c->u.command[1]->status);
	}
      else if (secondpid < 0)
	error(127,0,"Forking error");
      else
	{
	  close(fd[0]);
	  close(fd[1]);
	  int f_status;
	  int s_status;
	  pid_t returnpid = waitpid(-1, &f_status, 0);
	  if (returnpid == firstpid)
	    {
	      //set LHS exit status
	      c->u.command[0]->status = WEXITSTATUS(f_status);
	      //set RHS exit status and pipe cmd exit status
	      waitpid(secondpid, &s_status, 0);
	      c->u.command[1]->status = WEXITSTATUS(s_status);
	      c->status = c->u.command[1]->status;
	    }
	  else if (returnpid == secondpid)
	    {
	      //set RHS exit status and pipe cmd exit status
	      c->u.command[1]->status = WEXITSTATUS(f_status);
	      c->status = c->u.command[1]->status;
	      //set LHS exit status
	      waitpid(firstpid, &s_status, 0);
	      c->u.command[0]->status = WEXITSTATUS(s_status);
	    }
	  else
	    error(127, 0, "waitpid error");
	}
    }
}


void exe_sub_cmd(command_t c)
{
  exe_redi_cmd(c);
  exe_cmd(c->u.subshell_command);
  c->status = c->u.subshell_command->status;
}

void exe_cmd(command_t c)
{
  if (!c) error(1, 0, "Command cannot exec");
  switch (c->type)
    {
    case SIMPLE_COMMAND:
      exe_simple_cmd(c);
      break;
    case AND_COMMAND:
      exe_and_cmd(c);
      break;
    case OR_COMMAND:
      exe_or_cmd(c);
      break;
    case SEQUENCE_COMMAND:
      exe_sequence_cmd(c);
      break;
    case SUBSHELL_COMMAND:
      exe_sub_cmd(c);
      break;
    case PIPE_COMMAND:
      exe_pipe_cmd(c);
      break;
    default:
      error(1, 0, "not specified");
    }
}

void
execute_command (command_t c, bool time_travel)
{
  if (time_travel == 0)
    exe_cmd(c);
}
