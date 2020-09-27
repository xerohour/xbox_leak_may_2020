#include "derived1.h"

int derived1::derived1_static_var = 1;

derived1::derived1(void)
{
	derived1_public_var = base1_var + base2_var;
	enum_class_var = two_class;
	derived1_private_var = derived1_public_var + 1;
	derived1_protected_var = derived1_private_var + 1;
}

// non-inherited, non-overloaded member function.
int derived1::derived1_public_func(int i)
{
	return derived1_public_var + i;
}

// virtual inherited non-overloaded, overridden member function
int derived1::base1_virtual_func(int i)
{
	return derived1_public_var + i;
}

/*
int derived1::derived1_static_func(int i)
	{
	return derived_static_var + i;
	}
*/

// inherited, overloaded, non-overridden member function
int derived1::base1_overloaded_func(char ch)
{
	return derived1_public_var + ch;
}

// inherited, non-overloaded, overridden member function
int derived1::base1_overridden_func(int i)
{
	return derived1_public_var + i;
}

// protected member function.
int derived1::derived1_protected_func(int i)
{
	return derived1_public_var + i;
}

// private member function.
int derived1::derived1_private_func(int i)
{
	return derived1_public_var + i;
}




