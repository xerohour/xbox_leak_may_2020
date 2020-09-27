/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    perfbvt.h

Abstract:

    Perf BVT tests - common declarations

--*/

#ifndef _PERFBVT_H
#define _PERFBVT_H

//
// Read the processor timestamp counter in msec
//
extern INT _perfCounterFreq;
inline ULONG ReadTimestamp() {
    LARGE_INTEGER perfCount;
    QueryPerformanceCounter(&perfCount);
    return (ULONG) (perfCount.QuadPart * 1000 / _perfCounterFreq);
}

//
// Park and Miller pseudo-random number generator
//  x = 48271 * x % 2147483647
//  range: [1..2147483646]
//
extern ULONG _randseed;
inline VOID Srand(ULONG seed) {
    _randseed = seed;
}

inline ULONG Rand() {
    __asm {
        mov eax, 48271
        mul _randseed
        mov ebx, 2147483647
        div ebx
        mov _randseed, edx
        mov eax, edx
    }
}

//
// Logging functions
//
VOID ConnectToServer();
VOID DisconnectFromServer();
VOID Print(CHAR*, ...);

inline VOID LogEntry(CHAR* entryname, ULONG result, CHAR* unit=NULL) {
    Print("%30s: %6u.%03u %s\n",
        entryname,
        result / 1000,
        result % 1000,
        unit ? unit : "(seconds)");
}

//
// Debug spew macro
//
extern "C" ULONG _cdecl DbgPrint(CHAR*, ...);
inline VOID DoAssert(CHAR* filename, INT linenum, CHAR* msg) {
    DbgPrint("!!! ASSERTION FAILED: %s(%d) - %d\n", filename, linenum, GetLastError());
    DbgPrint("    %s\n", msg);
    DebugBreak();
}

#define ASSERT(cond) do { if (!(cond)) DoAssert(__FILE__, __LINE__, #cond); } while(0)
#define CHECKHR(expr) do { hr = (expr); ASSERT(SUCCEEDED(hr)); } while (0)

//
// Server IP address and port numbers
//
#define SERVER_PORT     6001
#define TCP_TEST_PORT   6002
#define UDP_TEST_PORT   6003
#define MIN_UDP_DGRAM   4
#define MAX_UDP_DGRAM   128
#define UDP_DGRAM_COUNT (64*1024)
#define MIN_TCP_SEND    1
#define MAX_TCP_SEND    4096
#define TCP_BYTE_COUNT  (32*1024*1024)

//
// Set send/recv timeout to be 10sec
//
inline VOID SetSocketTimeouts(SOCKET s)
{
    UINT timeout = 10000;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (CHAR*) &timeout, sizeof(timeout));
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (CHAR*) &timeout, sizeof(timeout));
}

//
// Allocate and free virtual memory pages
//
inline VOID* valloc(UINT size) {
    return VirtualAlloc(NULL, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
}

inline VOID vfree(VOID* ptr) {
    if (ptr) { VirtualFree(ptr, 0, MEM_RELEASE); }
}


//
// Declare test function prototype
//
typedef ULONG (*TESTPROC)(CHAR*);
#define DECLARE_TEST_FUNCTION(name) ULONG name(CHAR* testparam)

//
// Current test name
//
extern CHAR* curtestname;

//
// D3D device
//
extern IDirect3DDevice8* d3ddev;

#endif // !_PERFBVT_H

