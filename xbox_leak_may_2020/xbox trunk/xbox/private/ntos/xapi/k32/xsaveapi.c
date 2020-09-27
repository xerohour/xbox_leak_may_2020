#include "basedll.h"
#include "xmeta.h"

// Closest prime number to 2^48:
// 2^48 - 59 =  281474976710597 =  0xFFFFFFFFFFC5

#define SAVE_DIR_HASH_M 0xFFFFFFFFFFC5

#define HASHED_SAVE_GAME_CHARACTERS 12


__inline CHAR HexDigitToChar(INT d)
{
    return (d <= 9) ? (d + '0') : (d - 0xA + 'A');
}

//
// Unicode save game names are hashed into 48 bit numbers (12 8-bit hex characters)
//

VOID
XapiSaveGameHashEncode(
    PCWSTR pszSource,
    PSTR pszDestination,
    int cchDestination)
{
    int nChar = 0;
    DWORDLONG qwHashValue = 0;

    ASSERT(pszSource);
    ASSERT(pszDestination);
    ASSERT(cchDestination > HASHED_SAVE_GAME_CHARACTERS);

    while (pszSource[nChar])
    {
        //
        // This is effectively what we are doing below:
        //
        // qwHashValue = (((0x10000) * qwHashValue) + pszSource[nChar]) % SAVE_DIR_HASH_M;
        //

        qwHashValue = ((qwHashValue << 16) + pszSource[nChar]) % SAVE_DIR_HASH_M;
        nChar++;
    }

    for (nChar = (HASHED_SAVE_GAME_CHARACTERS - 1); nChar >= 0; nChar--)
    {
        pszDestination[nChar] =
            HexDigitToChar(
                ((INT) (qwHashValue >> (4 * ((HASHED_SAVE_GAME_CHARACTERS - 1) - nChar)))) & 0xF);
    }

    pszDestination[HASHED_SAVE_GAME_CHARACTERS] = '\0';
}

#if 0

//
// Note: hMetaFile should have been opened without FILE_SHARE_READ or
// FILE_SHARE_WRITE set, because we are potentially rewriting the entire file
// in this function
//

