// UCLA CS 111 Lab 1 command reading
#define UNUSED(X) (void)(X)
#include "command.h"
#include "command-internals.h"
#include "alloc.h"

#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <stdio.h>
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

struct elements
{
  char* data;
  bool is_op;
  bool is_sub;
};

//stack for op
struct stack
{
  size_t size;
  size_t capacity;
  struct elements** c;
};

struct elements* top(struct stack* s)
{
  if (s->size == 0) return NULL;
  return (s->c[s->size-1]);
}

struct elements* pop(struct stack* s)
{
  if (s->size == 0) return NULL;
  struct elements* t = top(s);
  s->size--;
  return t;
}

void free_stack(struct stack* s)
{
  while (top(s)) {pop(s);}
  free(s->c);
}

void push(struct stack* s, struct elements *e)
{
  s->c[s->size] = e;
  s->size++;
  if (s->size >= s->capacity){
    s->capacity*=2;
    s->c = checked_grow_alloc(s->c, &(s->capacity));
  }
}

void init(struct stack* s)
{
  s->size = 0;
  s->capacity = 64;
  s->c = checked_malloc(sizeof(struct elements*) * 64);
}


//stack for cmd
struct cmd_stack
{
  size_t size;
  size_t capacity;
  command_t* c;
};

command_t cmd_top(struct cmd_stack* s)
{
  if (s->size == 0) return NULL;
  return (s->c[s->size-1]);
}

command_t cmd_pop(struct cmd_stack* s)
{
  if (s->size == 0) return NULL;
  command_t t = cmd_top(s);
  s->size--;
  return t;
}

void free_cmd_stack(struct cmd_stack* s)
{
  while (cmd_top(s)) {cmd_pop(s);}
  free(s->c);
}

void cmd_push(struct cmd_stack* s, command_t e)
{
  s->c[s->size] = e;
  s->size++;
  if (s->size >= s->capacity){
    s->capacity*=2;
    s->c = checked_grow_alloc(s->c, &(s->capacity));
  }
}

void cmd_init(struct cmd_stack* s)
{
  s->size = 0;
  s->capacity = 64;
  s->c = checked_malloc(sizeof(command_t) * 64);
}

void c_strcpy(char* dest, char* src)
{
  char* tmp = checked_malloc(sizeof(char) * strlen(src) + 1);
  strcpy(tmp,src);
  tmp[strlen(src)] = 0;
  strcpy(dest,tmp);
  dest[strlen(tmp)] = 0;
  free(tmp);//MEMLEAK
}

#ifdef _TEST_OLD
char* get_first_none_space(char* str)
{
  char* p1 = strchr(str, ' ');
  char* p2 = strchr(str, '\t');
  if (p1 == NULL) return p2;
  if (p2 == NULL) return p1;
  return p1 < p2 ? p1 :p2;
}


char* get_last_none_space(char*str)
{
  char* p1 = strrchr(str, ' ');
  char* p2 = strrchr(str, '\t');
  if (p1 == NULL)
  {
    if (p2 == NULL) return NULL;
    return p2-1;
  }
  if (p2 == NULL) return p1-1;
  return p1>p2 ? (p1-1) : (p2-1);
}

void c_strncpy(char* dest, char* src_str, char* src_end)
{
  int size = (src_end-src_str)+1;
  char* tmp = checked_malloc(sizeof(char) * (size+1));
  strncpy(tmp, src_str, size);
  tmp[size] = 0;
  free(dest);
  dest = tmp;
  free(tmp);
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
    c_strncpy(func, func, get_last_none_space(func));
    return func;
  }
  strncpy(func, p1, p2-p1+1);
  return func;
}
#else

bool is_special(char *str)
{
  char c = str[0];
  return (c == '(' || c == '|' || c == '&' || c == ')' || c == ';');
}

char* get_first_none_space(char* str)
{
  size_t size = strlen(str), i;
  for (i = 0 ; i < size; i++)
    if (str[i] != '\t' && str[i] != ' ' && str[i] != '\n') return str + i;
  return NULL;
}


char* get_last_none_space(char*str)
{
  size_t size = strlen(str), i;
  for (i = size - 1; i > 0; i--)
    if (str[i] != '\t' && str[i] != ' ' && str[i] != '\n') return str + i;
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

char** get_func(char* str)
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
  }
  else {
    strncpy(func, p1, p2-p1+1);
    func[p2-p1+1] = 0;
  }
  size_t size = strlen(func);
  size_t i = 0,
	 p = 0,
	 cp = 0;
  char* t; 
  bool b = true;
  char** r = checked_malloc(sizeof(char*) * (strlen(str) + 1));
  for(i = 0; i < size; i++)
  {
    if (b && (func[i] == ' ' || func[i] == '\t'))
    {
      t = c_strncpy(NULL, func+p, func+i-1);
      r[cp] = t;
      cp++;
      b = false;
      continue;
    }
    if (!b && (func[i] != ' ' && func[i] != '\t'))
    {
      p = i;
      b = true;
    }
  }
  t = c_strncpy(NULL, func+p, func+i-1);
  r[cp] = t;
  cp++;
  r[cp] = NULL;
  free(func);
  return r;
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
#endif

