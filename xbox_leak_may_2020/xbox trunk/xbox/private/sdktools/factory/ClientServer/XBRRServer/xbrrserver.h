#include <ntos.h>  //ntos has to come before xtl.h
#include <xtl.h>
#include <xboxp.h>
#include <xconfig.h>
#include <av.h>
#include <stdio.h>
#include <tchar.h>
#include <cryptkeys.h>

#include "PerBoxData.h"
#include "CNetWork.h"
//#include "..\\include\\fileformat.h"


#define BREAK_INTO_DEBUGGER     _asm { int 3 }

// Globals

// Prototypes
// debugprint from xapi.lib
extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}


#define FATAL 1


DWORD WINAPI 
ServerSession(LPVOID lpParam);

BOOL
StartThread( LPTHREAD_START_ROUTINE lpStartAddress, 
             LPVOID Param );
BOOL
WriteEEPROM ( IN PDATA_PACKET dpData );

HRESULT
ReadEEPROM ( IN PDATA_PACKET dpData );


VOID DebugOut (PCHAR szMessage,...);

HRESULT ReturnInvalidCommandError(IN CNetWorkConnection *c, IN PDATA_PACKET pPacket);

HRESULT
DoWritePerBoxData(IN CNetWorkConnection *c, IN PDATA_PACKET pDataPacket);

HRESULT
DoVerifyPerBoxData(IN CNetWorkConnection *c, IN PDATA_PACKET pPacket);
 
HRESULT
LockHardDrive(IN CNetWorkConnection *c, IN PDATA_PACKET pPacket);