BOOL
XapiDeleteValueInMetaFile(
    HANDLE hMetaFile,
    LPCWSTR pszTag)
{
    PBYTE pBuffer;
    BOOL fRet = FALSE;
    DWORD dwSize = GetFileSize(hMetaFile, NULL);

    ASSERT(pszTag);

    if (dwSize)
    {
        pBuffer = (PBYTE) LocalAlloc(LMEM_FIXED, dwSize + sizeof(WCHAR));
        if (pBuffer)
        {
            PBYTE pCurrent = pBuffer;
            DWORD dwBytesRead;

            //
            // Place a NULL at the end of the buffer that should never
            // be overwritten - this makes it safe to use the wcsstr()
            // function later without worrying about running of the end
            //

            *((PWSTR) (&pBuffer[dwSize])) = UNICODE_NULL;

            SetFilePointer(hMetaFile, 0, NULL, FILE_BEGIN);

            while (ReadFile(hMetaFile,
                            pCurrent,
                            pBuffer - pCurrent - sizeof(WCHAR),
                            &dwBytesRead,
                            NULL))
            {
                if (0 == dwBytesRead)
                {
                    //
                    // The whole file has been read in
                    //

                    fRet = TRUE;
                    break;
                }
                pCurrent += dwBytesRead;

                if (pBuffer - pCurrent < sizeof(WCHAR))
                {
                    //
                    // Hmm.. the file is bigger than it used to be
                    //

                    break;
                }
            }

            if (fRet)
            {
                //
                // Scan the buffer for the tag/value pairs
                //

                DWORD dwNewSize = dwSize;
                PWSTR pszMatchTag, pszCRLF;
                int nTagLength = wcslen(pszTag);

                pCurrent = pBuffer;

                while ((pszMatchTag = wcsstr((PWSTR) pCurrent, pszTag)) &&
                       (pszCRLF = wcsstr((PWSTR) pCurrent, g_cszCRLF)))
                {
                    if ((pszMatchTag == (PWSTR) pCurrent) &&
                        (g_chEqual == pszMatchTag[nTagLength]))
                    {
                        //
                        // Got one
                        //

                        DWORD dwCutSize = sizeof(WCHAR) * (pszCRLF + g_cchCRLF - pszMatchTag);

                        ASSERT(dwNewSize >= dwCutSize);

                        dwNewSize -= dwCutSize;

                        //
                        // If we're not at the end of the buffer, slide the remaining
                        // buffer back to remove the part we just cut out
                        //

                        if (dwNewSize > (DWORD) (pCurrent - pBuffer))
                        {
                            RtlMoveMemory(pCurrent,
                                          pCurrent + dwCutSize,
                                          dwNewSize - (pCurrent - pBuffer));
                        }
                    }
                    else
                    {
                        pCurrent = (PBYTE) ((PWSTR) (pszCRLF + g_cchCRLF));
                    }

                    if ((DWORD) (pCurrent - pBuffer) >= dwNewSize)
                    {
                        //
                        // We've hit the end of the valid buffer
                        //

                        break;
                    }
                }

                if (dwNewSize != dwSize)
                {
                    DWORD dwBytesWritten;

                    //
                    // We've shrunk the size, so we need to write the file out
                    //

                    SetFilePointer(hMetaFile, 0, NULL, FILE_BEGIN);
                    SetEndOfFile(hMetaFile);

                    fRet = FALSE;

                    pCurrent = pBuffer;

                    while (WriteFile(hMetaFile,
                                     pCurrent,
                                     pBuffer + dwNewSize - pCurrent,
                                     &dwBytesWritten,
                                     NULL))
                    {
                        pCurrent += dwBytesWritten;

                        if ((DWORD) (pCurrent - pBuffer) >= dwNewSize)
                        {
                            fRet = TRUE;
                            break;
                        }
                    }
                }
            }

            SetFilePointer(hMetaFile, 0, NULL, FILE_BEGIN);

            LocalFree(pBuffer);
        }
    }

    return fRet;
}

#endif // 0

BOOL
XapiValidateAndSkipUnicodeSignature(
    HANDLE hMetaFile)
{
    WCHAR wchSig;
    DWORD dwRead;
    
    ASSERT(INVALID_HANDLE_VALUE != hMetaFile);
    ASSERT(hMetaFile);

    SetFilePointer(hMetaFile, 0, NULL, FILE_BEGIN);

    return (ReadFile(hMetaFile, &wchSig, sizeof(wchSig), &dwRead, NULL) &&
            (dwRead == sizeof(wchSig)) ||
            (wchSig == g_chUnicodeSignature));
}

