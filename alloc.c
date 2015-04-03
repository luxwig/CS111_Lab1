// UCLA CS 111 Lab 1 storage allocation

#include "alloc.h"

#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

static void
memory_exhausted (int errnum)
{
  error (1, errnum, "memory exhausted");
}

static void *
check_nonnull (void *p)
{
  if (! p)
    memory_exhausted (errno);
  return p;
}

void *
checked_malloc (size_t size)
{
  void *ptr = check_nonnull (malloc (size ? size : 1));
  printf("MEMORY ALLOCATED - ADDR: %p\tSIZE : %d\n", ptr, (unsigned int)size);
  return ptr;
}

void *
checked_realloc (void *ptr, size_t size)
{
  void *ptr1 = check_nonnull (realloc (ptr, size ? size : 1));
  printf("MEMORY REALLOCATED - %p->%p\tSIZE : %d\n", ptr1, ptr, (unsigned int)size);
  return ptr1;
}

void *
checked_grow_alloc (void *ptr, size_t *size)
{
  size_t max = -1;
  if (*size == max)
    memory_exhausted (0);
  *size = *size < max / 2 ? 2 * *size : max;
  return checked_realloc (ptr, *size);
}
