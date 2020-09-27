#include "mbctest.h"

TCHAR glbDBNumbers[21];

int file_two()
{
TCHAR *pch = glbDBNumbers;
TCHAR *t1 = "ã‡", *t2 = "ÇO";				// ã‡= 8be0,ÇO= 824f

_tcscpy( glbDBNumbers, DBC_NUMBERS );		// Copy ã‡ÇPÇQÇRÅBÅBÅB

_tccpy( pch, t1 );

/*
	pch now points to 'ã‡' DBCS character
*/

_tccpy( pch, t2 );


return 0;
}


