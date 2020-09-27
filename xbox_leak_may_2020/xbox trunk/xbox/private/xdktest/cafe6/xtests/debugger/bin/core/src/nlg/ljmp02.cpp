#include "nlgmain.h"
#include <setjmp.h>
#include <float.h>
#include <math.h>

//jmp_buf mark;

void ljmp02(jmp_buf mark, int nCase)
{
	int i = (int)(fmod(nCase, 3.0));
	if( i == 0 )
		longjmp( mark, -1 );
	else
		longjmp( mark, i );
}