BOOL
XapiFindValueInMetaFile(
    HANDLE hMetaFile,
    LPCWSTR pszTag,
    LPWSTR pszValue,
    int cchValue)
{
    BOOL  fRet = FALSE;
    DWORD dwBytesRead;
    WCHAR szBuffer[MAX_METADATA_LINE];
    PWSTR pszNextRead = szBuffer;
    UINT  cchRead = ARRAYSIZE(szBuffer) - 1;
    BOOL  fSkipThroughNextCRLF = FALSE;
    BOOL  fReuseBuffer = FALSE;
    int   nTagLength = wcslen(pszTag);

    //
    // Null terminate the end of the read buffer - we should never overwrite this
    //

    szBuffer[cchRead] = UNICODE_NULL;

    while (fReuseBuffer ||
           (ReadFile(hMetaFile,
                     pszNextRead,
                     cchRead * sizeof(WCHAR),
                     &dwBytesRead,
                     NULL) &&
            (0 != dwBytesRead)))
    {
        DWORD dwBytesValid = dwBytesRead + ((pszNextRead - szBuffer) * sizeof(WCHAR));

        fReuseBuffer = FALSE;

        if (fSkipThroughNextCRLF)
        {
            PWSTR pszCRLF = wcsstr(szBuffer, g_cszCRLF);
            pszNextRead = szBuffer;
            cchRead = ARRAYSIZE(szBuffer) - 1;

            if (pszCRLF)
            {
                DWORD dwBytesSkip;

                pszCRLF += g_cchCRLF;
                dwBytesSkip = ((PBYTE) pszCRLF - (PBYTE) szBuffer);

                ASSERT(dwBytesValid >= dwBytesSkip);

                if (dwBytesSkip < dwBytesValid)
                {
                    //
                    // move the next line of data that we just read to
                    // the beginning of the buffer
                    //

                    DWORD dwBytesMove = dwBytesValid - dwBytesSkip;

                    RtlMoveMemory(szBuffer,
                                  ((PBYTE) szBuffer) + dwBytesSkip,
                                  dwBytesMove);

                    pszNextRead = (PWSTR) (((PBYTE) szBuffer) + dwBytesMove);
                    cchRead = ARRAYSIZE(szBuffer) - 1 - (dwBytesMove >> 1);
                }

                fSkipThroughNextCRLF = FALSE;
            }
        }
        else
        {
            PWSTR pszMatchTag = wcsstr(szBuffer, pszTag);
            if (pszMatchTag && (g_chEqual == pszMatchTag[nTagLength]))
            {
                PWSTR pszCRLF;

                pszMatchTag += (nTagLength + 1);
                pszCRLF = wcsstr(pszMatchTag, g_cszCRLF);

                ASSERT(cchValue > 0);

                if (pszCRLF)
                {
                    DWORD dwBytesUsed;
                    int cchCopy = min(cchValue - 1, (pszCRLF - pszMatchTag));

                    //
                    // Copy the value string to the output buffer
                    //

                    wcsncpy(pszValue, pszMatchTag, cchCopy);

                    //
                    // Null terminate the output buffer
                    //

                    pszValue[cchCopy] = UNICODE_NULL;

                    pszCRLF += g_cchCRLF;

                    dwBytesUsed = ((PBYTE) pszCRLF - (PBYTE) szBuffer);

                    ASSERT(dwBytesValid >= dwBytesUsed);

                    if (dwBytesUsed < dwBytesValid)
                    {
                        //
                        // Move the file pointer back if we didn't use all of
                        // the data that we read
                        //

                        SetFilePointer(hMetaFile,
                                       (dwBytesUsed - dwBytesValid),
                                       NULL,
                                       FILE_CURRENT);
                    }

                    fRet = TRUE;

                    break;
                }
                else
                {
                    fSkipThroughNextCRLF = TRUE;
                    pszNextRead = szBuffer;
                    cchRead = ARRAYSIZE(szBuffer) - 1;
                }
            }
            else
            {
                fSkipThroughNextCRLF = TRUE;
                fReuseBuffer = TRUE;
            }
        }
    }

    return fRet;
}

BOOL
XapiFillInSaveGameData(
    PXGAME_FIND_DATA pFindGameData,
    PCOSTR pszRootDir,
    int cchRootDir)
{
    int nNameLen;
    int iCurIdentity;
    BOOL fRet = FALSE;

    ASSERT(pFindGameData);

    if (0 == (pFindGameData->wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        XDBGWRN("XAPI", "XFindSaveGame: Unexpected file found on save game drive");
        return FALSE;
    }

    nNameLen = ocslen(pFindGameData->wfd.cFileName);

    ASSERT(cchRootDir + nNameLen < ARRAYSIZE(pFindGameData->szSaveGameDirectory));

    ocscpy(pFindGameData->szSaveGameDirectory, pszRootDir);
    ocscpy(&(pFindGameData->szSaveGameDirectory[cchRootDir]),
           pFindGameData->wfd.cFileName);

    //
    // Append a backslash if there is room
    //

    if (cchRootDir + nNameLen < ARRAYSIZE(pFindGameData->szSaveGameDirectory) - 1)
    {
        pFindGameData->szSaveGameDirectory[cchRootDir + nNameLen] = OTEXT('\\');
        pFindGameData->szSaveGameDirectory[cchRootDir + nNameLen + 1] = OBJECT_NULL;
    }

    //
    // Attempt to open the metadata file
    //

    if (cchRootDir + nNameLen < (int) ARRAYSIZE(pFindGameData->szSaveGameDirectory) - g_cchSaveMetaFileName)
    {
        HANDLE hMetaFile;

        //
        // Borrow the output buffer temporarily to append the metadata filename
        //

        ocscpy(&(pFindGameData->szSaveGameDirectory[cchRootDir + nNameLen]), g_cszSaveMetaFileName);

        hMetaFile = CreateFile(pFindGameData->szSaveGameDirectory,
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
                                               pFindGameData->szSaveGameName,
                                               ARRAYSIZE(pFindGameData->szSaveGameName));
            }

            CloseHandle(hMetaFile);
        }

        //
        // Restore the buffer (truncate to remove the metadata filename)
        //

        pFindGameData->szSaveGameDirectory[cchRootDir + nNameLen + 1] = OBJECT_NULL;
    }

    return fRet;
}

