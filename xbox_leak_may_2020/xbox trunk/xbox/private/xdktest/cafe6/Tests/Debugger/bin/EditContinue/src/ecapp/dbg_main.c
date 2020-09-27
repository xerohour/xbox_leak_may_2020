#include "dbg_main.h"
#include "chainx1.h"

extern int ChainMain();
extern int OtherMain();

#if !defined( SAVELABEL )
#define SAVELABEL( l )	if (0) goto l
#endif

void dbg_main (void)
{
	int x;
	static int y = -1;
	static double z;

	SAVELABEL(TagCallNoCVInfo);
	SAVELABEL(TagCallOtherMain);
	SAVELABEL(TagCallChainMain);

	TagCallChainMain:
	ChainMain();

	TagCallOtherMain: 
	OtherMain();

	TagCallNoCVInfo: 
	y = chain_x1(99);

	x = y;
}




