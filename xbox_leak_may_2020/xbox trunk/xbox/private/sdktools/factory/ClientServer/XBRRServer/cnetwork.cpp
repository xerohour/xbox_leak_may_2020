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


CNetWork::CNetWork()
{                                                  

	WSADATA       wsa;
	SOCKADDER_IN local;
	XNetStartupParams xnsp = {sizeof(XNetStartupParams ),
							 XNET_STARTUP_BYPASS_SECURITY};    
	// Must call to get the net stack working.
	XNetStartup(&xnsp);
	// Start the IpAdvertise thread.
	StartThread( IpAdvertise, (LPVOID)this);


	m_dwAcceptIndex = 0;
	if ( -1 == WSAStartup( MAKEWORD(2,2), &wsa) )
	{
		return; // failure, no recovery
	}
	// Create our listening socket
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sClient == SOCKET_ERROR) 
	{
		return; // fail, no recovery
	}
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_family = AF_INET;
	local.sin_port = htons( COMMUNICATION_PORT );


	if (bind(sClient, (struct sockaddr *)&local, sizeof(local)) == SOCKET_ERROR) 
	{
		return; // fail, no recovery
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
	SOCKET	AcceptReturned;
	CHAR	szIpString[18]= "";

	struct	sockaddr_in  client;
	int		iAddrSize;

	iAddrSize = sizeof(client);
	AcceptReturned = accept(sClient,(struct sockaddr *)&client, &iAddrSize);

	if (AcceptReturned == INVALID_SOCKET) 
	{
		return -1; // fail
	}
	XNetInAddrToString(client.sin_addr,szIpString,sizeof(szIpString)/sizeof(CHAR));
	++m_dwAcceptIndex;
   
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
   if (hThread == NULL) 
   {
      return FALSE;
   }
   CloseHandle(hThread);
   return TRUE;
}





DWORD WINAPI
CNetWork::
IpAdvertise( LPVOID lpParam )
{
   
   INT  iRet;
   SOCKET sBroadCast;
   BOOL   bBroadCast = TRUE;
   SOCKADDER_IN sddrFrom;
   char szInBuffer[20] = "";  // This zeroes the entire buffer if you didn't know this trick.
   XNADDR  xna = {sizeof(XNADDR)};
   INT  iFromLen;
   WSADATA       wsd;
   BOOL bStatus = TRUE;
   BYTE    byteIP[15] = "";
   CHAR    szIpString[18]= "";


	iRet = WSAStartup(MAKEWORD (2,2), &wsd);
	if (iRet != 0) 
	{
		WSACleanup();
		return FALSE;
	}
	int iRetries = 0;
	while (1)
	{
		// Do a recvfrom so we then broadcast out our IP
		sBroadCast = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sBroadCast == SOCKET_ERROR) 
		{
			bStatus = FALSE;
			if(iRetries++ > 5)
			{
				// if we get 5 failures in a row,
				bStatus = FALSE;
				break;
			}
			continue;
		}

		// Set this to a broadcast socket.
		setsockopt(sBroadCast,SOL_SOCKET, SO_BROADCAST,(char *)&bBroadCast, sizeof(bBroadCast));
		sddrFrom.sin_family = AF_INET;
		sddrFrom.sin_addr.s_addr = htonl(INADDR_ANY) ;
		sddrFrom.sin_port = htons(ADVERTISE_PORT);

		if ( SOCKET_ERROR == bind(sBroadCast, (SOCKADDR *)&sddrFrom, sizeof(sddrFrom)))
		{
			closesocket( sBroadCast);
			if(iRetries++ > 5)
			{
				// if we get 5 failures in a row,
				bStatus = FALSE;
				break;
			}
			continue;
		}


		iFromLen =  sizeof(sddrFrom);
		iRet = recvfrom( sBroadCast,
				szInBuffer,
				20,
				0,
				(SOCKADDR *)&sddrFrom,
				&iFromLen);
		if(SOCKET_ERROR == iRet)
		{
			if(iRetries++ > 5)
			{
				// if we get 5 failures in a row,
				bStatus = FALSE;
				break;
			}
		}
		// inet_ntoa should use XNetInAddrToString
		XNetInAddrToString(sddrFrom.sin_addr,szIpString,sizeof(szIpString)/sizeof(CHAR));
		closesocket( sBroadCast);

		// Do a send of our IP.
		sBroadCast = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		RtlZeroMemory( &sddrFrom, sizeof(sddrFrom));
		if (sBroadCast == SOCKET_ERROR) 
		{
			bStatus = FALSE;
			continue;
		}

		// Sleep 2 second so we have time on the other end
		// To setup a receive.
		Sleep(2000);
		sddrFrom.sin_family      = AF_INET;
		sddrFrom.sin_addr.s_addr = inet_addr( szIpString );
		sddrFrom.sin_port        = htons(ADVERTISE_PORT);

		if (connect(sBroadCast, (struct sockaddr *)&sddrFrom, 
				  sizeof(sddrFrom)) == SOCKET_ERROR) 
		{
			closesocket( sBroadCast);
			if(iRetries++ > 5)
			{
				// if we get 5 failures in a row,
				bStatus = FALSE;
				break;
			}
			continue;
		}
		XNetGetTitleXnAddr( &xna );
		RtlCopyMemory(byteIP, (BYTE*) &xna.ina, sizeof(xna.ina));
		XNetInAddrToString(xna.ina, szIpString,sizeof(szIpString)/sizeof(CHAR));
		iRet = send(sBroadCast,(CHAR*)byteIP,sizeof(byteIP)/sizeof(CHAR), 0);
		if(SOCKET_ERROR == iRet)
		{
			if(iRetries++ > 5)
			{
				// if we get 5 failures in a row,
				bStatus = FALSE;
				break;
			}
			continue;
		}
		closesocket( sBroadCast);
	} // while(1)

	WSACleanup();
	return bStatus;
}


CNetWorkConnection::~CNetWorkConnection() 
{
   if (0 != sConnection )
      closesocket(sConnection);
}

INT 
CNetWorkConnection::
SendData( LPVOID pvData, INT iSize)
{

   INT ret;
   INT retTot = 0;

   PDATA_PACKET p =(PDATA_PACKET)pvData;
   // Send the data 
   while(iSize) {
       ret = send(sConnection,(const char*) pvData, iSize, 0);
       if(ret <= 0)
           return ret;
       pvData = (void*)((BYTE*)pvData + ret);
       iSize -= ret;
       retTot += ret;
   }
   return retTot;
}


INT 
CNetWorkConnection::
RecvData( LPVOID pvData, INT iSize)
{
   
   INT ret;
   INT retTot = 0;

   PDATA_PACKET p = (PDATA_PACKET)pvData;
   while(iSize) {
       ret = recv( sConnection,(char*)pvData, iSize, 0);
       if(ret <= 0)
           return ret;
       pvData = (void*)((BYTE*)pvData + ret);
       iSize -= ret;
       retTot += ret;
   }
   return retTot;
}


