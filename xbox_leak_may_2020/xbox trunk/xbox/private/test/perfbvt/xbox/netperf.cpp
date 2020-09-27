/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    netperf.cpp

Abstract:

    Network related perf BVT tests

--*/

#include "precomp.h"

#ifdef PERFBVT_SERVER

//
// TCP perf tests
//
DECLARE_TEST_FUNCTION(TcpSendReceive)
{
    // Fix random number sequence
    Srand('TCPC');

    DbgPrint("TCP send / recv tests...\n");
    ULONG time1 = ReadTimestamp();

    static CHAR buf[MAX_TCP_SEND];
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT(s != INVALID_SOCKET);

    // Set socket send/recv timeouts
    SetSocketTimeouts(s);

    struct sockaddr_in sockname;
    memset(&sockname, 0, sizeof(sockname));
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(TCP_TEST_PORT);
    sockname.sin_addr.s_addr = inet_addr(PERFBVT_SERVER);

    INT err = connect(s, (struct sockaddr*) &sockname, sizeof(sockname));
    ASSERT(err == NO_ERROR);

    //
    // Send 32MB in 4K blocks
    //
    // Initial results:
    //  win2k: 800Mhz P3, Intel EEPRO100B
    //  Xbox: 733Mhz P3, Intel EEPRO100B
    //
    //  win2k: ~3.3s
    //  xbox: ~2.8s
    //
    INT total = TCP_BYTE_COUNT;
    INT count;
    while (total) {
        count = min(total, MAX_TCP_SEND);
        count = send(s, buf, count, 0);
        ASSERT(count != SOCKET_ERROR);
        total -= count;
    }

    ULONG time2 = ReadTimestamp();

    //
    // Receive 32MB that was sent in 4K blocks
    //  win2k: ~4.1s
    //  xbox: ~2.8s
    //
    total = TCP_BYTE_COUNT;
    while (total) {
        count = recv(s, buf, MAX_TCP_SEND, 0);
        ASSERT(count > 0);
        total -= count;
    }

    ULONG time3 = ReadTimestamp();

    //
    // Send 32MB in variable size blocks
    //  win2k: ~3.5s
    //  xbox: ~2.9s
    //
    total = TCP_BYTE_COUNT;
    while (total) {
        count = MIN_TCP_SEND + Rand() % (MAX_TCP_SEND-MIN_TCP_SEND+1);
        count = min(count, total);
        count = send(s, buf, count, 0);
        ASSERT(count != SOCKET_ERROR);
        total -= count;
    }

    ULONG time4 = ReadTimestamp();

    //
    // Receive 32MB that was sent in variable size blocks
    //  win2k: ~3.8s
    //  xbox: ~2.9s
    //
    total = TCP_BYTE_COUNT;
    while (total) {
        count = recv(s, buf, MAX_TCP_SEND, 0);
        ASSERT(count > 0);
        total -= count;
    }

    closesocket(s);
    ULONG time5 = ReadTimestamp();

    LogEntry("TcpSend4KBlock", time2-time1);
    LogEntry("TcpRecv4KBlock", time3-time2);
    LogEntry("TcpSendVariable", time4-time3);
    LogEntry("TcpRecvVariable", time5-time4);

    return 0;
}


//
// UDP perf tests
//  win2k: 6.9 - 7.1s
//  xbox: 5.8s
//
DECLARE_TEST_FUNCTION(UdpSendReceive)
{
    // Fix random number sequence
    Srand('UDPC');

    DbgPrint("UDP send/recv test...\n", MIN_UDP_DGRAM, MAX_UDP_DGRAM);
    ULONG starttime = ReadTimestamp();

    SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT(s != INVALID_SOCKET);

    struct sockaddr_in sockname;
    memset(&sockname, 0, sizeof(sockname));
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(UDP_TEST_PORT);

    INT err = bind(s, (struct sockaddr*) &sockname, sizeof(sockname));
    ASSERT(err == NO_ERROR);

    sockname.sin_addr.s_addr = inet_addr(PERFBVT_SERVER);
    err = connect(s, (struct sockaddr*) &sockname, sizeof(sockname));
    ASSERT(err == NO_ERROR);

    UINT sendseq = 0;
    UINT recvseq = 0;
    UINT totalsendbytes, totalsenddgrams;
    UINT totalrecvbytes, totalrecvdgrams;

    totalsendbytes = totalsenddgrams = 
    totalrecvbytes = totalrecvdgrams = 0;

    fd_set fdset1;
    fd_set fdset2;
    fd_set* readfds;
    fd_set* writefds;
    static CHAR buf[MAX_UDP_DGRAM];

    while (TRUE) {
        if (sendseq < UDP_DGRAM_COUNT) {
            FD_ZERO(&fdset2);
            FD_SET(s, &fdset2);
            writefds = &fdset2;
        } else {
            writefds = NULL;
        }

        if (recvseq < UDP_DGRAM_COUNT-2) {
            FD_ZERO(&fdset1);
            FD_SET(s, &fdset1);
            readfds = &fdset1;
        } else {
            readfds = NULL;
        }

        if (!readfds && !writefds) break;
        INT count = select(0, readfds, writefds, NULL, NULL);
        ASSERT(count > 0);

        if (writefds && FD_ISSET(s, writefds)) {
            count = MIN_UDP_DGRAM + Rand() % (MAX_UDP_DGRAM-MIN_UDP_DGRAM+1);
            *((UINT*) buf) = ++sendseq;
            err = send(s, buf, count, 0);
            ASSERT(err == count);
            totalsendbytes += count;
            totalsenddgrams++;
        }

        if (readfds && FD_ISSET(s, readfds)) {
            count = recv(s, buf, sizeof(buf), 0);
            ASSERT(count >= MIN_UDP_DGRAM);
            recvseq = *((UINT*) buf);
            totalrecvbytes += count;
            totalrecvdgrams++;
        }
    }

    ULONG stoptime = ReadTimestamp();
    Sleep(100);
    closesocket(s);

    DbgPrint("  send: %d datagrams, %d bytes\n", totalsenddgrams, totalsendbytes);
    DbgPrint("  recv: %d datagrams, %d bytes\n", totalrecvdgrams, totalrecvbytes);
    return stoptime-starttime;
}

#endif // PERFBVT_SERVER
