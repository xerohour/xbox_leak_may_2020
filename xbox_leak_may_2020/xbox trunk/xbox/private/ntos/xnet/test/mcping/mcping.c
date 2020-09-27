// This is a simple program for testing the following functionality:
//  multicast send and receive
//  RAW socket
//  Winsock select API

#include "precomp.h"
#include <stdlib.h>

UINT WINAPI tcpipxsum(UINT xsum, const VOID* buf, UINT buflen);

#define RCVTIMEOUT  3
#define PINGBUFSIZE 2048
#define TESTADDR    "229.0.0.1"

INT testFlag = 2;
INT err;
CHAR databuf[PINGBUFSIZE];
INT rcvlen;
WORD id, seqno;
CHAR pingdata[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";


VOID
SendIcmpMessage(
    SOCKET s,
    struct sockaddr_in* toaddr,
    IcmpMessage* icmpmsg,
    UINT msglen
    )
{
    icmpmsg->checksum = 0;
    icmpmsg->checksum = (WORD) ~tcpipxsum(0, icmpmsg, msglen);

    err = _sendto(s, (CHAR*) icmpmsg, &msglen, toaddr);
    if (err != NO_ERROR) {
        WARNFAIL(sendto);
    }
}


BOOL
RecvIcmpMessage(
    SOCKET s,
    struct sockaddr_in* fromaddr
    )
{
    TIMEVAL timeout;
    fd_set readfds;
    DWORD len;

    // Wait until there is data to be read

    timeout.tv_sec = RCVTIMEOUT;
    timeout.tv_usec = 0;

    FD_ZERO(&readfds);
    FD_SET(s, &readfds);

    err = select(1, &readfds, NULL, NULL, &timeout);
    if (err == SOCKET_ERROR) {
        WARNFAIL(select); return FALSE;
    }

    if (err == 0) return FALSE;

    len = PINGBUFSIZE;
    err = _recvfrom(s, databuf, &len, fromaddr);
    if (err != NO_ERROR) {
        WARNFAIL(recvfrom); return FALSE;
    }

    rcvlen = len;
    return TRUE;
}


VOID
McastPing()
{
    SOCKET sock;
    struct sockaddr_in sockname;
    IcmpMessage* icmpmsg;
    struct in_addr mcastif;
    struct ip_mreq mcastreq;

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock == INVALID_SOCKET) {
        WARNFAIL(socket); return;
    }

    ZeroMem(&sockname, sizeof(sockname));
    sockname.sin_family = AF_INET;
    sockname.sin_port = 0;
    sockname.sin_addr.s_addr = INADDR_ANY;

    err = _bind(sock, &sockname);
    if (err != NO_ERROR) {
        WARNFAIL(bind); goto exit;
    }

    mcastif.s_addr = INADDR_ANY;
    err = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (char*) &mcastif, sizeof(mcastif));
    if (err != NO_ERROR) {
        WARNFAIL(setmcastif); goto exit;
    }

    mcastreq.imr_multiaddr.s_addr = inet_addr(TESTADDR);
    mcastreq.imr_interface.s_addr = INADDR_ANY;
    err = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mcastreq, sizeof(mcastreq));
    if (err != NO_ERROR) {
        WARNFAIL(addmember); goto exit;
    }

    do {
        DbgPrint(".");
        if (testFlag == 2) {
            icmpmsg = (IcmpMessage*) databuf;
            icmpmsg->type = ICMPTYPE_ECHO_REQUEST;
            icmpmsg->code = 0;
            icmpmsg->id = id++;
            icmpmsg->seqno = seqno++;
            CopyMem(icmpmsg->origdata, pingdata, sizeof(pingdata));
            sockname.sin_addr.s_addr = inet_addr(TESTADDR);

            DbgPrint("Sending out ICMP echo request...\n");
            DbgPrint("  id = %d, seqno = %d\n", icmpmsg->id, icmpmsg->seqno);

            SendIcmpMessage(
                sock,
                &sockname,
                icmpmsg,
                ICMPHDRLEN + sizeof(pingdata));

            testFlag = 1;
        }

        if (RecvIcmpMessage(sock, &sockname)) {
            IpHeader* iphdr = (IpHeader*) databuf;
            INT iphdrlen, iplen;

            DbgPrint("Received ICMP message from %s:\n", inet_ntoa(sockname.sin_addr));

            if (rcvlen < IPHDRLEN ||
                (iphdrlen = GETIPHDRLEN(iphdr)) == 0 ||
                iphdrlen > rcvlen ||
                (iplen = GETIPLEN(iphdr)) > rcvlen ||
                iphdrlen > iplen) {
                DbgPrint("  Bad IP header!\n");
                continue;
            }

            rcvlen = iplen - iphdrlen;
            icmpmsg = (IcmpMessage*) &databuf[iphdrlen];
            
            if (rcvlen < ICMPHDRLEN ||
                tcpipxsum(0, icmpmsg, rcvlen) != 0xffff) {
                DbgPrint("  Bad ICMP header!\n");
            }
            
            DbgPrint("  type = %d, code = %d\n", icmpmsg->type, icmpmsg->code);
            DbgPrint("  id = %d, seqno = %d\n", icmpmsg->id, icmpmsg->seqno);
            DbgPrint("  %d bytes of data: %s\n", rcvlen - ICMPHDRLEN, icmpmsg->origdata);

            if (icmpmsg->type == ICMPTYPE_ECHO_REQUEST) {
                DbgPrint("Sending out ICMP echo reply...\n");
                icmpmsg->type = ICMPTYPE_ECHO_REPLY;
                SendIcmpMessage(sock, &sockname, icmpmsg, rcvlen);
            }
        }
    } while (testFlag > 0);

    err = setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*) &mcastreq, sizeof(mcastreq));
    if (err != NO_ERROR) {
        WARNFAIL(dropmember); goto exit;
    }

exit:
    closesocket(sock);
}


void __cdecl main()
{
    WSADATA wsadata;

    DbgPrint("Loading XBox network stack...\n");
    err = XnetInitialize(NULL, TRUE);
    if (err != NO_ERROR) {
        WARNFAIL(XnetInitialize); goto stop;
    }

    err = WSAStartup(WINSOCK_VERSION, &wsadata);
    if (err != NO_ERROR) {
        WARNFAIL(WSAStartup); goto unload;
    }

    DbgPrint("*** Test started, press 'g' to continue...\n");
    DbgPrint("*** To quit, press CTRL-C and type: ed %x 0;g\n", &testFlag);
    BREAK_INTO_DEBUGGER

    id = (WORD) (GetTickCount() >> 16);
    seqno = (WORD) (GetTickCount() & 0xffff);

    McastPing();
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

