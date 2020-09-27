// Simple DNS client test program

#include "precomp.h"
#include <stdlib.h>

#ifdef TEST_PPP
#include <xppp.h>
#endif // TEST_PPP

INT testFlag = 1;
INT err;


#define BREAK_INTO_DEBUGGER __asm int 3
#define WARNFAIL(_apiname) \
        DbgPrint(#_apiname " failed: %d %d\n", err, GetLastError())


CHAR* testnames[] = {
    "davidxu",
    "davidxu.ntdev.microsoft.com",
    "foobar",
    "www.msn.com."
};

struct {
    BYTE addr[4];
} testaddrs[] = {
    { 157, 56, 11, 44 },
    { 192, 55, 34, 25 }
};


VOID
PrintHostent(
    struct hostent* host
    )
{
    CHAR** p;
    if (host == NULL) {
        DbgPrint("  failed: %d\n", GetLastError());
    } else {
        DbgPrint("  %s\n", host->h_name);
        p = host->h_aliases;
        while (*p) {
            DbgPrint("  %s\n", *p);
            p++;
        }

        p = host->h_addr_list;
        while (*p) {
            DbgPrint("  %s\n", inet_ntoa(*((struct in_addr*) *p)));
            p++;
        }
    }
}

VOID
DnsTest()
{
    INT index, count;
    struct hostent* host;
    CHAR localhost[32];

    err = gethostname(localhost, 32);
    if (err != NO_ERROR) {
        WARNFAIL(gethostname);
    } else {
        DbgPrint("Lookup hostname: %s...\n", localhost);
        host = gethostbyname(localhost);
        PrintHostent(host);
    }

    count = ARRAYCOUNT(testnames);
    for (index=0; index < count; index++) {
        DbgPrint("Lookup hostname: %s...\n", testnames[index]);
        host = gethostbyname(testnames[index]);
        PrintHostent(host);
    }

    count = ARRAYCOUNT(testaddrs);
    for (index=0; index < count; index++) {
        struct in_addr* addr = (struct in_addr*) &testaddrs[index];
        DbgPrint("Lookup address: %s\n", inet_ntoa(*addr));
        host = gethostbyaddr((char*) addr, sizeof(*addr), AF_INET);
        PrintHostent(host);
    }
}


void __cdecl main()
{
    WSADATA wsadata;

    DbgPrint("Loading XBox network stack...\n");
    err = XnetInitialize(NULL, TRUE);
    if (err != NO_ERROR) {
        WARNFAIL(XnetInitialize); goto stop;
    }

#ifdef TEST_PPP
    PppConnect("guest", "password", "", "9-786-1000");
//    PppConnect("004vgorok@spinway", "Ob4seBa5", "", "9-827-3261");
    Sleep(90000);
#endif // TEST_PPP

    err = WSAStartup(WINSOCK_VERSION, &wsadata);
    if (err != NO_ERROR) {
        WARNFAIL(WSAStartup); goto unload;
    }

    DbgPrint("*** Test started, press 'g' to continue...\n");
    BREAK_INTO_DEBUGGER

    while (testFlag > 0) {
        Sleep(1000);
        DnsTest();
        DbgPrint("*** To quit, press CTRL-C and type: ed %x 0;g\n", &testFlag);
        BREAK_INTO_DEBUGGER
    }

    WSACleanup();

unload:
    DbgPrint("Unloading XBox network stack...\n");
    XnetCleanup();

stop:
    if (testFlag == 0)
        HalReturnToFirmware(HalRebootRoutine);
    else
        Sleep(INFINITE);
}

