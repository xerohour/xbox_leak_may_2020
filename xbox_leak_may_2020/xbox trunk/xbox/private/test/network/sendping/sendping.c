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
#include <xtestlib.h>

LPVOID lpHeapMemory = NULL;
HANDLE SendPingHeapHandle;

volatile BOOL quitFlag = FALSE;

#define RCVTIMEOUT  3
#define PINGBUFSIZE 2048
#define BuffLen 200
#define UNICAST 0
#define MULTICAST 1
#define IPADDRESSLENGTH 16
#define PASS 0
#define FAIL 1


//
// Functions local to this module
//
INT  OldPing(HANDLE, INT, CHAR *);
VOID SendIcmpMessage(SOCKET, struct sockaddr_in*, IcmpMessage*, INT);
BOOL RecvIcmpMessage(SOCKET, struct sockaddr_in*);
INT  Ping(HANDLE, INT, CHAR *);
INT  PingLoopback(HANDLE);
INT  PingUnicast(HANDLE, CHAR *);
INT  PingMulticast(HANDLE, CHAR *);

CHAR TESTADDR[IPADDRESSLENGTH];
CHAR UCASTTESTADDR[IPADDRESSLENGTH];


INT err, result, AddressType = 0;
CHAR databuf[PINGBUFSIZE];
INT rcvlen;
WORD id, seqno;

char szBuffer[BuffLen];
char iniBuffer[BuffLen];


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



/*
 * Main thread for SendPing
 *
 *
 */
void DoSendPing(HANDLE hLog) {

    int x = 0;
    int length = 0;
    WSADATA wsadata;


    OutputDebugString(L"SendPing: Loading XBox network stack...\n");
    err = XnetInitialize(NULL, TRUE);
    if (!NT_SUCCESS(err)) {
        OutputDebugString(L"SendPing: XNetInitialize failed...\n");
        WARNFAIL(XnetInitialize); goto stop;
    }


    Sleep(2000);
    err = WSAStartup(WINSOCK_VERSION, &wsadata);
    if (err != NO_ERROR) {
        OutputDebugString(L"SendPing: WSAStartup failed...\n");
        WARNFAIL(WSAStartup); goto unload;
    }

    id = (WORD) (GetTickCount() >> 16);
    seqno = (WORD) (GetTickCount() & 0xffff);


    //
    // Test 1: Ping Loopback of 127.0.0.1
    //
    DbgPrint("SendPing: *****************\n");
    DbgPrint("SendPing: * Test 1        *\n");
    DbgPrint("SendPing: *****************\n");
    result = PingLoopback(hLog);
    if (result != PASS)
        xLog(hLog, XLL_FAIL, "The ping failed.");
    else
        xLog(hLog, XLL_PASS, "The Ping succeeded.");

    xEndVariation( hLog );



    //
    // Test 2: Unicast ping
    //

    DbgPrint("SendPing: *****************\n");
    DbgPrint("SendPing: * Test 2        *\n");
    DbgPrint("SendPing: *****************\n");

    ZeroMem(&iniBuffer,BuffLen);
    ZeroMem(&TESTADDR,IPADDRESSLENGTH);
    GetProfileStringA("sendping", "IpToPing", "127.0.0.1", iniBuffer, IPADDRESSLENGTH);
    IniRemoveStringCommentA( iniBuffer );
    xSetComponent(hLog, "Network", "ICMP");
    xSetFunctionName( hLog, "PING");
    xStartVariation( hLog, "Ping a valid address");

    DbgPrint("SendPing:  iniBuffer = [%s]\n", iniBuffer);
    strcpy(TESTADDR, iniBuffer);
    DbgPrint("SendPing:  TESTADDR = [%s]\n", TESTADDR);
//    DbgPrint("SendPing:  Run Ping Test, press 'g' to continue...\n");
//    BREAK_INTO_DEBUGGER

//
//  New code !
//
    result = PingUnicast(hLog, iniBuffer);
//    result = Ping(hLog, AddressType, TESTADDR);
    if (result != PASS)
        xLog(hLog, XLL_FAIL, "The ping failed.");
    else
        xLog(hLog, XLL_PASS, "The Ping succeeded.");

    xEndVariation( hLog );



    //
    // Test 3: multicast ping
    //

    DbgPrint("SendPing: *****************\n");
    DbgPrint("SendPing: * Test 3        *\n");
    DbgPrint("SendPing: *****************\n");

    ZeroMem(&iniBuffer,BuffLen);
    ZeroMem(&TESTADDR, IPADDRESSLENGTH);
    GetProfileStringA("sendping", "MCastIpToPing", "229.0.0.1", iniBuffer, IPADDRESSLENGTH);
    IniRemoveStringCommentA( iniBuffer );
    AddressType = MULTICAST;
    xSetComponent(hLog, "Network", "ICMP");
    xSetFunctionName( hLog, "PING");
    xStartVariation( hLog, "Ping a valid address");

    DbgPrint("SendPing:  iniBuffer = [%s]\n", iniBuffer);
    strcpy(TESTADDR, iniBuffer);
    xStartVariation( hLog, "Ping a valid Multicast address");
    DbgPrint("SendPing:  TESTADDR = [%s]\n", TESTADDR);
//    DbgPrint("SendPing:  Run McastPing Test, press 'g' to continue...\n");
//    BREAK_INTO_DEBUGGER


//
//  More New code !
//
    result = PingMulticast(hLog, iniBuffer);
//    result = Ping(hLog, AddressType, TESTADDR);
    if (result != PASS)
        xLog(hLog, XLL_FAIL, "The ping failed.");
    else
        xLog(hLog, XLL_PASS, "The Ping succeeded.");

    xEndVariation( hLog );



    WSACleanup();

unload:
    DbgPrint("SendPing: Unloading XBox network stack...\n");
    XnetCleanup();

stop:
    DbgPrint("SendPing: Test stopped, type .reboot now\n");
//    BREAK_INTO_DEBUGGER



}