DWORD
XapiVerifyGameName(
    PCOSTR lpMetaFilePath,
    LPCWSTR lpSaveGameName)
{
    DWORD dwRet;

    HANDLE hMetaFile = CreateFile( lpMetaFilePath,
                                   SYNCHRONIZE | GENERIC_READ,
                                   0,
                                   NULL,
                                   OPEN_ALWAYS,
                                   FILE_ATTRIBUTE_SYSTEM,
                                   NULL);

    if (INVALID_HANDLE_VALUE != hMetaFile)
    {
        WCHAR szName[MAX_GAMENAME];

        if (XapiValidateAndSkipUnicodeSignature(hMetaFile) &&
            XapiFindValueInMetaFile(hMetaFile, g_cszNameTag, szName, ARRAYSIZE(szName)) &&
            (0 == wcscmp(szName, lpSaveGameName)))
        {
            dwRet = ERROR_SUCCESS;
        }
        else
        {
            dwRet = ERROR_NO_MATCH;
        }

        CloseHandle(hMetaFile);
    }
    else
    {
        dwRet = GetLastError();
    }

    return dwRet;
}

VOID
XapiTouchDirectoryTimestamp(
    LPCSTR pcszDir
    )
{
    HANDLE hDir = CreateFile(pcszDir,
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             FILE_FLAG_BACKUP_SEMANTICS,
                             NULL);

    if (INVALID_HANDLE_VALUE != hDir)
    {
        FILE_BASIC_INFORMATION BasicInfo;
        IO_STATUS_BLOCK IoStatusBlock;

        //
        // Zero all the time values we can set.
        //

        RtlZeroMemory(&BasicInfo, sizeof(BasicInfo));

        //
        // Set the last write times
        //

        KeQuerySystemTime(&BasicInfo.LastWriteTime);

        NtSetInformationFile(
                    hDir,
                    &IoStatusBlock,
                    &BasicInfo,
                    sizeof(BasicInfo),
                    FileBasicInformation
                    );

        CloseHandle(hDir);
    }
}

