#include <stdio.h>
#include "xkdctest.h"


XKTVerbose g_XKTVerbose = XKTVerbose_Status;


void TestMsg(char *message, ...)
{
    if (g_XKTVerbose >= XKTVerbose_All)
    {
        // Print a ":" preceding any message that is called without a verbose level to notify the developer of this
        printf(":");

        va_list arglist;

        va_start(arglist, message);
        char string[256];
        _vsnprintf(string, sizeof(string), message, arglist);
        va_end(arglist);

        printf(string);
    }
}


void TestMsg(XKTVerbose xktvLevel, char *message, ...)
{
    if (g_XKTVerbose >= xktvLevel)
    {
        for (INT i = 0; i < xktvLevel - 1; i++)
        {
            printf("  ");
        }

        va_list arglist;

        va_start(arglist, message);
        char string[256];
        _vsnprintf(string, sizeof(string), message, arglist);
        va_end(arglist);

        printf(string);
    }
}


void TestAssertHR(HRESULT hr, HRESULT *hrFunc)
{
    if (hr != S_OK)
    {
        if (hrFunc)
        {
            *hrFunc = E_FAIL;
        }
    }
}


void TestAssert(BOOL condition, HRESULT *hrFunc)
{
    if (!condition)
    {
        if (hrFunc)
            *hrFunc = E_FAIL;
    }
}


BOOL IsHex(char *sz, INT length)
{
    BOOL yes = TRUE;
    for (INT i = 0; i < length; i++)
    {
        if (!((sz[i] >= '0' && sz[i] <= '9') || (sz[i] >= 'A' && sz[i] <= 'F') || (sz[i] >= 'a' && sz[i] <= 'f')))
        {
            yes = FALSE;
        }
    }
    return yes;
}


HRESULT FillMemDbg(char *pb, INT cb)
{
    LONGLONG qwPattern = 0xc0fedeadbeefc0fe;
    char *bPattern = ((char*)&qwPattern);
    INT nPatternLength = 8;
    INT nPatternIndex = 0;
    for (INT i = 0; i < cb; i++)
    {
        pb[i] = bPattern[nPatternIndex];
        nPatternIndex++;
        if (nPatternIndex == nPatternLength)
        {
            nPatternIndex = 0;
        }
    }
    
    return S_OK;
}


HRESULT CheckMemDbg(char *pb, INT cb)
{
    HRESULT hr = S_OK;
    
    LONGLONG qwPattern = 0xc0fedeadbeefc0fe;
    char *bPattern = ((char*)&qwPattern);
    INT nPatternLength = 8;
    INT nPatternIndex = 0;
    for (INT i = 0; i < cb; i++)
    {
        if (pb[i] != bPattern[nPatternIndex])
        {
            return E_FAIL;
        }
        nPatternIndex++;
        if (nPatternIndex == nPatternLength)
        {
            nPatternIndex = 0;
        }
    }

    return hr;
}
