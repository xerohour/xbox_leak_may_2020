#include "spcfc01.h"

void nest_func(int N)
{
 	if (func(1)) 
   	func (recur_func() + func(N));
};

int func(int N)
{ 
//		__asm int 3
	return N*2;
};

