// identical comdat folding

#include "dbgtest.h"

int NonUniqueComdatFunction1(int i1, char c1)
{
    return i1;
}

unsigned int NonUniqueComdatFunction2(unsigned int i2, unsigned char c2)
{
    return i2;
}

void Test_ICF()
{
    NonUniqueComdatFunction1(0, 'A');
    NonUniqueComdatFunction2(0, 'A');
}
