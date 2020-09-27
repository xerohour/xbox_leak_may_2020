/*
 * File             :       SendPing.cpp
 * Author           :       Jim Y
 * Revision History
 *      Aug/02/2000   JimY        Created    
 *
 * This file contains code for sending a Ping
 */


#include "precomp.h"

#include <xbttcp.h>
#include <xtl.h>
#include <stdlib.h>
#include <xlog.h>

#define RCVTIMEOUT  3
#define PINGBUFSIZE 2048
#define BuffLen 200
#define UNICAST 0
#define MULTICAST 1
#define IPADDRESSLENGTH 16
#define PASS 0
#define FAIL 1

//
// Functions exported by XNET.DLL
//

INT WSAAPI XnetInitialize(const XnetInitParams*, BOOL);
INT WSAAPI XnetCleanup(VOID);

//
// Functions local to this module
//
INT Ping(HANDLE, INT, CHAR *);
VOID SendIcmpMessage(SOCKET, struct sockaddr_in*, IcmpMessage*, INT);
BOOL RecvIcmpMessage(SOCKET, struct sockaddr_in*);



CHAR TESTADDR[IPADDRESSLENGTH];
CHAR UCASTTESTADDR[IPADDRESSLENGTH];   


INT err, result;
CHAR databuf[PINGBUFSIZE];
INT rcvlen;
WORD id, seqno;
//CHAR pingdata[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
CHAR pingdata[] = "XBOXPING";

char szBuffer[BuffLen];
char iniBuffer[BuffLen];

SOCKET sock;
struct sockaddr_in sockname;
IcmpMessage* icmpmsg;
struct in_addr mcastif;
//struct in_addr ucastreq;  //unicast
struct ip_mreq mcastreq;  //multicast



