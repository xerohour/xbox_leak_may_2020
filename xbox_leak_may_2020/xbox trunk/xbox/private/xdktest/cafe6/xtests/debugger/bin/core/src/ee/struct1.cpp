#include "struct1.h"

struct1::struct1(void)
	{
	struct1_var = 0;
	}

int struct1::struct1_func(int i)
	{
	struct1_var = i;
	return struct1_var;
	}
