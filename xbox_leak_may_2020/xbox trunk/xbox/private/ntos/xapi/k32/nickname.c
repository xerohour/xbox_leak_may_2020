#include "basedll.h"

const OBJECT_STRING NickFile = CONSTANT_OBJECT_STRING(OTEXT("\\Device\\Harddisk0\\partition1\\UDATA\\NICKNAME.XBN"));

BOOL
WINAPI
XSetNicknameW(
    IN LPCWSTR lpNickname,
    IN BOOL fPreserveCase)
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    IO_STATUS_BLOCK Iosb;
    HANDLE FileHandle;
    X_NICK_DB_ENTRY Nicknames[NUM_NICK_ENTRIES];
    DWORD dwTitleID;

    RIP_ON_NOT_TRUE("XSetNicknameW()", (NULL != lpNickname));
    RIP_ON_NOT_TRUE("XSetNicknameW()", (wcslen(lpNickname) < MAX_NICKNAME));

    if (L'\0' == *lpNickname)
    {
        return FALSE;
    }

    dwTitleID = XeImageHeader()->Certificate->TitleID;

    InitializeObjectAttributes(&Obja, (POBJECT_STRING) &NickFile, OBJ_CASE_INSENSITIVE, NULL, NULL);

    //
    // Attempt to open an existing nickname file
    //

    Status = NtCreateFile(&FileHandle,
                          SYNCHRONIZE | GENERIC_READ | GENERIC_WRITE,
                          &Obja,
                          &Iosb,
                          NULL,
                          FILE_ATTRIBUTE_SYSTEM,
                          FILE_SHARE_READ,
                          FILE_OPEN,
                          FILE_WRITE_THROUGH | FILE_SYNCHRONOUS_IO_NONALERT);

    if (NT_SUCCESS(Status))
    {
        Status = NtReadFile(FileHandle,
                            NULL,
                            NULL,
                            NULL,
                            &Iosb,
                            Nicknames,
                            sizeof(Nicknames),
                            NULL);

        if (NT_SUCCESS(Status) && (sizeof(Nicknames) != (DWORD) Iosb.Information))
        {
            //
            // We didn't read the right number of bytes, so don't do anything with this
            // data (the file will need to be recreated below)
            //
            Status = STATUS_END_OF_FILE;
        }

        if (NT_SUCCESS(Status))
        {
            WCHAR szNewNick[MAX_NICKNAME];
            int i;
            BOOL fNewEntry;

            lstrcpynW(szNewNick, lpNickname, ARRAYSIZE(szNewNick));

            //
            // Look to see if this nickname/titleid combo already exists in the table
            //

            for (i = 0; i < ARRAYSIZE(Nicknames); i++)
            {
                if ((FALSE != (fNewEntry = (L'\0' == Nicknames[i].szNickname[0]))) ||
                    ((dwTitleID == Nicknames[i].dwTitleID) &&
                        (0 == _wcsicmp(lpNickname, Nicknames[i].szNickname))))
                {
                    if (!fNewEntry && !fPreserveCase)
                    {
                        //
                        // The title didn't want us to preserve the case passed in, so
                        // use the name as it was last written instead
                        //

                        lstrcpynW(szNewNick, Nicknames[i].szNickname, ARRAYSIZE(szNewNick));
                    }
                    break;
                }
            }

            if (ARRAYSIZE(Nicknames) == i)
            {
                fNewEntry = TRUE;
            }

            if (!fNewEntry && (0 == i) && (0 == (wcscmp(szNewNick, Nicknames[0].szNickname))))
            {
                //
                // No change - the most recent nickname is exactly the same, so just close the file
                // and return TRUE without rewriting the exact same data.
                //

                NtClose(FileHandle);
                return TRUE;
            }

            if (0 != i)
            {
                //
                // Shift all of the entries further down the table to make room for this entry
                // at the top of the table
                //

                RtlMoveMemory(&(Nicknames[1]),
                              &(Nicknames[0]),
                              sizeof(X_NICK_DB_ENTRY) * min(i, ARRAYSIZE(Nicknames) - 1));
            }

            //
            // Write the new entry at the top of the table
            //

            Nicknames[0].dwTitleID = dwTitleID;
            lstrcpynW(Nicknames[0].szNickname, szNewNick, ARRAYSIZE(Nicknames[0].szNickname));
        }
        else
        {
            NtClose(FileHandle);
        }
    }
    else if (Status != STATUS_OBJECT_NAME_NOT_FOUND)
    {
        //
        // The open failed for an unexpected reason - we don't want to proceed and
        // overwrite a valid nickname file...
        //

        return FALSE;
    }

    if (!NT_SUCCESS(Status))
    {
        //
        // Attempt to create a new nickname file
        //

        Status = NtCreateFile(&FileHandle,
                              SYNCHRONIZE | GENERIC_READ | GENERIC_WRITE,
                              &Obja,
                              &Iosb,
                              NULL,
                              FILE_ATTRIBUTE_SYSTEM,
                              FILE_SHARE_READ,
                              FILE_OVERWRITE_IF,
                              FILE_WRITE_THROUGH | FILE_SYNCHRONOUS_IO_NONALERT);

        if (NT_SUCCESS(Status))
        {
            //
            // Zero out the memory that we're going to write to the file
            //

            RtlZeroMemory(&Nicknames, sizeof(Nicknames));

            //
            // Write the new entry at the top of the table
            //

            Nicknames[0].dwTitleID = dwTitleID;
            lstrcpynW(Nicknames[0].szNickname, lpNickname, ARRAYSIZE(Nicknames[0].szNickname));
        }
    }

    if (NT_SUCCESS(Status))
    {
        //
        // We have a valid FileHandle and Nicknames array at this point.
        // It is time to write it back to disk and close the file..
        //

        LARGE_INTEGER liByteOffset;

        liByteOffset.QuadPart = 0;
        
        Status = NtWriteFile(FileHandle,
                             NULL,
                             NULL,
                             NULL,
                             &Iosb,
                             Nicknames,
                             sizeof(Nicknames),
                             &liByteOffset);

        NtClose(FileHandle);
    }

    return(NT_SUCCESS(Status));
}

