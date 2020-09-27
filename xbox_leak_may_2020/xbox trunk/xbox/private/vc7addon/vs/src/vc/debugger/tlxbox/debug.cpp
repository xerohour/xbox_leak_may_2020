#include <precomp.h>

#ifdef DBG

TCHAR  rgchDebug[512];
ULONG  DBG_uDebugLevel = 0;

VOID
WINAPIV
DebugPrint(
    LPTSTR szFormat,
    ...
    )
{
    va_list  marker;
    int n;

    va_start( marker, szFormat );
    n = _vsnprintf(rgchDebug, ARRAYSIZE(rgchDebug), szFormat, marker );
    va_end( marker);

    if (n == -1) {
        rgchDebug[ARRAYSIZE(rgchDebug)-1] = 0;
    }

    OutputDebugString( rgchDebug );
}

#endif 
