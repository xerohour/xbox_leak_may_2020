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
CONST DWORD DO_VERIFY_WRITEPERBOXDATA  = 0x00000001;
CONST DWORD DO_VERIFY_VERIFYPERBOXDATA = 0x00000002;
CONST DWORD DO_VERIFY_DATA_PASS        = 0x00000004;
CONST DWORD DO_VERIFY_CHECKSUM         = 0x00000008;
CONST DWORD DO_SENDBACK_EEPROM         = 0x00000010;
CONST DWORD DO_LOCK_HARDDRIVE          = 0x00000020;
CONST DWORD DO_UNLOCK_HARDDRIVE        = 0x00000040;
CONST DWORD DO_VERIFY_RETAILSYSTEM     = 0x00000080;


typedef struct 
_DATA_PACKET
{
         
   CHAR  szMessage[PACKET_MSG_SIZE];
   DWORD dwCommand;
   BYTE  byteData[PACKET_DATA_SIZE];
   ULONG ulDataSize;
   BOOL  bDevKit;
   BYTE  byteDataBackup[PACKET_DATA_SIZE];

} DATA_PACKET, *PDATA_PACKET;


// used in XeepromDump.
typedef struct 
_CRITICAL_KEY_PAIR {
   BYTE HardDriveKey[16];
   BYTE OnlineKey[16];
}CRITICAL_KEY_PAIR, *PCRITICAL_KEY_PAIR;
#endif

