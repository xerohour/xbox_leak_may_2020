/*DOC*
**
** Suite : 
** File  : chain2.c.   Intended to be compiled with partial debug info.
**
** Thus have line number information, but do not have information about locals ...
**
** Revision History :
** M001 2-Feb-1994
** - created from chain2.c from old callstack sources.
**
**DOC*/

#include "foo.h"
#include "chain2.h"
#include "chain3.h"


int Global;

///////////////////////////////////////////////////////////////////////////
int   chain_2(int x)
{
    int Sum;
START:
    Sum=x+chain_3(x);
END:;
    return Sum;
};
