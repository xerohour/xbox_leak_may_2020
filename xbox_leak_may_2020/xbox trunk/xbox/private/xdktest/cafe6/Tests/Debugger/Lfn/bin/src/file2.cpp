#include "mainfile.h"

TCHAR glbDBNumbers[21];

int file_two()
{
TCHAR *pch = glbDBNumbers;
TCHAR *t1 = "9876543210", *t2 = "0123456789";

_tcscpy( glbDBNumbers, STR_NUMBERS ); //            This is a comment

_tccpy( pch, t1 );

/*
	pch now points to '9' character
*/

_tccpy( pch, t2 );

return 0;
}


