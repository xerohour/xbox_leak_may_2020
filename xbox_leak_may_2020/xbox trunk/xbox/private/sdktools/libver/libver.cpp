/*++

Copyright (c) 2002  Microsoft Corporation

Module Name:

    libver.cpp

Abstract:

    This module implements a program to dump the version of an Xbox library.

--*/

#include "libverp.h"

CHAR g_szCurFilePath[MAX_PATH];
PSTR g_pszCurFilename;
XBEIMAGE_LIBRARY_VERSION g_LibVer = { 0 };
UINT g_cLibVers = 0;

const CHAR* LibvApprovedStatus[] = {
    "unapproved",
    "possibly approved",
    "approved"
};

DECLSPEC_NORETURN
VOID
LibvExitProcess(
    int ExitCode
    )
{
    exit(ExitCode);
}


VOID
LibvFormatVersion(
    PSTR pszVersion,
    PXBEIMAGE_LIBRARY_VERSION pLibVer
    )
{
    wsprintf(pszVersion,
             "%8.8s.LIB %d.%d.%d.%d%s [%s]",
             pLibVer->LibraryName,
             pLibVer->MajorVersion,
             pLibVer->MinorVersion,
             pLibVer->BuildVersion,
             pLibVer->QFEVersion,
             pLibVer->DebugBuild ? " (Debug)" : "",
             LibvApprovedStatus[pLibVer->ApprovedLibrary]);
}


VOID
LibvParseOutputLine(
    PSTR pszLine
    )
{
    DWORD dwAddr;
    UINT rgbVerData[16];
    BYTE rgbyteVerData[16];
    PXBEIMAGE_LIBRARY_VERSION pLibVer = (PXBEIMAGE_LIBRARY_VERSION) rgbyteVerData;
    CHAR szTextData[17];
    
    if (18 != sscanf(pszLine,
                     "  %08x: %02x %02x %02x %02x %02x %02x %02x %02x "
                     "%02x %02x %02x %02x %02x %02x %02x %02x %s",
                     &dwAddr,
                     &(rgbVerData[0]), &(rgbVerData[1]),
                     &(rgbVerData[2]), &(rgbVerData[3]),
                     &(rgbVerData[4]), &(rgbVerData[5]),
                     &(rgbVerData[6]), &(rgbVerData[7]),
                     &(rgbVerData[8]), &(rgbVerData[9]),
                     &(rgbVerData[10]), &(rgbVerData[11]),
                     &(rgbVerData[12]), &(rgbVerData[13]),
                     &(rgbVerData[14]), &(rgbVerData[15]),
                     szTextData))
    {
        //
        // Not a rawdata dump line
        //
        
        return;
    }

    for (int i = 0; i < ARRAYSIZE(rgbyteVerData); i++)
    {
        rgbyteVerData[i] = (BYTE) rgbVerData[i];
    }

    if (0 == g_cLibVers)
    {
        g_LibVer = *pLibVer;
    }
    else
    {
        if (0 != memcmp(&g_LibVer, pLibVer, sizeof(g_LibVer)))
        {
            CHAR szVersion[64];
            LibvFormatVersion(szVersion, pLibVer);
            LibvResourcePrintWarning(IDS_MULTIPLE_VERSIONS, g_pszCurFilename, szVersion);
        }
    }

    g_cLibVers++;
}

