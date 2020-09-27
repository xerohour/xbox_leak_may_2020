#ifdef _XBOX
#include "convhelp.h"

//==================================================================================
// wsprintf
//==================================================================================
int wsprintf(char *pszOutput, char const *pszFormat, ...)
{
	int		nRet = 0;
	WCHAR	szWideOutput[1024], szWideFormat[1024];

	va_list pArgs;
	va_start(pArgs, pszFormat);

	if(strlen(pszFormat) > 1023)
		return -1;

	// Covert the original format string to wide...
	mbstowcs(szWideFormat, pszFormat, strlen(pszFormat));
	szWideFormat[strlen(pszFormat)] = 0;

	// Do the wide version of wsprintf
	if((nRet = swprintf(szWideOutput, szWideFormat, pArgs)) > 1023)
		return -1;

	va_end(pArgs);

	// Convert the created wide string to multi-byte...
	wcstombs(pszOutput, szWideOutput, wcslen(szWideOutput));
	pszOutput[wcslen(szWideOutput)] = 0;

	return nRet;
}

HANDLE FindFirstFile(LPCSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData)
{
	WCHAR szWideFileName[MAX_PATH];

	mbstowcs(szWideFileName, lpFileName, strlen(lpFileName));
	szWideFileName[strlen(lpFileName)] = 0;

	return FindFirstFile(szWideFileName, lpFindFileData);
}

#endif // ! XBOX
