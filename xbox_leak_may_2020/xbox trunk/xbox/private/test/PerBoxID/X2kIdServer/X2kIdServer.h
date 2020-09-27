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
#include "CXboxVideo.h"
#include "..\\include\\fileformat.h"


#define BREAK_INTO_DEBUGGER     _asm { int 3 }

// Globals
CXBoxVideo g_xv;

// Prototypes

DWORD WINAPI 
ClientThread(LPVOID lpParam);

DWORD WINAPI 
DrawScreenThread(LPVOID lpParam);
BOOL
StartThread( LPTHREAD_START_ROUTINE lpStartAddress, 
             LPVOID Param );
BOOL
WriteEEPROM ( IN PDATA_PACKET dpData );

HRESULT
ReadEEPROM ( IN PDATA_PACKET dpData );

VOID 
FileToDrive( LPCSTR lpFileName,
             LPVOID pvFileContents,
             DWORD dwFileSize,
             DWORD dwWriteFlags );

VOID DebugOut (PCHAR szMessage,...);


HRESULT
TestWritePerBoxData(IN SOCKET s);

HRESULT
TestVerifyPerBoxData(IN SOCKET s);
 
HRESULT
SendBackEEPROMContents(IN SOCKET s);

HRESULT
LockHardDrive(IN SOCKET s, 
              IN BOOL   bLock);

HRESULT
TestRetailSystem ( IN SOCKET RecvSocket );  


