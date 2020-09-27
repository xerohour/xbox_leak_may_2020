//lint -e416    
#include "globals.h"
#include "dxconio.h"

DWORD g_dwDebugLevel = 5;
DWORD g_dwIndentLevel = 0;
DWORD g_dwPassed = 0;
DWORD g_dwFailed = 0;


HRESULT LogInit(void)
{
HRESULT hr = S_OK;

    if (!g_TestParams.bLogToScreen)
        return S_OK;

    //To load another texture, use xSetBackground.
    xCreateConsole(NULL);
            xSetBackgroundColor(
                             255, 
                             255, 
                             255, 
                             255
                             );

    xSetBackgroundImage("T:\\DMTest1\\BGImage.bmp");
//    xprintf("Welcome to DMusic!!!\n");
    return hr;
};


/********************************************************************************
********************************************************************************/
HRESULT LogFree(void)
{
HRESULT hr = S_OK;

if (!g_TestParams.bLogToScreen)
    return S_OK;

xReleaseConsole();
return hr;
};



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
HRESULT LogConsole
(
    LPSTR                   szFormat,
    ...
)
{
    va_list va;
    char szBuffer[1000];

    if (!g_TestParams.bLogToScreen)
        return S_OK;

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    va_end(va);

    //A carriage return goes here when we've decided not to output to VC anymore!!
    xprintf("%s\n", szBuffer);
    return S_OK;
};


/********************************************************************************
********************************************************************************/
HRESULT Log
(
    DWORD                   dwLevel,
    DWORD                   dwDestinationFlags,
    LPSTR                   szFormat,
    ...
)
{

    va_list va;
    if (dwLevel > g_dwDebugLevel)
        return S_OK;
    char szBuffer[1000];

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    va_end(va);
    
    if (dwDestinationFlags & LOG_DEBUG)
        DbgPrint("%s\n", szBuffer);
    if (dwDestinationFlags & LOG_FILE)
        xLog(g_hLog, XLL_INFO, szBuffer);
    return S_OK;
};

/********************************************************************************
********************************************************************************/
HRESULT Log
(
    DWORD                   dwLevel,
    LPSTR                   szFormat,
    ...
)
{

    va_list va;
    if (dwLevel > g_dwDebugLevel)
        return S_OK;
    char szBuffer[1000];

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    va_end(va);
    
        DbgPrint("%s\n", szBuffer);
        LogConsole(szBuffer);

    return S_OK;
};

