#include "precomp.h"
#include <stdio.h>
#include <stdlib.h>

INT	
NTAPI
GetIniInt( 
	IN LPCSTR	lpAppName,				// section name
	IN LPCSTR	lpKeyName,				// key name
	IN INT		nDefault,				// return value if key name not found
	IN LPCSTR	lpFileName				// initialization file name
)
{
	return GetPrivateProfileIntA( lpAppName, lpKeyName, nDefault, lpFileName );
}


FLOAT 
NTAPI
GetIniFloat( 
	IN LPCSTR	lpAppName,				// section name
	IN LPCSTR	lpKeyName,				// key name
	IN FLOAT	fDefault,				// return value if key name not found
	IN LPCSTR	lpFileName				// initialization file name
)
{
	CHAR szBuf[MAX_PATH];
	CHAR szDefault[MAX_PATH];

	FLOAT fVal = 0.0f;

	sprintf( szDefault, "%f", fDefault );

	GetPrivateProfileStringA( (LPCSTR)lpAppName, (LPCSTR)lpKeyName, szDefault, szBuf, MAX_PATH, (LPCSTR)lpFileName );

	fVal = (FLOAT)atof( szBuf );

	return fVal;
}

INT
NTAPI
GetIniString( 
	IN LPCSTR	lpAppName,				// section name
	IN LPCSTR	lpKeyName,				// key name
	IN LPCSTR	lpDefault,				// return value if key name not found
	OUT LPSTR	lpReturnedString,
	IN DWORD	nSize,
	IN LPCSTR	lpFileName				// initialization file name
)
{
	INT nNumChars = 0;
	CHAR* pChar;

	nNumChars = GetPrivateProfileStringA( lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName );
	if ( 0 == nNumChars )
	{
		strcpy( lpReturnedString, lpDefault );
		return nNumChars;
	}

	pChar = strtok( lpReturnedString, "\n;" );
	strcpy( lpReturnedString, pChar );
	nNumChars = strlen( lpReturnedString );
	return nNumChars;
}
