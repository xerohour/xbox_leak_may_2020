//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include <windows.h>
#include <xtl.h>
#include "..\\include\\datapacket.h"


typedef struct sockaddr_in SOCKADDER_IN;
typedef struct sockaddr SOCKADDR;

   CONST USHORT ADVERTISE_PORT = 5159;
   CONST USHORT COMMUNICATION_PORT = 5150;



class CNetWorkConnection{
public:

   INT SendData( LPVOID pvData, INT iSize);
   INT RecvData( LPVOID pvData, INT iSize);
   VOID SetConnection(SOCKET s){ sConnection = s; }

   ~CNetWorkConnection();
   CNetWorkConnection();
   CNetWorkConnection( SOCKET s){ sConnection = s;}

private:

   SOCKET sConnection;
};



class CNetWork {

private:
   DWORD  m_dwAcceptIndex;
   SOCKET sClient;

public:

   
   SOCKET Accept ( struct sockaddr FAR *addr, int FAR *addrlen );
   SOCKET Accept ( VOID );
   static DWORD WINAPI IpAdvertise( LPVOID lpParam );
   BOOL StartThread( LPTHREAD_START_ROUTINE lpStartAddress, LPVOID Param );

   ~CNetWork();
   CNetWork();
};
