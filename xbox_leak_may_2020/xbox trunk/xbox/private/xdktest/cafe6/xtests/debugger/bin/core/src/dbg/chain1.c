/*DOC*
**
** Suite : 
** File  : chain1.c
**
** Revision History :
** M001 2-Feb-1994
** - created from chain1.c from old callstack sources.
**
**DOC*/

#include "foo.h"
#include "chain1.h"
#include "chain2.h"


int Global;

///////////////////////////////////////////////////////////////////////////
int   chain_1(int x)
{
    int Sum;
START:
    Sum=x+chain_2(x);
END:;
    return Sum;
};


///////////////////////////////////////////////////////////////////////////
int   chain_5(int x)
{
START:
    x=x*2;
END:;
    return x;
};

