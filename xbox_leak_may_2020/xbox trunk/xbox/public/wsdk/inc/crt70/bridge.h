// This source code is only intended as a supplement to the
// Broadcast Architecture Programmer's Reference.
// See the reference for detailed information regarding
// Broadcast Architecture.
//
// This file contains preliminary documentation and subject to change

#ifndef BRIDGE_H
#define BRIDGE_H

#include "packet.h"

#ifndef	EXTERN_C
#ifdef	__cplusplus
#define	EXTERN_C	extern "C"
#else
#define	EXTERN_C
#endif
#endif

#define	MSBDN_BRIDGE_CALLBACKS_VERSION	0x00020000U

struct	PACKET_LIST_ENTRY {
	LIST_ENTRY		ListEntry;
	PACKET_BUFFER *	PacketBuffer;
};

struct MSBDN_OUTPUT_SUBSYSTEM;

typedef DWORD MSBDN_SUBSYSTEM_ID;
typedef	MSBDN_OUTPUT_SUBSYSTEM MSBDN_OUTPUT_SUBSYSTEM;

typedef struct MSBDN_BRIDGE_CALLBACKS {
	DWORD	Version;
	HRESULT	(*ReportState)			(MSBDN_OUTPUT_SUBSYSTEM *, DWORD state, LPCSTR message);
	HRESULT	(*ReportActivity)		(MSBDN_OUTPUT_SUBSYSTEM *, WORD type, DWORD amount);
	HRESULT	(*ReportEvent)			(MSBDN_OUTPUT_SUBSYSTEM *, WORD, DWORD, LPCWSTR);
	HRESULT (*GetNextPacket)		(MSBDN_OUTPUT_SUBSYSTEM *, PACKET_BUFFER **);
	HRESULT	(*PacketListEntryAlloc)	(PACKET_LIST_ENTRY **);
	HRESULT	(*PacketListEntryFree)	(PACKET_LIST_ENTRY *);
} MSBDN_BRIDGE_CALLBACKS;

// this is analogous to the kernel's DEVICE_OBJECT, kind of.
struct MSBDN_OUTPUT_SUBSYSTEM {
	DWORD						Version;
	MSBDN_BRIDGE_CALLBACKS		BridgeCallbacks;
	MSBDN_SUBSYSTEM_ID			OutputSubsystemID;
	HKEY						RegistryKey;
	LPVOID						DriverContext;
};

#define	MSBDN_OUTPUT_SUBSYSTEM_VERSION_1	1
#define	MSBDN_OUTPUT_SUBSYSTEM_VERSION		MSBDN_OUTPUT_SUBSYSTEM_VERSION_1

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef MSBDNOUTPUTAPI
#define MSBDNOUTPUTAPI __declspec (dllexport)
#endif

#ifndef MSBDNBRIDGEAPI
#define MSBDNBRIDGEAPI __declspec (dllimport)
#endif

// for backward combustibility, i mean compatability
typedef PACKET_BUFFER * HPACKET;


// this is the second parameter in msbdnBridgeReportState
#define MSBDN_OUTPUT_DISABLED   0
#define MSBDN_OUTPUT_ENABLED    1
#define MSBDN_OUTPUT_FORCED     2
#define MSBDN_OUTPUT_WARNING    10

// this is the first parameer in msbdnBridgeReportActivity
#define MSBDN_REPORT_RATE       100
#define MSBDN_REPORT_OUTPUT     101
#define MSBDN_REPORT_ERROR      102
#define MSBDN_REPORT_TIMEOUT    103     // dwOutputTimeouts
#define MSBDN_REPORT_FULLFIFO   104     // dwOutputFullFifo

#define	MSBDN_OUTPUT_API_VERSION	0x00020000

#define	MSBDN_OUTPUT_VALUE_STRING		0
#define	MSBDN_OUTPUT_VALUE_DWORD		1
#define	MSBDN_OUTPUT_VALUE_IPADDRESS	2
#define	MSBDN_OUTPUT_VALUE_BOOLEAN		3

typedef struct MSBDN_OUTPUT_VALUE {
	DWORD	Index;
	DWORD	Type;
	LPWSTR	Name;
	union {
		DWORD	Dword;
		LPWSTR	String;
		IN_ADDR	IPAddress;
		BOOL	Boolean;
	};
} MSBDN_OUTPUT_VALUE;

typedef	HRESULT	(*PMSBDNOUTPUTCREATE)			(MSBDN_OUTPUT_SUBSYSTEM *, DWORD);
typedef	HRESULT	(*PMSBDNOUTPUTDESTROY)			(MSBDN_OUTPUT_SUBSYSTEM *);
typedef	HRESULT	(*PMSBDNOUTPUTSETHWADDR)		(MSBDN_OUTPUT_SUBSYSTEM *, LPBYTE, LPBYTE, LPWORD);
typedef	HRESULT	(*PMSBDNOUTPUTSETBANDWIDTH)		(MSBDN_OUTPUT_SUBSYSTEM *, DWORD);
typedef	HRESULT	(*PMSBDNOUTPUTSETROUTINGINFO)	(MSBDN_OUTPUT_SUBSYSTEM *, LPVOID);
typedef	HRESULT	(*PMSBDNOUTPUTSTREAMSTATUS)		(MSBDN_OUTPUT_SUBSYSTEM *, WORD, LPBYTE, BOOL);
typedef	HRESULT	(*PMSBDNOUTPUTSENDPACKET)		(MSBDN_OUTPUT_SUBSYSTEM *, PACKET_BUFFER *);
typedef	HRESULT	(*PMSBDNOUTPUTSETVALUE)			(MSBDN_OUTPUT_SUBSYSTEM *, MSBDN_OUTPUT_VALUE *);
typedef	HRESULT	(*PMSBDNOUTPUTGETVALUE)			(MSBDN_OUTPUT_SUBSYSTEM *, MSBDN_OUTPUT_VALUE *);
typedef	HRESULT	(*PMSBDNOUTPUTGETVALUECOUNT)	(MSBDN_OUTPUT_SUBSYSTEM *, DWORD *);
typedef	HRESULT	(*PMSBDNOUTPUTFREEBUFFER)		(MSBDN_OUTPUT_SUBSYSTEM *, LPVOID);
typedef	HRESULT	(*PMSBDNOUTPUTGETSTATUS)		(MSBDN_OUTPUT_SUBSYSTEM *, HRESULT *);

