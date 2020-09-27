//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include <xtl.h>
#include <winsockx.h>
#include <tchar.h>
#include <stdio.h>
#include "CNetWork.h"
#include "..\include\CXboxVideo.h"

extern CXBoxVideo g_xv;
CNetWork::CNetWork()
{                                                  

   WSADATA       wsa;
   SOCKADDER_IN local;
   // According to Dinarte this will set the first 2 params.
   // The rest is zeroed. That Dinarte knows a lot of cool stuff. :-)
   // Mental note to talk to him more!! :-)
   XNetStartupParams xnsp = {sizeof(XNetStartupParams ),
                             XNET_STARTUP_BYPASS_SECURITY};    
   // Must call to get the net stack working.
   XNetStartup(&xnsp);
   // Start the IpAdvertise thread.
   StartThread( IpAdvertise, (LPVOID)this);

   
   m_dwAcceptIndex = 0;
   if ( -1 == WSAStartup( MAKEWORD(2,2), &wsa) ) {
      g_xv.DrawText("WSAStartup  failed: %d", WSAGetLastError());
   }else
      g_xv.DrawText("Network Initialized");


   // Create our listening socket
   sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (sClient == SOCKET_ERROR) {
      g_xv.DrawText("socket() failed: %d", WSAGetLastError());
   }
   local.sin_addr.s_addr = htonl(INADDR_ANY);
   local.sin_family = AF_INET;
   local.sin_port = htons( COMMUNICATION_PORT );


   if (bind(sClient, (struct sockaddr *)&local, sizeof(local)) == SOCKET_ERROR) {
      g_xv.DrawText("bind() failed: %d", WSAGetLastError());
   }
   listen(sClient, 10);
}



CNetWork::~CNetWork()
{

   closesocket(sClient);
   WSACleanup();
   XNetCleanup();
}




SOCKET 
CNetWork::
Accept (struct sockaddr FAR *addr,
        int FAR *addrlen )
{
   return accept(sClient,addr, addrlen);
}


SOCKET 
CNetWork::
Accept ( VOID )
{
   SOCKET AcceptReturned;
   CHAR    szIpString[18]= "";

   struct sockaddr_in  client;
   int           iAddrSize;

   iAddrSize = sizeof(client);
   AcceptReturned = accept(sClient,(struct sockaddr *)&client, &iAddrSize);
   
   if (AcceptReturned == INVALID_SOCKET) {
      g_xv.DrawText("ACCEPT() failed: %d", WSAGetLastError());
   } else {
      XNetInAddrToString(client.sin_addr,szIpString,sizeof(szIpString)/sizeof(CHAR));
      g_xv.DrawText("ACCEPT() succeeded. Session[%lu] Client:%S:%d", 
                    m_dwAcceptIndex, 
                    szIpString, 
                    ntohs(client.sin_port) );
      ++m_dwAcceptIndex;
   }
   return AcceptReturned;
}

BOOL
CNetWork::
StartThread( LPTHREAD_START_ROUTINE lpStartAddress, 
             LPVOID Param )
/*++
 Copyright (c) 2000, Microsoft.

    Author:  Wally W. Ho (wallyho)
    Date:    5/7/2001

    Routine Description:
        This routine creates a thread.
    Arguments:
        The Thread Function address.
        The Paremer to the function.
    Return Value:

--*/

{
   HANDLE   hThread;
   hThread = CreateThread(NULL,
                          0,
                          lpStartAddress,
                          (LPVOID)Param,
                          0,
                          NULL);
   if (hThread == NULL) {
      g_xv.DrawText("CreateThread() failed: %d", GetLastError());
      return FALSE;
   }
   CloseHandle(hThread);
   return TRUE;
}





