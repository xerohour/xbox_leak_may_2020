/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    slixboxsock.cpp

Abstract:

    declares networking functions for slixbox to communicate with slix2k

Author:

    Jason Gould (a-jasgou) June 2000

--*/
#ifndef __USBXBTransfer_h_
#define __USBXBTransfer_h_

#define NODSOUND
#include <xtl.h>
#include "..\inclib\USBXBTransfer.h" 

#ifdef __cplusplus
extern "C" {
#include <winsockx.h>
#endif
	ULONG DebugPrint(PCHAR Format, ...);	//prints stuff to the debugger computer
    DWORD WINAPI TestMain();              //our entry-point
	int WINAPI wWinMain(HINSTANCE hi, HINSTANCE hprev, LPSTR lpcmd, int nShow);
#ifdef __cplusplus
}
#endif
//int ssSend(SOCKET hsock, TRANSFERDATA * sockdata, int tosend);
//int ssRecv(SOCKET hsock, TRANSFERDATA  * sockdata, int maxrecv, int timeout = RECV_TIMEOUT);
int ssCloseTCP(SOCKET hsock, TRANSFERDATA * inbuf, int len);
SOCKET ssInitServer();
SOCKET ssInitEventServer();

#endif