// XBox entry point

BOOL WINAPI SendPingDllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved)
    {
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(lpReserved);

    if(fdwReason == DLL_PROCESS_ATTACH)
        {
        SendPingHeapHandle = HeapCreate(0, 0, 0);

        if(!SendPingHeapHandle)
            {
            DbgPrint("SendPing: Unable to create heap\n");
            }
        }
    else if(fdwReason == DLL_PROCESS_DETACH)
        {
        if(SendPingHeapHandle)
            {
            HeapDestroy(SendPingHeapHandle);
            }
        }

    return TRUE;
    }






/*
Xbox:
Routine Description:


Arguments:

    HANDLE hLog - to the logging object
    bool stressmode - true for infinate loop

Return Value:

    DWORD

*/
DWORD RunAll(HANDLE hLog, BOOL stressmode /*=false*/)
    {

    DbgPrint("SendPing: Start of RunAll\n");

    do {

        } while(stressmode);

    DbgPrint("SendPing: End of RunAll\n");
    return 0;
    }





/*
XBox:
Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

*/
VOID WINAPI SendPingStartTest(HANDLE hLog)
    {

    DoSendPing(hLog);
    RunAll(hLog, FALSE);
    DbgPrint("SendPing: Leaving StartTest()\n");

    }




/*
Xbox:
Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

*/
VOID WINAPI SendPingEndTest(void)
    {
    }

INT
PingLoopback(HANDLE hLog)
{
    IcmpMessage *myIcmpMessage;
    CHAR databuf[PINGBUFSIZE];
    CHAR ResponseIP[IPADDRESSLENGTH];
    UINT myOffset = 0;
    INT result = FAIL;
    INT iphdrlen, iplen;
    CHAR *mypingmessage;
    CHAR ActualIP[IPADDRESSLENGTH];
    CHAR ExpectedIP[IPADDRESSLENGTH];


    // Clear memory
    ZeroMem(&ActualIP, IPADDRESSLENGTH);
    ZeroMem(&ExpectedIP, IPADDRESSLENGTH);
    ZeroMem(&iniBuffer,BuffLen);
    ZeroMem(&TESTADDR,IPADDRESSLENGTH);

    // Read data from .INI file
    GetProfileStringA("sendping", "bogusaddressxyzzy", "127.0.0.1", iniBuffer, IPADDRESSLENGTH);
    IniRemoveStringCommentA( iniBuffer );

    // Log some info
    xSetComponent(hLog, "Network", "ICMP");
    xSetFunctionName( hLog, "PING");
    xStartVariation( hLog, "Ping Loopback of 127.0.0.1");
    DbgPrint("SendPing.PingLoopback iniBuffer = [%s]\n", iniBuffer);
    strcpy(TESTADDR, iniBuffer);
    DbgPrint("SendPing.PingLoopback TESTADDR = [%s]\n", TESTADDR);
//    DbgPrint("SendPing.PingLoopback Run Ping Test, press 'g' to continue...\n");
//    BREAK_INTO_DEBUGGER

    // Send the ping packet
    mypingmessage = HeapAlloc(SendPingHeapHandle, 0, strlen("myping-a-roonie")+1);
    strcpy(mypingmessage, "myping-a-roonie");
    result = xbtUCastPing(TESTADDR, &myOffset, databuf, PINGBUFSIZE, ResponseIP, mypingmessage, strlen(mypingmessage)+1 );


    // Examine data returned from the ping
    myIcmpMessage = (IcmpMessage*) &databuf[myOffset];
    if (myIcmpMessage != NULL)  {
       DbgPrint("SendPing.PingLoopback  ResponseIP      = %s\n", ResponseIP);
       DbgPrint("SendPing.PingLoopback  myOffset        = %d\n", myOffset);
       DbgPrint("SendPing.PingLoopback  type = %d, code = %d\n", myIcmpMessage->type, myIcmpMessage->code);
       DbgPrint("SendPing.PingLoopback  id = %d,  seqno = %d \n", myIcmpMessage->id, myIcmpMessage->seqno);
       iplen = GETIPLEN( (IpHeader*) databuf );
       iphdrlen = GETIPHDRLEN( (IpHeader*) databuf );
       rcvlen = iplen - iphdrlen;
       DbgPrint("SendPing.PingLoopback  %d bytes of data: %s\n", rcvlen - ICMPHDRLEN, myIcmpMessage->origdata);

       //  Compare for PASS vs FAIL
       strcpy(ExpectedIP, "127.0.0.1");
       DbgPrint("SendPing.PingLoopback  ResponseIP = [%s] ExpectedIP = [%s]\n", ResponseIP, ExpectedIP );
       DbgPrint("SendPing.PingLoopback  strcmp: = %d\n", strcmp(ResponseIP, ExpectedIP));
       if (!strcmp(ResponseIP, ExpectedIP))  {
           xLog(hLog, XLL_INFO, "ResponseIP matches ExpectedIP");
           DbgPrint("SendPing.PingLoopback: ResponseIP matches ExpectedIP\n");

           if (myIcmpMessage->type = ICMPTYPE_ECHO_REPLY)  {
               result = PASS;
           }
           else  {
               xLog(hLog, XLL_INFO, "MessageType is not what was expected");
               DbgPrint("SendPing.PingLoopback  type = %d, code = %d\n", myIcmpMessage->type, myIcmpMessage->code);
           }
       }
       else {
           xLog(hLog, XLL_INFO, "ResponseIP does NOT match ExpectedIP");
           DbgPrint("SendPing.PingLoopback: ResponseIP does NOT match ExpectedIP\n");
       }

    }

    return result;
}



