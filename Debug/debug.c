// UCLA CS 111 Lab 1 command reading
#define UNUSED(X) (void)(X)
#include "alloc.h"

#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <stdio.h>
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */


void c_strcpy(char* dest, char* src)
{
  char* tmp = checked_malloc(sizeof(char) * strlen(src) + 1);
  strcpy(tmp,src);
  tmp[strlen(src)] = 0;
  strcpy(dest,tmp);
  dest[strlen(tmp)] = 0;
}

char* get_first_none_space(char* str)
{
  size_t size = strlen(str), i;
  for (i = 0 ; i < size; i++)
    if (str[i] != '\t' && str[i] != ' ') return str + i;
  return NULL;
}


char* get_last_none_space(char*str)
{
  size_t size = strlen(str), i;
  for (i = size - 1; i > 0; i--)
    if (str[i] != '\t' || str[i] != ' ') return str + i;
  return str;
}

char* c_strncpy(char* dest, char* src_str, char* src_end)
{
  int size = (src_end-src_str)+1;
  char* tmp = checked_malloc(sizeof(char) * (size+1));
  strncpy(tmp, src_str, size);
  tmp[size] = 0;
  free(dest);
  return tmp;
}

char* get_func(char* str)
{
  char* func = checked_malloc(sizeof(char) * (strlen(str) + 1));
  strcpy(func, str);
  char* r1 = strchr(str,'<');
  char* r2 = strchr(str,'>');
  char *p1 = get_first_none_space(str),
       *p2 = get_last_none_space(str);
  if (!(p1&&p2)) return NULL;
  if (r1 || r2)
  {
    r1 = r1?r1:r2;
    strncpy(func, p1, r1-p1);
    func[r1-p1] = 0;
    func = c_strncpy(func, func, get_last_none_space(func));
    return func;
  }
  strncpy(func, p1, p2-p1+1);
  return func;
}

char* get_input(char *str)
{
  char* func = checked_malloc(sizeof(char) * (strlen(str) + 1));
  strcpy(func, str);
  char* r1 = strchr(str,'<');
  char* r2 = strchr(str,'>');
  if (r1)
  { 
    r2 = r2? r2-1 : str + strlen(str) - 1;
    strncpy(func, r1+1, r2-r1);
    func[r2-r1] = 0;
    func = c_strncpy(func,
	get_first_none_space(func),
	get_last_none_space(func));
    return func;
  }
  return NULL;
}

char* get_output(char *str)
{
  char* func = checked_malloc(sizeof(char) * (strlen(str) + 1));
  strcpy(func, str);
  char* r1 = strchr(str,'>');
  char* r2 = str + strlen(str) - 1;
  if (r1)
  {
    strncpy(func, r1+1, r2-r1);
    func[r2-r1] = 0;
    func = c_strncpy(func,
	get_first_none_space(func),
	get_last_none_space(func));
    return func;
  }
  return NULL;
}

