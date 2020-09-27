//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
/*++
 Copyright (c) 2000, Microsoft.

    Author:  Wally W. Ho (wallyho)
    Date:    6/26/2001

    Description:
        This is the cpp file for the Network class.

--*/
#include "CNetwork.h"


BOOL
RemoteNCipherDecryptKeys( IN  LPBYTE lpbEncryptedKey,
                          OUT LPBYTE lpbDecryptedKey,
                          IN  DWORD  dwSizeDecryptedKey)
/*++
 Copyright (c) 2000, Microsoft.

    Author:  Wally W. Ho (wallyho)
    Date:    6/26/2001

    Routine Description:
        This routines calls our remoting function to decrypt using the NCipher box.
    Arguments:
        encrypted key and then decrypted key.
    Return Value:
      TRUE upon success.
      FALSE upon failure.

--*/

{
   
   CNetWork* n1 = new CNetWork("wallyho-ts", NULL , COMMUNICATION_SERVICE_PORT );
   DATA_PACKET In;
   DATA_PACKET Out;
   RtlCopyMemory(Out.byteData, lpbEncryptedKey, ONLINE_KEY_PKENC_SIZE);
   sprintf(Out.szMessage,"%s","Encrypted Key");
   n1->SendData(&Out,sizeof(Out));
   // Decrypted key.
   n1->RecvData( &In, sizeof(In));
   CopyMemory(lpbDecryptedKey, In.byteData, dwSizeDecryptedKey);
   delete n1;
   return TRUE;
}





BOOL
CNetWork::
IpAdvertise( VOID )
{

   SOCKET sBroadCast;
   BOOL   bBroadCast = TRUE;
   SOCKADDER_IN sddrFrom;
   char       szInBuffer[20];
   INT        iFromLen;
   WSADATA    wsd;
   
   printf("Waiting for broadcast to send IP\n" );
   if (WSAStartup(MAKEWORD(2,2), &wsd) != 0) {
      printf("Failed to load Winsock library!\n");
      // return FALSE;
   }

   // Do a recvfrom so we then broadcast out our IP
   // Create a broadcast socket.
   sBroadCast = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if (sBroadCast == SOCKET_ERROR) {
      printf("broadcast socket() failed: %d\n", WSAGetLastError());
   }

   // Set this to a broadcast socket.
   setsockopt(sBroadCast,SOL_SOCKET, SO_BROADCAST,(char *)&bBroadCast, sizeof(bBroadCast));
   
   sddrFrom.sin_family = AF_INET;
   sddrFrom.sin_addr.s_addr = htonl(INADDR_ANY) ;
   sddrFrom.sin_port = htons(5159);

   printf( "Waiting for broadcast to send IP\n" );

   if ( SOCKET_ERROR == bind(sBroadCast, (SOCKADDR *)&sddrFrom, sizeof(sddrFrom))){
      printf("bind() failed: %d\n", WSAGetLastError());
   }
   printf("3\n" );
   
   iFromLen =  sizeof(sddrFrom);
   recvfrom( sBroadCast,
             szInBuffer,
             20,
             0,
             (SOCKADDR *)&sddrFrom,
             &iFromLen);
   printf("Address where broadcast came from %s\n",  inet_ntoa(sddrFrom.sin_addr) );
   printf("Message %s", szInBuffer );
   
   closesocket(sBroadCast);
   WSACleanup();

   return TRUE;
}

 
BOOL
CNetWork::
GetXboxIpFromName ( IN CHAR* szXboxName, OUT CHAR* szIp)
/*++
 Copyright (c) 2000, Microsoft.

    Author:  Wally W. Ho (wallyho)
    Date:    4/26/2001

    Routine Description:
        This will get the Ip address of the Xbox specified.
    Arguments:
        The name. szIp will be the buffer containing the Ip.
        Buffer should be 16 bytes or larger. to contain aaa.bbb.ccc.ddd\0
    Return Value:
      True for success False for failure.

--*/
{

   BOOL bStatus = TRUE;
   szXboxName =0;
   SOCKET sBroadCast;
   BOOL   bBroadCast;
   SOCKADDER_IN sinCast;
   SOCKADDER_IN sddrFrom;
   SOCKADDER_IN sddrAccept;
   SOCKET sRead;
   INT iAddrLen;

   WSADATA       wsd;
   char *szInBuffer = "Give me your IP";
   BYTE  byteIP[15] = "";
   IN_ADDR* inaddr = (IN_ADDR *)byteIP;


   if (WSAStartup(MAKEWORD(2,2), &wsd) != 0) {
      printf("Failed to load Winsock library!\n");
      exit(1);
   }
   // Create a broadcast socket.
   sBroadCast = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   
   if (sBroadCast == SOCKET_ERROR) {
      printf("broadcast socket() failed: %d", WSAGetLastError());
   }
   // Set this to a broadcast socket.
   bBroadCast = TRUE;
   setsockopt(sBroadCast,SOL_SOCKET, SO_BROADCAST,
              (char *)&bBroadCast, sizeof(bBroadCast));

   sinCast.sin_family = AF_INET;
   sinCast.sin_addr.s_addr = INADDR_BROADCAST;
   sinCast.sin_port = htons(ADVERTISE_PORT);
   
   if ( SOCKET_ERROR == sendto( sBroadCast,
                                szInBuffer,
                                strlen(szInBuffer),
                                0,
                                (SOCKADDR*)&sinCast,
                                sizeof(sinCast))) {
      printf("  OK\t\t\tBroadcast failed %d\n", WSAGetLastError());
   }else
      printf("  OK\t\t\tBroadcasting\n");

   closesocket(sBroadCast);

   // Do a recvfrom so we then broadcast out our IP
   sBroadCast = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (sBroadCast == SOCKET_ERROR) {
      printf("Opening socket() failed: %d\n", WSAGetLastError());
      bStatus = FALSE;
      goto exit;
   }
   
   sddrFrom.sin_family = AF_INET;
   sddrFrom.sin_addr.s_addr = htonl(INADDR_ANY) ;
   sddrFrom.sin_port = htons(ADVERTISE_PORT);

   if (bind(sBroadCast, (struct sockaddr *)&sddrFrom, sizeof(sddrFrom)) == SOCKET_ERROR) {
      printf("Bind() failed: %d", WSAGetLastError());
   }
   
   listen(sBroadCast,1);
   iAddrLen = sizeof(sddrAccept);
   sRead = accept( sBroadCast,(SOCKADDR *)&sddrAccept,&iAddrLen);
   closesocket(sBroadCast);

   // receive IP.
   recv( sRead, (CHAR *)byteIP, sizeof(byteIP),0);
   sprintf(szIp,"%s", inet_ntoa(*inaddr) );
   printf("  OK\t\t\tFinished broadcasting\n");
   printf("  OK\t\t\tXBOX's IP is %s\n", szIp);
   WSACleanup();


/***********************************************************************

     DmUseSharedConnection(TRUE);
     if (FAILED(DmSetXboxName(szXboxName))){
        printf( "Could not set the Xbox Name\n");
        b = FALSE;
        goto exit;
     }
     if (SUCCEEDED(DmGetXboxName(szXboxName, &cch))) {
        ULONG ulAddr;
        BYTE *pb = (BYTE *)&ulAddr;
        if (SUCCEEDED(DmResolveXboxName(&ulAddr))){
           printf(" Xbox target system %s (%d.%d.%d.%d)\n", szXboxName,
                   pb[3], pb[2], pb[1], pb[0]);
           sprintf(szIp,"%d.%d.%d.%d",  pb[3], pb[2], pb[1], pb[0]);
           sprintf(szIp,"157.56.13.226");
        } else {
           szIp = NULL;
           b = FALSE;
        }
     }
     exit:
     DmUseSharedConnection(FALSE);
***********************************************************************/
   exit:
   return bStatus;
}



