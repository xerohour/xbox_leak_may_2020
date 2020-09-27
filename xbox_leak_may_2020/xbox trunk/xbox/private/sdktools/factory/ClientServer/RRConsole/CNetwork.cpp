//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
/*++
 Copyright (c) 2001, Microsoft.

    Author:  Dennis Krueger [a-denkru]
    Date:    6/26/2001

    Description:
        This is the cpp file for the Network class.

--*/
//#include "CNetwork.h"
#include "stdafx.h"
#include "RRConsole.h"



UINT CNetErrorIDs[] = {
	IDS_SUCCESS,
	IDS_SOCKINITERR,
	IDS_INVALSOCKET,
	IDS_NOXSERVER,
	IDS_XCONNECTFAIL,
	IDS_SENDFAIL,
	IDS_RCVFAILED,
	IDS_XCOMMANDFAIL
};


#define SITAddr 0x8e8e0101  // static ip address of 142.142.1.1






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
	int Retry = 0;
	ULONG One = 1; // use as parameter for ioctlsocket

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

	if (sBroadCast == SOCKET_ERROR) 
	{
	   
		printf("broadcast socket() failed: %d", WSAGetLastError());
		exit(1);
	}
	// Set this to a broadcast socket.
	bBroadCast = TRUE;
	// setsockopt goes here 
	sinCast.sin_family = AF_INET;
	sinCast.sin_addr.s_addr = INADDR_BROADCAST; //htonl(SITAddr);//bugbug test /*INADDR_BROADCAST ; SITAddr*///BUGBUG REVIEW
	sinCast.sin_port = htons(ADVERTISE_PORT);
	setsockopt(sBroadCast,SOL_SOCKET, SO_BROADCAST,
              (char *)&bBroadCast, sizeof(bBroadCast));
 
	if ( SOCKET_ERROR == sendto( sBroadCast,
                                szInBuffer,
                                strlen(szInBuffer),
                                0,
                                (SOCKADDR*)&sinCast,
                                sizeof(sinCast))) 
   {
		return FALSE;
   }


	closesocket(sBroadCast);

	// Do a recvfrom so we then broadcast out our IP
	sBroadCast = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sBroadCast == SOCKET_ERROR) 
	{
		printf("Opening socket() failed: %d\n", WSAGetLastError());
		bStatus = FALSE;
		goto exit;
	}
   
	sddrFrom.sin_family = AF_INET;
	sddrFrom.sin_addr.s_addr = htonl(SITAddr/*INADDR_ANY*/) ; // use static ip addr
	sddrFrom.sin_port = htons(ADVERTISE_PORT);

	if (bind(sBroadCast, (struct sockaddr *)&sddrFrom, sizeof(sddrFrom)) == SOCKET_ERROR)
	{
		printf("Bind() failed: %d", WSAGetLastError());
		return FALSE;

	}
   
	ioctlsocket(sBroadCast,FIONBIO,&One);
	listen(sBroadCast,1);
	iAddrLen = sizeof(sddrAccept);
retry:
	sRead = accept( sBroadCast,(SOCKADDR *)&sddrAccept,&iAddrLen);
	if(INVALID_SOCKET == sRead && Retry < 10)
	{
		Retry++;
		Sleep(500);
		goto retry;
	}
	closesocket(sBroadCast);
	if(INVALID_SOCKET == sRead)
	{
		char buffer[100];
		sprintf(buffer,"WSA Error = %d",WSAGetLastError());
		MessageBox(NULL,buffer,NULL,MB_OK);

		return FALSE;
	}

	// receive IP.
	recv( sRead, (CHAR *)byteIP, sizeof(byteIP),0);
	sprintf(szIp,"%s", inet_ntoa(*inaddr) );
	WSACleanup();

	exit:
	return bStatus;
}


// Below is network class.
CNetWork::CNetWork()
{
	;
}


CNetWork::~CNetWork()
{
   closesocket(m_sXBClient);
   closesocket(m_sSFClient);
   WSACleanup();
}



CNetErrors 
CNetWork::
SendData(SOCKET sTarget, LPVOID pvData, INT iSize)
{
   
   INT ret;
   PDATA_PACKET p = (PDATA_PACKET)pvData;
   // Send the data 
   ret = send(sTarget,(const char*) pvData, iSize, 0);
   if (ret == SOCKET_ERROR ) 
   { // SOCKET_ERROR == -1
	   ret = SendFailed;
   }
   
   return CNetSuccess;
}


CNetErrors 
CNetWork::
RecvData(SOCKET sTarget, LPVOID pvData, INT iSize)
{
   
   INT ret;
   PDATA_PACKET p = (PDATA_PACKET)pvData;

   ret = recv( sTarget,(char *) pvData, iSize, 0);
   if (ret == SOCKET_ERROR) {
      return ReceiveFailed;
   }
   
   return CNetSuccess;
}


INT 
CNetWork::
CloseSocket( SOCKET sTarget )
{
   
   return closesocket( sTarget );

}


CNetErrors CNetWork::InitXB(LPSTR szIpString, LPSTR szXboxName, USHORT usPort)
{

   WSADATA       wsd;
   char          szIp[ 20 ];
   struct sockaddr_in server;
   struct hostent*    host = NULL;
   BOOL			bResult;

   // Get the Ip for the specified Xbox.
   if (NULL == szIpString[0] ) 
   {
      bResult = GetXboxIpFromName ( szXboxName, szIp);
	  if(FALSE == bResult)
	  {
		  return NoServer;
	  }

      strncpy(szIpString, szIp, strlen(szIp)+1);
   }
   
   if (WSAStartup(MAKEWORD(2,2), &wsd) != 0) 
   {
		return WSA_Error;
   }
   
   // Create the socket, and attempt to connect to the server
   m_sXBClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (INVALID_SOCKET == m_sXBClient ) 
   {
		return InvalidSocket;
   }
 
   server.sin_family = AF_INET;
   server.sin_port = htons(usPort);
   server.sin_addr.s_addr = inet_addr( szIpString );
   // If the supplied server address wasn't in the form
   // "aaa.bbb.ccc.ddd" it's a hostname, so try to resolve it
   
   if (INADDR_NONE == server.sin_addr.s_addr) {
      host = gethostbyname( szIpString );
      if (	NULL == host) 
	  {
		 return NoServer;
      } else 
	  {
         CopyMemory(&server.sin_addr, host->h_addr, host->h_length);
      }
   }
   if (connect(m_sXBClient, (struct sockaddr *)&server, 
               sizeof(server)) == SOCKET_ERROR) 
   {
      return ConnectFailed;
   }
   
   
   return CNetSuccess;
}


