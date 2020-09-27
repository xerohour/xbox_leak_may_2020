#include "base2.h"

// inherited, non-overloaded, non-overridden member function
int base2::base2_func(int i)
{
	return base2_var + i;
}

int base2::base2_virtual_func(int i)
{
	return base2_var + i;
}