// Below is network class.
CNetWork::CNetWork()
{
   CNetWork( NULL, "wallyhox" );
}


CNetWork::CNetWork( LPSTR szIpString, LPSTR szXboxName )
{
   CNetWork( szIpString, szXboxName, COMMUNICATION_PORT );

} 


CNetWork::CNetWork( LPSTR szIpString, LPSTR szXboxName, USHORT usPort )
{
   

   WSADATA       wsd;
   char          szIp[ 20 ];
   struct sockaddr_in server;
   struct hostent*    host = NULL;


   // Get the Ip for the specified Xbox.
   if (szIpString[0] == NULL) {
      GetXboxIpFromName ( szXboxName, szIp);
      strncpy(szIpString, szIp, strlen(szIp));
   }
   
   if (WSAStartup(MAKEWORD(2,2), &wsd) != 0) {
      printf("Failed to load Winsock library!\n");
      exit(1);
   }else{
     // printf("WsaStartup called!\n");
   }
   
   
   // Create the socket, and attempt to connect to the server
   sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (sClient == INVALID_SOCKET) {
      printf("socket() failed: %d\n", WSAGetLastError());
      exit(1);
   }
   
   server.sin_family = AF_INET;
   server.sin_port = htons(usPort);
   server.sin_addr.s_addr = inet_addr( szIpString );
   // If the supplied server address wasn't in the form
   // "aaa.bbb.ccc.ddd" it's a hostname, so try to resolve it
   
   if (server.sin_addr.s_addr == INADDR_NONE) {
      host = gethostbyname( szIpString );
      if (host == NULL) {
         printf("Unable to resolve server: %s\n", szIpString);
         exit(1);
      } else {
         CopyMemory(&server.sin_addr, host->h_addr, host->h_length);
         printf("  OK\t\t\t Resolved server: %s %s\n", szIpString, inet_ntoa( server.sin_addr ) );
      }
   }
   if (connect(sClient, (struct sockaddr *)&server, 
               sizeof(server)) == SOCKET_ERROR) {
      printf("connect() failed: %d\n", WSAGetLastError());
      exit(1);
   }
}



CNetWork::~CNetWork()
{
   closesocket(sClient);
   WSACleanup();
}



INT 
CNetWork::
SendData( LPVOID pvData, INT iSize)
{
   
   INT ret;
   PDATA_PACKET p = (PDATA_PACKET)pvData;
   // Send the data 
   ret = send(sClient,(const char*) pvData, iSize, 0);
   if (ret == SOCKET_ERROR ) { // SOCKET_ERROR == -1
      printf("  NOT_OK\t\t SEND %s failed: %d ret %d\n", 
               p->szMessage,
               WSAGetLastError(),
               ret);
   }else
      printf("  OK\t\t\t SEND [%d bytes] MSG: [SENT]: %s\n", ret, p->szMessage);
   return ret;
}


INT 
CNetWork::
RecvData( LPVOID pvData, INT iSize)
{
   
   INT ret;
   PDATA_PACKET p = (PDATA_PACKET)pvData;

   ret = recv( sClient,(char *) pvData, iSize, 0);
   if (ret == SOCKET_ERROR) {
      printf("  NOT_OK\t\t\t RECV failed: %d\n", WSAGetLastError());
   }else{
      printf("  OK\t\t\t RECV [%d bytes] MSG: [RECV]: %s\n", ret, p->szMessage);
   }
   return ret;
}


INT 
CNetWork::
CloseSocket( VOID )
{
   
   return closesocket( sClient );

}