struct command_stream
{
  command_t* m_command;
  command_t* p_current;
  size_t size;
  size_t capacity;
};

int get_precedence(char* op)
{
  if (op[0] == '(') return -1;
  if (op[0] == ';') return 0;
  if (op[1] == '&' || op[1] == '|') return 1;
  return 2;
}

enum command_type get_type(char *op)
{
  if (op[0] == ';') return SEQUENCE_COMMAND;
  if (op[1] == '&') return AND_COMMAND;
  if (op[1] == '|') return OR_COMMAND;
  return PIPE_COMMAND;
}
command_t create_cmd(struct elements* e, command_t op1, command_t op2)
{
  command_t r = checked_malloc(sizeof(struct command));
  if (e->is_op)
  {
    r->type = get_type(e->data);
    r->status = -1;
    r->u.command[0] = op1;
    r->u.command[1] = op2;
    r->output = NULL;
    r->input = NULL;
    if (e->is_sub)
    {
      command_t r1 = r;
      r = checked_malloc(sizeof(struct command));
      r->type = SUBSHELL_COMMAND;
      r->status = -1;
      r->output = NULL;
      r->input = NULL;
      r->u.subshell_command = r1;
    }
    return r;
  }
  r->type = SIMPLE_COMMAND;
  r->status = -1;
  r->u.word = get_func(e->data);
  r->input = get_input(e->data);
  r->output = get_output(e->data);
  return r;
}

int get_special(char* str)
{
  char const d_label[][3] = { "&&", ";", "||", "|", "(", ")" };
  char *dptr = NULL,
       *tmp;
  int i; 
  for (i = 0; i < 6; i++)
  {
    tmp = strstr(str, d_label[i]);
    if (!tmp) continue;
    if (dptr)
      dptr = dptr>tmp?tmp:dptr;
    else
      dptr = tmp;
  }
  return dptr?dptr-str:-1;
}