#define BREAK_INTO_DEBUGGER __asm int 3
#define WARNFAIL(_apiname) \
        DbgPrint(#_apiname " failed: %d %d\n", err, GetLastError())


__inline _bind(SOCKET s, struct sockaddr_in* addr) {
    return bind(s, (struct sockaddr*) addr, sizeof(*addr));
}

__inline INT _sendto(SOCKET s, CHAR* buf, DWORD buflen, struct sockaddr_in* to) {
    WSABUF wsabuf = { buflen, buf };
    return WSASendTo(s, &wsabuf, 1, &buflen, 0, (struct sockaddr*) to, sizeof(*to), 0, 0);
}

__inline INT _recvfrom(SOCKET s, CHAR* buf, DWORD* buflen, struct sockaddr_in* from) {
    WSABUF wsabuf = { *buflen, buf };
    INT fromlen = sizeof(*from);
    DWORD flags = 0;
    return WSARecvFrom(s, &wsabuf, 1, buflen, &flags, (struct sockaddr*) from, &fromlen, 0, 0);
}


VOID
SendIcmpMessage(
    SOCKET s,
    struct sockaddr_in* toaddr,
    IcmpMessage* icmpmsg,
    INT msglen
    )
{
    icmpmsg->checksum = 0;
    icmpmsg->checksum = (WORD) ~tcpipxsum(0, icmpmsg, msglen);

    err = _sendto(s, (CHAR*) icmpmsg, msglen, toaddr);
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




INT
Ping(HANDLE hLog, INT AddressType, CHAR * TESTADDR)
{
    SOCKET sock;
    struct sockaddr_in sockname;
    IcmpMessage* icmpmsg;
    struct in_addr mcastif;
//    struct in_addr ucastreq;  //unicast
    struct ip_mreq mcastreq;  //multicast
    INT result = FAIL;

    CHAR ActualIP[IPADDRESSLENGTH];
    CHAR ExpectedIP[IPADDRESSLENGTH];   




    DbgPrint("SendPing: Inside Ping()...\n");

    ZeroMem(&ActualIP, IPADDRESSLENGTH);
    ZeroMem(&ExpectedIP, IPADDRESSLENGTH);

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock == INVALID_SOCKET) {
        WARNFAIL(socket); return result;
    }

    ZeroMem(&sockname, sizeof(sockname));
    sockname.sin_family = AF_INET;
    sockname.sin_port = 0;
    sockname.sin_addr.s_addr = INADDR_ANY;

    err = _bind(sock, &sockname);
    if (err != NO_ERROR) {
        WARNFAIL(bind); goto exit;
    }


    //
    // If a Multicast ping
    //

    if (AddressType == MULTICAST)  {

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
     }



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
            }

            rcvlen = iplen - iphdrlen;
            icmpmsg = (IcmpMessage*) &databuf[iphdrlen];
            
            if (rcvlen < ICMPHDRLEN ||
                tcpipxsum(0, icmpmsg, rcvlen) != 0xffff) {
                DbgPrint("  Bad ICMP header!\n");
            }
            
//            DbgPrint("  type = %d, code = %d\n", icmpmsg->type, icmpmsg->code);
//            DbgPrint("  id = %d, seqno = %d\n", icmpmsg->id, icmpmsg->seqno);
//            DbgPrint("  %d bytes of data: %s\n", rcvlen - ICMPHDRLEN, icmpmsg->origdata);

            //
            // These conditions must be met for the PING to be declared PASS 
            //
            if (AddressType == MULTICAST)  {
            // gethostname()
            // strcpy(ExpectedIP, gethostbyname());
            }
            if (AddressType == UNICAST)  {
                strcpy(ExpectedIP, TESTADDR);
            }

            strcpy(ActualIP, inet_ntoa(sockname.sin_addr));
            strcpy(ExpectedIP, TESTADDR);
            DbgPrint("  ActualIP = [%s]\n", ActualIP);
            DbgPrint("  ExpectedIP = [%s]\n", ExpectedIP);
            DbgPrint("  strstr = %d\n", strstr(ActualIP, ExpectedIP) );
            if (strstr(ActualIP, ExpectedIP))  {
                xLog(hLog, XLL_INFO, "ActualIP matches ExpectedIP");
                DbgPrint("SendPing: ActualIP matches ExpectedIP\n");

                if ((icmpmsg->seqno == seqno) && (icmpmsg->type = ICMPTYPE_ECHO_REPLY))  {
                    result = PASS;
                }
                else  {
                    xLog(hLog, XLL_INFO, "SequenceNumber or MessageType is not what's expected");                 
                    DbgPrint("  type = %d, code = %d\n", icmpmsg->type, icmpmsg->code);
                    DbgPrint("  id = %d, seqno = %d\n", icmpmsg->id, icmpmsg->seqno);
                    DbgPrint("  %d bytes of data: %s\n", rcvlen - ICMPHDRLEN, icmpmsg->origdata);
                }
            }
            else {
                xLog(hLog, XLL_INFO, "ActualIP does NOT match ExpectedIP");
                DbgPrint("SendPing: ActualIP does NOT match ExpectedIP\n");
            }


            if (icmpmsg->type == ICMPTYPE_ECHO_REQUEST) {
                DbgPrint("Sending out ICMP echo reply...\n");
                icmpmsg->type = ICMPTYPE_ECHO_REPLY;
                SendIcmpMessage(sock, &sockname, icmpmsg, rcvlen);
            }

        }


    //
    // If a Multicast ping
    //
    if (AddressType == MULTICAST)  {
        err = setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*) &mcastreq, sizeof(mcastreq));
        if (err != NO_ERROR) {
            WARNFAIL(dropmember); goto exit;
        }
    }



exit:
    closesocket(sock);
    return result; 
}



INT xbtUCastPing(CHAR *TESTADDR, UINT *Offset, CHAR *mydatabuf, UINT datalength, CHAR *ResponseIP,  CHAR *mypingmessage, UINT mypingmessagelength)
{

    INT iphdrlen, iplen;
    INT result = FAIL;


    DbgPrint("xbttcp: In xbtUCastPing...\n");
    DbgPrint("xbttcp: mypingmessge = [%s]\n", mypingmessage);
    *Offset = 0;

    // 
    // Create the Socket
    //

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock == INVALID_SOCKET) {
        WARNFAIL(socket); goto exit;
    }


    //
    // Set some values in the socket struct
    // 

    ZeroMem(&sockname, sizeof(sockname));
    sockname.sin_family = AF_INET;
    sockname.sin_port = 0;
    sockname.sin_addr.s_addr = INADDR_ANY;

 
    //
    // Bind the socket
    // 

    err = _bind(sock, &sockname);
    if (err != NO_ERROR) {
        WARNFAIL(bind); goto exit;
    }


    // 
    // Fill in ICMP values to be sent out
    //

    icmpmsg = (IcmpMessage*) databuf;
    icmpmsg->type = ICMPTYPE_ECHO_REQUEST;
    icmpmsg->code = 0;
    icmpmsg->id = (WORD) (GetTickCount() >> 16);
