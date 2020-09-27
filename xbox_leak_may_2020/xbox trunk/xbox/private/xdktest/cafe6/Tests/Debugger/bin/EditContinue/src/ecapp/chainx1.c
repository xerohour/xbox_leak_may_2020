// Intended to be compiled without debug info

#include "chainx1.h"
#include "chainx2.h"
#include "chain3.h"

int chain_x1(int x)
{
    int Sum;

	// call to a function compiled with partial debug info
    Sum=x+chain_x2(x);

	// call to a function compiled without debug info
	Sum+=chain_3(x);

    return Sum;
};



