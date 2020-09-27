/*************************************************************************************************

  Copyright (c) 2001 Microsoft Corporation

  Module Name:

	xbserver.h

  Abstract:

	Defines the Xbox server side for the GamePusher tool.

  Author:

	Mabel Santos (t-msanto) 03-Dec-2001

  Revision History:

*************************************************************************************************/

#ifndef _XBSERVER_H_
#define _XBSERVER_H_

#define NTLEANANDMEAN
#define _X86_
#define _WIN32_WINNT 0x0500

#define _HAL_
#define _MM_
#define _IO_
#define _FSCACHE_

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntexapi.h>
#include <scsi.h>
#include <ntddscsi.h>
#include <ntos.h>
#include <xtl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <xbdm.h>
#include <winsockx.h>
#include "..\..\..\..\ntos\recovery\recovpch.h"
#include "..\..\dvdread\bitfont.h"

#include <assert.h>
#include <D3DX8Math.h>
#ifdef _XBOX
#include <xgraphics.h>
#endif

#define PORT 2000
#define BUFFLEN 1024
#define ROM_SIZE (1024 * 1024)
#define ROM_FLASH_RETRIES 25

enum PacketTypes{ FILEDESC = 1, FILEDATA, FILEDONE };
enum ErrorCodes{ PKTERROR = 1, CHKSMERROR, DIRERROR, FILEERROR, KRNLERROR, FLASHERROR };

// network packet structures

struct FilePacketHeader
{
	unsigned char PacketType;
	unsigned long SequenceNo;
	unsigned long FileSize;
	unsigned short DataSize;
	unsigned long Crc;
};

struct FilePacket : public FilePacketHeader
{
	char Data[BUFFLEN];
};

class XbServer
{
private:
	// socket for network communication
	SOCKET ServerSock;

	// flag to indicate whether WinSock has been initialized
	BOOL WsaInitialized;

	// flag to indicate whether an error has occurred
	int ErrorFlag;

	// flag to indicate when a successful ROM flash has occurred
	BOOL ExitFlag;

	// total number of bytes to be received
	unsigned long TotalBytes;

	// number of bytes received so far
	unsigned long RecvdBytes;

	// sequence number of current file's description packet
	DWORD BasePkt;

	// number of packets needed for current file
	DWORD NumPkts;
	
	// number of packets received for current file
	DWORD RecvdPkts;

	// stream for writing files to hard drive
	FILE* OutFile;

	// buffer for holding Xbox name
	char NameBuf[BUFFLEN];

public:
	XbServer(void);
	~XbServer(void);
	BOOL PrepareWinSock(void);
	void MapDrives(void);
	BOOL SendReply(void);
	void RecvFiles(void);
	void ReadPacket(FilePacket* Packet);
	BOOL CheckPkts(void);
	void StartFile(DWORD FileSize, DWORD PktNum);
	BOOL CreateDirs(char* PathName);
	void UnMapDrives(void);
	void FlashLED(void);
	DWORD GetCRC(unsigned char* Data, WORD Size);
};

extern XbServer* Server;
extern int DebugOn;

// debug output
void DebugPrint(char* ptszFormat, ...);

// setting LED colors
NTSTATUS WriteSMC(unsigned char addr, unsigned char value);

// mapping drives
DWORD MapDrive( char cDriveLetter, char* pszPartition );
DWORD UnMapDrive( char cDriveLetter );

// graphics output
WCHAR* Ansi2UnicodeHack(char *str);
HRESULT InitGraphics(void);
void GraphicPrint(char* format, ...);

extern IDirect3D8*		    g_d3d;
extern IDirect3DDevice8*	g_pDevice;
extern BitFont				g_font;
extern IDirect3DTexture8*	g_pd3dtText;
extern TVertex				g_prText[4];
extern D3DLOCKED_RECT		g_d3dlr;

#endif