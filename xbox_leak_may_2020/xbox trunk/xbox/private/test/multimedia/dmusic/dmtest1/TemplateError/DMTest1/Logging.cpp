
#include "globals.h"
DWORD g_dwDebugLevel = 5;
DWORD g_dwIndentLevel = 0;


/********************************************************************************
********************************************************************************/
void LogSetDbgLevel(DWORD dwLevel)
{
    g_dwDebugLevel = dwLevel;
}


void LogIncrementIndent()
{
    g_dwIndentLevel++;
};


void LogDecrementIndent()
{
    g_dwIndentLevel--;
};


/********************************************************************************
********************************************************************************/
void LogConsole
(
    LPSTR                   szFormat,
    ...
)
{
    //FOR NOW, SINCE WE DON'T HAVE CONSOLE OUTPUT!!
    return;
    
    va_list va;
    char szBuffer[1000];

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    va_end(va);

    //A carriage return goes here when we've decided not to output to VC anymore!!
    DbgPrint("CONSOLE:%s\n",szBuffer);

};


/********************************************************************************
********************************************************************************/
void Log
(
    DWORD                   dwLevel,
    DWORD                   dwDestinationFlags,
    LPSTR                   szFormat,
    ...
)
{

    va_list va;
    if (dwLevel > g_dwDebugLevel)
        return;
    char szBuffer[1000];

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    va_end(va);
    
    if (dwDestinationFlags & DEBUG)
        DbgPrint("%s\n", szBuffer);
/*
    if (dwDestinationFlags & FILE)
        xLog(g_hLog, XLL_PASS, szBuffer);
*/
    if (dwDestinationFlags & CONSOLE)
        LogConsole(szBuffer);
};

/********************************************************************************
********************************************************************************/
void Log
(
    DWORD                   dwLevel,
    LPSTR                   szFormat,
    ...
)
{

    va_list va;
    if (dwLevel > g_dwDebugLevel)
        return;
    char szBuffer[1000];

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    va_end(va);
    
        DbgPrint("%s\n", szBuffer);
        LogConsole(szBuffer);
};

/********************************************************************************
********************************************************************************/
void fnsLog
(
    DWORD                   dwLevel,
    DWORD                   dwDestinationFlags,
    LPSTR                   szFormat,
    ...
)
{

    va_list va;
    if (dwLevel > g_dwDebugLevel)
        return;
    char szBuffer[1000];

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    va_end(va);
    
    if (dwDestinationFlags & DEBUG)
        DbgPrint("%s\n", szBuffer);
    if (dwDestinationFlags & FILE)
        xLog(g_hLog, XLL_PASS, szBuffer);
    if (dwDestinationFlags & CONSOLE)
        LogConsole(szBuffer);
};

/********************************************************************************
********************************************************************************/
void fnsLog
(
    DWORD                   dwLevel,
    LPSTR                   szFormat,
    ...
)
{

    va_list va;
    if (dwLevel > g_dwDebugLevel)
        return;
    char szBuffer[1000];

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    va_end(va);
    
        DbgPrint("%s\n", szBuffer);
//        xLog(g_hLog, XLL_PASS, szBuffer);
        LogConsole(szBuffer);
};



/*
void Log
(
    DWORD                   dwLevel,
    LPSTR                   szFormat,
    ...
)
{
    char szBuffer[1000];
    va_list va;
    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);

    Log(dwLevel, ALL, szFormat, va);

    va_end(va);
}


void fnsLog
(
    DWORD                   dwLevel,
    DWORD                   dwDestinationFlags, 
    LPSTR                   szFormat,
    ...
)
{
    char szBuffer[1000];
    va_list va;
    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);

    Log(dwLevel, dwDestinationFlags, szFormat, va);
    va_end(va);
}


void fnsLog
(
    DWORD                   dwLevel,
    LPSTR                   szFormat,
    ...
)
{
    char szBuffer[1000];
    va_list va;
    va_start(va, szFormat);

    vsprintf(szBuffer, szFormat, va);

    fnsLog(dwLevel, ALL, szFormat, va);
    va_end(va);
}
*/