command_t str_to_cmd (char* str)
{
  struct stack s;
  init(&s);
  char* tmp = checked_malloc(sizeof(char) * (strlen(str) + 1));
  strcpy(tmp, str);
  tmp[strlen(str)] = 0;
  size_t size = 0;
  struct elements* e = checked_malloc(sizeof(struct elements) * (strlen(str) + 1));
  int d = get_special(tmp);
  size_t len = strlen(str);
  while (d >= 0)
  {
    if ( d != 0 ){
    e[size].data = checked_malloc(sizeof(char) * (strlen(str) + 1));
    strncpy(e[size].data,tmp,d);
    e[size].data[d] = 0;
    e[size].is_op = false;
    e[size].is_sub = false;
    size++;
    c_strcpy(tmp, tmp+d);
    len-=d;
    tmp[len] = 0;}
    e[size].data = checked_malloc(sizeof(char) * 3);
    e[size].data[0] = tmp[0];
    e[size].data[1] = 0;
    e[size].is_op = true;
    e[size].is_sub = false;
    c_strcpy(tmp, tmp+1);
    len--;
    tmp[len] = 0;
    if ( (e[size].data[0] != ')') && ( tmp[0] == '&' || tmp[0] == '|')){
      e[size].data[1] = tmp[0];
      e[size].data[2] = 0;
      c_strcpy(tmp, tmp+1);
      len--;
      tmp[len] = 0;
    }
    size++;
    d = get_special(tmp);
  };
  if (strlen(tmp) != 0)
  {
    e[size].data = checked_malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(e[size].data,tmp);
    e[size].data[strlen(tmp)] = 0;
    e[size].is_op = false;
    size++;
  }

  free(tmp); // MEMLEAK
  /* DEBUG : 
  size_t i;
  for (i = 0; i < size; i++)
  {
    printf("%s %d\n",e[i].data,e[i].is_op);
  }
  printf("\n");
  error(1,0,"dd");
  */
  size_t o_p = 0;
  struct elements** output = checked_malloc(sizeof(struct elements*) * size); 
  size_t i = 0;
  for (i = 0; i < size; i++)
  {
    // if it is a command 
    if (!e[i].is_op)
    {
      output[o_p] = e + i;
      o_p++;
      continue;
    }

    // if it is a (
    if (e[i].data[0] == '(') { push(&s, e + i); continue; }

    // if it is an op and the stack is empty
    if (!top(&s)) { push(&s, e + i); continue; }
    
    // if it is )
    if (e[i].data[0] == ')')
    {
      while (top(&s)->data[0] != '(')
      {
	output[o_p] = pop(&s);
	o_p++;
      }
      output[o_p - 1]->is_sub = true;
      pop(&s);
      continue;
    }

    // if it is an op and the stact is not empty
    int pc = get_precedence(e[i].data);
    while (top(&s) && get_precedence(top(&s)->data) >= pc)
    {
      output[o_p] = pop(&s);
      o_p++;
    }
    push(&s, e + i);
  }
  while (top(&s))
  {
    output[o_p] = pop(&s);
    o_p++;
  }
  command_t cmd;
  
  /* DEBUG :
  for (i = 0; i < o_p; i++)
    printf("%s ",output[i]->data);
  */
  free_stack(&s);
  struct cmd_stack cs;
  cmd_init(&cs);
  for (i = 0; i < o_p; i++)
  {
    if (output[i]->is_op)
    {
      command_t op1, op2;
      op2 = cmd_pop(&cs);
      op1 = cmd_pop(&cs);
      cmd = create_cmd(output[i], op1, op2); 
      cmd_push(&cs,cmd);
      continue;
    }
    cmd = create_cmd(output[i],NULL,NULL);
    cmd_push(&cs,cmd);
  }
  command_t rr = cmd_pop(&cs);
  free_cmd_stack(&cs);
  for (i = 0; i < size; i ++)  //MEMLEAK
    free(e[i].data);
  free(e);
  free(output);
  return rr;
}

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
/* DEBUG : STACK
  struct stack st;
  init(&st);
  char * a="#1";
  char * b="#2";
  char * c="#3";
  push(&st,a);
  push(&st,b);
  push(&st,c);
  printf("%s",pop(&st));
*/
  void *fm = get_next_byte_argument;
  size_t size = 1024,
	 count = 0;
  int prant = 0;
  char t;
  char *buffer = checked_malloc(size);
  command_t cmd;
  command_stream_t ct = checked_malloc(sizeof(struct command_stream));
  ct->capacity = 64;
  ct->size=0;
  ct->m_command = checked_malloc(sizeof(command_t) * 64);
  ct->p_current = ct->m_command;
  do{
    t = get_next_byte(fm);
    if (t == EOF || t < 0 ) break;
    if (t == '#') {
    // TODO : move towards the next line
    }
    if (t == '\n')
    {
      if (prant == 0)
      {	
	if (count > 0  && 
	    !is_special(get_last_none_space(buffer)) &&
	    buffer[count-1] == '\n') 
	{
          buffer[count - 1]=0;
          // DEBUG : printf("*%s\n", buffer);
          count = 0;
          cmd = str_to_cmd(buffer);
          (ct->m_command)[ct->size] = cmd;
          ct->size++;
          if (ct->size >= ct->capacity)
          {
	    ct->capacity*=2;
	    ct->m_command = checked_grow_alloc(ct->m_command, &(ct->capacity));
	    ct->p_current = ct->m_command;
	  }
	  continue;
        }
        else 
        {
	  if ( is_special(get_last_none_space(buffer)) ) buffer[count] = ' ';
	  else buffer[count] = '\n'; 
        }
      }
      if (prant > 0) buffer[count] = ' ';
      if (prant < 0) error(1,0, "ERROR"); // TODO : ERROR MSG HANDLE
    }
    else
    { 
      if (count > 0 && buffer[count-1] == '\n')
	buffer[count-1] = ';';
      buffer[count] = t;
    }
    if (t == '(') prant++;
    if (t == ')') prant--;
    if (++count >= size) {
      size*=2;
      buffer = checked_grow_alloc(buffer, &size);
    }
    buffer[count] = 0;
  }while(t>=0 && t != EOF);
  if (count != 0)
  { 
          cmd = str_to_cmd(buffer);
          (ct->m_command)[ct->size] = cmd;
          ct->size++;
          if (ct->size >= ct->capacity)
          {
	    ct->capacity*=2;
	    ct->m_command = checked_grow_alloc(ct->m_command, &(ct->capacity));
	    ct->p_current = ct->m_command;
	  }
  }
  ct->p_current = ct->m_command; 
  
  free(buffer);
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  return ct; 
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  if ((s->m_command) + s->size == (s->p_current)) return NULL;
  ((s->p_current))++;
  return *((s->p_current)-1);
  //error (1, 0, "command reading not yet implemented");
}