DWORD
WINAPI
XCreateSaveGame(
    IN PCOSTR lpRootPathName,
    IN LPCWSTR lpSaveGameName,
    IN DWORD dwCreationDisposition,
    IN DWORD dwCreateFlags,
    OUT POSTR lpPathBuffer,
    IN UINT uSize)
{
    OCHAR szDirName[MAX_PATH];
    int nPathLength;
    int iCurIdentity;
    HANDLE hMetaFile;
    BOOL fNewCreation;
    DWORD dwNameVerifyResult;

#if DBG
    if ((NULL == lpSaveGameName) ||
        (wcslen(lpSaveGameName) >= MAX_GAMENAME) ||
        (NULL != wcsstr(lpSaveGameName, g_cszCRLF)) ||
        (NULL == lpRootPathName) ||
        (OTEXT('\0') == lpRootPathName[0]) ||
        (OTEXT(':') != lpRootPathName[1]) ||
        (OTEXT('\\') != lpRootPathName[2]) ||
        (OTEXT('\0') != lpRootPathName[3]) ||
        ((CREATE_NEW != dwCreationDisposition) &&
         (OPEN_EXISTING != dwCreationDisposition) &&
         (OPEN_ALWAYS != dwCreationDisposition)))
    {
        RIP("XCreateSaveGame() invalid parameter");
    }

    RIP_ON_NOT_TRUE("XCreateSaveGame()",
                    ((0 == dwCreateFlags) ||
                     ((XSAVEGAME_NOCOPY == dwCreateFlags) && (OPEN_EXISTING != dwCreationDisposition))));

    {
        //
        // Removing the 0x20 bit will make lower case characters uppercase
        //

        OCHAR chDrive = lpRootPathName[0] & (~0x20);

        if (((chDrive < MU_FIRST_DRIVE) || (chDrive > MU_LAST_DRIVE)) &&
            (HD_UDATA_DRIVE != chDrive) &&
            (HD_ALT_UDATA_DRIVE != chDrive))
        {
            RIP("XCreateSaveGame() invalid drive letter parameter");
        }
    }

#endif // DBG

    ocscpy(szDirName, lpRootPathName);
    nPathLength = ocslen(szDirName);
    XapiSaveGameHashEncode(lpSaveGameName,
                           &(szDirName[nPathLength]),
                           ARRAYSIZE(szDirName) - nPathLength - g_cchSaveMetaFileName);

    nPathLength = ocslen(szDirName);

    ASSERT(nPathLength < (int) ARRAYSIZE(szDirName) - g_cchSaveMetaFileName);

    if (OPEN_EXISTING == dwCreationDisposition)
    {
        ocscpy(&(szDirName[nPathLength]), g_cszSaveMetaFileName);
        dwNameVerifyResult = XapiVerifyGameName(szDirName, lpSaveGameName);
        szDirName[nPathLength] = OBJECT_NULL;

        if (ERROR_SUCCESS == dwNameVerifyResult)
        {
            //
            // The existing directory has a metadata file with the same game name,
            // so we're done
            //
            fNewCreation = FALSE;
        }
        else
        {
            return dwNameVerifyResult;
        }
    }
    else
    {
        if (CreateDirectory(szDirName, NULL))
        {
            fNewCreation = TRUE;
        }
        else
        {
            DWORD dwErr = GetLastError();

            dwNameVerifyResult = ERROR_NO_MATCH;

            if (ERROR_ALREADY_EXISTS == dwErr)
            {
                ocscpy(&(szDirName[nPathLength]), g_cszSaveMetaFileName);
                dwNameVerifyResult = XapiVerifyGameName(szDirName, lpSaveGameName);
                szDirName[nPathLength] = OBJECT_NULL;

                if (ERROR_SUCCESS == dwNameVerifyResult)
                {
                    //
                    // This is the right directory - touch the timestamp
                    // so that the dashboard will see this as the most recently
                    // used save game
                    //

                    XapiTouchDirectoryTimestamp(szDirName);
                }
                else
                {
                    return ERROR_CANNOT_MAKE;
                }
            }

            if ((CREATE_NEW != dwCreationDisposition) && (ERROR_SUCCESS == dwNameVerifyResult))
            {
                //
                // The save game name in the metadata file matched, so we're ok to
                // continue with the rest of this function
                //
                fNewCreation = FALSE;
            }
            else
            {
                return dwErr;
            }
        }
    }

    if (fNewCreation)
    {
        OBJECT_ATTRIBUTES objectAttributes;
        IO_STATUS_BLOCK   ioStatusBlock;
        LARGE_INTEGER     allocationSize;
        OBJECT_STRING     metaFilePathString;
        NTSTATUS          status;

        //
        // Create SAVEMETA.XBX file underneath the save game directory and
        // write metadata information there
        //
        ocscpy(&(szDirName[nPathLength]), g_cszSaveMetaFileName);

        RtlInitObjectString(&metaFilePathString, szDirName);
        
        InitializeObjectAttributes(
            &objectAttributes,
            &metaFilePathString,
            OBJ_CASE_INSENSITIVE,
            ObDosDevicesDirectory(),
            NULL
            );

        allocationSize.QuadPart = 1;

        status = NtCreateFile(
                      &hMetaFile,
                      SYNCHRONIZE | GENERIC_READ | GENERIC_WRITE,
                      &objectAttributes,
                      &ioStatusBlock,
                      &allocationSize,
                      FILE_ATTRIBUTE_SYSTEM,
                      0,
                      FILE_OPEN_IF,
                      FILE_WRITE_THROUGH | FILE_SYNCHRONOUS_IO_NONALERT
                      );

        szDirName[nPathLength] = OBJECT_NULL;

        if(FAILED(status))
        {
            DWORD dwErr = RtlNtStatusToDosError(status);

            //
            // We failed, so try to remove the directory that we just created
            // so we don't leave an orphan empty directory in the file system.
            //

            RemoveDirectory(szDirName);

            return dwErr;
        }
        else
        {
            DWORD dwBytesWritten = 0;
            DWORD dwSize;

            //
            // One signature WCHAR plus a line of meta data
            //
            
            WCHAR szBuffer[1 + MAX_METADATA_LINE + g_cchNoCopyTrue];

            //
            // Only write the name into the metadata file when we first create it
            //

            _snwprintf(szBuffer,
                       ARRAYSIZE(szBuffer),
                       (XSAVEGAME_NOCOPY & dwCreateFlags) ?
                           L"%lc%ls%lc%ls%ls%ls" :
                           L"%lc%ls%lc%ls%ls",
                       g_chUnicodeSignature,
                       g_cszNameTag,
                       g_chEqual,
                       lpSaveGameName,
                       g_cszCRLF,
                       g_cszNoCopyTrue);

            dwSize = sizeof(WCHAR) * wcslen(szBuffer);

            while (dwSize)
            {
                DWORD dwWrittenNow;
                
                if (!WriteFile(hMetaFile,
                               (PBYTE) szBuffer + dwBytesWritten,
                               dwSize,
                               &dwWrittenNow,
                               NULL))
                {
                    DWORD dwErr = GetLastError();

                    CloseHandle(hMetaFile);
                    
                    //
                    // We failed, so try to remove the directory that we just created
                    // so we don't leave an orphan empty directory in the file system.
                    //
                    
                    RemoveDirectory(szDirName);

                    return dwErr;
                }

                dwSize -= dwWrittenNow;
            }

            CloseHandle(hMetaFile);
        }
    }
#if DBG
    else if (0 != dwCreateFlags)
    {
        XDBGWRN("XAPI",
                "XCreateSaveGame() ignoring dwCreateFlags because save game %ls already exists",
                lpSaveGameName);
    }
#endif // DBG

    if (NULL != lpPathBuffer)
    {
        lstrcpynO(lpPathBuffer, szDirName, uSize);

        if ((UINT) nPathLength < (uSize - 1))
        {
            //
            // Append a backslash (if there is room)
            //

            lpPathBuffer[nPathLength] = OTEXT('\\');
            lpPathBuffer[nPathLength + 1] = OBJECT_NULL;
        }
    }

    return ERROR_SUCCESS;
}

