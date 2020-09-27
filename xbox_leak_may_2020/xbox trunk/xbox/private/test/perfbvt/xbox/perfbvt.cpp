/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    perfbvt.cpp

Abstract:

    Perf BVT tests - main program

--*/

#include "precomp.h"

//
// Table of available tests
//
struct TestSelection {
    CHAR* testname;
    TESTPROC testproc;
    INT testcount;
    CHAR* testparam;
};


DECLARE_TEST_FUNCTION(MemoryTests);
DECLARE_TEST_FUNCTION(SetFileCacheSize);
DECLARE_TEST_FUNCTION(DiskWriteVariable);
DECLARE_TEST_FUNCTION(DiskWrite8KBlock);
DECLARE_TEST_FUNCTION(DiskReadSeq32KBlock);
DECLARE_TEST_FUNCTION(DiskReadSeqVariable);
DECLARE_TEST_FUNCTION(DiskReadRandomAccess);
DECLARE_TEST_FUNCTION(FileAllocation);
DECLARE_TEST_FUNCTION(CreateDirAndFiles);
DECLARE_TEST_FUNCTION(EnumDirAndReadFiles);
DECLARE_TEST_FUNCTION(DeleteDirAndFiles);
DECLARE_TEST_FUNCTION(DvdStreamingTest);

DECLARE_TEST_FUNCTION(TcpSendReceive);
DECLARE_TEST_FUNCTION(UdpSendReceive);

DECLARE_TEST_FUNCTION(TriCTest);

#define NUMTESTS (sizeof(alltests) / sizeof(alltests[0]))
#define DEFINE_TEST_ENTRY(name) { #name, name, },
TestSelection alltests[] = {
    DEFINE_TEST_ENTRY(MemoryTests)
    DEFINE_TEST_ENTRY(SetFileCacheSize)
    DEFINE_TEST_ENTRY(DiskWriteVariable)
    DEFINE_TEST_ENTRY(DiskWrite8KBlock)
    DEFINE_TEST_ENTRY(DiskReadSeq32KBlock)
    DEFINE_TEST_ENTRY(DiskReadSeqVariable)
    DEFINE_TEST_ENTRY(DiskReadRandomAccess)
    DEFINE_TEST_ENTRY(FileAllocation)
    DEFINE_TEST_ENTRY(CreateDirAndFiles)
    DEFINE_TEST_ENTRY(EnumDirAndReadFiles)
    DEFINE_TEST_ENTRY(DeleteDirAndFiles)
    DEFINE_TEST_ENTRY(DvdStreamingTest)

    #ifdef PERFBVT_SERVER
    DEFINE_TEST_ENTRY(TcpSendReceive)
    DEFINE_TEST_ENTRY(UdpSendReceive)
    #endif

    DEFINE_TEST_ENTRY(TriCTest)
};


ULONG _randseed = 1;        // Pseudo-random number seed
INT _perfCounterFreq;       // performance counter frequency
CHAR* curtestname;

SOCKET serversock;


//
// Select a specified test function
//
VOID SelectTest(CHAR* testname, INT testcount, CHAR* testparam)
{
    // Special case
    BOOL selectall = (strcmp(testname, "ALL") == 0);

    TestSelection* cur = alltests;
    TestSelection* end = cur + NUMTESTS;
    do {
        if (selectall || strcmp(testname, cur->testname) == 0) {
            cur->testcount = testcount;
            cur->testparam = testparam;
            if (!selectall) break;
        }
    } while (++cur < end);
}


//
// Receive test selections from the server
//
#define MAX_TEST_OPTION_LINELEN 512

VOID ReadTestSelections()
{
    CHAR buf[MAX_TEST_OPTION_LINELEN];
    CHAR* cur = buf;
    CHAR* end = cur + MAX_TEST_OPTION_LINELEN;

    while (TRUE) {
        INT count = end - cur;
        count = recv(serversock, cur, count, 0);
        ASSERT(count > 0);

        // Find the last nul-terminator in the buffer
        cur += count;
        CHAR* q = cur;
        while (q != buf && q[-1]) q--;

        if (q == buf) {
            // No nul-terminator is found.
            // The line buffer must not be full yet.
            ASSERT(cur < end);
            continue;
        }

        CHAR* testname = buf;
        CHAR* testcount;
        CHAR* testparam;

        while (testname != q) {
            // Empty line is the terminating condition
            if (*testname == '\0') return;

            CHAR* p = testname + strlen(testname) + 1;
            testparam = NULL;
            testcount = strchr(testname, ',');
            if (testcount) {
                *testcount++ = '\0';
                testparam = strchr(testcount, ',');
                if (testparam) *testparam++ = '\0';
            }

            INT n = testcount ? atoi(testcount) : 1;
            DbgPrint("Test selected: %s %d %s\n",
                testname,
                n,
                testparam ? testparam : "<null>");

            // Make a copy of the testparam string
            if (testparam) {
                testparam = _strdup(testparam);
                ASSERT(testparam != NULL);
            }

            SelectTest(testname, n, testparam);
            testname = p;
        }
        
        // Move the partial line leftover to the beginning of the buffer
        INT leftover = cur-q;
        if (leftover) {
            memmove(buf, q, leftover);
        }
        cur = &buf[leftover];
    }
}


