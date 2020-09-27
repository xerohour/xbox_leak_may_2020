//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#ifndef _CNETWORK_H_
#define _CNETWORK_H_

#include <windows.h>
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

   CONST USHORT SF_ADVERTISE_PORT = 5259;
   CONST USHORT SF_COMMUNICATION_PORT = 5250;
   CONST USHORT SF_COMMUNICATION_SERVICE_PORT = 5261;


// this enum must be kept sync'ed with the CNetErrorIDs array in CNetwork.cpp

typedef enum CNetErrors {
	CNetSuccess = 0,
	WSA_Error,
	InvalidSocket,
	NoServer,
	ConnectFailed,
	SendFailed,
	ReceiveFailed,
	XBCommandFailed,
};

typedef struct 
{
	char 	cSerialNumber[XBOX_SERIAL_NUMBER_LEN + 1]; // 12 char serial number + 0 termination
	DWORD	dwAVRegion;
	DWORD	dwGameRegion;
	BYTE	bMACAddress[ETHERNET_MAC_ADDR_LEN];
	char	bHDKey[(HARD_DRIVE_KEY_LEN*2)+1];
	char	cRecoveryKey[8/*RECOVERY_KEY_LEN*/ + 1];
	DWORD	dwOnlineKeyVersion;
	char	cOnlineKey[172/*ONLINE_KEY_LEN*/+1];

}GENDATA, *PGENDATA; 



class CNetWork {

private:
	char *	m_pszXBoxName;
public:
	CNetErrors InitXB(LPSTR szIpString,
		LPSTR szXboxName = "RR_XBServer",
		USHORT usPort = COMMUNICATION_PORT
		);


	CNetErrors  SendData( SOCKET sTarget, LPVOID pvData, INT iSize);
	CNetErrors  RecvData( SOCKET sTarget, LPVOID pvData, INT iSize);
	INT  CloseSocket( SOCKET sTarget );
	BOOL GetXboxIpFromName ( IN CHAR* szXboxName, OUT CHAR* szIp);
	

	~CNetWork();
	CNetWork();


	SOCKET	m_sXBClient;
	SOCKET  m_sSFClient;

};
#endif