BOOL
GetNextNickname(
    PFINDNICKNAME_HANDLE pFindNick,
    LPWSTR lpNickname,
    UINT uSize)
{
    DWORD dwTitleID;

    ASSERT(pFindNick && (FH_SIG_NICKNAME == pFindNick->dwSignature));

    dwTitleID = XeImageHeader()->Certificate->TitleID;

    for (; pFindNick->uCurIndex < ARRAYSIZE(pFindNick->Nicknames); (pFindNick->uCurIndex)++)
    {
        if ((L'\0' != pFindNick->Nicknames[pFindNick->uCurIndex].szNickname[0]) &&
            (wcslen(pFindNick->Nicknames[pFindNick->uCurIndex].szNickname) < uSize) &&
            (!pFindNick->fThisTitleOnly ||
             (dwTitleID == pFindNick->Nicknames[pFindNick->uCurIndex].dwTitleID)))
        {
            BOOL fPrevMatch = FALSE;
#if DBG
            //
            // Always check for previous matches in the debug build
            // (so we can validate that the nickfile doesn't have duplicates with
            // the same title id)
            //
#else  // DBG
            //
            // If we're searching all title's, we need to check for previous matches
            // so that we don't return duplicate names (the same name can appear
            // many times with a different title id each time)
            //
            if (!pFindNick->fThisTitleOnly)
#endif // DBG
            {
                UINT uCompIndex;
                for (uCompIndex = 0; uCompIndex < pFindNick->uCurIndex; uCompIndex++)
                {
                    if (0 == _wcsicmp(pFindNick->Nicknames[pFindNick->uCurIndex].szNickname,
                                      pFindNick->Nicknames[uCompIndex].szNickname))
                    {
                        fPrevMatch = TRUE;
                        break;
                    }
                }
            }

            if (pFindNick->fThisTitleOnly || !fPrevMatch)
            {
#if DBG
                if (pFindNick->fThisTitleOnly && fPrevMatch)
                {
                    XDBGERR("XAPI", "XFindNextNicknameW() found corrupt data (duplicate nicknames)");
                }
#endif // DBG
                wcscpy(lpNickname, pFindNick->Nicknames[pFindNick->uCurIndex].szNickname);
                (pFindNick->uCurIndex)++;
                return TRUE;
            }
        }
    }
    
    return FALSE;    
}