//    icmpmsg->id = id++;
    icmpmsg->seqno = (WORD) (GetTickCount() & 0xffff);
//    icmpmsg->seqno = seqno++;
//    CopyMem(icmpmsg->origdata, pingdata, sizeof(pingdata));
    CopyMem(icmpmsg->origdata, mypingmessage, strlen(mypingmessage));
    sockname.sin_addr.s_addr = inet_addr(TESTADDR);


    //
    // Send a PING request (ECHO_REQUEST)
    // 
    DbgPrint("xbttcp: Sending out ICMP echo request...\n");
    DbgPrint("xbttcp:   id = %d, seqno = %d\n", icmpmsg->id, icmpmsg->seqno);

    SendIcmpMessage(sock, &sockname, icmpmsg, ICMPHDRLEN + strlen(mypingmessage));
//    SendIcmpMessage(sock, &sockname, icmpmsg, ICMPHDRLEN + sizeof(pingdata));


    //
    // Receive any ICMP message (response)
    //

    if (RecvIcmpMessage(sock, &sockname)) 
    {

        IpHeader* iphdr = (IpHeader*) databuf;

        ZeroMem(ResponseIP, IPADDRESSLENGTH);
        strcpy(ResponseIP, inet_ntoa(sockname.sin_addr));
        DbgPrint("xbttcp: Received ICMP message from %s:\n", ResponseIP);

        if (rcvlen < IPHDRLEN || (iphdrlen = GETIPHDRLEN(iphdr)) == 0 || iphdrlen > rcvlen || (iplen = GETIPLEN(iphdr)) > rcvlen || iphdrlen > iplen) 
        {
            DbgPrint("xbttcp:  Bad IP header!\n");
        }

        rcvlen = iplen - iphdrlen;
        icmpmsg = (IcmpMessage*) &databuf[iphdrlen];
            
        if (rcvlen < ICMPHDRLEN || tcpipxsum(0, icmpmsg, rcvlen) != 0xffff) 
        {
            DbgPrint("xbttcp:  Bad ICMP header!\n");
        }

        *Offset = iphdrlen;
        CopyMem(mydatabuf, databuf, datalength);
            
        DbgPrint("xbttcp:  type = %d, code = %d\n", icmpmsg->type, icmpmsg->code);
        DbgPrint("xbttcp:  id = %d, seqno = %d\n", icmpmsg->id, icmpmsg->seqno);
        DbgPrint("xbttcp:  %d bytes of data: %s\n", rcvlen - ICMPHDRLEN, icmpmsg->origdata);
        DbgPrint("xbttcp:  Offset = %d\n", *Offset);

        result = PASS;

   }
   //
   // If we don't receive a ICMP response (IP address is not in use)    
   //
   else
   {
        DbgPrint("xbttcp: Did not receive an ICMP reply from %s:\n", *TESTADDR);
        ZeroMem(ResponseIP, IPADDRESSLENGTH);
        strcpy(ResponseIP, "0.0.0.0");
        DbgPrint("xbttcp: Setting status to FAIL and IP to %s:\n", ResponseIP);

        result = FAIL;

   }


exit:
    closesocket(sock);
    return result;

}



