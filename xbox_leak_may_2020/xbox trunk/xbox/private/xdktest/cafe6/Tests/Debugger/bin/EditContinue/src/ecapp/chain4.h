#include "foo.h"

#if TEST == cBASIC

#ifdef hasPARAMS
#define  fooParams int paramA, int paramB
#else
#define  fooParams
#endif

#ifdef returnsVALUE
#define  fooReturn  int
#else
#define  fooReturn  void
#endif

fooReturn FUNCTYPE chainmain( fooParams );

#endif  // TEST == cBASIC

int  recursive_dll(int lim , int code);
int  chain_4(int x);
