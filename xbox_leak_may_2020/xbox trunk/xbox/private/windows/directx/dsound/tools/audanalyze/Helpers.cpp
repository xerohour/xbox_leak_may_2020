#include "globals.h"


/********************************************************************************
HELPERS.CPP
********************************************************************************/
BOOL YesOrNo(void)
{
    CHAR c = 'a';

    do
    {
        //c = (char)_getch();
        scanf("%c", &c);
    }
    while (tolower(c) != 'n' && tolower(c) != 'y');

    if (tolower(c) != 'y')
        return FALSE;
    else
        return TRUE;
};




/********************************************************************************
********************************************************************************/
/*
void CreateClassLinkFile(TCHAR *pszFileName)

{
        char *szHeader[] = {
            "HRESULT MyDirectMusicCreateInstance(REFCLSID clsid,        \r\n",
            "                                    LPUNKNOWN pUnkOuter,   \r\n", 
            "                                    REFIID iid,            \r\n", 
            "                                    LPVOID *ppvInterface)  \r\n",
            "{                                                          \r\n",
            "   HRESULT hr = E_FAIL;\r\n",
            "                       \r\n",
            "   switch (clsid)      \r\n",
            "   {                   \r\n",
        };

        char *szTail[] = {

            "    default:    \r\n",
            "        printf(\"Unknown CLSID: %08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x\", clsid.Data1, clsid.Data2, clsid.Data3, clsid.Data4[0], clsid.Data4[1], clsid.Data4[2], clsid.Data4[3], clsid.Data4[4], clsid.Data4[5], clsid.Data4[6], clsid.Data4[7] );\r\n",
            "        break;      \r\n", 
            "    };             \r\n",
            "    return hr;     \r\n",
            "}                  \r\n"

        };




    //Check to see if the file exists.
    if (_access(pszFileName, 00) == 0)
    {

        //Check to see if the file is writeable.
        if (_access(pszFileName, 02) != 0)
        {
            printf("Error: Cannot write to %s.", pszFileName);
            return;
        }

        printf("%s exists, overwrite? (y/n) ", pszFileName);
        if (!YesOrNo())
            return;
    }

    HANDLE hFile = CreateFile(pszFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
		DWORD dwBytesWritten;
        for (DWORD dwI = 0; dwI < NUMELEMS(szHeader); dwI++)
        {
            WriteFile(hFile,szHeader[dwI],strlen(szHeader[dwI]),&dwBytesWritten,NULL);
        }
        CClassTracker *pScan = g_pClassList;
        for (;;)
        {
            CClassTracker *pBestSoFar = NULL;
            DWORD dwMaxCount = 0;
            for (pScan = g_pClassList;pScan;pScan = pScan->m_pNext)
            {
                if (!pScan->m_fSaved)
                {
                    if (pScan->m_dwCount > dwMaxCount)
                    {
                        pBestSoFar = pScan;
                        dwMaxCount = pScan->m_dwCount;
                    }
                }
            }
            if (!pBestSoFar)
            {
                break;
            }
            pBestSoFar->m_fSaved = TRUE;
            char szData[400];
            wsprintfA(szData,"    case %s:\r\n        // %s.\r\n        hr = %s;\r\n        break;\r\n",
                pBestSoFar->m_pszClassName,pBestSoFar->m_pszComment,pBestSoFar->m_pszFunctionName);
            WriteFile(hFile,szData,strlen(szData),&dwBytesWritten,NULL);
        }
        for (dwI = 0; dwI < NUMELEMS(szTail); dwI++)
        {
            WriteFile(hFile,szTail[dwI],strlen(szTail[dwI]),&dwBytesWritten,NULL);
        }
        CloseHandle(hFile);
    }
}
*/


#ifndef _UNICODE
//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertAnsiStringToWide()
// Desc: This is a UNICODE conversion utility to convert a CHAR string into a
//       WCHAR string. cchDestChar defaults -1 which means it 
//       assumes strDest is large enough to store strSource
//-----------------------------------------------------------------------------
VOID DXUtil_ConvertAnsiStringToWide( WCHAR* wstrDestination, const CHAR* strSource, 
                                     int cchDestChar )
{
    if( wstrDestination==NULL || strSource==NULL )
        return;

    if( cchDestChar == -1 )
        cchDestChar = strlen(strSource)+1;

    MultiByteToWideChar( CP_ACP, 0, strSource, -1, 
                         wstrDestination, cchDestChar-1 );

    wstrDestination[cchDestChar-1] = 0;
}
#endif

//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertGenericStringToWide()
// Desc: This is a UNICODE conversion utility to convert a TCHAR string into a
//       WCHAR string. cchDestChar defaults -1 which means it 
//       assumes strDest is large enough to store strSource
//-----------------------------------------------------------------------------
VOID DXUtil_ConvertGenericStringToWide( WCHAR* wstrDestination, const TCHAR* tstrSource, 
                                        int cchDestChar )
{
    if( wstrDestination==NULL || tstrSource==NULL )
        return;

#ifdef _UNICODE
    if( cchDestChar == -1 )
	    wcscpy( wstrDestination, tstrSource );
    else
	    wcsncpy( wstrDestination, tstrSource, cchDestChar );
#else
    DXUtil_ConvertAnsiStringToWide( wstrDestination, tstrSource, cchDestChar );
#endif
}



/********************************************************************************
********************************************************************************/
void InsertClass(const char *pszClassName, const char *pszFunctionName, const char *pszComment)
{
    CClassTracker *pScan = g_pClassList;
    for (;pScan;pScan = pScan->m_pNext)
    {
        if (strcmp(pScan->m_pszClassName,pszClassName)==0)
        {
            pScan->m_dwCount++;
            return;
        }
    }
    pScan = new CClassTracker;
    if (pScan)
    {
        pScan->m_pszClassName = pszClassName;
        pScan->m_pszFunctionName = pszFunctionName;
        pScan->m_pszComment = pszComment;
        pScan->m_pNext = g_pClassList;
        g_pClassList = pScan;
    }
}


/********************************************************************************
********************************************************************************/
void RemoveClass(const char *pszClassName)
{
    CClassTracker *pScan = g_pClassList;
    CClassTracker *pLast = NULL;
    
    
    for (;pScan;pScan = pScan->m_pNext)
    {
        if (strcmp(pScan->m_pszClassName, pszClassName)==0)
        {
            //If we're deleting the beginning of the list, then move the official root up one.
            if (g_pClassList == pScan)
            {
                g_pClassList = g_pClassList->m_pNext;
            }
            
            //If there's a node before us, point it to the next one.
            if (pLast)
            {
                pLast->m_pNext = pScan->m_pNext;                
                delete pScan;
            }

            break;  
        }

        pLast = pScan;
    }
}

/********************************************************************************
********************************************************************************/
BOOL CLSIDPresent( LPCSTR pszClassName)
{
    CClassTracker *pScan = g_pClassList;
    BOOL bFound = FALSE;
    
    for (;pScan;pScan = pScan->m_pNext)
    {
        if (strcmp(pScan->m_pszClassName, pszClassName)==0)
        {
            bFound = TRUE;
            break;  
        }

    }

    return bFound;

}




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
};


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

