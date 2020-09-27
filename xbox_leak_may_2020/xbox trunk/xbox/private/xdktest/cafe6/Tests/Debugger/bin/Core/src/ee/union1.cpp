#include "union1.h"

union1::union1(void)
	{
	union1_var = 0;
	}

int union1::union1_func(int i)
	{
	union1_var = i;
	return union1_var;
	}
