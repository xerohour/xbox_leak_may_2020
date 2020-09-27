#ifndef __SLDGMAIN_H__
#define __SLDGMAIN_H__

//****************
//*** Includes ***
//****************

#include <windows.h>
#include <tchar.h>
#include <winsock.h>

//***************
//*** Defines ***
//***************

#define MAX_CMDLINE_SIZE			100
#define SW_SHOWNORMAL				1
#define ROLLOVER_LIMIT				1000000

//*********************************
//*** Suggested log verbosities ***
//*********************************
#define LOG_EXCEPTION				0
#define LOG_FAIL					2
#define LOG_ABORT					4
#define LOG_SKIP					6
#define LOG_NOT_IMPLEMENTED			8
#define LOG_PASS					10
#define LOG_DETAIL					12
#define LOG_COMMENT					14

#define BUFF_PATTERN_REPEAT_CHAR
#define UDP_USE_SELECT

#define TIMEOUT_SECONDS				10
#define TIMEOUT_USECONDS			0

#define DEATH_BY_HATCHET			1
#define RECEIVE_TIMED_OUT			2
#define SLEEP_BEFORE_RECV_MS		5000
#define CLIENT_PERCENT_SLEEPY_RECVS	2
#define SERV_PERCENT_SLEEPY_RECVS	1
#define INFINITE_CYCLES				1000
#define UDP_SLEEP_BEFORE_NEXT		1000
#define TCP_SERV_PORT				2001
#define UDP_SERV_PORT				2112
#define UDP_BASE_CLIENT_PORT		UDP_SERV_PORT
#define SERV_TCP					FALSE
#define SERV_UDP					FALSE
#define SERV_TCP_ADDR				INADDR_LOOPBACK
#define SERV_UDP_ADDR				INADDR_LOOPBACK
#define SERV_RECV_LEN				64
#define CLIENT_TCP_CONNS			0
#define CLIENT_TCP_CYCLES			0
#define CLIENT_UDP_CONNS			0
#define CLIENT_UDP_CYCLES			0
#define MIN_PACKET_LEN				1
#define MAX_PACKET_LEN				2048
#define PACKETS_PER_CONN			25
#define IN_FLIGHT_CLOSES			FALSE
#define SERV_BACKLOG				10
#define PACKET_MAX_LEN				4096
#define	MAX_PACKETS_IN_FLIGHT		512

#define CLIENT_TCP_CONNECT_RETRIES	5

#define EAT_WHITE_SPACE(ptr) while((ptr != NULL) && _istspace(*ptr)) ptr++

//****************
//*** Typedefs ***
//****************

typedef struct 
{
	int     Len;
	int     Index;
	ULONG   CheckSum;
} PACKETS;

typedef enum
{
    CONN_TCP,
    CONN_UDP,
    CONN_IDLE
} CONN_TYPES;

typedef enum
{
	CONSTANT,
	INCREMENT,
	RANDOM
} PACKET_TYPE;

typedef struct
{
    BOOL		ServTCP;
    BOOL		ServUDP;
    ULONG		ServTCPAddr;
    ULONG		ServUDPAddr;
    int			ServRecvLen;
    BOOL		ClientTCPConns;
    int			ClientTCPCycles;
    BOOL		ClientUDPConns;
    int			ClientUDPCycles;
    int			TCPMinPacketLen;
    int			TCPMaxPacketLen;
    int			UDPMinPacketLen;
    int			UDPMaxPacketLen;
    int			PacketsPerConn;
    int			ClientSleepyRecvs;
    int			ServSleepyRecvs;
	int			UDPSleepBeforeNext;
    BOOL		InFlightCloses;
} SLEDGE_PARMS;

typedef struct 
{
    CONN_TYPES	ConnType;
    int			Index; 
    int			Cycles;
    int			Cycle;
    BOOL		Connected;
    BOOL		SendThreadDown;
    BOOL		RecvThreadDown;
    PACKETS		PacketsOut[MAX_PACKETS_IN_FLIGHT];   
    int			PacketsOutHead;
    int			PacketsOutTail;
    int			PacketsOutCount;
    HANDLE		hPacketsOutEvent;
    HANDLE		hPort;
    SOCKET		Sock;
    int			PacketLenMin;
    int			PacketLenMax;
    ULONG		TotalPacketsOut;
    ULONG		TotalBytesOut;
    ULONG		DeltaBytesOut;
    ULONG		TotalPacketsIn;
    ULONG		TotalBytesIn;
    ULONG		DeltaBytesIn;
	ULONG		TotalLatePackets;
	ULONG		TotalLostPackets;
	ULONG		TotalBadPackets;
    BOOL		SleepyRecv;
    BOOL		InSend;
    BOOL		InRecv;
    BOOL		InClose;
    char		NextChar;
} CLIENT_CONNS;

typedef struct 
{
    CONN_TYPES	ConnType;
    int			Index;
    HANDLE		hPort;
    SOCKET		Sock;
    int			PacketLenMin;
    int			PacketLenMax;
    ULONG		TotalPacketsLoop;
    ULONG		TotalBytesLoop;
    ULONG		DeltaBytesLoop;
    BOOL		SleepyRecv;
    BOOL		InSend;
    BOOL		InRecv;
    BOOL		InClose;
} SERV_CONNS;

typedef struct
{
	SERV_CONNS	*pConn;
} SERV_RECV_THREAD_PARAM;

typedef struct
{
	CLIENT_CONNS	*pConn;
} CLIENT_THREAD_PARAM;

//******************
//*** Prototypes ***
//******************
void  usage();
BOOL ParseConfigFile();
DWORD WINAPI HatchetThread(LPVOID *);
DWORD WINAPI DisplayThread(LPVOID *);
DWORD WINAPI TCPServThread(LPVOID *);
DWORD WINAPI UDPServThread(LPVOID *);
DWORD WINAPI ServRecvThread(LPVOID *);
DWORD WINAPI ClientSendThread(LPVOID *);
DWORD WINAPI ClientRecvThread(LPVOID *);
void OpenConn(CLIENT_CONNS *);
void CloseConn(CLIENT_CONNS *);
int RecvPacketSTREAM(CLIENT_CONNS *pConn, int Len, BYTE Buff[], BOOL);
int SendPacketSTREAM(CLIENT_CONNS *, int, BYTE *);
int RecvPacketDGRAM(CLIENT_CONNS *, int *, BYTE *);
int SendPacketDGRAM(CLIENT_CONNS *, int, BYTE *);
ULONG RandomRange(ULONG, ULONG);
ULONG BuildTestPacket(CLIENT_CONNS *, int, BYTE *, int);
ULONG CheckSum(int, BYTE *);
TCHAR *GetLastErrorText();
#ifdef UNDER_CE
void ExitProcess(UINT);
#endif

#endif // __SLDGMAIN_H__
