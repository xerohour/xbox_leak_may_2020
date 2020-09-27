// Utilities.h
//

#ifndef _UTILITIES_
#define _UTILITIES_

#include <math.h>

typedef long    TCENT;

TCENT Mils2TimeCents(DWORD dwMils);
DWORD TimeCents2Mils(TCENT tcTime);

#define DEF_ROUND_MULDIV

#ifdef DEF_ROUND_MULDIV

#define MUL_DIV(x,y,z) (long) ( ((x) * (y) / (float) (z)) + 0.5 )

#else

#define MUL_DIV(x,y,z) MulDiv((x),(y),(z))

#endif //DEF_ROUND_MULDIV
#endif //_UTILITIES_