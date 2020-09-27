/*DOC*
**
** CHAIN0.C
**
** Revision History :
** M001 2-Feb-1994
** - created from chain0.c, old callstack sources
**
**DOC*/


#include <string.h>
#include "foo.h"
#include "chain1.h"


#ifdef DLL                 // define DLL to enable dll tests
#include "chain4.h"
#endif

int Global=0;
int Result=0;
char * String=" Some sample string";


int ChainMain() {
  TagCallChain1:
  Result=chain_1(3);
DONE:
  return 1;
};

