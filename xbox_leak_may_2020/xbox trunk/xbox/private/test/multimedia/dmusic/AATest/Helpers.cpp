#include "..\globals.h"

/**********************************************************************
**********************************************************************/
LPCSTR PathFindFileName(LPCSTR pPath)
{
    LPCSTR pT;

    for (pT = pPath; *pPath; pPath++) {
        if ((pPath[0] == '\\' || pPath[0] == ':' || pPath[0] == '/')
            && pPath[1] &&  pPath[1] != '\\'  &&   pPath[1] != '/')
            pT = pPath + 1;
    }

    return pT;
}

/********************************************************************************
********************************************************************************/
HRESULT ChopPath(const char *p_szFullString, LPSTR p_szPathOut, LPSTR p_szFileOut)
{
HRESULT hr = S_OK;
LPCSTR szFile = NULL;
DWORD dwLength;
CHAR szPathOut[MAX_PATH] = {0};
CHAR szFileOut[MAX_PATH] = {0};

ASSERT(p_szPathOut || p_szFileOut);
ASSERT(p_szFullString);

if (SUCCEEDED(hr))
{
    if (!p_szFullString)
    {
        hr = E_POINTER;
    }
}

//Parse out the file name.
if (SUCCEEDED(hr))
{
    //If the name ends in a '/' or a '\\' then there is no file name.
    if ('/' == p_szFullString[strlen(p_szFullString) - 1] ||
        '\\' == p_szFullString[strlen(p_szFullString) - 1] )
    {
        szFile = &p_szFullString[strlen(p_szFullString)];
    }
    else
    {    
        szFile = (LPSTR)PathFindFileName(p_szFullString);
    }

    //Should never happen.
    ASSERT(szFile);

    //Store the file name.
    strcpy(szFileOut, szFile);
}

if (SUCCEEDED(hr))
{
    //Store the path name.
    dwLength = strlen(p_szFullString) - strlen(szFile);
    strncpy(szPathOut, p_szFullString, dwLength);
    szPathOut[dwLength] = NULL;
}

//Regardless of whether the test passed, set these out-parameters.
if (p_szPathOut)
    strcpy(p_szPathOut, szPathOut);
if (p_szFileOut)
    strcpy(p_szFileOut, szFileOut);

return S_OK;
}


