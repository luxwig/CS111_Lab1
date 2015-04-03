#include "test.h"

int main()
{
  char* cmd1 = "ls|ls<f>v";
  printf("Input:%s\nOutput:%s\n",get_input(cmd1), get_output(cmd1));
  char* cmd2 = "ls<f";
  printf("Input:%s\nOutput:%s\n",get_input(cmd2), get_output(cmd2));
  char* cmd3 = "ls>f";
  printf("Input:%s\nOutput:%s\n",get_input(cmd3), get_output(cmd3));

}


