/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    perfbvt_server.cpp

Abstract:

    Perf BVT test server program (runs on win2k)

--*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>

//
// Debug spews
//
#define ASSERT(cond) do { if (!(cond)) DoAssert(__FILE__, __LINE__, #cond); } while(0)
inline VOID DoAssert(CHAR* filename, INT linenum, CHAR* msg) {
    fprintf(stderr, "!!! ASSERTION FAILED: %s(%d) - %d\n", filename, linenum, GetLastError());
    fprintf(stderr, "    %s\n", msg);
    exit(-1);
}

CHAR* programname;
struct sockaddr_in clientaddr;
HANDLE startevent;
HANDLE tcptestthread, udptestthread;

//
// Various port numbers
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

// Create a socket of the specified type and bind it to a particular port
SOCKET CreateSocket(INT type, USHORT port)
{
    SOCKET s = socket(AF_INET, type, 0);

    if (s != INVALID_SOCKET) {
        struct sockaddr_in sockname;
        INT err;

        memset(&sockname, 0, sizeof(sockname));
        sockname.sin_family = AF_INET;
        sockname.sin_port = htons(port);
        err = bind(s, (struct sockaddr*) &sockname, sizeof(sockname));
        if (err != NO_ERROR) {
            closesocket(s);
            s = INVALID_SOCKET;
        }
    }
    return s;
}


//
// Park and Miller pseudo-random number generator
//  x = 48271 * x % 2147483647
//  range: [1..2147483646]
//
ULONG _randseed = 1;
inline VOID Srand(ULONG seed) {
    _randseed = seed;
}

#pragma warning(disable:4035)
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
#pragma warning(default:4035)


//
// TCP test thread
//
DWORD WINAPI TcpTestThread(VOID* param)
{
    SOCKET tcpsock = CreateSocket(SOCK_STREAM, TCP_TEST_PORT);
    ASSERT(tcpsock != INVALID_SOCKET);
    
    INT err = listen(tcpsock, 1);
    ASSERT(err == NO_ERROR);

    SOCKET s = accept(tcpsock, NULL, NULL);
    ASSERT(s != INVALID_SOCKET);

    // Fix random number sequence
    Srand('TCPS');

    // Receive 32MB sent in 4KB blocks
    CHAR buf[MAX_TCP_SEND];
    INT total = TCP_BYTE_COUNT;
    INT count;
    while (total) {
        count = recv(s, buf, MAX_TCP_SEND, 0);
        ASSERT(count > 0);
        total -= count;
    }

    // Send 32MB in 4KB blocks
    total = TCP_BYTE_COUNT;
    while (total) {
        count = min(total, MAX_TCP_SEND);
        count = send(s, buf, count, 0);
        ASSERT(count != SOCKET_ERROR);
        total -= count;
    }

    // Receive 32MB sent in variable size blocks
    total = TCP_BYTE_COUNT;
    while (total) {
        count = recv(s, buf, MAX_TCP_SEND, 0);
        ASSERT(count > 0);
        total -= count;
    }

    // Send 32MB in variable size blocks
    total = TCP_BYTE_COUNT;
    while (total) {
        count = MIN_TCP_SEND + Rand() % (MAX_TCP_SEND-MIN_TCP_SEND+1);
        count = min(count, total);
        count = send(s, buf, count, 0);
        ASSERT(count != SOCKET_ERROR);
        total -= count;
    }

    closesocket(s);
    closesocket(tcpsock);
    return 0;
}


//
// UDP test thread
//
DWORD WINAPI UdpTestThread(VOID* param)
{
    SOCKET udpsock = CreateSocket(SOCK_DGRAM, UDP_TEST_PORT);
    ASSERT(udpsock != INVALID_SOCKET);

    // Wait for the test to start
    WaitForSingleObject(startevent, INFINITE);

    struct sockaddr_in sockname;
    memset(&sockname, 0, sizeof(sockname));
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(UDP_TEST_PORT);
    sockname.sin_addr = clientaddr.sin_addr;

    INT err = connect(udpsock, (struct sockaddr*) &sockname, sizeof(sockname));
    ASSERT(err == NO_ERROR);

    CHAR buf[MAX_UDP_DGRAM];
    memset(buf, 0, sizeof(buf));

    fd_set fdset1;
    fd_set fdset2;
    fd_set* readfds;
    fd_set* writefds;

    UINT sendseq = 0;
    UINT recvseq = 0;

    while (TRUE) {
        if (recvseq < UDP_DGRAM_COUNT-2) {
            FD_ZERO(&fdset1);
            FD_SET(udpsock, &fdset1);
            readfds = &fdset1;
        } else {
            readfds = NULL;
        }

        if (sendseq < UDP_DGRAM_COUNT && recvseq != 0) {
            FD_ZERO(&fdset2);
            FD_SET(udpsock, &fdset2);
            writefds = &fdset2;
        } else {
            writefds = NULL;
        }

        if (!readfds && !writefds) break;
        INT count = select(0, readfds, writefds, NULL, NULL);
        ASSERT(count > 0);

        if (readfds && FD_ISSET(udpsock, readfds)) {
            // Fix random number sequence
            if (recvseq == 0) {
                Srand('UDPS');
            }
            count = recv(udpsock, buf, sizeof(buf), 0);
            if (count >= MIN_UDP_DGRAM) {
                // Assme the other end has already close the socket
                recvseq = UDP_DGRAM_COUNT;
            } else {
                recvseq = *((UINT*) buf);
            }
        }

        if (writefds && FD_ISSET(udpsock, writefds)) {
            count = MIN_UDP_DGRAM + Rand() % (MAX_UDP_DGRAM-MIN_UDP_DGRAM+1);
            *((UINT*) buf) = ++sendseq;
            err = send(udpsock, buf, count, 0);
            ASSERT(err == count);
        }
    }

    Sleep(100);
    closesocket(udpsock);
    return 0;
}


