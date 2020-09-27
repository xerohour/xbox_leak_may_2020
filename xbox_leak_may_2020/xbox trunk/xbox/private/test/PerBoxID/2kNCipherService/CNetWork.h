//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include <windows.h>
#include <stdio.h>
#include <winsock2.h>
#include "..\\include\\datapacket.h"

typedef struct sockaddr_in SOCKADDER_IN;
typedef struct sockaddr SOCKADDR;


CONST USHORT COMMUNICATION_SERVICE_PORT = 5161;

class CNetWorkConnection{
public:

   INT SendData( LPVOID pvData, INT iSize);
   INT RecvData( LPVOID pvData, INT iSize);
   
   VOID SetSocket( SOCKET s){ sConnection = s;}

   ~CNetWorkConnection();
   CNetWorkConnection();
   CNetWorkConnection( SOCKET s){ sConnection = s;}

private:
   SOCKET sConnection;

};



class CNetWork{

private:
   DWORD  m_dwAcceptIndex;
   SOCKET sClient;

public:

   SOCKET Accept ( struct sockaddr FAR *addr, int FAR *addrlen );
   SOCKET Accept ( VOID );
   BOOL StartThread( LPTHREAD_START_ROUTINE lpStartAddress, LPVOID Param );

   VOID InitNetWork( VOID );

   ~CNetWork();
   CNetWork();
};
