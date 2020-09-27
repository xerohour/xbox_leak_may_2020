#include "pch.h"

class_pch::class_pch(void)
	{
	class_pch_var = 0;
	}

int class_pch::class_pch_func(int i)
	{
	class_pch_var = i;
	return class_pch_var;
	}

struct_pch::struct_pch(void)
	{
	struct_pch_var = 0;
	}

int struct_pch::struct_pch_func(int i)
	{
	struct_pch_var = i;
	return struct_pch_var;
	}

union_pch::union_pch(void)
	{
	union_pch_var = 0;
	}

int union_pch::union_pch_func(int i)
	{
	union_pch_var = i;
	return union_pch_var;
	}


