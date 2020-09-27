#include "global.h"

// global variable.
int global_var = 0;

// non-overloaded global function.
int global_func(int i)
{
	return global_var + i;
}

// over-loaded global function.
int overloaded_global_func(int i)
{
	return global_var + i;
}

char overloaded_global_func(char ch)
{
	return global_var + ch;
}
