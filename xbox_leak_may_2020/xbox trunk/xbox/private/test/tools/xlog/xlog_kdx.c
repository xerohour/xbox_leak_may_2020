/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  xlog_kdx.c

Abstract:

  This module is the kernel debugger extension for the xLog library

Author:

  Steven Kehrli (steveke) 1-Nov-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace xLogNamespace;

WINDBG_EXTENSION_APIS  ExtensionApis;      // ExtensionApis are the callbacks to the functions for standard operations.  See wdbgexts.h for more info.
USHORT                 SavedMajorVersion;  // SavedMajorVersion indicates whether the system is a checked build (0x0C) or a free build (0x0F)
USHORT                 SavedMinorVersion;  // SavedMinorVersion indicates the system build number

EXT_API_VERSION        ApiVersion = { (VER_PRODUCTVERSION_W >> 8), (VER_PRODUCTVERSION_W & 0xff), EXT_API_VERSION_NUMBER, 0 };  // ApiVersion is the version of the debugger extension



BOOL
WINAPI
DllMain(
    IN HINSTANCE hInstance,
    IN DWORD     dwReason,
    IN LPVOID    lpContext
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  DLL entry point

Arguments:

  hInstance - handle to the module
  dwReason - indicates the reason for calling the function
  lpContext - reserved

Return Value:

  TRUE on success

------------------------------------------------------------------------------*/
{
    return TRUE;
}



VOID
WinDbgExtensionDllInit(
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    USHORT                 MajorVersion,
    USHORT                 MinorVersion
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Initializes the debugger extension

Arguments:

  lpExtensionApis - Pointer to the callbacks to the functions for standard operations
  MajorVersion - Indicates whether the system is a checked build (0x0C) or a free build (0x0F)
  MinorVersion - Indicates the system build number

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // Save the pointer to the callbacks
    ExtensionApis = *lpExtensionApis;
    // Save the major version
    SavedMajorVersion = MajorVersion;
    // Save the minor version
    SavedMinorVersion = MinorVersion;
}



LPEXT_API_VERSION
ExtensionApiVersion(
    VOID
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Returns the version number of the debugger extension

Arguments:

  None

Return Value:

  LPEXT_API_VERSION - Pointer to the version number of the debugger extension

------------------------------------------------------------------------------*/
{
    return &ApiVersion;
}



VOID
CheckVersion(
    VOID
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Checks the version number of the debugger extension against the version number of the system

Arguments:

  None

Return Value:

  None

------------------------------------------------------------------------------*/
{
    dprintf("\n");
    dprintf("*** xlog_kdx CheckVersion\n");
#if DBG
    if ((0x0c != SavedMajorVersion) || (VER_PRODUCTBUILD != SavedMinorVersion)) {
        dprintf("  Extension DLL (%d Checked) does not match target system (%d %s)\n\n", VER_PRODUCTBUILD, SavedMinorVersion, (0x0f == SavedMajorVersion) ? "Free" : "Checked");
    }
#else
    if ((0x0f != SavedMajorVersion) || (VER_PRODUCTBUILD != SavedMinorVersion)) {
        dprintf("  Extension DLL (%d Free) does not match target system (%d %s)\n\n", VER_PRODUCTBUILD, SavedMinorVersion, (0x0f == SavedMajorVersion) ? "Free" : "Checked");
    }
#endif
}



DECLARE_API( help )
{
    dprintf("\n");
    dprintf("*** xlog_kdx help:\n");
    dprintf("  help                                    - Display this help information\n");
    dprintf("  version                                 - Display the version number for xlog_kdx\n");
    dprintf("  dumphlog <address>                      - Display the log object\n");
    dprintf("  dumpfunctioninfo <address> [<thread>]   - Display the function info for the log object and optional thread\n");
    dprintf("  dumpvariationlist <address> [<thread>]  - Display the variation list for the log object and optional thread\n");
    dprintf("\n");
}



DECLARE_API( version )
{
    dprintf("\n");
    dprintf("*** xlog_kdx version\n");
#if DBG
    dprintf("  %d Checked\n\n", VER_PRODUCTBUILD);
#else
    dprintf("  %d Free\n\n", VER_PRODUCTBUILD);
#endif
}



DECLARE_API( dumphlog )
{
    // Address is the address of the log object
    ULONGLONG         Address = 0;
    // LogObject is the log object
    XLOG_OBJECT       LogObject;
    // szString is a string
    LPSTR             szString = NULL;

    // bReturnValue is the return value of ReadMemory()
    BOOL              bReturnValue = FALSE;
    // cbBytesRead is the number of bytes read for ReadMemory()
    ULONG             cbBytesRead = 0;



    // Get the address of the log object
    Address = (ULONGLONG) GetExpression(args);

    if (NULL == Address) {
        dprintf("dumphlog <address>\n");
        return;
    }

    // Get the XLOG_OBJECT structure
    ZeroMemory(&LogObject, sizeof(XLOG_OBJECT));
    bReturnValue = ReadMemory((ULONG) Address, &LogObject, sizeof(XLOG_OBJECT), &cbBytesRead);

    if ((FALSE == bReturnValue) || (cbBytesRead != sizeof(XLOG_OBJECT))) {
        dprintf("Cannot read 0x%08x bytes at 0x%08x for XLOG_OBJECT\n", sizeof(XLOG_OBJECT), Address);
        dprintf("\n");
        return;
    }

    dprintf("*** xlog_kdx dumphlog\n");
    dprintf("\n");

    dprintf("  hLog:                         0x%08x\n", Address);
    dprintf("    hMemObject:                 0x%08x\n", LogObject.hMemObject);
    dprintf("    szObjectUUID_A:             %s\n", LogObject.szObjectUUID_A);
    dprintf("    szOwnerAlias_A:             %s\n", LogObject.szOwnerAlias_A);
    dprintf("    szHostName_A:               %s\n", LogObject.szHostName_A);
    dprintf("    dwPlatformId:               %d\n", LogObject.dwPlatformId);

    switch (LogObject.dwPlatformId) {
    case OS_PLATFORM_XBOX:
        dprintf("    szPlatformId:               %s\n", "Xbox");
        dprintf("    KernelDebug:                %s\n", (TRUE == LogObject.KernelDebug) ? "True" : "False");
        dprintf("    KernelVersion:              %u\n", LogObject.KernelVersion);
        dprintf("    XapiDebug:                  %s\n", (TRUE == LogObject.XapiDebug) ? "True" : "False");
        dprintf("    XapiVersion:                %u\n", LogObject.XapiVersion);

        break;

    case OS_PLATFORM_NT:
        dprintf("    szPlatformId:               %s\n", "Nt");
        dprintf("    dwMajorVersion:             %u\n", LogObject.dwMajorVersion);
        dprintf("    dwMinorVersion:             %u\n", LogObject.dwMinorVersion);
        dprintf("    dwBuildNumber:              %u\n", LogObject.dwBuildNumber);

        break;

    default:
        dprintf("    szPlatformId:               %s\n", "Unknown");
        break;
    }

    if (NULL == LogObject.lpszLogFileName_A) {
        dprintf("    lpszLogFileName_A:          %s\n", LogObject.lpszLogFileName_A);
    }
    else {
        szString = (LPSTR) LocalAlloc(LPTR, LogObject.dwLogFileNameLen);
        if (NULL != szString) {
            bReturnValue = ReadMemory((ULONG) LogObject.lpszLogFileName_A, szString, LogObject.dwLogFileNameLen, &cbBytesRead);

            if ((FALSE == bReturnValue) || (cbBytesRead != LogObject.dwLogFileNameLen)) {
                dprintf("    Cannot read 0x%08x bytes at 0x%08x for lpszLogFileName_A\n", LogObject.dwLogFileNameLen, LogObject.lpszLogFileName_A);
            }
            else {
                dprintf("    lpszLogFileName_A:          %s\n", szString);
            }

            LocalFree(szString);
            szString = NULL;
        }
        else {
            dprintf("    Cannot allocate 0x%08x bytes for lpszLogFileName_A\n", LogObject.dwLogFileNameLen);
        }

    }

    dprintf("    hLogFile:                   0x%08x\n", LogObject.hLogFile);

    if (NULL == LogObject.lpszHttpServer_A) {
        dprintf("    lpszHttpServer_A:           %s\n", LogObject.lpszHttpServer_A);
    }
    else {
        szString = (LPSTR) LocalAlloc(LPTR, LogObject.dwHttpServerLen);
        if (NULL != szString) {
            bReturnValue = ReadMemory((ULONG) LogObject.lpszHttpServer_A, szString, LogObject.dwHttpServerLen, &cbBytesRead);

            if ((FALSE == bReturnValue) || (cbBytesRead != LogObject.dwHttpServerLen)) {
                dprintf("    Cannot read 0x%08x bytes at 0x%08x for lpszHttpServer_A\n", LogObject.dwHttpServerLen, LogObject.lpszHttpServer_A);
            }
            else {
                dprintf("    lpszHttpServer_A:           %s\n", szString);
            }

            LocalFree(szString);
            szString = NULL;
        }
        else {
            dprintf("    Cannot allocate 0x%08x bytes for lpszHttpServer_A\n", LogObject.dwHttpServerLen);
        }

    }

    dprintf("    HttpAddr:                   0x%08x\n", LogObject.HttpAddr);

    if (NULL == LogObject.lpszConfigPage_A) {
        dprintf("    lpszConfigPage_A:           %s\n", LogObject.lpszConfigPage_A);
    }
    else {
        szString = (LPSTR) LocalAlloc(LPTR, LogObject.dwConfigPageLen);
        if (NULL != szString) {
            bReturnValue = ReadMemory((ULONG) LogObject.lpszConfigPage_A, szString, LogObject.dwConfigPageLen, &cbBytesRead);

            if ((FALSE == bReturnValue) || (cbBytesRead != LogObject.dwConfigPageLen)) {
                dprintf("    Cannot read 0x%08x bytes at 0x%08x for lpszConfigPage_A\n", LogObject.dwConfigPageLen, LogObject.lpszConfigPage_A);
            }
            else {
                dprintf("    lpszConfigPage_A:           %s\n", szString);
            }

            LocalFree(szString);
            szString = NULL;
        }
        else {
            dprintf("    Cannot allocate 0x%08x bytes for lpszConfigPage_A\n", LogObject.dwConfigPageLen);
        }

    }

    if (NULL == LogObject.lpszStatePage_A) {
        dprintf("    lpszStatePage_A:            %s\n", LogObject.lpszStatePage_A);
    }
    else {
        szString = (LPSTR) LocalAlloc(LPTR, LogObject.dwStatePageLen);
        if (NULL != szString) {
            bReturnValue = ReadMemory((ULONG) LogObject.lpszStatePage_A, szString, LogObject.dwStatePageLen, &cbBytesRead);

            if ((FALSE == bReturnValue) || (cbBytesRead != LogObject.dwStatePageLen)) {
                dprintf("    Cannot read 0x%08x bytes at 0x%08x for lpszStatePage_A\n", LogObject.dwStatePageLen, LogObject.lpszStatePage_A);
            }
            else {
                dprintf("    lpszStatePage_A:            %s\n", szString);
            }

            LocalFree(szString);
            szString = NULL;
        }
        else {
            dprintf("    Cannot allocate 0x%08x bytes for lpszStatePage_A\n", LogObject.dwStatePageLen);
        }

    }

    if (NULL == LogObject.lpszLogPage_A) {
        dprintf("    lpszLogPage_A:              %s\n", LogObject.lpszLogPage_A);
    }
    else {
        szString = (LPSTR) LocalAlloc(LPTR, LogObject.dwLogPageLen);
        if (NULL != szString) {
            bReturnValue = ReadMemory((ULONG) LogObject.lpszLogPage_A, szString, LogObject.dwLogPageLen, &cbBytesRead);

            if ((FALSE == bReturnValue) || (cbBytesRead != LogObject.dwLogPageLen)) {
                dprintf("    Cannot read 0x%08x bytes at 0x%08x for lpszLogPage_A\n", LogObject.dwLogPageLen, LogObject.lpszLogPage_A);
            }
            else {
                dprintf("    lpszLogPage_A:              %s\n", szString);
            }

            LocalFree(szString);
            szString = NULL;
        }
        else {
            dprintf("    Cannot allocate 0x%08x bytes for lpszLogPage_A\n", LogObject.dwLogPageLen);
        }

    }

    if (NULL == LogObject.lpszWebPostFileName_A) {
        dprintf("    lpszWebPostFileName_A:      %s\n", LogObject.lpszWebPostFileName_A);
    }
    else {
        szString = (LPSTR) LocalAlloc(LPTR, LogObject.dwWebPostFileNameLen);
        if (NULL != szString) {
            bReturnValue = ReadMemory((ULONG) LogObject.lpszWebPostFileName_A, szString, LogObject.dwWebPostFileNameLen, &cbBytesRead);

            if ((FALSE == bReturnValue) || (cbBytesRead != LogObject.dwWebPostFileNameLen)) {
                dprintf("    Cannot read 0x%08x bytes at 0x%08x for lpszWebPostFileName_A\n", LogObject.dwWebPostFileNameLen, LogObject.lpszWebPostFileName_A);
            }
            else {
                dprintf("    lpszWebPostFileName_A:      %s\n", szString);
            }

            LocalFree(szString);
            szString = NULL;
        }
        else {
            dprintf("    Cannot allocate 0x%08x bytes for lpszWebPostFileName_A\n", LogObject.dwWebPostFileNameLen);
        }

    }

    dprintf("    hWebPostFile:               0x%08x\n", LogObject.hWebPostFile);
    dprintf("    sConsoleSocket:             0x%08x\n", LogObject.sConsoleSocket);
    dprintf("    dwLogLevel:                 0x%08x\n", LogObject.dwLogLevel);
    dprintf("    dwLogOptions:               0x%08x\n", LogObject.dwLogOptions);
    dprintf("    dwVariationsTotal:          %u\n", LogObject.dwVariationsTotal);
    dprintf("    dwVariationsException:      %u\n", LogObject.dwVariationsException);
    dprintf("    dwVariationsBreak:          %u\n", LogObject.dwVariationsBreak);
    dprintf("    dwVariationsFail:           %u\n", LogObject.dwVariationsFail);
    dprintf("    dwVariationsWarning:        %u\n", LogObject.dwVariationsWarning);
    dprintf("    dwVariationsBlock:          %u\n", LogObject.dwVariationsBlock);
    dprintf("    dwVariationsPass:           %u\n", LogObject.dwVariationsPass);
    dprintf("    dwVariationsUnknown:        %u\n", LogObject.dwVariationsUnknown);
    dprintf("    pThreadInfo:                0x%08x\n", LogObject.pThreadInfo);

    dprintf("\n");
}



DECLARE_API( dumpfunctioninfo )
{
    // Address is the address of the log object
    ULONGLONG         Address = 0;
    // ThreadId is the thread id
    DWORD             ThreadId = 0;
    // LogObject is the log object
    XLOG_OBJECT       LogObject;
    // ThreadInfoAddress is the address of the thread info
    PTHREAD_INFO      ThreadInfoAddress = NULL;
    // ThreadInfo is the thread info
    THREAD_INFO       ThreadInfo;
    // FunctionInfoAddress is the address of the function info
    PFUNCTION_INFO    FunctionInfoAddress = NULL;
    // FunctionInfo is the function info
    FUNCTION_INFO     FunctionInfo;

    // bReturnValue is the return value of ReadMemory()
    BOOL              bReturnValue = FALSE;
    // cbBytesRead is the number of bytes read for ReadMemory()
    ULONG             cbBytesRead = 0;



    // Get the address of the log object and the thread id
    sscanf(args, "%lx %lx", &Address, &ThreadId);

    if (NULL == Address) {
        dprintf("dumpfunctioninfo <address> [<thread>]\n");
        return;
    }

    // Get the XLOG_OBJECT structure
    ZeroMemory(&LogObject, sizeof(XLOG_OBJECT));
    bReturnValue = ReadMemory((ULONG) Address, &LogObject, sizeof(XLOG_OBJECT), &cbBytesRead);

    if ((FALSE == bReturnValue) || (cbBytesRead != sizeof(XLOG_OBJECT))) {
        dprintf("Cannot read 0x%08x bytes at 0x%08x for XLOG_OBJECT\n", sizeof(XLOG_OBJECT), Address);
        dprintf("\n");
        return;
    }

    // Get the pointer to the head of the thread info list
    ThreadInfoAddress = LogObject.pThreadInfo;

    dprintf("*** xlog_kdx dumpfunctioninfoall\n");
    dprintf("\n");

    if (NULL == ThreadInfoAddress) {
        dprintf("  No thread info\n");
        dprintf("\n");
    }
    else {
        dprintf("  hLog:                         0x%08x\n", Address);

        do {
            // Get the THREAD_INFO structure
            ZeroMemory(&ThreadInfo, sizeof(THREAD_INFO));
            bReturnValue = ReadMemory((ULONG) ThreadInfoAddress, &ThreadInfo, sizeof(THREAD_INFO), &cbBytesRead);

            if ((FALSE == bReturnValue) || (cbBytesRead != sizeof(THREAD_INFO))) {
                dprintf("    Cannot read 0x%08x bytes at 0x%08x for THREAD_INFO\n", sizeof(THREAD_INFO), ThreadInfoAddress);
                dprintf("\n");

                ThreadInfoAddress = NULL;
            }
            else {
                if ((0 == ThreadId) || (ThreadId == ThreadInfo.dwThreadId)) {
                    dprintf("    pThreadInfo:                0x%08x\n", ThreadInfoAddress);
                    dprintf("      dwThreadId:               0x%08x\n", ThreadInfo.dwThreadId);

                    // Get the pointer to the function info
                    FunctionInfoAddress = ThreadInfo.pFunctionInfo;

                    if (NULL == FunctionInfoAddress) {
                        dprintf("      No function info\n");
                        dprintf("\n");
                    }
                    else {
                        // Get the FUNCTION_INFO structure
                        ZeroMemory(&FunctionInfo, sizeof(FUNCTION_INFO));
                        bReturnValue = ReadMemory((ULONG) FunctionInfoAddress, &FunctionInfo, sizeof(FUNCTION_INFO), &cbBytesRead);

                        if ((FALSE == bReturnValue) || (cbBytesRead != sizeof(FUNCTION_INFO))) {
                            dprintf("      Cannot read 0x%08x bytes at 0x%08x for FUNCTION_INFO\n", sizeof(FUNCTION_INFO), FunctionInfoAddress);
                            dprintf("\n");
                        }
                        else {
                            if (NULL == FunctionInfo.lpszComponentName_A) {
                                dprintf("      lpszComponentName_A:      %s\n", FunctionInfo.lpszComponentName_A);
                            }
                            else {
                                dprintf("      lpszComponentName_A:      %s\n", FunctionInfo.szComponentName_A);
                            }

                            if (NULL == FunctionInfo.lpszSubcomponentName_A) {
                                dprintf("      lpszSubcomponentName_A:   %s\n", FunctionInfo.lpszSubcomponentName_A);
                            }
                            else {
                                dprintf("      lpszSubcomponentName_A:   %s\n", FunctionInfo.szSubcomponentName_A);
                            }

                            if (NULL == FunctionInfo.lpszFunctionName_A) {
                                dprintf("      lpszFunctionName_A:       %s\n", FunctionInfo.lpszFunctionName_A);
                            }
                            else {
                                dprintf("      lpszFunctionName_A:       %s\n", FunctionInfo.szFunctionName_A);
                            }

                            dprintf("\n");

                            break;
                        }
                    }
                }

                ThreadInfoAddress = ThreadInfo.pNextThreadInfo;
            }
        } while (NULL != ThreadInfoAddress);

        if ((0 != ThreadId) && (NULL == ThreadInfoAddress)) {
            dprintf("    Thread info not found for dwThreadId 0x%08x\n", ThreadId);
        }
    }
}



DECLARE_API( dumpvariationlist )
{
    // Address is the address of the log object
    ULONGLONG           Address = 0;
    // ThreadId is the thread id
    ULONGLONG           ThreadId = 0;
    // LogObject is the log object
    XLOG_OBJECT         LogObject;
    // ThreadInfoAddress is the address of the thread info
    PTHREAD_INFO        ThreadInfoAddress = NULL;
    // ThreadInfo is the thread info
    THREAD_INFO         ThreadInfo;
    // VariationElementAddress is the address of the variation element
    PVARIATION_ELEMENT  VariationElementAddress = NULL;
    // VariationElement is the variation element
    VARIATION_ELEMENT   VariationElement;

    // bReturnValue is the return value of ReadMemory()
    BOOL                bReturnValue = FALSE;
    // cbBytesRead is the number of bytes read for ReadMemory()
    ULONG               cbBytesRead = 0;



    // Get the address of the log object and the thread id
    sscanf(args, "%lx %lx", &Address, &ThreadId);

    if (NULL == Address) {
        dprintf("dumpvariationlistall <address>\n");
        return;
    }

    // Get the XLOG_OBJECT structure
    ZeroMemory(&LogObject, sizeof(XLOG_OBJECT));
    bReturnValue = ReadMemory((ULONG) Address, &LogObject, sizeof(XLOG_OBJECT), &cbBytesRead);

    if ((FALSE == bReturnValue) || (cbBytesRead != sizeof(XLOG_OBJECT))) {
        dprintf("Cannot read 0x%08x bytes at 0x%08x for XLOG_OBJECT\n", sizeof(XLOG_OBJECT), Address);
        dprintf("\n");
        return;
    }

    // Get the pointer to the head of the thread info list
    ThreadInfoAddress = LogObject.pThreadInfo;

    dprintf("*** xlog_kdx dumpvariationlistall\n");
    dprintf("\n");

    if (NULL == ThreadInfoAddress) {
        dprintf("  No thread info\n");
        dprintf("\n");
    }
    else {
        dprintf("  hLog:                         0x%08x\n", Address);

        do {
            // Get the THREAD_INFO structure
            ZeroMemory(&ThreadInfo, sizeof(THREAD_INFO));
            bReturnValue = ReadMemory((ULONG) ThreadInfoAddress, &ThreadInfo, sizeof(THREAD_INFO), &cbBytesRead);

            if ((FALSE == bReturnValue) || (cbBytesRead != sizeof(THREAD_INFO))) {
                dprintf("    Cannot read 0x%08x bytes at 0x%08x for THREAD_INFO\n", sizeof(THREAD_INFO), ThreadInfoAddress);
                dprintf("\n");

                ThreadInfoAddress = NULL;
            }
            else {
                if ((0 == ThreadId) || (ThreadId == ThreadInfo.dwThreadId)) {
                    dprintf("    pThreadInfo:                0x%08x\n", ThreadInfoAddress);
                    dprintf("      dwThreadId:               0x%08x\n", ThreadInfo.dwThreadId);
                    dprintf("\n");

                    // Get the pointer to the variation element
                    VariationElementAddress = ThreadInfo.pVariationList;

                    if (NULL == VariationElementAddress) {
                        dprintf("      No variation list\n");
                        dprintf("\n");
                    }
                    else {
                        do {
                            // Get the VARIATION_ELEMENT structure
                            ZeroMemory(&VariationElement, sizeof(VARIATION_ELEMENT));
                            bReturnValue = ReadMemory((ULONG) VariationElementAddress, &VariationElement, sizeof(VARIATION_ELEMENT), &cbBytesRead);

                            if ((FALSE == bReturnValue) || (cbBytesRead != sizeof(VARIATION_ELEMENT))) {
                                dprintf("      Cannot read 0x%08x bytes at 0x%08x for VARIATION_ELEMENT\n", sizeof(VARIATION_ELEMENT), VariationElementAddress);
                                dprintf("\n");
                            }
                            else {
                                dprintf("      pVariationElement:        0x%08x\n", VariationElementAddress);

                                if (NULL == VariationElement.lpszVariationName_A) {
                                    dprintf("        lpszVariationName_A:    %s\n", VariationElement.lpszVariationName_A);
                                }
                                else {
                                    dprintf("        lpszVariationName_A:    %s\n", VariationElement.szVariationName_A);
                                }

                                if (NULL == VariationElement.lpszLogString_A) {
                                    dprintf("        lpszLogString_A:        %s\n", VariationElement.lpszLogString_A);
                                }
                                else {
                                    dprintf("        lpszLogString_A:        %s\n", VariationElement.szLogString_A);
                                }

                                dprintf("        dwVariationResult:      0x%08x\n", VariationElement.dwVariationResult);

                                dprintf("\n");
                            }

                            VariationElementAddress = VariationElement.pNextVariationElement;
                        } while (NULL != VariationElementAddress);
                    }
                }

                ThreadInfoAddress = ThreadInfo.pNextThreadInfo;
            }
        } while (NULL != ThreadInfoAddress);

        if ((0 != ThreadId) && (NULL == ThreadInfoAddress)) {
            dprintf("    Thread info not found for dwThreadId 0x%08x\n", ThreadId);
        }
    }
}
