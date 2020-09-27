#include "dll.h"

dll_class::dll_class(void)
{
	dll_class_var = 0;
}

int dll_class::dll_class_func(int i)
{
	dll_class_var = i;
	return dll_class_var;
}
