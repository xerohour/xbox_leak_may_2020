//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#ifndef _CNETWORK_H_
#define _CNETWORK_H_

#include <windows.h>
#include <cryptkeys.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <XboxDbg.h>
#include "..\\include\\DataPacket.h"

typedef struct sockaddr_in SOCKADDER_IN;
typedef struct sockaddr SOCKADDR;
   
   CONST USHORT ADVERTISE_PORT = 5159;
   CONST USHORT COMMUNICATION_PORT = 5150;
   CONST USHORT COMMUNICATION_SERVICE_PORT = 5161;

BOOL
RemoteNCipherDecryptKeys( IN  LPBYTE lpbEncryptedKey,
                          OUT LPBYTE lpbDecryptedKey,
                          IN  DWORD  dwSizeDecryptedKey);


class CNetWork {
private:
   SOCKET sClient;

public:

   INT  SendData( LPVOID pvData, INT iSize);
   INT  RecvData( LPVOID pvData, INT iSize);
   INT  CloseSocket( VOID );
   BOOL GetXboxIpFromName ( IN CHAR* szXboxName, OUT CHAR* szIp);
   BOOL IpAdvertise( VOID );
   
   ~CNetWork();
   CNetWork();
   CNetWork( LPSTR szIpString, LPSTR szXboxName );
   CNetWork( LPSTR szIpString, LPSTR szXboxName, USHORT usPort );
};

#endif