//
//
//
INT xbtMCastPing(CHAR *TESTADDR, UINT *Offset, CHAR *mydatabuf, UINT datalength, CHAR *ResponseIP,  CHAR *mypingmessage, UINT mypingmessagelength)
{

    INT iphdrlen, iplen;
    INT result = FAIL;

    DbgPrint("xbttcp: In xbtMCastPing...\n");

    DbgPrint("xbttcp: mypingmessge = [%s]\n", mypingmessage);
    *Offset = 0;

    // 
    // Create the Socket
    //

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock == INVALID_SOCKET) {
        WARNFAIL(socket); goto exit;
    }


    //
    // Set some values in the socket struct
    // 

    ZeroMem(&sockname, sizeof(sockname));
    sockname.sin_family = AF_INET;
    sockname.sin_port = 0;
    sockname.sin_addr.s_addr = INADDR_ANY;

 
    //
    // Bind the socket
    // 

    err = _bind(sock, &sockname);
    if (err != NO_ERROR) {
        WARNFAIL(bind); goto exit;
    }


    //
    // Set Multicast option
    //
    mcastif.s_addr = INADDR_ANY;
    err = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (char*) &mcastif, sizeof(mcastif));
    if (err != NO_ERROR) {
        WARNFAIL(setmcastif); goto exit;
    }

    //
    // Set Multicast option
    //
    mcastreq.imr_multiaddr.s_addr = inet_addr(TESTADDR);
    mcastreq.imr_interface.s_addr = INADDR_ANY;
    err = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mcastreq, sizeof(mcastreq));
    if (err != NO_ERROR) {
        WARNFAIL(addmember); goto exit;
    }



    // 
    // Fill in ICMP values to be sent out
    //

    icmpmsg = (IcmpMessage*) databuf;
    icmpmsg->type = ICMPTYPE_ECHO_REQUEST;
    icmpmsg->code = 0;
    icmpmsg->id = (WORD) (GetTickCount() >> 16);
    icmpmsg->seqno = (WORD) (GetTickCount() & 0xffff);
    CopyMem(icmpmsg->origdata, mypingmessage, strlen(mypingmessage));
    sockname.sin_addr.s_addr = inet_addr(TESTADDR);


    //
    // Send a PING request (ECHO_REQUEST)
    // 
    DbgPrint("xbttcp: Sending out ICMP echo request...\n");
    DbgPrint("xbttcp:   id = %d, seqno = %d\n", icmpmsg->id, icmpmsg->seqno);

    SendIcmpMessage(sock, &sockname, icmpmsg, ICMPHDRLEN + strlen(mypingmessage));


    //
    // Receive any ICMP message (response)
    //

    if (RecvIcmpMessage(sock, &sockname)) 
    {

        IpHeader* iphdr = (IpHeader*) databuf;

        ZeroMem(ResponseIP, IPADDRESSLENGTH);
        strcpy(ResponseIP, inet_ntoa(sockname.sin_addr));
        DbgPrint("xbttcp: Received ICMP message from %s:\n", ResponseIP);

        if (rcvlen < IPHDRLEN || (iphdrlen = GETIPHDRLEN(iphdr)) == 0 || iphdrlen > rcvlen || (iplen = GETIPLEN(iphdr)) > rcvlen || iphdrlen > iplen) 
        {
            DbgPrint("xbttcp:  Bad IP header!\n");
        }

        rcvlen = iplen - iphdrlen;
        icmpmsg = (IcmpMessage*) &databuf[iphdrlen];
            
        if (rcvlen < ICMPHDRLEN || tcpipxsum(0, icmpmsg, rcvlen) != 0xffff) 
        {
            DbgPrint("xbttcp:  Bad ICMP header!\n");
        }

        *Offset = iphdrlen;
        CopyMem(mydatabuf, databuf, datalength);
            
        DbgPrint("xbttcp:  type = %d, code = %d\n", icmpmsg->type, icmpmsg->code);
        DbgPrint("xbttcp:  id = %d, seqno = %d\n", icmpmsg->id, icmpmsg->seqno);
        DbgPrint("xbttcp:  %d bytes of data: %s\n", rcvlen - ICMPHDRLEN, icmpmsg->origdata);
        DbgPrint("xbttcp:  Offset = %d\n", *Offset);

        result = PASS;

    }


    //
    // If a Multicast ping
    //
    err = setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*) &mcastreq, sizeof(mcastreq));
    if (err != NO_ERROR) {
        WARNFAIL(dropmember); goto exit;
    }


exit:
    closesocket(sock);
    return result;




}
