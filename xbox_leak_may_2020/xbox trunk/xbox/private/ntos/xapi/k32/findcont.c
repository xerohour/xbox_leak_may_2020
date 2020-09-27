#include "basedll.h"
#include "xmeta.h"
#include <xboxp.h>


BOOL
XapiFindValueInMetaFile(
    HANDLE hMetaFile,
    LPCWSTR pszTag,
    LPWSTR pszValue,
    int cchValue);

BOOL
XapiValidateAndSkipUnicodeSignature(
    HANDLE hMetaFile);

BOOL
XapiFillInContentData(
    PXCONTENT_FIND_DATA pFindContentData,
    DWORD dwFlagFilter,
    PCSTR pszRootDir,
    int cchRootDir)
{
    int nNameLen;
    int iCurIdentity;
    BOOL fRet = FALSE;
    PSTR pszEnd;

    ASSERT(pFindContentData);

    if (0 == (pFindContentData->wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        XDBGWRN("XAPI", "XFindContent: Unexpected file found in content directory");
        return FALSE;
    }

    nNameLen = strlen(pFindContentData->wfd.cFileName);

    if ((nNameLen != CONTENT_DIR_NAME_LENGTH) ||
        (CONTENT_DIR_DELIMETER != pFindContentData->wfd.cFileName[CONTENT_DIR_DELIMETER_INDEX]))
    {
        XDBGWRN("XAPI", "XFindContent: Unexpected directory found in content directory");
        return FALSE;
    }

    //
    // Temporarily replace delimiter with NULL
    //
    
    pFindContentData->wfd.cFileName[CONTENT_DIR_DELIMETER_INDEX] = '\0';

    //
    // Read the offering id and flags from the directory name
    //

    pFindContentData->dwOfferingId = strtoul(pFindContentData->wfd.cFileName, &pszEnd, 16);

    if (pszEnd != (pFindContentData->wfd.cFileName + (CCHMAX_HEX_DWORD - 1)))
    {
        XDBGWRN("XAPI", "XFindContent: Unexpected directory found in content directory");
        return FALSE;
    }
    
    pFindContentData->dwFlags = strtoul(&(pFindContentData->wfd.cFileName[CONTENT_DIR_DELIMETER_INDEX + 1]),
                                        &pszEnd,
                                        16);

    if (pszEnd != (pFindContentData->wfd.cFileName + (CONTENT_DIR_DELIMETER_INDEX + 1) + (CCHMAX_HEX_DWORD - 1)))
    {
        XDBGWRN("XAPI", "XFindContent: Unexpected directory found in content directory");
        return FALSE;
    }

    //
    // Restore the delimiter
    //
    
    pFindContentData->wfd.cFileName[CONTENT_DIR_DELIMETER_INDEX] = CONTENT_DIR_DELIMETER;

    if ((0 != dwFlagFilter) && (0 == (dwFlagFilter & pFindContentData->dwFlags)))
    {
        //
        // dwFlagFilter is set to ignore this directory, so bail out here
        //

        return FALSE;
    }

    ASSERT(cchRootDir + nNameLen < ARRAYSIZE(pFindContentData->szContentDirectory));

    strcpy(pFindContentData->szContentDirectory, pszRootDir);
    strcpy(&(pFindContentData->szContentDirectory[cchRootDir]),
           pFindContentData->wfd.cFileName);

    //
    // Append a backslash if there is room
    //

    if (cchRootDir + nNameLen < ARRAYSIZE(pFindContentData->szContentDirectory) - 1)
    {
        pFindContentData->szContentDirectory[cchRootDir + nNameLen] = '\\';
        pFindContentData->szContentDirectory[cchRootDir + nNameLen + 1] = '\0';
    }

    //
    // Attempt to open the metadata file
    //

    if (cchRootDir + nNameLen <
        (int) ARRAYSIZE(pFindContentData->szContentDirectory) - g_cchContentMetaFileName)
    {
        HANDLE hMetaFile;

        //
        // Borrow the output buffer temporarily to append the metadata filename
        //

        strcpy(&(pFindContentData->szContentDirectory[cchRootDir + nNameLen]),
               g_cszContentMetaFileName);

        hMetaFile = CreateFile(pFindContentData->szContentDirectory,
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);

        if (INVALID_HANDLE_VALUE != hMetaFile)
        {
            fRet = XapiValidateAndSkipUnicodeSignature(hMetaFile);

            if (fRet)
            {
                fRet = XapiFindValueInMetaFile(hMetaFile,
                                               g_cszNameTag,
                                               pFindContentData->szDisplayName,
                                               ARRAYSIZE(pFindContentData->szDisplayName));
            }

            CloseHandle(hMetaFile);
        }

        //
        // Restore the buffer (truncate to remove the metadata filename)
        //

        pFindContentData->szContentDirectory[cchRootDir + nNameLen + 1] = '\0';
    }

    return fRet;
}

HANDLE
WINAPI
XFindFirstContent(
    IN PCSTR lpRootPathName,
    IN DWORD dwFlagFilter,
    OUT PXCONTENT_FIND_DATA pFindContentData
    )
{
    CHAR szDirName[MAX_PATH];
    int cchRootDir;
    HANDLE hRet;

#if DBG
    if ((NULL == pFindContentData) ||
        (NULL == lpRootPathName) ||
        (OTEXT('\0') == lpRootPathName[0]) ||
        (OTEXT(':')  != lpRootPathName[1]) ||
        (OTEXT('\\') != lpRootPathName[2]) ||
        (OTEXT('\0') != lpRootPathName[3]))
    {
        RIP("XFindFirstContent() invalid parameter");
    }

    {
        //
        // Removing the 0x20 bit will make lower case characters uppercase
        //

        CHAR chDrive = lpRootPathName[0] & (~0x20);

        if ((HD_TDATA_DRIVE != chDrive) &&
            (HD_ALT_TDATA_DRIVE != chDrive))
        {
            RIP("XFindFirstContent() invalid drive letter parameter");
        }
    }
#endif // DBG

    //
    // Create <path>\$C\* in our own buffer
    //

    strcpy(szDirName, lpRootPathName);
    cchRootDir = strlen(szDirName);

    if (cchRootDir > ((int) ARRAYSIZE(szDirName) - 1 - g_cchContentSearch))
    {
        RIP("XFindFirstContent() lpRootPathName parameter too long");
    }

    strcpy(&(szDirName[cchRootDir]), g_cszContentSearch);

    hRet = FindFirstFile(szDirName, (PWIN32_FIND_DATA) pFindContentData);

    //
    // Recreate the <path>\$C without the wildcard string
    //

    ASSERT(g_cchContentDir < g_cchContentSearch);
    strcpy(szDirName, lpRootPathName);
    strcpy(&(szDirName[cchRootDir]), g_cszContentDir);
    cchRootDir += g_cchContentDir;

    if (INVALID_HANDLE_VALUE != hRet)
    {
        if (!XapiFillInContentData(pFindContentData,
                                   dwFlagFilter,
                                   szDirName,
                                   cchRootDir))
        {
            BOOL fFound;

            while (fFound = FindNextFile(hRet, (PWIN32_FIND_DATA) pFindContentData))
            {
                if (XapiFillInContentData(pFindContentData,
                                          dwFlagFilter,
                                          szDirName,
                                          cchRootDir))
                {
                    break;
                }
            }

            if (!fFound)
            {
                //
                // We didn't find a content directory
                //

                FindClose(hRet);
                hRet = INVALID_HANDLE_VALUE;
                SetLastError(ERROR_NO_MORE_FILES);
            }
        }
    }

    if (INVALID_HANDLE_VALUE != hRet)
    {
        PFINDCONTENT_HANDLE pFindContent = LocalAlloc(LMEM_FIXED, sizeof(FINDCONTENT_HANDLE));

        if (NULL != pFindContent)
        {
            pFindContent->dwSignature = FH_SIG_CONTENT;
            pFindContent->dwFlagFilter = dwFlagFilter;
            pFindContent->hFindFile = hRet;
            pFindContent->cchRootDir = cchRootDir;
            lstrcpynA(pFindContent->szRootDir, szDirName, ARRAYSIZE(pFindContent->szRootDir));
            hRet = (HANDLE) pFindContent;
        }
        else
        {
            FindClose(hRet);
            hRet = INVALID_HANDLE_VALUE;
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        }
    }

    return hRet;
}

BOOL
WINAPI
XFindNextContent(
    IN HANDLE hFindContent,
    OUT PXCONTENT_FIND_DATA pFindContentData
    )
{
    BOOL bRet = FALSE;

#if DBG
    if ((NULL == hFindContent) ||
        (INVALID_HANDLE_VALUE == hFindContent) ||
        (FH_SIG_CONTENT != ((PFINDCONTENT_HANDLE) hFindContent)->dwSignature) ||
        (NULL == pFindContentData))
    {
        RIP("XFindNextContent() invalid parameter");
    }
#endif // DBG

    {
        PFINDCONTENT_HANDLE pFindContent = (PFINDCONTENT_HANDLE) hFindContent;

        while (bRet = FindNextFile(pFindContent->hFindFile, (PWIN32_FIND_DATA) pFindContentData))
        {
            if (XapiFillInContentData(pFindContentData,
                                      pFindContent->dwFlagFilter,
                                      pFindContent->szRootDir,
                                      pFindContent->cchRootDir))
            {
                break;
            }
        }
    }

    return bRet;
}

