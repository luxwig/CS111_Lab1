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


//#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

void exe_cmd(command_t c);

int
command_status(command_t c)
{
  return c->status;
}

void exe_simple_cmd(command_t c)
{
  int pid = fork();
  //cannot fork
  if (pid < 0)
    {
      error(1, 0, "forking error");
      exit(127);
    }
  //child process
  else if (pid == 0)
    {
      if (c->input != NULL)
	{
	  int fd = open(c->input, O_RDONLY);
	  if (fd < 0)
	    {
	      error(1, 0, "cannot open file");
	      exit(127);
	    }
	  int redi = dup2(fd, 0);
	  if (redi < 0)
	    {
	      error(1, 0, "redirect input error");
	      exit(127);
	    }
	  close(fd);
	}
      if (c->output != NULL)
	{
	  int fd = open(c->output, O_CREAT | O_TRUNC | O_WRONLY, 0646);
	  if (fd < 0)
	    {
	      error(1, 0, "cannot open file");
	      exit(127);
	    }
	  int redi = dup2(fd, 1);
	  if (redi < 0)
	    {
	      error(1, 0, "redirect output error");
	      exit(127);
	    }
	  close(fd);
	}
      execvp(c->u.word[0], c->u.word);
      error(1, 0, "this should not happen");
      exit(127);
    }
  //parent process
  else
    {
      int status;
      if (waitpid(pid, &status, 0) < 0)
	{
	  error(1, 0, "waitpid failed");
	  exit(127);
	}
      else
	{
	  int exitstatus = WEXITSTATUS(status);
	  c->status = exitstatus;
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
  //if ot success, do not have to do the right part
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
  int firstpid = fork();
  if (firstpid == 0)
    {
      close(fd[1]);
      dup2(fd[0], 0);
      exe_cmd(c->u.command[1]);
    }
  else
    {
      int secondpid = fork();
      if (secondpid == 0)
	{
	  close(fd[0]);
	  dup2(fd[1], 1);
	  exe_cmd(c->u.command[0]);
	}
      else
	{
	  close(fd[0]);
	  close(fd[1]);
	  int status;
	  int returnpid = waitpid(-1, &status, 0);
	  if (returnpid == secondpid)
	    {
	      waitpid(firstpid, &status, 0);
	      c->status = c->u.command[0]->status;
	    }
	  if (returnpid == firstpid)
	    {
	      waitpid(secondpid, &status, 0);
	      c->status = c->u.command[1]->status;
	    }
	}
    }
}


void exe_sub_cmd(command_t c)
{
  exe_cmd(c->u.subshell_command);
  c->status = c->u.subshell_command->status;
}

void exe_cmd(command_t c)
{
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
execute_command(command_t c, bool time_travel)
{
  if (time_travel == 0)
    exe_cmd(c);
}