//
// Test selections
//
#define MAX_TEST_OPTION_LINELEN 512
#define MAX_TEST_OPTIONS 16
CHAR* testoptionfile;
CHAR* testoptions[MAX_TEST_OPTIONS];
INT numtestoptions;

inline VOID sendstr(SOCKET s, CHAR* buf)
{
    INT len = strlen(buf) + 1;
    while (len) {
        INT count = send(s, buf, len, 0);
        ASSERT(count != SOCKET_ERROR);
        buf += count;
        len -= count;
    }
}

VOID SendTestSelections(SOCKET s)
{
    for (INT i=0; i < numtestoptions; i++) {
        sendstr(s, testoptions[i]);
    }

    if (testoptionfile) {
        FILE* fin = fopen(testoptionfile, "r");
        ASSERT(fin != NULL);

        CHAR buf[MAX_TEST_OPTION_LINELEN];
        CHAR* p;
        while (fgets(buf, sizeof(buf), fin)) {
            // strip newline and inline comments
            p = buf;
            while (*p && *p != '#' && *p != '\r' && *p != '\n') p++;
            *p = '\0';

            // strip trailing spaces
            p--;
            while (p >= buf && isspace(*p)) *p-- = '\0';

            // strip leading spaces
            p = buf;
            while (*p && isspace(*p)) p++;

            if (*p) { sendstr(s, p); }
        }

        fclose(fin);
    }

    // Send an empty line to indicate the end of selections
    sendstr(s, "");
}


// Print out program usage information and exit
VOID usage()
{
    fprintf(stderr, "usage: %s [options]\n", programname);
    fprintf(stderr,
        "-o output-file\n"
        "   save the test log to the specified file\n"
        "-t test-selection\n"
        "   specify a test selection in the following format:\n"
        "     test-name[,repeat-count[,test-param]]\n"
        "   test-name is the name of the test function\n"
        "   repeat-count specifies how many times the test is run\n"
        "   test-param is a string that'll be passed to the test\n"
        "-i test-selection-file\n"
        "   read test selections from the specified file\n"
        "If there are no -t or -i options, all test functions are run once by default\n");

    exit(1);
}

//
// Main program
//
INT __cdecl main(INT argc, CHAR** argv)
{
    programname = *argv++;
    argc--;

    // Parse command line options
    FILE* fout = NULL;
    while (argc) {
        CHAR* opt = *argv;
        if (*opt != '-' && *opt != '/' || argc < 2) usage();

        switch (opt[1]) {
        case 'o':
            if (fout) usage();
            fout = fopen(argv[1], "wb");
            ASSERT(fout != NULL);
            break;

        case 'i':
            if (testoptionfile) usage();
            testoptionfile = argv[1];
            break;

        case 't':
            if (numtestoptions >= MAX_TEST_OPTIONS) usage();
            testoptions[numtestoptions++] = argv[1];
            break;

        default:
            usage();
        }

        argc -= 2;
        argv += 2;
    }

    // Run all tests by default
    if (!numtestoptions && !testoptionfile)
        testoptions[numtestoptions++] = "ALL";

    // Default output is the console
    if (!fout) fout = stdout;

    // Initialize winsock
    WSADATA wsadata;
    INT err = WSAStartup(WINSOCK_VERSION, &wsadata);
    ASSERT(err == NO_ERROR);

    // Create separate threads for handling TCP and UDP tests
    startevent = CreateEvent(NULL, TRUE, FALSE, NULL);
    ASSERT(startevent != NULL);

    DWORD tid;
    tcptestthread = CreateThread(NULL, 0, TcpTestThread, NULL, 0, &tid);
    ASSERT(tcptestthread != NULL);

    udptestthread = CreateThread(NULL, 0, UdpTestThread, NULL, 0, &tid);
    ASSERT(udptestthread != NULL);

    // Create the server socket 
    // which is used for logging test results.
    SOCKET listensock = CreateSocket(SOCK_STREAM, SERVER_PORT);
    ASSERT(listensock != INVALID_SOCKET);

    fprintf(stderr, "Listening for connection...\n");
    err = listen(listensock, 1);
    ASSERT(err == NO_ERROR);

    INT addrlen = sizeof(clientaddr);
    SOCKET serversock = accept(listensock, (struct sockaddr*) &clientaddr, &addrlen);
    ASSERT(serversock != INVALID_SOCKET);

    fprintf(fout, "Established connection with %s...\n", inet_ntoa(clientaddr.sin_addr));
    SetEvent(startevent);

    // Send test selections to the target machine
    SendTestSelections(serversock);

    while (TRUE) {
        CHAR buf[1024];
        INT count = recv(serversock, buf, sizeof(buf), 0);
        ASSERT(count != SOCKET_ERROR);
        if (count == 0) break;
        fwrite(buf, 1, count, fout);
    }

    SuspendThread(tcptestthread);
    CloseHandle(tcptestthread);
    SuspendThread(udptestthread);
    CloseHandle(udptestthread);
    fprintf(stderr, "Test completed.\n");

    closesocket(serversock);
    closesocket(listensock);
    WSACleanup();
    return 0;
}

