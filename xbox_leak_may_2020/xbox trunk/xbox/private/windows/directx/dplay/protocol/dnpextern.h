/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dnpextern.h
 *  Content:    This header exposes protocol entry points to the rest of Direct Network
 *@@BEGIN_MSINTERNAL
 *  History:
 *  Date		By      Reason
 *  ====		==      ======
 *  ??/??/??	ES		Created
 *	31/08/99	MJN		Changed first parameter to DNPProtocolInitialize to LPVOID
 *	03/22/00	mjn		Changed IDirectPlayAddress8 references to IDirectPlay8Address
 *  03/31/00    rmt     Added const to declarations of DNPSetProtocolCaps
 *@@END_MSINTERNAL
 *
 ***************************************************************************/


#ifndef	_DNPEXTERN_INCLUDED_
#define	_DNPEXTERN_INCLUDED_

#include "dnprot.h"

#ifdef	__cplusplus
extern	"C"
{
#endif	// __cplusplus

//	FOLLOWING FLAGS GO INTO PUBLIC HEADER FILE

#define	DN_SENDFLAGS_RELIABLE			0x00000001			// Deliver Reliably
#define	DN_SENDFLAGS_NON_SEQUENTIAL		0x00000002			// Deliver Upon Arrival
#define	DN_SENDFLAGS_HIGH_PRIORITY		0x00000004
#define	DN_SENDFLAGS_LOW_PRIORITY		0x00000008
#define	DN_SENDFLAGS_SET_USER_FLAG		0x00000040			// Protocol will deliver these two...
#define	DN_SENDFLAGS_SET_USER_FLAG_TWO	0x00000080			// ...flags to receiver

//	END OF PUBLIC FLAGS


/*
**		User Callback
**
**		This defines an all-purpose callback routine which clients
**	must provide to DNet.  I assume this will be replaced by something
**	official at a later date,  but for the time being this will
**	do the trick for debugging
**
**	parms are:  Opcode, EndPt, Data, Length, Context, Result
*/

//typedef	HRESULT	(*UserCallback)(DWORD, HANDLE, PCHAR, DWORD, LPVOID, HRESULT);
typedef struct _DN_PROTOCOL_INTERFACE_VTBL DN_PROTOCOL_INTERFACE_VTBL, *PDN_PROTOCOL_INTERFACE_VTBL;


//	USER CALLBACK OPCODES

#define	DN_USER_INDICATE_DATA					1
#define	DN_USER_INDICATE_SENDCOMPLETE			2
#define	DN_USER_INDICATE_CONNECTCOMPLETE		3
#define	DN_USER_INDICATE_NEW_CONNECTION			4
#define	DN_USER_INDICATE_CONNECTION_TERMINATED	5			// Connection has been aborted (abnormal discon)
#define	DN_USER_INDICATE_PARTNER_DISCONNECTING	6			// Orderly disconnect has been initiated
#define	DN_USER_INDICATE_DISCONNECT_COMPLETE	7			// Orderly disconnect has been completed
#define	DN_USER_INDICATE_ENUM_QUERY				8
#define	DN_USER_INDICATE_ENUM_QUERY_RESPONSE	9

//
// structure used to pass enum data from the protocol to DPlay
//
typedef	struct	_PROTOCOL_ENUM_DATA
{
	IDirectPlay8Address	*pSenderAddress;		//
	IDirectPlay8Address	*pDeviceAddress;		//
	BUFFERDESC			ReceivedData;			//
	HANDLE				hEnumQuery;				// handle of this query, returned in enum response

} PROTOCOL_ENUM_DATA;


typedef	struct	_PROTOCOL_ENUM_RESPONSE_DATA
{
	IDirectPlay8Address	*pSenderAddress;
	IDirectPlay8Address	*pDeviceAddress;
	BUFFERDESC			ReceivedData;
	DWORD				dwRoundTripTime;

} PROTOCOL_ENUM_RESPONSE_DATA;

// Pass per-endpoint data to DPlay

typedef struct _PROTOCOL_ENDPOINT_DATA
{
	UINT				uiMaxDatagramSize;				// Largest datagram allowed on this link
	UINT				uiMeasuredLatency;				// Approximate measured latency in milliseconds
	
} PROTOCOL_ENDPOINT_DATA, *PPROTOCOL_ENDPOINT_DATA;

// Protocol interface
typedef struct _DNPI_INT DNPI_INT, *PDNPI_INT;
// Protocol data
typedef	struct	protocoldata	ProtocolData, *PProtocolData;
// Protocol endpoint descriptor
typedef	struct	endpointdesc 	EPD, *PEPD;

// Service Provider interface
typedef struct IDP8ServiceProvider       IDP8ServiceProvider;
// Service Provider info data strucure
typedef	struct	_SPGETADDRESSINFODATA SPGETADDRESSINFODATA, *PSPGETADDRESSINFODATA;
// Service Provider event type
typedef enum _SP_EVENT_TYPE SP_EVENT_TYPE;

// Init/Term calls
#ifdef DPLAY_DOWORK
DWORD TimerWorkerThread(VOID);
void PeriodicTimer (void);
#endif

extern HRESULT DNPProtocolInitialize(PVOID, PProtocolData, PDN_PROTOCOL_INTERFACE_VTBL);
extern HRESULT DNPAddServiceProvider(PProtocolData, IDP8ServiceProvider*, HANDLE *);
extern HRESULT DNPRemoveServiceProvider(PProtocolData, HANDLE);
extern HRESULT DNPProtocolShutdown(PProtocolData);

// Data Transfer

extern HRESULT DNPConnect(PProtocolData, IDirectPlay8Address *const, IDirectPlay8Address *const, const HANDLE, const DWORD, const ULONG, PVOID, PHANDLE);
extern HRESULT DNPListen(PProtocolData, IDirectPlay8Address *const, const HANDLE, ULONG, PVOID, PHANDLE);
extern HRESULT DNPSendData(PProtocolData, HANDLE, UINT, PBUFFERDESC, UINT, UINT, ULONG, PVOID, PHANDLE);
extern HRESULT DNPDisconnectEndPoint(PProtocolData, HANDLE, PVOID, PHANDLE);
extern HRESULT DNPAbortEndPoint(PProtocolData, HANDLE);

// Misc Commands

extern HRESULT DNPCrackEndPointDescriptor(HANDLE, PSPGETADDRESSINFODATA);
extern HRESULT DNPCancelCommand(PProtocolData, HANDLE);
//extern HRESULT DNPSetEndPointContext(HANDLE, PVOID);

extern HRESULT DNPEnumQuery( PProtocolData pPData,
							 IDirectPlay8Address *const pHostAddress,
							 IDirectPlay8Address *const pDeviceAddress,
							 const HANDLE,
							 BUFFERDESC *const pBuffers,
							 const DWORD dwBufferCount,
							 const DWORD dwRetryCount,
							 const DWORD dwRetryInterval,
							 const DWORD dwTimeout,
							 const DWORD dwFlags,
							 void *const pUserContext,
							 HANDLE *const pCommandHandle );

extern HRESULT DNPEnumRespond( PProtocolData pPData,
						       const HANDLE hSPHandle,
							   const HANDLE hQueryHandle,				// handle of enum query being responded to
							   BUFFERDESC *const pResponseBuffers,	
							   const DWORD dwResponseBufferCount,
							   const DWORD dwFlags,
							   void *const pUserContext,
							   HANDLE *const pCommandHandle );

extern HRESULT DNPReleaseReceiveBuffer(HANDLE);

extern HRESULT DNPGetListenAddressInfo(HANDLE hCommand, PSPGETADDRESSINFODATA pSPData);
extern HRESULT DNPGetEPCaps(HANDLE, PDPN_CONNECTION_INFO);
extern HRESULT DNPSetProtocolCaps(PProtocolData pPData, const DPN_CAPS * const pData);
extern HRESULT DNPGetProtocolCaps(PProtocolData pPData, PDPN_CAPS pData);

extern HRESULT WINAPI DNP_Debug(ProtocolData *, UINT OpCode, HANDLE EndPoint, PVOID Data);

//	Lower Edge Entries

extern HRESULT WINAPI DNSP_IndicateEvent(IDP8SPCallback *, SP_EVENT_TYPE, PVOID);
extern HRESULT WINAPI DNSP_CommandComplete(IDP8SPCallback *, HANDLE, HRESULT, PVOID);

//	PLAY MODE
//
//	I am defining these here for the time being.  I figure these will be defined
//	elsewhere eventually

#define		DN_PLAYMODE_CLIENT				0
#define		DN_PLAYMODE_SERVER				1
#define		DN_PLAYMODE_PEER				2


//	NEW V-TABLE FOR CALLS INTO CORE LAYER

typedef HRESULT (*PFN_PINT_INDICATE_LISTEN_TERMINATED)
	(void *const pvUserContext,
	void *const pvEndPtContext,
	const HRESULT hr);
typedef HRESULT (*PFN_PINT_INDICATE_ENUM_QUERY)
	(void *const pvUserContext,
	void *const pvEndPtContext,
	const HANDLE hCommand,
	void *const pvEnumQueryData,
	const DWORD dwEnumQueryDataSize);
typedef HRESULT (*PFN_PINT_INDICATE_ENUM_RESPONSE)
	(void *const pvUserContext,
	const HANDLE hCommand,
	void *const pvCommandContext,
	void *const pvEnumResponseData,
	const DWORD dwEnumResponseDataSize);
typedef HRESULT (*PFN_PINT_INDICATE_CONNECT)
	(void *const pvUserContext,
	void *const pvListenContext,
	const HANDLE hEndPt,
	void **const ppvEndPtContext);
typedef HRESULT (*PFN_PINT_INDICATE_DISCONNECT)
	(void *const pvUserContext,
	void *const pvEndPtContext);
typedef HRESULT (*PFN_PINT_INDICATE_CONNECTION_TERMINATED)
	(void *const pvUserContext,
	void *const pvEndPtContext,
	const HRESULT hr);
typedef HRESULT (*PFN_PINT_INDICATE_RECEIVE)
	(void *const pvUserContext,
	void *const pvEndPtContext,
	void *const pvData,
	const DWORD dwDataSize,
	const HANDLE hBuffer,
	const DWORD dwFlags);
typedef HRESULT (*PFN_PINT_COMPLETE_LISTEN)
	(void *const pvUserContext,
	void **const ppvCommandContext,
	const HRESULT hr,
	const HANDLE hEndPt);
typedef HRESULT (*PFN_PINT_COMPLETE_LISTEN_TERMINATE)
	(void *const pvUserContext,
	void *const pvCommandContext,
	const HRESULT hr);
typedef HRESULT (*PFN_PINT_COMPLETE_ENUM_QUERY)
	(void *const pvUserContext,
	void *const pvCommandContext,
	const HRESULT hr);
typedef HRESULT (*PFN_PINT_COMPLETE_ENUM_RESPONSE)
	(void *const pvUserContext,
	void *const pvCommandContext,
	const HRESULT hr);
typedef HRESULT (*PFN_PINT_COMPLETE_CONNECT)
	(void *const pvUserContext,
	void *const pvCommandContext,
	const HRESULT hr,
	const HANDLE hEndPt,
	void **const ppvEndPtContext);
typedef HRESULT (*PFN_PINT_COMPLETE_DISCONNECT)
	(void *const pvUserContext,
	void *const pvCommandContext,
	const HRESULT hr);
typedef HRESULT (*PFN_PINT_COMPLETE_SEND)
	(void *const pvUserContext,
	void *const pvCommandContext,
	const HRESULT hr);
typedef	HRESULT	(*PFN_PINT_ADDRESS_INFO_CONNECT)
	(void *const pvUserContext,
	 void *const pvCommandContext,
	 const HRESULT hr,
	 IDirectPlay8Address *const pHostAddress,
	 IDirectPlay8Address *const pDeviceAddress );
typedef	HRESULT	(*PFN_PINT_ADDRESS_INFO_ENUM)
	(void *const pvUserContext,
	 void *const pvCommandContext,
	 const HRESULT hr,
	 IDirectPlay8Address *const pHostAddress,
	 IDirectPlay8Address *const pDeviceAddress );
typedef	HRESULT	(*PFN_PINT_ADDRESS_INFO_LISTEN)
	(void *const pvUserContext,
	 void *const pvCommandContext,
	 const HRESULT hr,
	 IDirectPlay8Address *const pDeviceAddress );

struct _DN_PROTOCOL_INTERFACE_VTBL
{
	PFN_PINT_INDICATE_LISTEN_TERMINATED		IndicateListenTerminated;
	PFN_PINT_INDICATE_ENUM_QUERY			IndicateEnumQuery;
	PFN_PINT_INDICATE_ENUM_RESPONSE			IndicateEnumResponse;
	PFN_PINT_INDICATE_CONNECT				IndicateConnect;
	PFN_PINT_INDICATE_DISCONNECT			IndicateDisconnect;
	PFN_PINT_INDICATE_CONNECTION_TERMINATED	IndicateConnectionTerminated;
	PFN_PINT_INDICATE_RECEIVE				IndicateReceive;
	PFN_PINT_COMPLETE_LISTEN				CompleteListen;
	PFN_PINT_COMPLETE_LISTEN_TERMINATE		CompleteListenTerminate;
	PFN_PINT_COMPLETE_ENUM_QUERY			CompleteEnumQuery;
	PFN_PINT_COMPLETE_ENUM_RESPONSE			CompleteEnumResponse;
	PFN_PINT_COMPLETE_CONNECT				CompleteConnect;
	PFN_PINT_COMPLETE_DISCONNECT			CompleteDisconnect;
	PFN_PINT_COMPLETE_SEND					CompleteSend;
	PFN_PINT_ADDRESS_INFO_CONNECT			AddressInfoConnect;
	PFN_PINT_ADDRESS_INFO_ENUM				AddressInfoEnum;
	PFN_PINT_ADDRESS_INFO_LISTEN			AddressInfoListen;
};

#ifdef	__cplusplus
}
#endif	// __cplusplus

#endif

