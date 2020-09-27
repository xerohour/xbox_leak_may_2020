#include "chain3.h"

extern int chain_4(int x);

int   chain_3(int x)
{
    int Sum;
    Sum=x+chain_4(x);
    return Sum;
};