INT
PingUnicast(HANDLE hLog, CHAR *ExpectedIP)
{
    IcmpMessage *myIcmpMessage;
    CHAR databuf[PINGBUFSIZE];
    CHAR ResponseIP[IPADDRESSLENGTH];
    UINT myOffset = 0;
    INT result = FAIL;
    INT iphdrlen, iplen;
    CHAR *mypingmessage;
    CHAR ActualIP[IPADDRESSLENGTH];


    // Clear memory
    ZeroMem(&ActualIP, IPADDRESSLENGTH);


    // Log some info
    DbgPrint("SendPing.PingUnicast ExpectedIP = [%s]\n", ExpectedIP);
    strcpy(TESTADDR, ExpectedIP);
    DbgPrint("SendPing.PingUnicast TESTADDR = [%s]\n", TESTADDR);
//    DbgPrint("SendPing.PingUnicast Run Ping Test, press 'g' to continue...\n");
//    BREAK_INTO_DEBUGGER

    // Send the ping packet
    mypingmessage = HeapAlloc(SendPingHeapHandle, 0, strlen("myping-a-roonie")+1);
    strcpy(mypingmessage, "myping-a-roonie");
    result = xbtUCastPing(TESTADDR, &myOffset, databuf, PINGBUFSIZE, ResponseIP, mypingmessage, strlen(mypingmessage)+1 );


    // Examine data returned from the ping
    myIcmpMessage = (IcmpMessage*) &databuf[myOffset];
    if (myIcmpMessage != NULL)  {
       DbgPrint("SendPing.PingUnicast  ResponseIP      = %s\n", ResponseIP);
       DbgPrint("SendPing.PingUnicast  myOffset        = %d\n", myOffset);
       DbgPrint("SendPing.PingUnicast  type = %d, code = %d\n", myIcmpMessage->type, myIcmpMessage->code);
       DbgPrint("SendPing.PingUnicast  id = %d,  seqno = %d \n", myIcmpMessage->id, myIcmpMessage->seqno);
       iplen = GETIPLEN( (IpHeader*) databuf );
       iphdrlen = GETIPHDRLEN( (IpHeader*) databuf );
       rcvlen = iplen - iphdrlen;
       DbgPrint("SendPing.PingUnicast  %d bytes of data: %s\n", rcvlen - ICMPHDRLEN, myIcmpMessage->origdata);

       //  Compare for PASS vs FAIL
       strcpy(ExpectedIP, iniBuffer);
       DbgPrint("SendPing.PingUnicast  ResponseIP = [%s] ExpectedIP = [%s]\n", ResponseIP, ExpectedIP );
       DbgPrint("SendPing.PingUnicast  strcmp: = %d\n", strcmp(ResponseIP, ExpectedIP));
       if (!strcmp(ResponseIP, ExpectedIP))  {
           xLog(hLog, XLL_INFO, "ResponseIP matches ExpectedIP");
           DbgPrint("SendPing.PingUnicast: ResponseIP matches ExpectedIP\n");

           if (myIcmpMessage->type = ICMPTYPE_ECHO_REPLY)  {
               result = PASS;
           }
           else  {
               xLog(hLog, XLL_INFO, "MessageType is not what was expected");
               DbgPrint("SendPing.PingUnicast  type = %d, code = %d\n", myIcmpMessage->type, myIcmpMessage->code);
           }
       }
       else {
           xLog(hLog, XLL_INFO, "ResponseIP does NOT match ExpectedIP");
           DbgPrint("SendPing.PingUnicast: ResponseIP does NOT match ExpectedIP\n");
       }

    }

    return result;
}


