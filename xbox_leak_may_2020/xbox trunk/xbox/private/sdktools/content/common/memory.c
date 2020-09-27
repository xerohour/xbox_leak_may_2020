/*
 * memory.c
 */

#include "cabarc.h"


byte *mem_malloc(long size)
{
   return (malloc(size));
}


void mem_free(byte *mem)
{
   free(mem);
}
