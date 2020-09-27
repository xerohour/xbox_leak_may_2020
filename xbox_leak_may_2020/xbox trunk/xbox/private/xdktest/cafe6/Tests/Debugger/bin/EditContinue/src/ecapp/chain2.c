#include "chain2.h"
#include "chain3.h"

int   chain_2(int x)
{
    int Sum;
    Sum=x+chain_3(x);
    return Sum;
};