void
__cdecl
main(
    int argc,
    char *argv[]
    )
{
    if ((argc != 2) ||
        (0 == lstrcmp(argv[1], "/?")) ||
        (0 == lstrcmp(argv[1], "-?")) ||
        (0 == lstrcmpi(argv[1], "/help")) ||
        (0 == lstrcmpi(argv[1], "-help")))
    {
        LibvResourcePrintLogoBanner();
        LibvResourcePrintRange(stderr, IDS_LIBVER_USAGE);
        LibvExitProcess(0);
    }

    HANDLE hFind;
    WIN32_FIND_DATA wfd;
    CHAR szPath[MAX_PATH];

    lstrcpyn(szPath, argv[1], ARRAYSIZE(szPath));
    if (PathIsDirectory(szPath))
    {
        PathAppend(szPath, "*");
    }
    
    hFind = FindFirstFile(szPath, &wfd);
    
    PathRemoveFileSpec(szPath);

    if (INVALID_HANDLE_VALUE == hFind)
    {
        LibvResourcePrintErrorAndExit(IDS_CANNOT_OPEN_INPUT_FILE, argv[1]);
    }

    while (INVALID_HANDLE_VALUE != hFind)
    {
        if (0 == (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            PathCombine(g_szCurFilePath, szPath, wfd.cFileName);
            g_pszCurFilename = wfd.cFileName;

            CHAR szLinkCmdLine[MAX_PATH * 2];
            wsprintf(szLinkCmdLine, "link /dump /rawdata /section:.XBLD$A /section:.XBLD$V %s", g_szCurFilePath);

            HANDLE hReadPipe, hWritePipe;
            SECURITY_ATTRIBUTES sa;
            sa.nLength = sizeof(sa);
            sa.lpSecurityDescriptor = NULL;
            sa.bInheritHandle = TRUE;
            
            if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0))
            {
                LibvResourcePrintErrorAndExit(IDS_NO_MEMORY);
            }

            PROCESS_INFORMATION pi;
            STARTUPINFO si;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            si.dwFlags = STARTF_USESTDHANDLES;
            si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
            si.hStdOutput = hWritePipe;
            si.hStdError = hWritePipe;

            if (!CreateProcess(NULL,
                               szLinkCmdLine,
                               NULL,
                               NULL,
                               TRUE,
                               0,
                               NULL,
                               NULL,
                               &si,
                               &pi))
            {
                LibvResourcePrintErrorAndExit(IDS_COULD_NOT_INVOKE_LINK);
            }

            CloseHandle(hWritePipe);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            DWORD dwBytesRead;
            CHAR szBuffer[2048];
            CHAR szLineBuffer[256];
            DWORD dwCurBufferPos = 0;

            while (ReadFile(hReadPipe,
                            &(szBuffer[dwCurBufferPos]),
                            ARRAYSIZE(szBuffer) - dwCurBufferPos - 1,
                            &dwBytesRead,
                            NULL) && (0 != dwBytesRead))
            {
                DWORD dwCurLinePos = 0;

                szBuffer[dwCurBufferPos + dwBytesRead] = '\0';
                PSTR pszLastGoodLine = szBuffer;
                PSTR pszCRLF = strstr(szBuffer, "\r\n");

                if (NULL == pszCRLF)
                {
                    LibvResourcePrintErrorAndExit(IDS_INTERNAL_TOOL_ERROR, __LINE__);
                }

                while (pszCRLF)
                {
                    DWORD dwLineLength = (pszCRLF - szBuffer) - dwCurLinePos;

                    if (dwLineLength > (ARRAYSIZE(szLineBuffer) - 1))
                    {
                        printf("Line length = %d\n", dwLineLength);
                        LibvResourcePrintErrorAndExit(IDS_INTERNAL_TOOL_ERROR, __LINE__);
                    }
                    
                    if (dwLineLength != 0)
                    {
                        CopyMemory(szLineBuffer,
                                   &(szBuffer[dwCurLinePos]),
                                   dwLineLength);

                        szLineBuffer[dwLineLength] = '\0';

                        LibvParseOutputLine(szLineBuffer);
                    }

                    //
                    // Skip past this line and the CRLF pair
                    //
                    
                    dwCurLinePos += (dwLineLength + 2);
                    
                    pszLastGoodLine = pszCRLF + 2;
                    pszCRLF = strstr(&(szBuffer[dwCurLinePos]), "\r\n");
                }

                //
                // Shift the remaining buffer back to the beginning of the buffer and
                // then read the next chunk
                //
                
                dwCurBufferPos = (dwCurBufferPos + dwBytesRead) - (pszLastGoodLine - szBuffer);

                RtlMoveMemory(szBuffer,
                              pszLastGoodLine,
                              dwCurBufferPos);
            }

            CloseHandle(hReadPipe);

            if (0 != g_cLibVers)
            {
                CHAR szVersion[64];
                LibvFormatVersion(szVersion, &g_LibVer);
                LibvResourcePrintf(stdout, IDS_VERSION_OUTPUT, g_pszCurFilename, szVersion, g_cLibVers);
                fputs("\n", stdout);
            }
            else
            {
                LibvResourcePrintWarning(IDS_NO_VERSION_DETECTED, g_pszCurFilename);
            }

            g_cLibVers = 0;
        }

        if (!FindNextFile(hFind, &wfd))
        {
            FindClose(hFind);
            hFind = INVALID_HANDLE_VALUE;
        }
    }
}
