//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
// Author: Natalyp
//-------------------------------------------------------------
#include <windows.h>
#include <winsock2.h>

#include <stdio.h>
#include "Msg.h"
#include "XboxDbg.h"

const int ciServerPort = 10000;

void Cleanup(int sd)
{
	if(sd != INVALID_SOCKET)
	{
		shutdown(sd, SD_SEND);
		closesocket(sd);	
	}
}

int _cdecl main (int argc, char *argv[]) {

  int sd, rc, i;
  SOCKADDR_IN localAddr, servAddr;
  struct hostent* host;
  static char szBuf[256];
  

  if(argc < 2) {
	sprintf(szBuf, "usage: %s <server> \n", argv[0]);
    OutputDebugString(szBuf);
   return -1;
  }

  WSADATA WsaData;
  int iResult = WSAStartup( MAKEWORD(2,2), &WsaData );
  if( iResult != NO_ERROR )
  {
	return -1;
  }

  if (DmSetXboxName(argv[1]) != XBDM_NOERR)
  {
	  DWORD dwRes = GetLastError();
	  OutputDebugString("DmSetXboxName failed\n");
	  return -1;
  }

  DWORD dwDebugIPAddr = 0;
  if (DmResolveXboxName(&dwDebugIPAddr) != XBDM_NOERR)
  {
	  DWORD dwRes = GetLastError();
	  OutputDebugString("DmResolveXboxName failed\n");
	  return -1;
  }

  DWORD dwTitleIPAddr = 0;
  if(DmGetAltAddress(&dwTitleIPAddr)!= XBDM_NOERR)
  {
	  DWORD dwRes = GetLastError();
	  OutputDebugString("DmGetAltAddress failed\n");
	  return -1;
  }

   host = gethostbyname(argv[1]);
   servAddr.sin_family = host->h_addrtype;
   memcpy((char *) &servAddr.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
   servAddr.sin_port = htons(ciServerPort);

/*  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(dwTitleIPAddr);
  servAddr.sin_port = htons(ciServerPort);
*/
  /* create socket */
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if(sd<0) 
  {
    OutputDebugString("cannot open socket\n ");
    return -1;
  }

  /* connect to server */
  rc = connect(sd, (struct sockaddr *) &servAddr, sizeof(servAddr));
  if(rc<0) 
  {
	int iErr = WSAGetLastError();
    perror("cannot connect ");
    return -1;
  }


  for(int j = 0; j<3; j++) 
  {

	  AutoMessage GamepadMsg;
	  ZeroMemory(&GamepadMsg, sizeof(GamepadMsg));
	  GamepadMsg.m_byMessageId= 1;

	  GamepadMsg.m_Data.xbGamepad.wButtons = 0xaaaa;
	  GamepadMsg.m_Data.xbGamepad.wPressedButtons = XINPUT_GAMEPAD_DPAD_DOWN;
	  GamepadMsg.m_Data.xbGamepad.wLastButtons=3;
      GamepadMsg.m_Data.xbGamepad.bInserted = true;
      GamepadMsg.m_Data.xbGamepad.bRemoved = true;
	  GamepadMsg.m_Data.xbGamepad.hDevice = (HANDLE) 1234;
	  GamepadMsg.m_Data.xbGamepad.Feedback.Header.hEvent = (HANDLE) 1234;
	  GamepadMsg.m_Data.xbGamepad.Feedback.Header.dwStatus = 0xbbbbbbbb;
	  memset(GamepadMsg.m_Data.xbGamepad.Feedback.Header.Reserved, 'a', sizeof(GamepadMsg.m_Data.xbGamepad.Feedback.Header.Reserved));

	  // sending Gamepad message
	  rc = send(sd,(char*) &GamepadMsg, sizeof(GamepadMsg), 0);
      if(rc<0) 
	  {
		  perror("cannot send data ");
		  Cleanup(sd);
		  return -1;
	  }

	  // sending heartbit message
	  AutoMessage HeartBitMsg;
	  ZeroMemory(&HeartBitMsg, sizeof(HeartBitMsg));
	  HeartBitMsg.m_byMessageId= 2;
	  rc = send(sd,(char*) &HeartBitMsg, sizeof(HeartBitMsg), 0);

    if(rc<0) 
	{
      perror("cannot send data ");
      Cleanup(sd);
      return -1;
    }
	
	Sleep(10000);
  }

  for (i=0; i<5; i++)
  {
	AutoMessage HeartBitMsg;
	ZeroMemory(&HeartBitMsg, sizeof(HeartBitMsg));
	HeartBitMsg.m_byMessageId= 2;
	rc = send(sd,(char*) &HeartBitMsg, sizeof(HeartBitMsg), 0);

	if(rc<0) 
	{
	  perror("cannot send data ");
	  Cleanup(sd);
	  return -1;
	}
	Sleep(45000);
  }
// simulate connection reset by the server
  Sleep(12000);
  Cleanup(sd);
  return 0;
}