INT
PingMulticast(HANDLE hLog, CHAR *ExpectedIP)
{
    IcmpMessage *myIcmpMessage;
    CHAR databuf[PINGBUFSIZE];
    CHAR ResponseIP[IPADDRESSLENGTH];
    UINT myOffset = 0;
    INT result = FAIL;
    INT iphdrlen, iplen;
    CHAR *mypingmessage;
    CHAR ActualIP[IPADDRESSLENGTH];


    // Clear memory
    ZeroMem(&ActualIP, IPADDRESSLENGTH);


    // Log some info
    DbgPrint("SendPing.PingMulticast ExpectedIP = [%s]\n", ExpectedIP);
    strcpy(TESTADDR, ExpectedIP);
    DbgPrint("SendPing.PingMulticast TESTADDR = [%s]\n", TESTADDR);
//    DbgPrint("SendPing.PingMulticast Run Ping Test, press 'g' to continue...\n");
//    BREAK_INTO_DEBUGGER

    // Send the ping packet
    mypingmessage = HeapAlloc(SendPingHeapHandle, 0, strlen("myping-a-roonie")+1);
    strcpy(mypingmessage, "myping-a-roonie");
    result = xbtMCastPing(TESTADDR, &myOffset, databuf, PINGBUFSIZE, ResponseIP, mypingmessage, strlen(mypingmessage)+1 );


    // Examine data returned from the ping
    myIcmpMessage = (IcmpMessage*) &databuf[myOffset];
    if (myIcmpMessage != NULL)  {
       DbgPrint("SendPing.PingMulticast  ResponseIP      = %s\n", ResponseIP);
       DbgPrint("SendPing.PingMulticast  myOffset        = %d\n", myOffset);
       DbgPrint("SendPing.PingMulticast  type = %d, code = %d\n", myIcmpMessage->type, myIcmpMessage->code);
       DbgPrint("SendPing.PingMulticast  id = %d,  seqno = %d \n", myIcmpMessage->id, myIcmpMessage->seqno);
       iplen = GETIPLEN( (IpHeader*) databuf );
       iphdrlen = GETIPHDRLEN( (IpHeader*) databuf );
       rcvlen = iplen - iphdrlen;
       DbgPrint("  %d bytes of data: %s\n", rcvlen - ICMPHDRLEN, myIcmpMessage->origdata);

       //  Compare for PASS vs FAIL
       strcpy(ExpectedIP, iniBuffer);
       DbgPrint("SendPing.PingMulticast ResponseIP = [%s] ExpectedIP = [%s]\n", ResponseIP, ExpectedIP );
       DbgPrint("SendPing.PingMulticast strcmp: = %d\n", strcmp(ResponseIP, ExpectedIP));
       if (!strcmp(ResponseIP, ExpectedIP))  {
           xLog(hLog, XLL_INFO, "ResponseIP matches ExpectedIP");
           DbgPrint("SendPing.PingMulticast: ResponseIP matches ExpectedIP\n");

           if (myIcmpMessage->type = ICMPTYPE_ECHO_REPLY)  {
               result = PASS;
           }
           else  {
               xLog(hLog, XLL_INFO, "MessageType is not what was expected");
               DbgPrint("SendPing.PingMulticast  type = %d, code = %d\n", myIcmpMessage->type, myIcmpMessage->code);
           }
       }
       else {
           xLog(hLog, XLL_INFO, "ResponseIP does NOT match ExpectedIP");
           DbgPrint("SendPing.PingMulticast: ResponseIP does NOT match ExpectedIP\n");
       }

    }

    return result;
}






//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( sendping )
#pragma data_seg()

BEGIN_EXPORT_TABLE( sendping )
    EXPORT_TABLE_ENTRY( "StartTest", SendPingStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", SendPingEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", SendPingDllMain )
END_EXPORT_TABLE( sendping )

