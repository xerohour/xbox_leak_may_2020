//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#ifndef _DATAPACKET_H_
#define _DATAPACKET_H_

#include <windows.h>

CONST INT PACKET_DATA_SIZE = 300;
CONST INT PACKET_MSG_SIZE = 200;

// Command Constants.
CONST DWORD DO_WRITEPERBOXDATA			= 1; // write generated eeprom data
CONST DWORD DO_VERIFYPERBOXDATA			= 2; // set hard drive password and clear mfg region
CONST DWORD DO_LOCK_HARDDRIVE			= 3; // set hard drive password, don't clear mfg region
CONST DWORD ERROR_FATAL					= 4;	
CONST DWORD ERROR_WARN					= 5;	


typedef struct 
_DATA_PACKET
{
         
   CHAR  szMessage[PACKET_MSG_SIZE];
   DWORD dwCommand;
   BYTE  byteData[PACKET_DATA_SIZE];
   ULONG ulDataSize;
   BOOL  bDevKit;
   DWORD dwStatus;
} DATA_PACKET, *PDATA_PACKET;

#endif

