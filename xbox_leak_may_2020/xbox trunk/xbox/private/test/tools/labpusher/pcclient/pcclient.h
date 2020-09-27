/*************************************************************************************************

  Copyright (c) 2001 Microsoft Corporation

  Module Name:

	pcclient.h

  Abstract:

	Defines the PC client side for the LabPusher tool

  Author:

	Mabel Santos (t-msanto) 03-Dec-2001

  Revision History:

*************************************************************************************************/

#ifndef _PCCLIENT_H_
#define _PCCLIENT_H_

#define NTLEANANDMEAN
#define _X86_
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <winsock2.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <xboxdbg.h>

#define BUFFLEN 1024

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

struct Xbox
{
	char Name[BUFFLEN];
	int State;
	int Thread;
	Xbox* Next;
};

class PcClient
{
private:
	
	// list of Xboxes
	Xbox* XboxList;
	Xbox* LastXbox;
	
	// socket for network communication
	SOCKET ClientSock;

	// flag to indicate whether WinSock has been initialized
	BOOL WsaInitialized;

	// byte counter
	unsigned long ByteCount;

	// packet counter
	unsigned long PacketCount;

public:
	PcClient(void);
	~PcClient(void);
	BOOL PrepareWinSock(void);
	BOOL PrepareXboxes(void);	
	BOOL CheckExclList(char* List, char* Name);
	BOOL GetTotalBytes(void);
	void GetFileBytes(char* File);
	BOOL SendQuery(int QueryType);
	BOOL SendFiles(void);
	BOOL SendFile(char* FileName);
	BOOL SendKernel(void);
	BOOL SendFlashCmd(void);
    BOOL SendFinishCmd(void);
	void RebootXboxes(void);	
	void ReportResults(void);
	BOOL SendPacket(BYTE PacketType, DWORD FileSize, WORD DataSize, char* Data);
	DWORD GetCRC(unsigned char* Data, WORD Size);
	void AddXbox(char* Name, int Thread);
	void SetXboxState(char* Name, int State);
	void GetXboxes(int State);
	void ClearXboxes(void);
	
	friend DWORD WINAPI PrepareXboxThread(void* ClientPtr);
	friend DWORD WINAPI RebootXboxThread(void* ClientPtr);

};

struct ThreadParam 
{
	PcClient* Client;
	int ThreadNum;
};

void StringTrim(char* String);
void LogPrint(char* Format, ...);

#endif