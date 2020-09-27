// Intended to be compiled with partial debug info

#include "chainx2.h"
#include "chain4.h"
#include "chain2.h"

int chain_x2(int x)
{
    int Sum;
	// call to a function compiled with partial debug info
    Sum=x+chain_2(x);

	// call to a function compiled with full debug info
	Sum+=chain_4(x);

    return Sum;
};
