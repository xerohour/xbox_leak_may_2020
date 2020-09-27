#include "CNetWork.h"


CNetWork::
CNetWork()
{                                                  
}

VOID
CNetWork::
InitNetWork( VOID)
{
   WSADATA       wsa;
   SOCKADDER_IN local;
   
   m_dwAcceptIndex = 0;
   if ( -1 == WSAStartup( MAKEWORD(2,2), &wsa) ) {
     printf(TEXT("WSAStartup  failed: %d"), WSAGetLastError());
   }
   
   // Create our listening socket
   sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (sClient == SOCKET_ERROR) {
       printf(TEXT("socket() failed: %d"), WSAGetLastError());
   }
   local.sin_addr.s_addr = htonl(INADDR_ANY);
   local.sin_family = AF_INET;
   local.sin_port = htons( COMMUNICATION_SERVICE_PORT );

   if (bind(sClient, (struct sockaddr *)&local, sizeof(local)) == SOCKET_ERROR) {
      printf(TEXT("bind() failed: %d"), WSAGetLastError());
   }
   printf(TEXT("Calling Listen.\n"));
   listen(sClient, 10);
}




CNetWork::~CNetWork()
{
   closesocket(sClient);
   WSACleanup();
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
   
   SOCKET         AcceptReturned;
   struct         sockaddr_in  client;
   int            iAddrSize;
   iAddrSize      = sizeof(client);
   AcceptReturned = accept( sClient,(struct sockaddr *)&client, &iAddrSize);
   

   if (AcceptReturned == INVALID_SOCKET) {
      printf(TEXT("ACCEPT() failed: %d\n"), WSAGetLastError());
   } else {
      printf(TEXT("ACCEPT() succeeded. Session[%lu] Client:%S:%d\n"), 
             m_dwAcceptIndex, 
             inet_ntoa(client.sin_addr), 
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
      printf(TEXT("CreateThread() failed: %d\n   "), GetLastError());
      return FALSE;
   }
   CloseHandle(hThread);
   return TRUE;
}



CNetWorkConnection::
~CNetWorkConnection() {
   
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
      printf(TEXT("  SEND failed: %d\n"), WSAGetLastError());
   }else{
      printf(TEXT("  SEND [%d bytes]\n"), ret);
      printf(TEXT("  SEND MSG: [SENT]: %s\n"), p->szMessage);
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
      printf(TEXT("  RECV failed: %d\n"), WSAGetLastError());
   }else{
      printf(TEXT("  RECV [%d bytes]\n"), ret );
      printf(TEXT("  RECV MSG: [RECEIVED]: %s\n"), p->szMessage);
   }
   return ret;
}


