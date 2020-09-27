#include "calls.h"

extern void dbg_main();
extern void Types_Tests(int);
extern int main_Calls();

#if !defined( SAVELABEL )
#define SAVELABEL( l )	if (0) goto l
#endif

int main()
{
	int iCalls, nLoop = 10;

	dbg_main();

	iCalls = main_Calls();

	Types_Tests(nLoop);

 	return 0;
}

