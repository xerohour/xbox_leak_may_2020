// utility functions

#include "tests.h"

///////////////////////////////////////////////////////////
// Win32 goodies
///////////////////////////////////////////////////////////

#ifndef _XBOX

char szValidLocalXbePath[MAX_PATH];
WCHAR wszValidLocalXbePath[MAX_PATH];
char szValidRemoteXbePath[MAX_PATH];

void BuildLocalXbe()
{
}

char *GetValidLocalXbePathA()
{
	return szValidLocalXbePath;
}

WCHAR *GetValidLocalXbePathW()
{
	return wszValidLocalXbePath;
}

char *GetValidRemoteXbePath()
{
	return szValidRemoteXbePath;
}

#endif
