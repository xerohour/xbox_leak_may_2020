/*DOC*
**
** Suite : 
** File  : chain3.c  intended to be compiled without debug information
**
** Revision History :
** M001 2-Feb-1994
** - created from chain3.c from old callstack sources.
**
**DOC*/


#include "foo.h"
#include "chain3.h"


int Global;


extern int chain_4(int x);

///////////////////////////////////////////////////////////////////////////
int   chain_3(int x)
{
    int Sum;
START:
    Sum=x+chain_4(x);
END:;
    return Sum;
};
