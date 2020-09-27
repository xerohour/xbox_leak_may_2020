#ifdef _XBOX
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

//==================================================================================
// wsprintf
//==================================================================================
int wsprintf(LPSTR pszOutput, LPCSTR pszFormat, ...);
HANDLE FindFirstFile(LPCSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData);
#endif 