#ifdef __cplusplus
}
#endif // __cplusplus


// decorated names for GetProcAddress
#define	MSBDN_PROC_MSBDNOUTPUTCREATE			"msbdnOutputCreate"
#define	MSBDN_PROC_MSBDNOUTPUTDESTROY			"msbdnOutputDestroy"
#define	MSBDN_PROC_MSBDNOUTPUTGETSTATUS			"msbdnOutputGetStatus"
#define	MSBDN_PROC_MSBDNOUTPUTSENDPACKET		"msbdnOutputSendPacket"
#define	MSBDN_PROC_MSBDNOUTPUTSETVALUE			"msbdnOutputSetValue"
#define	MSBDN_PROC_MSBDNOUTPUTGETVALUE			"msbdnOutputGetValue"
#define	MSBDN_PROC_MSBDNOUTPUTGETVALUECOUNT		"msbdnOutputGetValueCount"
#define	MSBDN_PROC_MSBDNOUTPUTFREEBUFFER		"msbdnOutputFreeBuffer"


#ifdef	IS_BRIDGE_OUTPUT_DRIVER
// code for VIF driver DLLs

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef	MRVIFAPI
#define	MRVIFAPI	__declspec (dllexport)
#endif

MRVIFAPI HRESULT msbdnOutputCreate			(MSBDN_OUTPUT_SUBSYSTEM *, DWORD);
MRVIFAPI HRESULT msbdnOutputDestroy			(MSBDN_OUTPUT_SUBSYSTEM *);
MRVIFAPI HRESULT msbdnOutputSendPacket		(MSBDN_OUTPUT_SUBSYSTEM *, PACKET_BUFFER *);
MRVIFAPI HRESULT msbdnOutputSetValue		(MSBDN_OUTPUT_SUBSYSTEM *, MSBDN_OUTPUT_VALUE *);
MRVIFAPI HRESULT msbdnOutputGetValue		(MSBDN_OUTPUT_SUBSYSTEM *, MSBDN_OUTPUT_VALUE *);
MRVIFAPI HRESULT msbdnOutputGetValueCount	(MSBDN_OUTPUT_SUBSYSTEM *, DWORD *);
MRVIFAPI HRESULT msbdnOutputFreeBuffer		(MSBDN_OUTPUT_SUBSYSTEM *, LPVOID);
MRVIFAPI HRESULT msbdnOutputGetStatus		(MSBDN_OUTPUT_SUBSYSTEM *, HRESULT *);

#ifdef __cplusplus
}
#endif // __cplusplus

#if 0
// driver DLL is expected to provide the instance
extern	MSBDN_BRIDGE_CALLBACKS	g_BridgeCallbacks;
#endif

static __inline DWORD msbdnPacketLength (PACKET_BUFFER * packet)
{
	return packet -> End - packet -> Start;
}

static __inline DWORD msbdnPacketMemcpy (LPVOID dest, PACKET_BUFFER * packet, DWORD pos, DWORD length)
{
	DWORD	buffer_length = packet -> End - packet -> Start;

	if (pos > buffer_length)
		return 0;

	if (pos + length > buffer_length)
		length = buffer_length - pos;

	memcpy (dest, packet -> Data + pos, length);
	return length;
}

static __inline PACKET_BUFFER * msbdnPacketGetData (PACKET_BUFFER * packet, LPBYTE * ret_data, LPDWORD ret_length)
{
	if (ret_length) *ret_length = packet -> End - packet -> Start;
	if (ret_data) *ret_data = packet -> Data;
	return NULL;
}

static __inline HRESULT msbdnBridgeReportState (MSBDN_OUTPUT_SUBSYSTEM * subsystem, DWORD state, LPCSTR message)
{
	return (*subsystem -> BridgeCallbacks.ReportState) (subsystem, state, message);
}

static __inline HRESULT msbdnBridgeReportActivity (MSBDN_OUTPUT_SUBSYSTEM * subsystem, WORD type, DWORD amount)
{
	return (*subsystem -> BridgeCallbacks.ReportActivity) (subsystem, type, amount);
}

static __inline HRESULT msbdnBridgeReportEvent (MSBDN_OUTPUT_SUBSYSTEM * subsystem, WORD type, DWORD error_code, LPCWSTR message)
{
	return (*subsystem -> BridgeCallbacks.ReportEvent)
		(subsystem, type, error_code, message);
}

static __inline HRESULT msbdnBridgeGetNextPacket (MSBDN_OUTPUT_SUBSYSTEM * subsystem, PACKET_BUFFER ** packet)
{
	return (*subsystem -> BridgeCallbacks.GetNextPacket)
		(subsystem, packet);
}

#endif // IS_BRIDGE_OUTPUT_DRIVER

#endif // BRIDGE_H
