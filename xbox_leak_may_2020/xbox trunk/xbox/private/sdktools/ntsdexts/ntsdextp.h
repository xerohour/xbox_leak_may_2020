/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    ntsdextp.h

Abstract:

    Common header file for NTSDEXTS component source files.

Author:

    Steve Wood (stevewo) 21-Feb-1995

Revision History:

--*/

#include <ntos.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
//#include <ntsdexts.h>

#define NOEXTAPI
#include <wdbgexts.h>
#undef DECLARE_API

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <heap.h>
#include <atom.h>
#include <stktrace.h>
#include <winsock2.h>
#include <lmerr.h>

#include <ntcsrsrv.h>

#define move(dst, src)\
try {\
    ReadMemory((LPVOID) (src), &(dst), sizeof(dst), NULL);\
} except (EXCEPTION_EXECUTE_HANDLER) {\
    return;\
}
#define moveBlock(dst, src, size)\
try {\
    ReadMemory((LPVOID) (src), &(dst), (size), NULL);\
} except (EXCEPTION_EXECUTE_HANDLER) {\
    return;\
}

CHAR igrepLastPattern[256];
DWORD_PTR igrepSearchStartAddress;
DWORD_PTR igrepLastPc;

#ifdef __cplusplus
#define CPPMOD extern "C"
#else
#define CPPMOD
#endif

#define DECLARE_API(s)                          \
    CPPMOD VOID                                 \
    s(                                          \
        HANDLE hCurrentProcess,                 \
        HANDLE hCurrentThread,                  \
        DWORD_PTR dwCurrentPc,                  \
        PWINDBG_EXTENSION_APIS lpExtensionApis, \
        LPSTR lpArgumentString                  \
     )

#define INIT_API() {                            \
    ExtensionApis = *lpExtensionApis;           \
    ExtensionCurrentProcess = hCurrentProcess;  \
    }

#define dprintf                 (ExtensionApis.lpOutputRoutine)
#define GetExpression           (ExtensionApis.lpGetExpressionRoutine)
#define GetSymbol               (ExtensionApis.lpGetSymbolRoutine)
#define Disasm                  (ExtensionApis.lpDisasmRoutine)
#define CheckControlC           (ExtensionApis.lpCheckControlCRoutine)
#define ReadMemory(a,b,c,d) \
    ((ExtensionApis.nSize == sizeof(WINDBG_OLD_EXTENSION_APIS)) ? \
    ReadProcessMemory( ExtensionCurrentProcess, (LPCVOID)(a), (b), (c), (d) ) \
  : ExtensionApis.lpReadProcessMemoryRoutine( (ULONG_PTR)(a), (b), (c), (d) ))

#define WriteMemory(a,b,c,d) \
    ((ExtensionApis.nSize == sizeof(WINDBG_OLD_EXTENSION_APIS)) ? \
    WriteProcessMemory( ExtensionCurrentProcess, (LPVOID)(a), (LPVOID)(b), (c), (d) ) \
  : ExtensionApis.lpWriteProcessMemoryRoutine( (ULONG_PTR)(a), (LPVOID)(b), (c), (d) ))

#define Ioctl                   (ExtensionApis.lpIoctlRoutine)

#ifndef malloc
#define malloc( n ) HeapAlloc( GetProcessHeap(), 0, (n) )
#endif
#ifndef free
#define free( p ) HeapFree( GetProcessHeap(), 0, (p) )
#endif

extern WINDBG_EXTENSION_APIS ExtensionApis;
extern HANDLE ExtensionCurrentProcess;

__inline VOID
GetTebAddress(
    PULONGLONG Address
    )
{
    GET_TEB_ADDRESS gpt;
    gpt.Address = 0;
    Ioctl(IG_GET_TEB_ADDRESS, (PVOID)&gpt, sizeof(gpt));
    *Address = gpt.Address;
}