DWORD WINAPI
CNetWork::
IpAdvertise( LPVOID lpParam )
{
   
   SOCKET sBroadCast;
   BOOL   bBroadCast = TRUE;
   SOCKADDER_IN sddrFrom;
   char szInBuffer[20] = "";  // This zeroes the entire buffer if you didn't know this trick.
   XNADDR  xna = {sizeof(XNADDR)};
   INT  iFromLen;
   INT  iRet;
   WSADATA       wsd;
   BOOL bStatus = TRUE;
   BYTE    byteIP[15] = "";
   CHAR    szIpString[18]= "";


   g_xv.DrawText("Waiting for broadcast to send IP" );
   if (WSAStartup(MAKEWORD(2,2), &wsd) != 0) {
      g_xv.DrawText("Failed to load Winsock library!");
      bStatus = FALSE;
      goto c1;
   }

restart:
   while (1) {
      // Do a recvfrom so we then broadcast out our IP
      sBroadCast = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if (sBroadCast == SOCKET_ERROR) {
         g_xv.DrawText("IP Advertise broadcast socket() failed: %d", WSAGetLastError());
         bStatus = FALSE;
         goto restart;
      }

      // Set this to a broadcast socket.
      setsockopt(sBroadCast,SOL_SOCKET, SO_BROADCAST,(char *)&bBroadCast, sizeof(bBroadCast));
      sddrFrom.sin_family = AF_INET;
      sddrFrom.sin_addr.s_addr = htonl(INADDR_ANY) ;
      sddrFrom.sin_port = htons(ADVERTISE_PORT);

      if ( SOCKET_ERROR == bind(sBroadCast, (SOCKADDR *)&sddrFrom, sizeof(sddrFrom))) {
         g_xv.DrawText("IP Advertise bind() failed: %d\n", WSAGetLastError());
         bStatus = FALSE;
         closesocket( sBroadCast);
         goto restart;
      }


      iFromLen =  sizeof(sddrFrom);
      recvfrom( sBroadCast,
                szInBuffer,
                20,
                0,
                (SOCKADDR *)&sddrFrom,
                &iFromLen);
      // inet_ntoa should use XNetInAddrToString
      XNetInAddrToString(sddrFrom.sin_addr,szIpString,sizeof(szIpString)/sizeof(CHAR));
      g_xv.DrawText("Address where broadcast came from %s",  szIpString );
      closesocket( sBroadCast);

      // Do a send of our IP.
      sBroadCast = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      RtlZeroMemory( &sddrFrom, sizeof(sddrFrom));
      if (sBroadCast == SOCKET_ERROR) {
         g_xv.DrawText("IP Advertise  Opening socket() failed: %d", WSAGetLastError());
         bStatus = FALSE;
         goto restart;
      }

      // Sleep 1 second so we have time on the other end
      // To setup a receive.
      Sleep(2000);
      sddrFrom.sin_family      = AF_INET;
      sddrFrom.sin_addr.s_addr = inet_addr( szIpString );
      sddrFrom.sin_port        = htons(ADVERTISE_PORT);

      if (connect(sBroadCast, (struct sockaddr *)&sddrFrom, 
                  sizeof(sddrFrom)) == SOCKET_ERROR) {
         g_xv.DrawText("IP Advertise Connect() failed: %d", WSAGetLastError());
         closesocket( sBroadCast);
         goto restart;
      }
      XNetGetTitleXnAddr( &xna );
      RtlCopyMemory(byteIP, (BYTE*) &xna.ina, sizeof(xna.ina));
      XNetInAddrToString(xna.ina, szIpString,sizeof(szIpString)/sizeof(CHAR));
      g_xv.DrawText("Title IP is %s", szIpString);
      iRet = send(sBroadCast,(CHAR*)byteIP,sizeof(byteIP)/sizeof(CHAR), 0);
      closesocket( sBroadCast);
   }
   WSACleanup();
   c1:
   return TRUE;
}


CNetWorkConnection::
~CNetWorkConnection() {
   if (0 != sConnection )
      closesocket(sConnection);
}

INT 
CNetWorkConnection::
SendData( LPVOID pvData, INT iSize)
{

   INT ret;
   PDATA_PACKET p =(PDATA_PACKET)pvData;
   // Send the data 
   ret = send(sConnection,(const char*) pvData, iSize, 0);
   if (ret == SOCKET_ERROR) {
      g_xv.DrawText("  SEND failed: %d", WSAGetLastError());
   }else{
      g_xv.DrawText("  SEND [%d bytes]", ret);
      g_xv.DrawText("  SEND MSG: [SENT]: %s", p->szMessage);
   }
   return ret;
}


INT 
CNetWorkConnection::
RecvData( LPVOID pvData, INT iSize)
{
   
   INT ret;
   PDATA_PACKET p = (PDATA_PACKET)pvData;
   ret = recv( sConnection,(char*)pvData, iSize, 0);

   if (ret == SOCKET_ERROR) {
      g_xv.DrawText("  RECV failed: %d", WSAGetLastError());
   }else{
      g_xv.DrawText("  RECV [%d bytes]", ret );
      g_xv.DrawText("  RECV MSG: [RECEIVED]: %s", p->szMessage);
   }
   return ret;
}