DWORD
WINAPI
XDeleteSaveGame(
    IN PCOSTR lpRootPathName,
    IN LPCWSTR lpSaveGameName)
{
    OCHAR szDirName[64 + MAX_PATH];
    int nPathLength;
    DWORD dwNameVerifyResult;
    int nPrefixLength;
    NTSTATUS Status;

#if DBG
    if ((NULL == lpSaveGameName) ||
        (wcslen(lpSaveGameName) >= MAX_GAMENAME) ||
        (NULL != wcsstr(lpSaveGameName, g_cszCRLF)) ||
        (NULL == lpRootPathName) ||
        (OTEXT('\0') == lpRootPathName[0]) ||
        (OTEXT(':')  != lpRootPathName[1]) ||
        (OTEXT('\\') != lpRootPathName[2]) ||
        (OTEXT('\0') != lpRootPathName[3]))
    {
        RIP("XDeleteSaveGame() invalid parameter");
    }

    {
        //
        // Removing the 0x20 bit will make lower case characters uppercase
        //

        OCHAR chDrive = lpRootPathName[0] & (~0x20);

        if (((chDrive < MU_FIRST_DRIVE) || (chDrive > MU_LAST_DRIVE)) &&
            (HD_UDATA_DRIVE != chDrive) &&
            (HD_ALT_UDATA_DRIVE != chDrive))
        {
            RIP("XDeleteSaveGame() invalid drive letter parameter");
        }
    }
#endif // DBG

    ocscpy(szDirName, OTEXT("\\??\\"));
    nPrefixLength = ocslen(szDirName);

    ocscpy(&(szDirName[nPrefixLength]), lpRootPathName);
    nPathLength = ocslen(szDirName);

    XapiSaveGameHashEncode(lpSaveGameName,
                           &(szDirName[nPathLength]),
                           ARRAYSIZE(szDirName) - nPathLength - g_cchSaveMetaFileName);

    nPathLength = ocslen(szDirName);

    ASSERT(nPathLength < (int) ARRAYSIZE(szDirName) - g_cchSaveMetaFileName);

    ocscpy(&(szDirName[nPathLength]), g_cszSaveMetaFileName);
    dwNameVerifyResult = XapiVerifyGameName(&(szDirName[nPrefixLength]), lpSaveGameName);
    szDirName[nPathLength] = OBJECT_NULL;

    if (ERROR_SUCCESS != dwNameVerifyResult)
    {
        return dwNameVerifyResult;
    }

    //
    // The directory exists with a metadata file in it with a matching game
    // name, so it is safe to proceed
    //
    
    szDirName[nPathLength] = OTEXT('\\');
    szDirName[nPathLength + 1] = OBJECT_NULL;

    Status = XapiNukeDirectory(szDirName);

    return RtlNtStatusToDosError(Status);
}