HANDLE
WINAPI
XFindFirstNicknameW(
    IN BOOL fThisTitleOnly,
    OUT LPWSTR lpNickname,
    IN UINT uSize)
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    IO_STATUS_BLOCK Iosb;
    HANDLE FileHandle;
    PFINDNICKNAME_HANDLE pFindNick = NULL;
    
    RIP_ON_NOT_TRUE("XFindFirstNicknameW()", (NULL != lpNickname));

    InitializeObjectAttributes(&Obja, (POBJECT_STRING) &NickFile, OBJ_CASE_INSENSITIVE, NULL, NULL);

    //
    // Attempt to open an existing nickname file
    //
    
    Status = NtCreateFile(&FileHandle,
                          SYNCHRONIZE | GENERIC_READ,
                          &Obja,
                          &Iosb,
                          NULL,
                          FILE_ATTRIBUTE_SYSTEM,
                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                          FILE_OPEN,
                          FILE_SYNCHRONOUS_IO_NONALERT);

    if (NT_SUCCESS(Status))
    {
        pFindNick = LocalAlloc(LMEM_FIXED, sizeof(FINDNICKNAME_HANDLE));
        
        if (pFindNick)
        {
            pFindNick->dwSignature = FH_SIG_NICKNAME;
            pFindNick->uCurIndex = 0;
            pFindNick->fThisTitleOnly = fThisTitleOnly;
            
            Status = NtReadFile(FileHandle,
                                NULL,
                                NULL,
                                NULL,
                                &Iosb,
                                pFindNick->Nicknames,
                                sizeof(pFindNick->Nicknames),
                                NULL);

            if (NT_SUCCESS(Status) && (sizeof(pFindNick->Nicknames) != (DWORD) Iosb.Information))
            {
                //
                // We didn't read the right number of bytes, so don't do anything with this
                // data (the file will need to be recreated below)
                //
                Status = STATUS_END_OF_FILE;
            }
        }
        else
        {
            Status = STATUS_INSUFFICIENT_RESOURCES;
        }

        NtClose(FileHandle);
    }

    if (NT_SUCCESS(Status))
    {
        if (!GetNextNickname(pFindNick, lpNickname, uSize))
        {
            Status = STATUS_UNSUCCESSFUL;
        }
    }

    if (!NT_SUCCESS(Status) && (NULL != pFindNick))
    {
        LocalFree(pFindNick);
        pFindNick = NULL;
    }

    return (HANDLE) (pFindNick ? pFindNick : INVALID_HANDLE_VALUE);
}

BOOL
WINAPI
XFindNextNicknameW(
    IN HANDLE hFindNickname,
    OUT LPWSTR lpNickname,
    IN UINT uSize)
{
    PFINDNICKNAME_HANDLE pFindNick = (PFINDNICKNAME_HANDLE) hFindNickname;

    RIP_ON_NOT_TRUE("XFindNextNicknameW()", (NULL != hFindNickname));
    RIP_ON_NOT_TRUE("XFindNextNicknameW()", (INVALID_HANDLE_VALUE != hFindNickname));
    RIP_ON_NOT_TRUE("XFindNextNicknameW()", (NULL != lpNickname));

#if DBG
    if (FH_SIG_NICKNAME != pFindNick->dwSignature)
    {
        RIP("XFindNextNicknameW() - invalid parameter (hFindNickname)");
    }
#endif // DBG
    
    return GetNextNickname(pFindNick, lpNickname, uSize);
}