//
// Connect to the server host
//
VOID ConnectToServer()
{
    #ifdef PERFBVT_SERVER

    DbgPrint("Initializing the network stack...\n");

    XNetStartupParams xnetparams = { sizeof(xnetparams), XNET_STARTUP_BYPASS_SECURITY };
    INT err = XNetStartup(&xnetparams);
    ASSERT(err == NO_ERROR);

    WSADATA wsadata;
    err = WSAStartup(WINSOCK_VERSION, &wsadata);
    ASSERT(err == NO_ERROR);

    struct sockaddr_in sockname;
    memset(&sockname, 0, sizeof(sockname));
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(SERVER_PORT);
    sockname.sin_addr.s_addr = inet_addr(PERFBVT_SERVER);

    CHAR buf[32];
    err = XNetInAddrToString(sockname.sin_addr, buf, sizeof(buf));
    ASSERT(err == NO_ERROR);
    DbgPrint("Connecting to server %s:%d...\n", buf, SERVER_PORT);

    serversock = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT(serversock != INVALID_SOCKET);

    // Keep trying to connect to the server until successful
    do {
        err = connect(serversock, (struct sockaddr*) &sockname, sizeof(sockname));
        if (err != NO_ERROR) {
            DbgPrint("Failed to connect to the server, error %d\n", WSAGetLastError());
            Sleep(5000);
        }
    } while (err != NO_ERROR);

    SetSocketTimeouts(serversock);

    // Read test selection data from the server
    ReadTestSelections();

    #else // !PERFBVT_SERVER

    // When there is no server involved, just run
    // all tests once with the default parameters.
    SelectTest("ALL", 1, NULL);

    #endif // !PERFBVT_SERVER
}


//
// Disconnect from the server
//
VOID DisconnectFromServer()
{
    #ifdef PERFBVT_SERVER
    closesocket(serversock);
    WSACleanup();
    XNetCleanup();
    #endif
}

//
// Spew out a message in the log
//
VOID Print(CHAR* format, ...)
{
    static CHAR outbuf[1024];

    va_list args;
    va_start(args, format);
    vsprintf(outbuf, format, args);
    va_end(args);

    DbgPrint("%s", outbuf);

    #ifdef PERFBVT_SERVER

    INT len = strlen(outbuf);
    CHAR* p = outbuf;
    while (len) {
        INT n = send(serversock, p, len, 0);
        ASSERT(n != SOCKET_ERROR);
        p += n;
        len -= n;
    }

    #endif // PERFBVT_SERVER
}


//
// One-time D3D initialization and cleanup
//
IDirect3DDevice8* d3ddev;

VOID D3DInitialize()
{
    ULONG time = ReadTimestamp();

    IDirect3D8* d3d = Direct3DCreate8(D3D_SDK_VERSION);
    ASSERT(d3d != NULL);

    D3DPRESENT_PARAMETERS d3dpp;
    HRESULT hr;

    memset(&d3dpp, 0, sizeof(d3dpp));
    d3dpp.BackBufferWidth                 = 640;
    d3dpp.BackBufferHeight                = 480;
    d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount                 = 1;
    d3dpp.Windowed                        = FALSE;
    d3dpp.EnableAutoDepthStencil          = TRUE;
    d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
    d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                   = NULL;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    CHECKHR(d3d->CreateDevice(
                D3DADAPTER_DEFAULT,
                D3DDEVTYPE_HAL,
                NULL,
                D3DCREATE_HARDWARE_VERTEXPROCESSING,
                &d3dpp,
                &d3ddev));

    d3d->Release();

    CHECKHR(d3ddev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000080, 1.0, 0));
    CHECKHR(d3ddev->Present(NULL, NULL, NULL, NULL));
    d3ddev->BlockUntilIdle();

    time = ReadTimestamp() - time;
    LogEntry("D3DInitialize", time);
}


//
// Main entrypoint
//
VOID __cdecl main()
{
    LARGE_INTEGER freq;
    BOOL ok = QueryPerformanceFrequency(&freq);
    _perfCounterFreq = freq.LowPart;
    ASSERT(ok && freq.HighPart == 0 && _perfCounterFreq > 0);

    // Remember the startup time.
    // This gives us an idea of how long the boot process took.
    ULONG startup_time = ReadTimestamp();
    DbgPrint("Starting perf BVT tests...\n");

    // Establish connect to the server host
    ConnectToServer();

    LogEntry("Start-up time", startup_time);

    // D3D initialization
    D3DInitialize();

    // Run various tests
    TestSelection* cur = alltests;
    TestSelection* end = cur + NUMTESTS;
    while (cur < end) {
        INT count = cur->testcount;
        while (count--) {
            curtestname = cur->testname;
            ULONG result = (*cur->testproc)(cur->testparam);
            if (result) {
                LogEntry(curtestname, result);
            }
        }
        cur++;
    }

    // Disconnect from the server
    DisconnectFromServer();

    DbgPrint("Perf BVT tests completed...\n");
    Sleep(INFINITE);
}