HANDLE
WINAPI
XFindFirstSaveGame(
    IN PCOSTR lpRootPathName,
    OUT PXGAME_FIND_DATA pFindGameData)
{
    OCHAR szDirName[MAX_PATH];
    int cchRootDir;
    HANDLE hRet;

#if DBG
    if ((NULL == pFindGameData) ||
        (NULL == lpRootPathName) ||
        (OTEXT('\0') == lpRootPathName[0]) ||
        (OTEXT(':')  != lpRootPathName[1]) ||
        (OTEXT('\\') != lpRootPathName[2]) ||
        (OTEXT('\0') != lpRootPathName[3]))
    {
        RIP("XFindFirstSaveGame() invalid parameter");
    }

    {
        //
        // Removing the 0x20 bit will make lower case characters uppercase
        //

        OCHAR chDrive = lpRootPathName[0] & (~0x20);

        if (((chDrive < MU_FIRST_DRIVE) || (chDrive > MU_LAST_DRIVE)) &&
            (HD_UDATA_DRIVE != chDrive) &&
            (HD_ALT_UDATA_DRIVE != chDrive))
        {
            RIP("XFindFirstSaveGame() invalid drive letter parameter");
        }
    }
#endif // DBG

    //
    // Create <path>\<*.*> in our own buffer
    //

    ocscpy(szDirName, lpRootPathName);
    cchRootDir = ocslen(szDirName);

    if (cchRootDir > ((int) ARRAYSIZE(szDirName) - 1 - g_cchStar))
    {
        RIP("XFindFirstSaveGame() lpRootPathName parameter too long");
    }

    ocscpy(&(szDirName[cchRootDir]), g_cszStar);

    hRet = FindFirstFile(szDirName, (PWIN32_FIND_DATA) pFindGameData);

    if (INVALID_HANDLE_VALUE != hRet)
    {
        if (!XapiFillInSaveGameData(pFindGameData, lpRootPathName, cchRootDir))
        {
            BOOL fFound;

            while (fFound = FindNextFile(hRet, (PWIN32_FIND_DATA) pFindGameData))
            {
                if (XapiFillInSaveGameData(pFindGameData, lpRootPathName, cchRootDir))
                {
                    break;
                }
            }

            if (!fFound)
            {
                //
                // We didn't find a save game directory
                //

                FindClose(hRet);
                hRet = INVALID_HANDLE_VALUE;
                SetLastError(ERROR_NO_MORE_FILES);
            }
        }
    }

    if (INVALID_HANDLE_VALUE != hRet)
    {
        PFINDGAME_HANDLE pFindGame = LocalAlloc(LMEM_FIXED, sizeof(FINDGAME_HANDLE));

        if (NULL != pFindGame)
        {
            pFindGame->dwSignature = FH_SIG_SAVEGAME;
            pFindGame->hFindFile = hRet;
            pFindGame->cchRootDir = cchRootDir;
            lstrcpynO(pFindGame->szRootDir, lpRootPathName, ARRAYSIZE(pFindGame->szRootDir));
            hRet = (HANDLE) pFindGame;
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
XFindNextSaveGame(
    IN HANDLE hFindGame,
    OUT PXGAME_FIND_DATA pFindGameData)
{
    BOOL bRet = FALSE;

#if DBG
    if ((NULL == hFindGame) ||
        (INVALID_HANDLE_VALUE == hFindGame) ||
        (FH_SIG_SAVEGAME != ((PFINDGAME_HANDLE) hFindGame)->dwSignature) ||
        (NULL == pFindGameData))
    {
        RIP("XFindNextSaveGame() invalid parameter");
    }
#endif // DBG

    {
        PFINDGAME_HANDLE pFindGame = (PFINDGAME_HANDLE) hFindGame;

        while (bRet = FindNextFile(pFindGame->hFindFile, (PWIN32_FIND_DATA) pFindGameData))
        {
            if (XapiFillInSaveGameData(pFindGameData, pFindGame->szRootDir, pFindGame->cchRootDir))
            {
                break;
            }
        }
    }

    return bRet;
}

BOOL
WINAPI
XFindClose(
    IN HANDLE hFindGame)
{
    BOOL fRet = FALSE;

    //
    // Note that all XFind handles begin with a DWORD signature that
    // identifies the type of XFind handle we're dealing with
    //

#if DBG
    if ((INVALID_HANDLE_VALUE == hFindGame) ||
        (NULL == hFindGame) ||
        ((FH_SIG_SAVEGAME != *((PDWORD) hFindGame)) &&
         (FH_SIG_NICKNAME != *((PDWORD) hFindGame)) &&
         (FH_SIG_CONTENT!= *((PDWORD) hFindGame)) &&
         (FH_SIG_SOUNDTRACK != *((PDWORD) hFindGame))))
    {
        RIP("XFindClose() invalid parameter (hFindGame)");
    }
#endif // DBG

    switch (*((PDWORD) hFindGame))
    {
        case FH_SIG_SAVEGAME:
        {
            PFINDGAME_HANDLE pFindGame = (PFINDGAME_HANDLE) hFindGame;
            HANDLE hFindFile = pFindGame->hFindFile;
            LocalFree(pFindGame);

            fRet = FindClose(hFindFile);
            break;
        }

        case FH_SIG_CONTENT:
        {
            PFINDCONTENT_HANDLE pFindCont = (PFINDCONTENT_HANDLE) hFindGame;
            HANDLE hFindFile = pFindCont->hFindFile;
            LocalFree(pFindCont);

            fRet = FindClose(hFindFile);
            break;
        }

        case FH_SIG_SOUNDTRACK:
        {
            PSNDTRK_ENUMSTATE pst = (PSNDTRK_ENUMSTATE) hFindGame;
            HANDLE hFindFile = pst->DbHandle;
            LocalFree(pst);

            fRet = FindClose(hFindFile);
            break;
        }

        case FH_SIG_NICKNAME:
        {
            LocalFree(hFindGame);
            fRet = TRUE;
            break;
        }
    }

    return fRet;
}

