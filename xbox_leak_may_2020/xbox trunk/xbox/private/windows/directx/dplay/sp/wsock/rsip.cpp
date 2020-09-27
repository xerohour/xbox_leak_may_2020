/*==========================================================================
 *
 *  Copyright (C) 1999 - 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Rsip.cpp
 *  Content:	Realm Specific IP Support
 *
 *  History:
 *  Date		By		Reason
 *  ====		==		======
 *  12/7/99		aarono	Original
 *  03/17/2000	johnkan	Converted to class for DPlay8
 *
 *  Notes:
 *
 *  Could optimize the building of messages with pre-initialized
 *  structures for each command, since most of the command is the same
 *  on every request anyway.
 *
 ***************************************************************************/


#include "wsockspi.h"

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_WSOCK

//**********************************************************************
// Constant definitions
//**********************************************************************

/*
 * Whats implemented:
 *  as_req_rsap(localaddr=0,localports=4-a:b:c:d,
 *              remaddr=0,remports=0)
 *  as_res_rsap(localaddr=24.128.34.21,localports=4-A:B:C:D,
 *              remaddr=0,remports=0)
 *  where a,b,c,d are listening ports on the client (192.168.0.2)
 *        and A,B,C,D are the associated ports visible
 *        on the external IP address 24.128.34.21.
 *
 * Operation: When packet from the outside is destined for
 *            24.128.34.21:A is recieved, the ip->dst and udp->dst
 *            (or tcp->dst) fields are changed to 192.168.0.2:a
 *            and then sent along - unencapsulated.
 *            When a packet is sent out from the client, the
 *            app (e.g. dplay) uses 192.168.0.2:a as in the
 *            IP+UDP/TCP header but uses 24.128.34.21:A for
 *            any information encoded in the data portion of
 *            the packet.
 */
#define RSIP_VERSION 1
#define RSIP_HOST_PORT 2234

// How often we scan the list for liscence renewals
#define RSIP_RENEW_TEST_INTERVAL 60000

/* start RSIP retry timer at 12.5 ms */
#define RSIP_tuRETRY_START	12500
/* max starting retry is 100 ms */
#define RSIP_tuRETRY_MAX	100000

#define RESP_BUF_SIZE	100

enum { /* TUNNELS */
  TUNNEL_RESERVED = 0,
  TUNNEL_IP_IP = 1,
  TUNNEL_GRE = 2, /* PPTP */
  TUNNEL_L2TP = 3,
  /*TUNNEL_NONE = 4, /* THIS IS NOT PART OR THE SPEC */
};
enum { /* METHODS */
  METHOD_RESERVED = 0,
  RSA_IP = 1,
  RSAP_IP = 2,
  RSA_IP_IPSEC = 3,
  RSAP_IP_IPSEC = 4,
};
enum { /* FLOWS */
  FLOW_RESERVED = 0,
  FLOW_MACRO = 1,
  FLOW_MICRO = 2,
  FLOW_NONE = 3,
};
enum { /* ERROR CODES */
  UNKNOWNERROR = 1,
  BADBINDID = 2,
  BADCLIENTID = 3,
  MISSINGPARAM = 4,
  DUPLICATEPARAM = 5,
  ILLEGALPARAM = 6,
  ILLEGALMESSAGE = 7,
  REGISTERFIRST = 8,
  BADMESSAGEID = 9,
  ALREADYREGISTERED = 10,
  ALREADYUNREGISTERED = 11,
  BADTUNNELTYPE = 12,
  ADDRUNAVAILABLE = 13,
  PORTUNAVAILABLE = 14,
};

enum { /* MESSAGES */
  RSIP_ERROR_RESPONSE = 1,
  RSIP_REGISTER_REQUEST = 2,
  RSIP_REGISTER_RESPONSE = 3,
  RSIP_DEREGISTER_REQUEST = 4,
  RSIP_DEREGISTER_RESPONSE = 5,
  RSIP_ASSIGN_REQUEST_RSA_IP = 6,
  RSIP_ASSIGN_RESPONSE_RSA_IP = 7,
  RSIP_ASSIGN_REQUEST_RSAP_IP = 8,
  RSIP_ASSIGN_RESPONSE_RSAP_IP = 9,
  RSIP_EXTEND_REQUEST = 10,
  RSIP_EXTEND_RESPONSE = 11,
  RSIP_FREE_REQUEST = 12,
  RSIP_FREE_RESPONSE = 13,
  RSIP_QUERY_REQUEST = 14,
  RSIP_QUERY_RESPONSE = 15,
  RSIP_DEALLOCATE = 16,
  RSIP_OK = 17,
  RSIP_LISTEN_REQUEST = 18,
  RSIP_LISTEN_RESPONSE = 19,
};
enum { /* PARAMETERS */
  RSIP_ADDRESS_CODE = 1,
  RSIP_PORTS_CODE = 2,
  RSIP_LEASE_CODE = 3,
  RSIP_CLIENTID_CODE = 4,
  RSIP_BINDID_CODE = 5,
  RSIP_MESSAGEID_CODE = 6,
  RSIP_TUNNELTYPE_CODE = 7,
  RSIP_RSIPMETHOD_CODE = 8,
  RSIP_ERROR_CODE = 9,
  RSIP_FLOWPOLICY_CODE = 10,
  RSIP_VENDOR_CODE = 11,
};

/*
 * MS specific Vendor Codes
 */
#define RSIP_MS_VENDOR_ID 734
enum {
  RSIP_NO_TUNNEL = 1,
  RSIP_TCP_PORT = 2,
  RSIP_UDP_PORT = 3,
  RSIP_SHARED_UDP_LISTENER = 4,
  RSIP_QUERY_MAPPING = 5,
};

#define	ANY_PORT	0

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

#pragma pack(push,1)

typedef struct _RSIP_MSG_HDR {
	CHAR	version;
	CHAR	msgtype;
} RSIP_MSG_HDR, *PRSIP_MSG_HDR;

typedef struct _RSIP_PARAM {
	CHAR	code;
	WORD	len;
} RSIP_PARAM, *PRSIP_PARAM;

typedef struct _RSIP_MESSAGEID{
	CHAR	code;
	WORD	len;
	DWORD   msgid;	
} RSIP_MESSAGEID, *PRSIP_MESSAGEID;

typedef struct _RSIP_CLIENTID {
	CHAR 	code;
	WORD	len;
	DWORD	clientid;
} RSIP_CLIENTID, *PRSIP_CLIENTID;

typedef struct _RSIP_ADDRESS {
	CHAR 	code;
	WORD	len;
	CHAR    version;	// 1==v4
	DWORD	addr;
} RSIP_ADDRESS, *PRSIP_ADDRESS;

typedef struct _RSIP_PORT {
	CHAR	code;
	WORD 	len;
	CHAR	nports;		// we only do 1 port at a time
	WORD	port;
} RSIP_PORT, *PRSIP_PORT;

typedef struct _RSIP_LEASE {
	CHAR	code;
	WORD	len;
	DWORD	leasetime;
} RSIP_LEASE, *PRSIP_LEASE;

typedef struct _RSIP_BINDID {
	CHAR	code;
	WORD	len;
	DWORD	bindid;
} RSIP_BINDID, *PRSIP_BINDID;

typedef struct _RSIP_TUNNEL {
	CHAR	code;
	WORD	len;
	CHAR	tunneltype;
} RSIP_TUNNEL, *PRSIP_TUNNEL;

// Vendor Specific structures

typedef struct _RSIP_MSVENDOR_CODE {
	CHAR	code;
	WORD	len;
	WORD	vendorid;
	WORD	option;
}RSIP_MSVENDOR_CODE, *PRSIP_MSVENDOR_CODE;

//
// RSIP Message templates
//

typedef struct _MSG_RSIP_REGISTER {
	CHAR			version;
	CHAR			command;
	RSIP_MESSAGEID  msgid;
} MSG_RSIP_REGISTER, *PMSG_RSIP_REGISTER;

typedef struct _MSG_RSIP_DEREGISTER {
	CHAR			version;
	CHAR			command;
	RSIP_CLIENTID   clientid;
	RSIP_MESSAGEID  msgid;
} MSG_RSIP_DEREGISTER, *PMSG_RSIP_DEREGISTER;

typedef struct _MSG_RSIP_ASSIGN_PORT {
	CHAR 			version;
	CHAR			command;
	RSIP_CLIENTID	clientid;
	RSIP_ADDRESS	laddress;	// local
	RSIP_PORT		lport;
	RSIP_ADDRESS	raddress;	// remote
	RSIP_PORT		rport;
	RSIP_LEASE		lease;
	RSIP_TUNNEL		tunnel;
	RSIP_MESSAGEID  msgid;

	RSIP_MSVENDOR_CODE  porttype;
	RSIP_MSVENDOR_CODE  tunneloptions;
} MSG_RSIP_ASSIGN_PORT, *PMSG_RSIP_ASSIGN_PORT;

typedef struct _MSG_RSIP_LISTEN_PORT {
	CHAR 			version;
	CHAR			command;
	RSIP_CLIENTID	clientid;
	RSIP_ADDRESS	laddress;	// local
	RSIP_PORT		lport;
	RSIP_ADDRESS	raddress;	// remote
	RSIP_PORT		rport;
	RSIP_LEASE		lease;
	RSIP_TUNNEL		tunnel;
	RSIP_MESSAGEID  msgid;

	RSIP_MSVENDOR_CODE porttype;
	RSIP_MSVENDOR_CODE tunneloptions;
	RSIP_MSVENDOR_CODE listentype;
} MSG_RSIP_LISTEN_PORT, *PMSG_RSIP_LISTEN_PORT;


typedef struct _MSG_RSIP_EXTEND_PORT {
	CHAR			version;
	CHAR			command;
	RSIP_CLIENTID	clientid;
	RSIP_BINDID		bindid;
	RSIP_LEASE		lease;
	RSIP_MESSAGEID	msgid;
} MSG_RSIP_EXTEND_PORT, *PMSG_RSIP_EXTEND_PORT;

typedef struct _MSG_RSIP_FREE_PORT {
	CHAR			version;
	CHAR			command;
	RSIP_CLIENTID	clientid;
	RSIP_BINDID     bindid;
	RSIP_MESSAGEID	msgid;	
} MSG_RSIP_FREE_PORT, *PMSG_RSIP_FREE_PORT;

typedef struct _MSG_RSIP_QUERY {
	CHAR			version;
	CHAR			command;
	RSIP_CLIENTID	clientid;
	RSIP_ADDRESS	address;
	RSIP_PORT		port;

	RSIP_MSVENDOR_CODE porttype;
	RSIP_MSVENDOR_CODE querytype;

	RSIP_MESSAGEID	msgid;
} MSG_RSIP_QUERY, *PMSG_RSIP_QUERY;

typedef struct _RSIP_RESPONSE_INFO {
	DWORD	clientid;
	DWORD	messageid;
	DWORD	bindid;
	DWORD	leasetime;
	CHAR    version;
	CHAR    msgtype;
	CHAR	tunneltype;
	CHAR	rsipmethod;
	DWORD	lAddressV4;	
	WORD	lPort;
	DWORD   rAddressV4;
	WORD    rPort;
	WORD	error;
} RSIP_RESPONSE_INFO, *PRSIP_RESPONSE_INFO;

#pragma pack(pop)

typedef struct _RSIP_LEASE_RECORD {
	struct _RSIP_LEASE_RECORD * pNext;
	DWORD   dwRefCount;
	BOOL    ftcp_udp;
	DWORD	tExpiry;
	DWORD   bindid;
	DWORD   addrV4; // remote IP address
	SHORT   rport; 	// remote port
	SHORT	port;	// local port
} RSIP_LEASE_RECORD, *PRSIP_LEASE_RECORD;

// Cache of queried address mappings so we don't
// need to requery the mappings over and over
typedef struct _ADDR_ENTRY {
	struct _ADDR_ENTRY *pNext;
	BOOL	ftcp_udp;
	DWORD	tExpiry;
	DWORD	addr;
	DWORD	raddr;
	WORD	port;
	WORD	rport;
} ADDR_ENTRY, *PADDR_ENTRY;

//**********************************************************************
// Variable definitions
//**********************************************************************

#if 0
static const char *rsip_error_strs[]={
  "RESERVED",
  "UNKNOWNERROR",
  "BADBINDID",
  "BADCLIENTID",
  "MISSINGPARAM",
  "DUPLICATEPARAM",
  "ILLEGALPARAM",
  "ILLEGALMESSAGE",
  "REGISTERFIRST",
  "BADMESSAGEID",
  "ALREADYREGISTERED",
  "ALREADYUNREGISTERED",
  "BADTUNNELTYPE",
  "ADDRUNAVAILABLE",
  "PORTUNAVAILABLE",
  (char *)0
};
#endif

//
// initialization of static member
//
LONG CRsip::m_MsgID = 0;

//**********************************************************************
// Function prototypes
//**********************************************************************
//typedef unsigned __int64 ULONGLONG;
typedef DWORD (WINAPI *LpFnGetAdaptersInfo)(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);
typedef DWORD (WINAPI *LpFnGetBestInterface)( const UINT IPAddr, ULONG *const pIndex );
typedef DWORD (WINAPI *LpFnIpRenewAddress)(PIP_ADAPTER_INDEX_MAP AdapterInfo);


//**********************************************************************
// Function definitions
//**********************************************************************


/*=============================================================================

	CRsip::CRsip - constructor

    Description:

		Constructor

    Parameters:

    	Nothing.

    Return Values:

		Nothing.

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::CRsip"

CRsip::CRsip():
	m_iRefCount( 0 ),
	m_sRsip( INVALID_SOCKET ),
	m_ClientID( 0 ),
	m_pRsipLeaseRecords( NULL ),
	m_pAddrEntry( NULL ),
	m_tuRetry( 0 ),
	m_pThreadPool( NULL ),
	m_fLockInitialized( FALSE )
{
	
}

/*=============================================================================

	CRsip::~CRsip - destructor

    Description:

		Destructor

    Parameters:

    	Nothing.

    Return Values:

		Nothing.

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::~CRsip"

CRsip::~CRsip()
{
	DNASSERT( m_iRefCount == 0 );
	DNASSERT( m_sRsip == INVALID_SOCKET );
	DNASSERT( m_ClientID == 0 );
	DNASSERT( m_pRsipLeaseRecords == NULL );
	DNASSERT( m_pAddrEntry == NULL );
	DNASSERT( m_tuRetry == 0 );
	DNASSERT( m_pThreadPool == NULL );
	DNASSERT( m_fLockInitialized == FALSE );
}

/*=============================================================================

	CRsip::Initialize - Initialize RSIP support.  If this function succeeds then there
			   is an RSIP gateway on the network and the SP should call the
			   RSIP services when creating and destroying sockets that need
			   to be accessed from machines outside the local realm.

    Description:

		Looks for the Gateway, then check to see if it is RSIP enabled.

    Parameters:

		Pointer to thread pool
		Pointer to base socket address
			Used to take SP guid to look up gateway on Win95.  Since this
			only works on Win9x, the GUID is hard-coded in the function call
			because old DPlay is guaranteed to be available on a Win9x system.
			Any new systems won't care because they'll support the GetAdaptersInfo
			call.
		Boolean indicating that this is an Rsip host

    Return Values:

		TRUE  - found and RSIP gateway and initialized.
		FALSE - no RSIP gateway found.

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::Initialize"

BOOL	CRsip::Initialize( CThreadPool *const pThreadPool, const SOCKADDR *const pBaseSocketAddress, const BOOL fIsRsipServer )
{
//	HRESULT		hr;
//	char		gwipaddr[32];
	BOOL		bReturn = TRUE;
	BOOL		fBroadcastFlag;
	INT_PTR		iReturnValue;
	SOCKADDR_IN saddr;
	DN_TIME		RsipRetryTime;
	DN_TIME		RsipTimeoutTime;


	DNASSERT( ( pThreadPool != NULL ) || ( fIsRsipServer != FALSE ) );
	DNASSERT( m_pThreadPool == NULL );
	m_pThreadPool = pThreadPool;

	RsipRetryTime.Time32.TimeHigh = 0;
	RsipRetryTime.Time32.TimeLow = RSIP_RENEW_TEST_INTERVAL;
	RsipTimeoutTime.Time32.TimeHigh = 0;
	RsipTimeoutTime.Time32.TimeLow = 0;

	DNASSERT( m_fLockInitialized == FALSE );
	if ( DNInitializeCriticalSection( &m_Lock ) == FALSE )
	{
		bReturn = FALSE;
		goto Exit;
	}
	m_fLockInitialized = TRUE;

//	// find the default gateway
//	hr = FindGateway( p_inet_addr(AN_IP_ADDRESS), gwipaddr );
//	if(hr!=DPN_OK){
//
//	    hr = GetGatewayFromRegistry( reinterpret_cast<BYTE*>( gwipaddr ), sizeof( gwipaddr ) );
//
//	    if(hr != DPN_OK){
//	    	bReturn=FALSE;
//	    	goto Exit;
//	    }
//
//	    DPF(0,"Found suggested RSIP gateway in registry %s, running on Win95?\n",gwipaddr);
//	}

	// create a SOCKADDR to address the RSIP service on the gateway
	memset(&m_saddrGateway, 0, sizeof( m_saddrGateway ) );
	m_saddrGateway.sin_family		= AF_INET;
	m_saddrGateway.sin_port			= p_htons( RSIP_HOST_PORT );

	// create an address to specify the port to bind our datagram socket to.
	memset(&saddr,0,sizeof(SOCKADDR_IN));
	saddr.sin_family	  = AF_INET;
	saddr.sin_port        = p_htons(0);

	//
	// if this is the Rsip server, bind to all adapters and talk via loopback.
	// If not, bind to the particular adapter and use broadcast to find the
	// Rsip server
	//
	if ( fIsRsipServer != FALSE )
	{
		m_saddrGateway.sin_addr.S_un.S_addr = p_htonl( INADDR_LOOPBACK );
		saddr.sin_addr.s_addr = p_htonl( INADDR_ANY );
	}
	else
	{
		m_saddrGateway.sin_addr.S_un.S_addr = p_htonl( INADDR_BROADCAST );
		saddr.sin_addr.s_addr = reinterpret_cast<const SOCKADDR_IN*>( pBaseSocketAddress )->sin_addr.s_addr;
	}

	// create a datagram socket for talking to the RSIP facility on the gateway
	if( ( m_sRsip = p_socket( AF_INET, SOCK_DGRAM, 0 ) ) == INVALID_SOCKET ){
		DPF(0,"ERROR: rsipInit() socket call for RSIP listener failed\n");
		bReturn = FALSE;
		goto Exit;
	}

	//
	// set socket to allow broadcasts
	//
	fBroadcastFlag = TRUE;
	DBG_CASSERT( sizeof( &fBroadcastFlag ) == sizeof( char * ) );
	iReturnValue = p_setsockopt( m_sRsip,			// socket
	    						 SOL_SOCKET,		// level (set socket options)
	    						 SO_BROADCAST,		// set broadcast option
	    						 reinterpret_cast<char *>( &fBroadcastFlag ),	// allow broadcast
	    						 sizeof( fBroadcastFlag )	// size of parameter
	    						 );
	if ( iReturnValue == SOCKET_ERROR )
	{
		DWORD	dwErrorCode;


		dwErrorCode = p_WSAGetLastError();
	    DPF( 0, "Unable to set socket options!" );
	    DisplayWinsockError( 0, dwErrorCode );
		bReturn = FALSE;
	    goto Exit;
	}
	
	// bind the datagram socket to any local address and port.
	if( p_bind(m_sRsip, (PSOCKADDR)&saddr, sizeof(saddr) ) != 0){
		DPF(0,"ERROR: rsipInit() bind for RSIP listener failed\n");
		bReturn=FALSE;
		goto Exit;
	}

	m_tuRetry=12500; // start retry timer at 12.5 ms

	// find out if there is an rsip service and register with it.
	if ( Register() != DPN_OK )
	{
		bReturn=FALSE;
		goto Exit;
	}


	//
	// Attempt to add timer job.  If this fails, too bad.  Rsip might time out
	// the lease if there's no traffic.
	//
	if ( pThreadPool != NULL )
	{
		AddRef();
		m_fTimerJobSubmitted = TRUE;
		if ( pThreadPool->SubmitTimerJob( 1,
										  TRUE,
										  RsipRetryTime,
										  TRUE,
										  RsipTimeoutTime,
										  CRsip::RsipTimerFunction,
										  CRsip::RsipTimerComplete,
										  this ) != DPN_OK )
		{
			m_fTimerJobSubmitted = FALSE;
			DecRef();
			DPF( 0, "Failed to submit timer job to watch over Rsip!" );
		}
	}

Exit:
	if( bReturn == FALSE )
	{
		if( m_sRsip != INVALID_SOCKET )
		{
			p_closesocket( m_sRsip );
			m_sRsip = INVALID_SOCKET;
		}
	}

	return bReturn;
}


/*=============================================================================

	CRsip::Deinitialize - Shut down RSIP support

	   All threads that might access RSIP MUST be stopped before this
	   is called.

    Description:

		Deregisters with the Rsip Agent on the gateway, and cleans up
		the list of lease records.

    Parameters:

		Pointer to thread pool

    Return Values:

		None.

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::Deinitialize"

void	CRsip::Deinitialize( void )
{
	RSIP_LEASE_RECORD	*pLeaseWalker;
	RSIP_LEASE_RECORD	*pNextLease;
	ADDR_ENTRY			*pAddrWalker;
	ADDR_ENTRY			*pNextAddr;


	if ( m_pThreadPool != NULL )
	{
		if ( m_fTimerJobSubmitted != FALSE )
		{
			m_pThreadPool->StopTimerJob( this, DPNERR_USERCANCEL );
			m_fTimerJobSubmitted = FALSE;
		}

		m_pThreadPool = NULL;
	}

	if(m_sRsip!=INVALID_SOCKET){
		Deregister();	
		p_closesocket(m_sRsip);
		m_sRsip=INVALID_SOCKET;
	}	
	
	if ( m_fLockInitialized != FALSE )
	{
		DNDeleteCriticalSection( &m_Lock );
		m_fLockInitialized = FALSE;
	}

	// free the leases
	pLeaseWalker = m_pRsipLeaseRecords;
	while( pLeaseWalker ){
		pNextLease = pLeaseWalker->pNext;
		DNFree(pLeaseWalker);
		pLeaseWalker=pNextLease;
	}
	m_pRsipLeaseRecords=NULL;

	// free the cached address mappings
	pAddrWalker=m_pAddrEntry;
	while(pAddrWalker){
		pNextAddr=pAddrWalker->pNext;
		DNFree(pAddrWalker);
		pAddrWalker=pNextAddr;
	}
	m_pAddrEntry=NULL;

	m_MsgID = 0;
	m_tuRetry = 0;
	memset( &m_saddrGateway, 0x00, sizeof( m_saddrGateway ) );
}


//**********************************************************************
// ------------------------------
// CRsip::RsipIsRunningOnThisMachine - return Boolean indicating whether this machine
//		is an Rsip machine.
//
// Entry:		Pointer to socket address to be filled in with public address
//
// Exit:		Boolean
//				TRUE = this is Rsip machine
//				FALSE = this is not Rsip machine
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::RsipIsRunningOnThisMachine"

BOOL	CRsip::RsipIsRunningOnThisMachine( SOCKADDR *const pPublicSocketAddress )
{
	SOCKET		Socket;
	BOOL		fReturn;


	Socket = INVALID_SOCKET;
	fReturn = FALSE;
	
	//
	// Attempt to bind to the Rsip port.  If we can't bind to it, assume that
	// Rsip is running on this machine.
	//
	Socket = p_socket( AF_INET, SOCK_DGRAM, 0 );
	if ( Socket != INVALID_SOCKET )
	{
		SOCKADDR_IN	addr;
		

		memset( &addr, 0, sizeof(SOCKADDR_IN ) );
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_addr = p_htonl( INADDR_LOOPBACK );
		addr.sin_port = p_htons( RSIP_HOST_PORT );
		DBG_CASSERT( sizeof( addr ) == sizeof( SOCKADDR ) );
		
		if ( p_bind( Socket, reinterpret_cast<SOCKADDR*>( &addr ), sizeof( addr ) ) != 0 )
		{
			fReturn = TRUE;
		}

		p_closesocket( Socket );
		Socket = INVALID_SOCKET;
	}

	//
	// This is an Rsip server, create a local socket and bind it to the server
	// to find out what the public address is.
	//
	if ( fReturn != FALSE )
	{		
		HRESULT			hr;
		SOCKADDR_IN 	SocketAddress;
		INT				iBoundAddressSize;
		SOCKADDR_IN		BoundAddress;
		SOCKADDR		RsipAssignedAddress;
		DWORD			dwBindID;

			
		Socket = p_socket( AF_INET, SOCK_DGRAM, 0 );
		if ( Socket == INVALID_SOCKET )
		{
			fReturn = FALSE;
			goto Failure;
		}

		memset( &SocketAddress, 0x00, sizeof( SocketAddress ) );
		SocketAddress.sin_family = AF_INET;
		SocketAddress.sin_addr.S_un.S_addr = p_htonl( INADDR_ANY );
		SocketAddress.sin_port = p_htons( ANY_PORT );

		DBG_CASSERT( sizeof( SocketAddress ) == sizeof( SOCKADDR ) );
		if ( p_bind( Socket, reinterpret_cast<SOCKADDR*>( &SocketAddress ), sizeof( SocketAddress ) ) != 0 )
		{
			fReturn = FALSE;
			goto Failure;
		}

		iBoundAddressSize = sizeof( BoundAddress );
		DBG_CASSERT( sizeof( BoundAddress ) == sizeof( SOCKADDR ) );
		if ( p_getsockname( Socket, reinterpret_cast<SOCKADDR*>( &BoundAddress ), &iBoundAddressSize ) != 0 )
		{
			fReturn = FALSE;
			goto Failure;
		}
		DNASSERT( iBoundAddressSize == sizeof( BoundAddress ) );

		DBG_CASSERT( sizeof( SocketAddress ) == sizeof( SOCKADDR ) );
		if ( Initialize( NULL, reinterpret_cast<SOCKADDR*>( &SocketAddress ), TRUE ) == FALSE )
		{
			fReturn = FALSE;
			goto Failure;
		}

		hr = AssignPort( FALSE, BoundAddress.sin_port, &RsipAssignedAddress, &dwBindID );
		if ( hr != DPN_OK )
		{
			fReturn = FALSE;
			DPF( 0, "Failed to assign port when attempting to determine public network address!" );
			goto Failure;
		}

		DBG_CASSERT( sizeof( *pPublicSocketAddress ) == sizeof( RsipAssignedAddress ) );
		memcpy( pPublicSocketAddress, &RsipAssignedAddress, sizeof( *pPublicSocketAddress ) );
		
		FreePort( dwBindID );
	}

Exit:
	if ( Socket != INVALID_SOCKET )
	{
		p_closesocket( Socket );
		Socket = INVALID_SOCKET;
	}
	
	return	fReturn;

Failure:
	goto Exit;
}
//**********************************************************************


///*=============================================================================
//
//    CRsip::FindGateway - find the address of the internet gateway (possibly RSIP
//    			  host) for this machine.
//
//
//    Description:
//
//    	Uses the ip helper api to find the default IP gateway.
//
//    Parameters:
//
//    	uint32 myip      - ip adapter to find default gateway for
//    	char   *gwipaddr - gateway address if found
//
//    Return Values:
//
//    	DPN_OK - found gateway
//    	DPNERR_GENERIC - failed.
//
//    Note: code stolen from Rick Lamb (rlamb)
//
//-----------------------------------------------------------------------------*/
//HRESULT CRsip::FindGateway( const UINT myip, char *const gwipaddr )
//{
//    PIP_ADAPTER_INFO	pAdapterInfo = NULL;
//    PIP_ADAPTER_INFO	p0AdapterInfo = NULL;
//    DWORD		error = 0;
//    DWORD		len = 0;
//    UINT		i;
//    HMODULE		hIpHlpApi;
//    ULONG		bindex;
//
//
//    IP_ADAPTER_INDEX_MAP ipaim;
//    LpFnGetAdaptersInfo	lpFnGetAdaptersInfo;
//    LpFnGetBestInterface lpFnGetBestInterface;
//    LpFnIpRenewAddress	 lpFnIpRenewAddress;
//
// /*
//     * See if there is an RSIP server running.
//     * If so, we must be running on the server itself
//     * so use the loopback interface.
//     */
//    if(gwipaddr) {
//    	SOCKET s;
//    	SOCKADDR_IN addr;
//
//    	if ( ( s = p_socket( AF_INET, SOCK_DGRAM, 0 ) ) != INVALID_SOCKET ) {
//    			memset(&addr,0,sizeof(SOCKADDR_IN));
//    			addr.sin_family = AF_INET;
//    			addr.sin_addr.s_addr = p_inet_addr(LOOPBACK_ADDR);
//    			addr.sin_port = p_htons(RSIP_HOST_PORT);
//    			if( p_bind(s,(struct sockaddr *)&addr,sizeof(addr)) < 0) {
//    		/*
//    	 	* Something is there already
//    		*/
//    		memcpy(gwipaddr,LOOPBACK_ADDR,sizeof(LOOPBACK_ADDR));
//    		DPF(0,"USING LOOPBACK: default gateway %s\n",gwipaddr);
//    		p_closesocket(s);
//    		goto done;
//    			}
//    			p_closesocket(s);
//    	}
//
//    }
//
//    hIpHlpApi = LoadLibrary("IPHLPAPI.DLL");
//    if(hIpHlpApi == NULL) {
//    	error = DPNERR_GENERIC;
//    	DPF(0,"[NET] failed to load IPHLPAIP.DLL\n");
//    goto done;
//    }
//
//    lpFnGetAdaptersInfo = (LpFnGetAdaptersInfo) GetProcAddress(hIpHlpApi, "GetAdaptersInfo");
//    if(lpFnGetAdaptersInfo == NULL) {
//    	DPF(0,"[NET] failed to find GetAdaptersInfo\n");
//    DNASSERT( FALSE );
//    error = DPNERR_GENERIC;
//    	goto done;
//    }
//
//    error = (*lpFnGetAdaptersInfo) (pAdapterInfo, &len);
//    if(error != ERROR_BUFFER_OVERFLOW) {
//    	DPF(0,"[NET] GetAdaptersInfo failed error 0x%lx\n", error);
//    	error=DPNERR_GENERIC;
//    	goto done;
//    }
//
//    p0AdapterInfo = pAdapterInfo = static_cast<IP_ADAPTER_INFO*>( DNMalloc( len ) );
//    if(pAdapterInfo == NULL) {
//    	DPF(0,"[NET] memory allocation failed\n");
//    DNASSERT( FALSE );
//    	error=DPNERR_GENERIC;
//    	goto done;
//    }
//
//    error = (*lpFnGetAdaptersInfo) (pAdapterInfo, &len);
//    if(error != 0) {
//    	DPF(0, "[NET] GetAdaptersInfo failed error 0x%lx\n", error);
//    	error=DPNERR_GENERIC;
//    	goto done;
//    }
//
//    lpFnIpRenewAddress = (LpFnIpRenewAddress) GetProcAddress(hIpHlpApi,"IpRenewAddress");
//    if(lpFnIpRenewAddress == NULL) {
//    	DPF(0," failed to find IpRenewAddress\n");
//    DNASSERT( FALSE );
//    	error=DPNERR_GENERIC;
//    	goto done;
//    }
//
//
//    lpFnGetBestInterface = (LpFnGetBestInterface) GetProcAddress(hIpHlpApi, "GetBestInterface");
//    if(lpFnGetBestInterface == NULL) {
//    	DPF(0," failed to find GetBestInterface\n");
//    DNASSERT( FALSE );
//    	error=DPNERR_GENERIC;
//    	goto done;
//    }
//
//    error = (*lpFnGetBestInterface) (myip, &bindex);
//    if(error != 0) {
//    	DPF(0,"[NET] GetBestInterface failed error 0x%lx\n", error);
//    	error=DPNERR_GENERIC;
//    	goto done;
//    }
//    /*printf("Renew Interface Index = %d\n",bindex);/**/
//
//    for(i = 0; pAdapterInfo != NULL; i++, pAdapterInfo = pAdapterInfo->Next) {
//    	DPF(8,"[NET] Adapter Info\n");
//    	DPF(8,"[NET] \t name %s\n",pAdapterInfo->AdapterName);
//    	DPF(8,"[NET] \t description %s\n",pAdapterInfo->Description);
//    	DPF(8,"[NET] \t index %d\n",pAdapterInfo->Index);
//    	DPF(8,"[NET] \t combo index %d\n",pAdapterInfo->ComboIndex);
//    	if(pAdapterInfo->Index == bindex) break;
//    }
//
//    if(pAdapterInfo == NULL) {
//    	DPF(8,"No match\n");
//    	error=DPNERR_GENERIC;
//    	goto done;
//    }
//
//    {
//    	PIP_ADDR_STRING ips;
//    	ips = &pAdapterInfo->GatewayList;
//    	if(gwipaddr) {
//    		strcpy(gwipaddr,ips->IpAddress.String);
//    		DPF(0,"default gateway %s\n",gwipaddr);
//    		goto done;
//    	}
//    }
//
//done:
//    if ( p0AdapterInfo != NULL )
//    {
//    	DNFree(p0AdapterInfo);
//    }
//
//    if( hIpHlpApi != NULL )
//    {
//    	FreeLibrary(hIpHlpApi);
//    }
//
//    DPF(8,"[NET] < FindGateway\n");
//    return error;
//}


/*=============================================================================

	CRsip::ExchangeAndParse - send a request to the rsip server and
						   wait for and parse the reply


    Description:

	Since there is almost no scenario where we don't immediately need to know
	the response to an rsipExchange, there is no point in doing this
	asynchronously.  The assumption is that an RSIP server is sufficiently
	local that long retries are not necessary.  We use the approach suggested
	in the IETF draft protocol specification, that is 12.5ms retry timer
	with 7-fold exponential backoff.  This can lead to up to a total 1.5
	second wait in the worst case.  (Note this section may no longer be
	available since the rsip working group decided to drop UDP support)

    Parameters:

		pRequest  - a fully formatted RSIP request buffer
		cbReq     - size of request buffer
		pRespInfo - structure that returns response parameters
		messageid - the message id of this request
		bConnect  - whether this is the register request, we use a different
					timer strategy on initial connect because we really
					don't want to miss it if there is a gateway.
		pRecvSocketAddress - pointer to recv address destination (needed for connect)

    Return Values:

		DPN_OK - exchange succeeded, reply is in the reply buffer.
		otw, failed, RespInfo is bubkas.

-----------------------------------------------------------------------------*/

#define MAX_RSIP_RETRY	6

struct timeval tv0={0,0};

#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::ExchangeAndParse"

HRESULT CRsip::ExchangeAndParse( PCHAR pRequest,
								 const UINT cbReq,
								 RSIP_RESPONSE_INFO *const pRespInfo,
								 const DWORD messageid,
								 const BOOL bConnect,
								 SOCKADDR *const pRecvSocketAddress )
{
	CHAR	RespBuffer[ RESP_BUF_SIZE ];
	DWORD	dwRespLen = sizeof( RespBuffer );
	INT		iRecvSocketAddressSize = sizeof( *pRecvSocketAddress );

	struct timeval tv;
	FD_SET readfds;
	INT    nRetryCount=0;
	int    rc;
	int    cbReceived;
	HRESULT hr=DPN_OK;

	
	DNASSERT( pRequest != NULL );
	DNASSERT( pRespInfo != NULL );
	DNASSERT( pRecvSocketAddress != NULL );

	DPF( 8, "==>RsipExchangeAndParse msgid %d\n", messageid );

	memset(RespBuffer, 0, RESP_BUF_SIZE);
	memset(pRespInfo, 0, sizeof(RSIP_RESPONSE_INFO));

	if(!bConnect){
		tv.tv_usec = m_tuRetry;
		tv.tv_sec  = 0;
		nRetryCount = 0;
	} else {
		// on a connect request we try twice with a 1 second total timeout
		tv.tv_usec = 500000;	// 0.5 seconds
		tv.tv_sec  = 0;
		nRetryCount = MAX_RSIP_RETRY-2;
	}


	FD_ZERO(&readfds);
	FD_SET(m_sRsip, &readfds);

	rc=0;

	// First clear out any extraneous responses.
	while( p_select( 0, &readfds, NULL, NULL, &tv0 ) )
	{
		cbReceived = p_recvfrom( m_sRsip, RespBuffer, dwRespLen, 0, NULL, NULL );
		switch ( cbReceived )
		{
			//
			// nothing received, try again
			//
			case 0:
			{
				break;
			}

			//
			// read failure, bail!
			//
			case SOCKET_ERROR:
			{
				rc = p_WSAGetLastError();
				DPF( 0, "Got sockets error %d trying to receive (clear incoming queue) on RSIP socket\n", rc );
				hr = DPNERR_GENERIC;
				
				break;
			}

			default:
			{
				DPF( 7, "Found extra response from previous RSIP request\n" );
				if( m_tuRetry < RSIP_tuRETRY_MAX )
				{
					// don't re-try so quickly
					m_tuRetry *= 2;
					DPF( 7, "rsip Set m_tuRetry to %d usec\n", m_tuRetry );
				}	
				
				break;
			}
		}
		
		FD_ZERO(&readfds);
		FD_SET(m_sRsip, &readfds);
	}


	// Now do the exchange, get a response to the request, does retries too.
	do{

		if(++nRetryCount > MAX_RSIP_RETRY) {
			break;
		}

		DPF( 7, "Sending RSIP Request to gateway, RetryCount=%d", nRetryCount );
		DBG_CASSERT( sizeof( m_saddrGateway ) == sizeof( SOCKADDR ) );
		DumpSocketAddress( 0, reinterpret_cast<SOCKADDR*>( &m_saddrGateway ), AF_INET);

		// First send off the request
		rc=p_sendto(m_sRsip, pRequest, cbReq, 0, (SOCKADDR *)&m_saddrGateway, sizeof(SOCKADDR) );

		if( rc == SOCKET_ERROR )
		{
			rc = p_WSAGetLastError();
			DPF( 0, "Got sockets error %d on sending to RSIP gateway\n", rc );
			hr = DPNERR_GENERIC;
			goto exit;
		}

		if( rc != (int)cbReq )
		{
			DPF( 0, "Didn't send entire datagram?  shouldn't happen\n" );
			hr=DPNERR_GENERIC;
			goto exit;
		}

		// Now see if we get a response.
select_again:		
		FD_ZERO(&readfds);
		FD_SET(m_sRsip, &readfds);

		rc=p_select(0,&readfds,NULL,NULL,&tv);

		if(rc==SOCKET_ERROR){
			rc=p_WSAGetLastError();
			DPF(0,"Got sockets error %d trying to select on RSIP socket\n",rc);
			hr=DPNERR_GENERIC;
		}

		DPF(0,"Return From Select %d",rc);

		
		//
		// there's only one item in the set, make sure of this, and if there
		// was a signalled socket, make sure it's our socket.
		//
		if( p___WSAFDIsSet(m_sRsip, &readfds)){
			break;
		}

		if(!bConnect){
			DPF( 7, "Didn't get response, increasing timeout value" );
			// don't use exponential backoff on initial connect
			tv.tv_usec *= 2;	// exponential backoff.
		}	

		DNASSERT(tv.tv_usec < 4000000);

	} while (rc==0); // keep retrying...


	if(rc == SOCKET_ERROR){
		DPF(0,"GotSocketError on select, extended error %d\n",p_WSAGetLastError());
		hr=DPNERR_GENERIC;
		goto exit;
	}

	if(rc)
	{
		// We Got Mail, err data....
		dwRespLen=RESP_BUF_SIZE;
		
		DPF( 7, "Receiving Data" );

		memset( pRecvSocketAddress, 0x00, sizeof( *pRecvSocketAddress ) );
		cbReceived=p_recvfrom(m_sRsip, RespBuffer, dwRespLen, 0, pRecvSocketAddress, &iRecvSocketAddressSize);

		// BUGBUG:Could get and check addrfrom to avoid spoofing, not that paranoid

		DPF( 7, "cbReceived = %d", cbReceived );

		if( ( cbReceived == 0 ) || ( cbReceived == SOCKET_ERROR ) )
		{
			rc=p_WSAGetLastError();
			DPF( 0, "Got sockets error %d trying to receive on RSIP socket\n", rc );
			hr=DPNERR_GENERIC;
		}
		else
		{
			DPF( 7, "Parsing Receive Buffer" );
			Parse( RespBuffer, cbReceived, pRespInfo );
			if(pRespInfo->messageid != messageid)
			{
				// Got a dup from a previous retry, go try again.
				DPF( 0, "Got messageid %d, expecting messageid %d\n", pRespInfo->messageid, messageid );
				goto select_again;
			}
		}
	}

	DPF( 8, "<==RsipExchangeAndParse hr=%x, Resp msgid %d\n", hr, pRespInfo->messageid );

exit:
	return hr;
}

/*=============================================================================

	CRsip::Parse - parses an RSIP request and puts fields into a struct.

    Description:

		This parser parses and RSIP request or response and extracts
		out the codes into a standard structure.  This is not completely
		general, as we know that we will only operate with v4 addresses
		and our commands will never deal with more than 1 address at a
		time.  If you need to handle multiple address requests
		and responses, then you will need to change this function.

	Limitations:

		This function only deals with single address/port responses.
		Rsip allows for multiple ports to be allocated in a single
		request, but we do not take advantage of this feature.

    Parameters:

		pBuf  		- buffer containing an RSIP request or response
		cbBuf 		- size of buffer in bytes
		pRespInfo   - a structure that is filled with the parameters
					  from the RSIP buffer.

    Return Values:

		DPN_OK - connected to the RSIP server.

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::Parse"

HRESULT CRsip::Parse( CHAR *pBuf, DWORD cbBuf, PRSIP_RESPONSE_INFO pRespInfo )
{
	// character pointer version of parameter pointer.

	BOOL bGotlAddress=FALSE;
	BOOL bGotlPort   =FALSE;

	DWORD code;
	DWORD codelen;

	PRSIP_MSG_HDR pHeader;
	PRSIP_PARAM   pParam,pNextParam;
	CHAR          *pc;

	CHAR *pBufEnd = pBuf+cbBuf;

	if(cbBuf < 2){
		return DPNERR_INVALIDPARAM;
	}	

	pHeader=(PRSIP_MSG_HDR)pBuf;

	pRespInfo->version = pHeader->version;
	pRespInfo->msgtype = pHeader->msgtype;

	DPF(0,"rsipParse: version %d msgtype %d\n",pRespInfo->version, pRespInfo->msgtype);

	pParam = (PRSIP_PARAM)(pHeader+1);

	while((CHAR*)(pParam+1) < pBufEnd)
	{
		pc=(CHAR *)(pParam+1);
		pNextParam = (PRSIP_PARAM)(pc + pParam->len);

		if((CHAR *)pNextParam > pBufEnd){
			break;
		}	

		switch(pParam->code){

			case RSIP_ADDRESS_CODE:

				// Addresses are type[1]|addr[?]

				switch(*pc){
					case 1:
						if(!bGotlAddress){
							DPF(0,"rsipParse: lAddress %s\n",p_inet_ntoa(*((PIN_ADDR)(pc+1))));
							memcpy((char *)&pRespInfo->lAddressV4, pc+1, 4);
							bGotlAddress=TRUE;
						} else {	
							bGotlPort=TRUE; // just in case there wasn't a local port
							DPF(0,"rsipParse: rAddress %s,",p_inet_ntoa(*((PIN_ADDR)(pc+1))));
							memcpy((char *)&pRespInfo->rAddressV4, pc+1, 4);
						}	
						break;
					case 0:
					case 2:
					case 3:
					case 4:
					case 5:
						DPF(0,"Unexpected RSIP address code type %d\n",*pc);
					break;
				}
				break;

			case RSIP_PORTS_CODE:

				// Ports are Number[1]|Port[2]....Port[2]
				// BUGBUG: I think ports are backwards.
				if(!bGotlPort){
					DPF(0,"rsipParse lPort: %d\n", *((WORD *)(pc+1)));
					memcpy((char *)&pRespInfo->lPort, pc+1,2);
				} else {
					DPF(0,"rsipParse rPort: %d\n", *((WORD *)(pc+1)));
					memcpy((char *)&pRespInfo->rPort, pc+1,2);
					bGotlPort=TRUE;					
				}
				break;

  			case RSIP_LEASE_CODE:
				if(pParam->len == 4){
					memcpy((char *)&pRespInfo->leasetime,pc,4);
					DPF(0,"rsipParse Lease: %d\n",pRespInfo->leasetime);
				}	
  				break;

  			case RSIP_CLIENTID_CODE:
  				if(pParam->len==4){
  					memcpy((char *)&pRespInfo->clientid,pc,4);
					DPF(0,"rsipParse clientid: %d\n",pRespInfo->clientid);
  				}
  				break;

  			case RSIP_BINDID_CODE:
  				if(pParam->len==4){
  					memcpy((char *)&pRespInfo->bindid,pc,4);
					DPF(0,"rsipParse bindid: %x\n",pRespInfo->bindid);
  				}
  				break;

  			case RSIP_MESSAGEID_CODE:
  				if(pParam->len==4){
  					memcpy((char *)&pRespInfo->messageid,pc,4);
					DPF(0,"rsipParse messageid: %d\n",pRespInfo->messageid);
  				}
  				break;

  			case RSIP_TUNNELTYPE_CODE:
  				DPF(0,"rsipParse Got Tunnel Type %d, ignoring\n",*pc);
  				break;

  			case RSIP_RSIPMETHOD_CODE:
  				DPF(0,"rsipParse Got RSIP Method %d, ignoring\n",*pc);
  				break;

  			case RSIP_ERROR_CODE:
  				if(pParam->len==2){
  					memcpy((char *)&pRespInfo->error,pc,2);
  				}
  				DPF(0,"rsipParse Got RSIP error %d\n",pRespInfo->error);
  				break;

  			case RSIP_FLOWPOLICY_CODE:
  				DPF(0,"rsipParse Got RSIP Flow Policy local %d remote %d, ignoring\n",*pc,*(pc+1));
  				break;

  			case RSIP_VENDOR_CODE:
  				break;

  			default:
  				DPF(0,"Got unknown parameter code %d, ignoring\n",pParam->code);
  				break;
		}

		pParam=pNextParam;

	}

	return DPN_OK;
}

/*=============================================================================

	CRsip::Register - register with the RSIP server on the gateway (if present)

    Description:

		Trys to contact the RSIP service on the gateway.

		Doesn't require lock since this is establishing the link during
		startup.  So no-one is racing with us.

    Parameters:

    	None.

    Return Values:

		DPN_OK 		  - connected to the RSIP server.
		DPNERR_GENERIC - can't find the RSIP service on the gateway.

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::Register"

HRESULT	CRsip::Register( void )
{
	HRESULT		hr;
	SOCKADDR	RecvSocketAddress;
	MSG_RSIP_REGISTER  RegisterReq;
	RSIP_RESPONSE_INFO RespInfo;


	DPF( 8, "==>RSIP Register\n" );

	// Initialize the message sequencing.  Each message response pair
	// is numbered sequentially to allow differentiation over UDP link.

	m_MsgID = 0;
	
	// Build the request

	RegisterReq.version    	= RSIP_VERSION;
	RegisterReq.command    	= RSIP_REGISTER_REQUEST;
	RegisterReq.msgid.code 	= RSIP_MESSAGEID_CODE;
	RegisterReq.msgid.len  	= sizeof(DWORD);
	RegisterReq.msgid.msgid = InterlockedIncrement( &m_MsgID );

	hr = ExchangeAndParse( (PCHAR)&RegisterReq,
						   sizeof(RegisterReq),
						   &RespInfo,
						   RegisterReq.msgid.msgid,
						   TRUE,
						   &RecvSocketAddress );
	if(hr!=DPN_OK){
		goto exit;
	}

	if(RespInfo.msgtype != RSIP_REGISTER_RESPONSE){
		DPF(0,"Failing registration, response was message type %d",RespInfo.msgtype);
		goto error_exit;
	}

	m_ClientID = RespInfo.clientid;
	DBG_CASSERT( sizeof( m_saddrGateway ) == sizeof( RecvSocketAddress ) );
	memcpy( &m_saddrGateway, &RecvSocketAddress, sizeof( m_saddrGateway ) );

	DPF( 8, "<==RSIP Register, ClientId %d", m_ClientID );

exit:
	return hr;

error_exit:
	DPF( 8, "<==RSIP Register FAILED" );
	return DPNERR_GENERIC;

}

/*=============================================================================

	CRsip::Deregister - close connection to RSIP gateway.

    Description:

    	Shuts down the registration of this application with the RSIP
    	gateway.  All port assignments are implicitly freed as a result
    	of this operation.

		- must be called with lock held.

    Parameters:

		None.

    Return Values:

		DPN_OK - successfully deregistered with the RSIP service.

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::Deregister"

HRESULT	CRsip::Deregister( void )
{
	HRESULT 	hr;
	SOCKADDR	RecvSocketAddress;

	MSG_RSIP_DEREGISTER  DeregisterReq;
	RSIP_RESPONSE_INFO RespInfo;

	DPF( 8, "==>RSIP Deregister\n" );

	// Build the request

	DeregisterReq.version    	    = RSIP_VERSION;
	DeregisterReq.command    	    = RSIP_DEREGISTER_REQUEST;

	DeregisterReq.clientid.code     = RSIP_CLIENTID_CODE;
	DeregisterReq.clientid.len      = sizeof(DWORD);
	DeregisterReq.clientid.clientid = m_ClientID;

	DeregisterReq.msgid.code 	    = RSIP_MESSAGEID_CODE;
	DeregisterReq.msgid.len  	    = sizeof(DWORD);
	DeregisterReq.msgid.msgid       = InterlockedIncrement( &m_MsgID );

	hr=ExchangeAndParse( (PCHAR)&DeregisterReq,
						 sizeof(DeregisterReq),
						 &RespInfo,
						 DeregisterReq.msgid.msgid,
						 FALSE,
						 &RecvSocketAddress );
	if(hr!=DPN_OK){
		goto exit;
	}

	if(RespInfo.msgtype != RSIP_DEREGISTER_RESPONSE){
		DPF(0,"Failing registration, response was message type %d\n",RespInfo.msgtype);
		goto error_exit;
	}

	DPF( 8, "<==RSIP Deregister Succeeded\n" );

exit:
	m_ClientID = 0;
	return hr;

error_exit:
	DPF( 8, "<==RSIP Deregister Failed\n" );
	hr = DPNERR_GENERIC;
	goto exit;
}

/*=============================================================================

	CRsip::AssignPort - assign a port mapping with the rsip server

    Description:

		Asks for a public realm port that is an alias for the local port on
		this local realm node.  After this request succeeds, all traffic
		directed to the gateway on the public side at the allocated public
		port, which the gateway provides and specifies in the response, will
		be directed to the specified local port.

		This function also adds the lease for the port binding to a list of
		leases and will renew the lease before it expires if the binding
		hasn't been released by a call to rsipFree first.

    Parameters:


		WORD	     port     - local port to get a remote port for (big endian)
    	BOOL		 ftcp_udp - whether we are assigning a UDP or TCP port
    	SOCKADDR     psaddr   - place to return assigned global realm address
    	PDWORD       pBindid  - identifier for this binding, used to extend
    							lease and/or release the binding (OPTIONAL).

    Return Values:

		DPN_OK - assigned succeeded, psaddr contains public realm address,
				*pBindid is the binding identifier.

		DPNERR_GENERIC - assignment of a public port could not be made.

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::AssignPort"

HRESULT	CRsip::AssignPort( const BOOL fTCP_UDP, const WORD wPort, SOCKADDR *const psaddr, DWORD *const pdwBindid )
{
	#define psaddr_in ((SOCKADDR_IN *)psaddr)

	HRESULT hr;

	MSG_RSIP_ASSIGN_PORT AssignReq;
	RSIP_RESPONSE_INFO RespInfo;
	PRSIP_LEASE_RECORD pLeaseRecord;
	SOCKADDR	RecvSocketAddress;


	Lock();

	DPF( 8, "==>RSIP Assign Port %d\n", p_htons( wPort ) );

	if(m_sRsip == INVALID_SOCKET){
		DPF(0,"rsipAssignPort: m_sRsip is invalid, bailing...\n");
		Unlock();
		return DPNERR_GENERIC;
	}

	if(pLeaseRecord=FindLease( fTCP_UDP, wPort )){

		// hey, we've already got a lease for this port, so use it.
		pLeaseRecord->dwRefCount++;

		if(psaddr_in){
			psaddr_in->sin_family = AF_INET;
			psaddr_in->sin_addr.s_addr = pLeaseRecord->addrV4;
			psaddr_in->sin_port		   = pLeaseRecord->rport;
		}	

		if(pdwBindid != NULL )
		{
			*pdwBindid = pLeaseRecord->bindid;
		}

		DPF( 7, "<==Rsip Assign, already have lease Bindid %d\n", pLeaseRecord->bindid );

		Unlock();

		hr=DPN_OK;


		goto exit;
	}

	// Build the request.

	AssignReq.version    		= RSIP_VERSION;
	AssignReq.command    		= RSIP_ASSIGN_REQUEST_RSAP_IP;

	AssignReq.clientid.code 	= RSIP_CLIENTID_CODE;
	AssignReq.clientid.len 		= sizeof(DWORD);
	AssignReq.clientid.clientid = m_ClientID;

	// Local Address (will be returned by RSIP server, us don't care value)

	AssignReq.laddress.code		= RSIP_ADDRESS_CODE;
	AssignReq.laddress.len		= sizeof(RSIP_ADDRESS)-sizeof(RSIP_PARAM);
	AssignReq.laddress.version	= 1; // IPv4
	AssignReq.laddress.addr		= 0; // Don't care

	// Local Port, this is a port we have opened that we are assigning a
	// global alias for.

	AssignReq.lport.code		= RSIP_PORTS_CODE;
	AssignReq.lport.len			= sizeof(RSIP_PORT)-sizeof(RSIP_PARAM);
	AssignReq.lport.nports      = 1;
	AssignReq.lport.port		= p_htons( wPort );

	// Remote Address (not used with our flow control policy, use don't care value)

	AssignReq.raddress.code		= RSIP_ADDRESS_CODE;
	AssignReq.raddress.len		= sizeof(RSIP_ADDRESS)-sizeof(RSIP_PARAM);
	AssignReq.raddress.version  = 1; // IPv4
	AssignReq.raddress.addr     = 0; // Don't care

	AssignReq.rport.code		= RSIP_PORTS_CODE;
	AssignReq.rport.len			= sizeof(RSIP_PORT)-sizeof(RSIP_PARAM);
	AssignReq.rport.nports      = 1;
	AssignReq.rport.port		= 0; // Don't care

	// Following parameters are optional according to RSIP spec...

	// Lease code, ask for an hour, but don't count on it.

	AssignReq.lease.code		 = RSIP_LEASE_CODE;
	AssignReq.lease.len			 = sizeof(RSIP_LEASE)-sizeof(RSIP_PARAM);
	AssignReq.lease.leasetime    = 3600;

	// Tunnell Type is IP-IP

	AssignReq.tunnel.code		 = RSIP_TUNNELTYPE_CODE;
	AssignReq.tunnel.len		 = sizeof(RSIP_TUNNEL)-sizeof(RSIP_PARAM);
	AssignReq.tunnel.tunneltype  = TUNNEL_IP_IP;

	// Message ID is optional, but we use it since we use UDP xport it is required.

	AssignReq.msgid.code 		 = RSIP_MESSAGEID_CODE;
	AssignReq.msgid.len  		 = sizeof(DWORD);
	AssignReq.msgid.msgid   	 = InterlockedIncrement( &m_MsgID );

	// Vendor specific - need to specify port type and no-tunneling

	AssignReq.porttype.code     	 = RSIP_VENDOR_CODE;
	AssignReq.porttype.len      	 = sizeof(RSIP_MSVENDOR_CODE)-sizeof(RSIP_PARAM);
	AssignReq.porttype.vendorid 	 = RSIP_MS_VENDOR_ID;
	AssignReq.porttype.option   	 = (fTCP_UDP)?RSIP_TCP_PORT:RSIP_UDP_PORT;

	AssignReq.tunneloptions.code     = RSIP_VENDOR_CODE;
	AssignReq.tunneloptions.len      = sizeof(RSIP_MSVENDOR_CODE)-sizeof(RSIP_PARAM);
	AssignReq.tunneloptions.vendorid = RSIP_MS_VENDOR_ID;
	AssignReq.tunneloptions.option   = RSIP_NO_TUNNEL;


	hr=ExchangeAndParse( (PCHAR)&AssignReq,
						 sizeof(AssignReq),
						 &RespInfo,
						 AssignReq.msgid.msgid,
						 FALSE,
						 &RecvSocketAddress );
	Unlock();

	if(hr!=DPN_OK){
		goto exit;
	}

	if(RespInfo.msgtype != RSIP_ASSIGN_RESPONSE_RSAP_IP){
		DPF(0,"Assignment failed? Response was %d\n",RespInfo.msgtype);
		goto error_exit;
	}

	if(psaddr_in){
		psaddr_in->sin_family = AF_INET;
		psaddr_in->sin_addr.s_addr = RespInfo.lAddressV4;
		psaddr_in->sin_port		   = p_htons(RespInfo.lPort);
	}

	if( pdwBindid != NULL )
	{
		*pdwBindid = RespInfo.bindid;
	}

	AddLease( RespInfo.bindid,
			  fTCP_UDP,
			  RespInfo.lAddressV4,
			  p_htons(RespInfo.lPort),
			  wPort,
			  RespInfo.leasetime);

	DPF( 8, "RSIP Port Assigned: " );
	DumpSocketAddress( 8, reinterpret_cast<SOCKADDR*>( psaddr_in ), psaddr_in->sin_family );
	DPF( 8, "<== BindId %d\n", RespInfo.bindid );

exit:
	return hr;

error_exit:
	DPF( 8, "<==Assign Port Failed\n" );
	return DPNERR_GENERIC;

	#undef psaddr_in
}

/*=============================================================================

	CRsip::ExtendPort - extend a port mapping

    Description:

		Extends the lease on a port mapping.

    Parameters:

    	DWORD        Bindid - binding identifier specified by the rsip service.
    	DWORD        ptExtend - amount of extra lease time granted.

    Return Values:

		DPN_OK - lease extended.
		DPNERR_GENERIC - couldn't extend the lease.

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::ExtendPort"

HRESULT CRsip::ExtendPort( const DWORD Bindid, DWORD *const ptExtend )
{
	HRESULT hr;

	MSG_RSIP_EXTEND_PORT  ExtendReq;
	RSIP_RESPONSE_INFO RespInfo;
	SOCKADDR	RecvSocketAddress;


	Lock();

	DPF( 8, "==>Extend Port, Bindid %d\n", Bindid );

	if(m_sRsip == INVALID_SOCKET){
		DPF(0,"rsipExtendPort: m_sRsip is invalid, bailing...\n");
		Unlock();
		return DPNERR_GENERIC;
	}

	// Build the request

	ExtendReq.version    		= RSIP_VERSION;
	ExtendReq.command    		= RSIP_EXTEND_REQUEST;

	ExtendReq.clientid.code 	= RSIP_CLIENTID_CODE;
	ExtendReq.clientid.len 		= sizeof(DWORD);
	ExtendReq.clientid.clientid = m_ClientID;

	ExtendReq.bindid.code 		= RSIP_BINDID_CODE;
	ExtendReq.bindid.len 		= sizeof(DWORD);
	ExtendReq.bindid.bindid 	= Bindid;

	// Lease code, ask for an hour, but don't count on it.

	ExtendReq.lease.code		= RSIP_LEASE_CODE;
	ExtendReq.lease.len			= sizeof(RSIP_LEASE)-sizeof(RSIP_PARAM);
	ExtendReq.lease.leasetime   = 3600;

	ExtendReq.msgid.code 		= RSIP_MESSAGEID_CODE;
	ExtendReq.msgid.len  		= sizeof(DWORD);
	ExtendReq.msgid.msgid   	= InterlockedIncrement( &m_MsgID );

	hr=ExchangeAndParse( (PCHAR)&ExtendReq,
						 sizeof(ExtendReq),
						 &RespInfo,
						 ExtendReq.msgid.msgid,
						 FALSE,
						 &RecvSocketAddress );

	Unlock();

	if(hr!=DPN_OK){
		goto exit;
	}

	if(RespInfo.msgtype != RSIP_EXTEND_RESPONSE){
		DPF(0,"Failing registration, response was message type %d\n",RespInfo.msgtype);
		goto error_exit;
	}

	*ptExtend=RespInfo.leasetime;

	DPF( 8, "<==Extend Port, Bindid %d Succeeded, extra lease time %d\n", Bindid, *ptExtend );

exit:
	return hr;

error_exit:
	DPF( 8, "<==Extend Port, Failed" );
	return DPNERR_GENERIC;

}

/*=============================================================================

	CRsip::FreePort - release a port binding

    Description:

		Removes the lease record for our port binding (so we don't renew it
		after we actually release the binding from the gateway).  Then informs
		the gateway that we are done with the binding.

    Parameters:

    	DWORD        Bindid - gateway supplied identifier for the binding

    Return Values:

		DPN_OK - port binding released.
		DPNERR_GENERIC - failed.

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::FreePort"

HRESULT	CRsip::FreePort( const DWORD dwBindid )
{
	HRESULT hr;

	MSG_RSIP_FREE_PORT  FreeReq;
	RSIP_RESPONSE_INFO RespInfo;
	SOCKADDR	RecvSocketAddress;


	Lock();

	DPF( 8, "==>Release Port, Bindid %d\n", dwBindid );

	if(m_sRsip == INVALID_SOCKET){
		DPF(0,"rsipFreePort: m_sRsip is invalid, bailing...\n");
		Unlock();
		return DPNERR_GENERIC;
	}

	RemoveLease( dwBindid );

	FreeReq.version    			= RSIP_VERSION;
	FreeReq.command    			= RSIP_FREE_REQUEST;

	FreeReq.clientid.code 		= RSIP_CLIENTID_CODE;
	FreeReq.clientid.len 		= sizeof(DWORD);
	FreeReq.clientid.clientid 	= m_ClientID;

	FreeReq.bindid.code 		= RSIP_BINDID_CODE;
	FreeReq.bindid.len 			= sizeof(DWORD);
	FreeReq.bindid.bindid 		= dwBindid;

	FreeReq.msgid.code 			= RSIP_MESSAGEID_CODE;
	FreeReq.msgid.len  			= sizeof(DWORD);
	FreeReq.msgid.msgid   		= InterlockedIncrement( &m_MsgID );

	hr=ExchangeAndParse( (PCHAR)&FreeReq,
						 sizeof(FreeReq),
						 &RespInfo,
						 FreeReq.msgid.msgid,
						 FALSE,
						 &RecvSocketAddress );

	Unlock();


	if(hr!=DPN_OK){
		goto exit;
	}

	if(RespInfo.msgtype != RSIP_FREE_RESPONSE){
		DPF(0,"Failing registration, response was message type %d\n",RespInfo.msgtype);
		goto error_exit;
	}

exit:
	DPF( 8, "<==Release Port, Succeeded" );
	return hr;

error_exit:
	DPF( 8, "<==Release Port, Failed" );
	return DPNERR_GENERIC;

}

/*=============================================================================

	CRsip::QueryLocalAddress - get the local address of a public address

    Description:

    	Before connecting to anyone we need to see if there is a local
    	alias for its global address.  This is because the gateway will
    	not loopback if we try and connect to the global address, so
    	we need to know the local alias.

    Parameters:

    	BOOL		 ftcp_udp - whether we are querying a UDP or TCP port
    	SOCKADDR     saddrquery - the address to look up
    	SOCKADDR     saddrlocal - local alias if one exists

    Return Values:

		DPN_OK - got a local address.
		other - no mapping exists.

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::QueryLocalAddress"

HRESULT CRsip::QueryLocalAddress( BOOL ftcp_udp, const SOCKADDR *const saddrquery, SOCKADDR *const saddrlocal )
{
	#define saddrquery_in ((const SOCKADDR_IN *)saddrquery)
	#define saddrlocal_in ((SOCKADDR_IN *)saddrlocal)
	HRESULT hr;

	MSG_RSIP_QUERY  QueryReq;
	RSIP_RESPONSE_INFO RespInfo;

	PADDR_ENTRY pAddrEntry;
	SOCKADDR	RecvSocketAddress;


	Lock();

	DPF( 8, "==>RSIP QueryLocalAddress" );
	DumpSocketAddress( 8, saddrquery, saddrquery->sa_family );

	if(m_sRsip == INVALID_SOCKET){
		DPF(0,"rsipQueryLocalAddress: m_sRsip is invalid, bailing...\n");
		Unlock();
		return DPNERR_GENERIC;
	}

	// see if we have a cached entry.

	if(pAddrEntry=FindCacheEntry(ftcp_udp,saddrquery_in->sin_addr.s_addr,saddrquery_in->sin_port)){
		if(pAddrEntry->raddr){
			saddrlocal_in->sin_family      = AF_INET;
			saddrlocal_in->sin_addr.s_addr = pAddrEntry->raddr;
			saddrlocal_in->sin_port        = pAddrEntry->rport;
			Unlock();
			DPF( 8, "<==Found Local address in cache.\n" );
			return DPN_OK;
		} else {
			DPF( 8, "<==Found lack of local address in cache\n" );
			Unlock();
			return DPNERR_GENERIC;
		}
	}		

	// Build the request

	QueryReq.version    		= RSIP_VERSION;
	QueryReq.command    		= RSIP_QUERY_REQUEST;

	QueryReq.clientid.code 		= RSIP_CLIENTID_CODE;
	QueryReq.clientid.len 		= sizeof(DWORD);
	QueryReq.clientid.clientid 	= m_ClientID;

	QueryReq.address.code		= RSIP_ADDRESS_CODE;
	QueryReq.address.len		= sizeof(RSIP_ADDRESS)-sizeof(RSIP_PARAM);
	QueryReq.address.version	= 1; // IPv4
	QueryReq.address.addr		= saddrquery_in->sin_addr.s_addr;

	QueryReq.port.code			= RSIP_PORTS_CODE;
	QueryReq.port.len			= sizeof(RSIP_PORT)-sizeof(RSIP_PARAM);
	QueryReq.port.nports      	= 1;
	QueryReq.port.port			= p_htons(saddrquery_in->sin_port);

	QueryReq.porttype.code      = RSIP_VENDOR_CODE;
	QueryReq.porttype.len       = sizeof(RSIP_MSVENDOR_CODE)-sizeof(RSIP_PARAM);
	QueryReq.porttype.vendorid  = RSIP_MS_VENDOR_ID;
	QueryReq.porttype.option    = (ftcp_udp)?RSIP_TCP_PORT:RSIP_UDP_PORT;

	QueryReq.querytype.code	    = RSIP_VENDOR_CODE;
	QueryReq.querytype.len	    = sizeof(RSIP_MSVENDOR_CODE)-sizeof(RSIP_PARAM);
	QueryReq.querytype.vendorid = RSIP_MS_VENDOR_ID;
	QueryReq.querytype.option   = RSIP_QUERY_MAPPING;

	QueryReq.msgid.code 	    = RSIP_MESSAGEID_CODE;
	QueryReq.msgid.len  	    = sizeof(DWORD);
	QueryReq.msgid.msgid   		= InterlockedIncrement( &m_MsgID );

	hr=ExchangeAndParse( (PCHAR)&QueryReq,
						 sizeof(QueryReq),
						 &RespInfo,
						 QueryReq.msgid.msgid,
						 FALSE,
						 &RecvSocketAddress );

	Unlock();

	if(hr!=DPN_OK){
		goto exit;
	}

	if(RespInfo.msgtype != RSIP_QUERY_RESPONSE){
		DPF(0,"Failing query, response was message type %d\n",RespInfo.msgtype);
		goto error_exit;
	}

	saddrlocal_in->sin_family      = AF_INET;
	saddrlocal_in->sin_addr.s_addr = RespInfo.lAddressV4;
	saddrlocal_in->sin_port        = p_htons(RespInfo.lPort);

	//rsipAddCacheEntry(pgd,ftcp_udp,saddrquery_in->sin_addr.s_addr,saddrquery_in->sin_port,RespInfo.lAddressV4,p_htons(RespInfo.lPort));

	DPF( 8, "<==RSIP QueryLocalAddress, local alias is" );
	DumpSocketAddress( 8, reinterpret_cast<SOCKADDR*>( saddrlocal_in ), saddrlocal_in->sin_family );

exit:
	return hr;

error_exit:
	AddCacheEntry(ftcp_udp,saddrquery_in->sin_addr.s_addr,saddrquery_in->sin_port,0,0);
	DPF( 8, "<==RSIP QueryLocalAddress, NO Local alias\n" );
	return DPNERR_GENERIC;

	#undef saddrlocal_in
	#undef saddrquery_in
}

/*=============================================================================

	CRsip::ListenPort - assign a port mapping with the rsip server with a fixed
					 port.

    Description:

		Only used for the host server port (the one that is used for enums).
		Other than the fixed port this works the same as an rsipAssignPort.

		Since the port is fixed, the local and public port address are
		obviously the same.

    Parameters:

    	WORD	     port     - local port to get a remote port for (big endian)
    	BOOL		 ftcp_udp - whether we are assigning a UDP or TCP port
    	SOCKADDR     psaddr   - place to return assigned global realm address
    	PDWORD       pBindid  - identifier for this binding, used to extend
    							lease and/or release the binding (OPTIONAL).
    Return Values:

		DPN_OK - assigned succeeded, psaddr contains public realm address,
				*pBindid is the binding identifier.

		DPNERR_GENERIC - assignment of a public port could not be made.


-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::ListenPort"

HRESULT CRsip::ListenPort( BOOL ftcp_udp, WORD port, SOCKADDR *psaddr, DWORD *pBindid )
{
	#define psaddr_in ((SOCKADDR_IN *)psaddr)

	HRESULT hr;
	SOCKADDR	RecvSocketAddress;

	MSG_RSIP_LISTEN_PORT ListenReq;
	RSIP_RESPONSE_INFO RespInfo;

	Lock();

	DPF( 8, "RSIP Listen Port %d\n", p_htons( port ) );

	if(m_sRsip == INVALID_SOCKET){
		DPF(0,"rsipListenPort: m_sRsip is invalid, bailing...\n");
		Unlock();
		return DPNERR_GENERIC;
	}

	// Build the request

	ListenReq.version    		  = RSIP_VERSION;
	ListenReq.command    		  = RSIP_LISTEN_REQUEST;

	ListenReq.clientid.code 	  = RSIP_CLIENTID_CODE;
	ListenReq.clientid.len 		  = sizeof(DWORD);
	ListenReq.clientid.clientid   = m_ClientID;

	// Local Address (will be returned by RSIP server, us don't care value)

	ListenReq.laddress.code		  = RSIP_ADDRESS_CODE;
	ListenReq.laddress.len		  = sizeof(RSIP_ADDRESS)-sizeof(RSIP_PARAM);
	ListenReq.laddress.version	  = 1; // IPv4
	ListenReq.laddress.addr		  = 0; // Don't care

	// Local Port, this is a port we have opened that we are assigning a
	// global alias for.

	ListenReq.lport.code		 = RSIP_PORTS_CODE;
	ListenReq.lport.len			 = sizeof(RSIP_PORT)-sizeof(RSIP_PARAM);
	ListenReq.lport.nports       = 1;
	ListenReq.lport.port		 = p_htons(port);//->little endian for wire

	// Remote Address (not used with our flow control policy, use don't care value)

	ListenReq.raddress.code		 = RSIP_ADDRESS_CODE;
	ListenReq.raddress.len		 = sizeof(RSIP_ADDRESS)-sizeof(RSIP_PARAM);
	ListenReq.raddress.version   = 1; // IPv4
	ListenReq.raddress.addr      = 0; // Don't care

	ListenReq.rport.code		 = RSIP_PORTS_CODE;
	ListenReq.rport.len			 = sizeof(RSIP_PORT)-sizeof(RSIP_PARAM);
	ListenReq.rport.nports       = 1;
	ListenReq.rport.port		 = 0; // Don't care

	// Following parameters are optional according to RSIP spec...

	// Lease code, ask for an hour, but don't count on it.

	ListenReq.lease.code		 = RSIP_LEASE_CODE;
	ListenReq.lease.len			 = sizeof(RSIP_LEASE)-sizeof(RSIP_PARAM);
	ListenReq.lease.leasetime    = 3600;

	// Tunnell Type is IP-IP

	ListenReq.tunnel.code		 = RSIP_TUNNELTYPE_CODE;
	ListenReq.tunnel.len		 = sizeof(RSIP_TUNNEL)-sizeof(RSIP_PARAM);
	ListenReq.tunnel.tunneltype  = TUNNEL_IP_IP;

	// Message ID is optional, but we use it since we use UDP xport it is required.

	ListenReq.msgid.code 		 = RSIP_MESSAGEID_CODE;
	ListenReq.msgid.len  		 = sizeof(DWORD);
	ListenReq.msgid.msgid   	 = InterlockedIncrement( &m_MsgID );

	// Vendor specific - need to specify port type and no-tunneling

	ListenReq.porttype.code      = RSIP_VENDOR_CODE;
	ListenReq.porttype.len       = sizeof(RSIP_MSVENDOR_CODE)-sizeof(RSIP_PARAM);
	ListenReq.porttype.vendorid  = RSIP_MS_VENDOR_ID;
	ListenReq.porttype.option    = (ftcp_udp)?(RSIP_TCP_PORT):(RSIP_UDP_PORT);

	ListenReq.tunneloptions.code     = RSIP_VENDOR_CODE;
	ListenReq.tunneloptions.len      = sizeof(RSIP_MSVENDOR_CODE)-sizeof(RSIP_PARAM);
	ListenReq.tunneloptions.vendorid = RSIP_MS_VENDOR_ID;
	ListenReq.tunneloptions.option   = RSIP_NO_TUNNEL;

	ListenReq.listentype.code     = RSIP_VENDOR_CODE;
	ListenReq.listentype.len      = sizeof(RSIP_MSVENDOR_CODE)-sizeof(RSIP_PARAM);
	ListenReq.listentype.vendorid = RSIP_MS_VENDOR_ID;
	ListenReq.listentype.option   = RSIP_SHARED_UDP_LISTENER;


	hr=ExchangeAndParse( (PCHAR)&ListenReq,
						 sizeof(ListenReq),
						 &RespInfo,
						 ListenReq.msgid.msgid,
						 FALSE,
						 &RecvSocketAddress );
	Unlock();

	if(hr!=DPN_OK){
		goto exit;
	}

	if(RespInfo.msgtype != RSIP_LISTEN_RESPONSE){
		DPF(0,"Assignment failed? Response was %d\n",RespInfo.msgtype);
		goto error_exit;
	}

	if(psaddr_in){
		psaddr_in->sin_family      = AF_INET;
		psaddr_in->sin_addr.s_addr = RespInfo.lAddressV4;
		psaddr_in->sin_port        = p_htons(RespInfo.lPort);// currently little endian on wire

		DPF( 8, "RSIP Listen, public address is" );
		DumpSocketAddress( 8, reinterpret_cast<SOCKADDR*>( psaddr_in ), psaddr_in->sin_family );
	}

	if(pBindid){
		*pBindid = RespInfo.bindid;
	}	

	// remember the lease so we will renew it when necessary.
	AddLease(RespInfo.bindid,ftcp_udp,RespInfo.lAddressV4,p_htons(RespInfo.lPort),port,RespInfo.leasetime);


exit:
	DPF( 8, "<==RSIP Listen succeeded\n" );
	return hr;

error_exit:
	DPF( 8, "<==RSIP Listen failed\n" );
	return DPNERR_GENERIC;

	#undef psaddr_in
}

/*=============================================================================

	CRsip::FindLease - see if we already have a lease for a local port.

    Description:

    Parameters:

    Return Values:

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::FindLease"

PRSIP_LEASE_RECORD CRsip::FindLease( BOOL ftcp_udp, WORD port)
{
	PRSIP_LEASE_RECORD pLeaseWalker;

	pLeaseWalker = m_pRsipLeaseRecords;

	while(pLeaseWalker){
		if( ( pLeaseWalker->ftcp_udp == ftcp_udp ) && ( pLeaseWalker->port == port ) )
		{
			break;
		}
		pLeaseWalker=pLeaseWalker->pNext;
	}

	return pLeaseWalker;
}

/*=============================================================================

	CRsip::AddLease - adds a lease record to our list of leases

    Description:

    Parameters:

    Return Values:

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::AddLease"

void	CRsip::AddLease( DWORD bindid, BOOL ftcp_udp, DWORD addrV4, WORD rport, WORD port, DWORD tLease)
{
	RSIP_LEASE_RECORD	*pLeaseWalker;
	RSIP_LEASE_RECORD	*pNewLease;
	DWORD	tNow;

	tNow=timeGetTime();

	// First see if we already have a lease for this port;
	Lock();

	// first make sure there isn't already a lease for this port
	pLeaseWalker = m_pRsipLeaseRecords;
	while(pLeaseWalker){
		if(pLeaseWalker->ftcp_udp == ftcp_udp &&
		   pLeaseWalker->port     == port     &&
		   pLeaseWalker->bindid   == bindid
		)
		{
			break;
		}
		pLeaseWalker=pLeaseWalker->pNext;
	}

	if(pLeaseWalker){
		pLeaseWalker->dwRefCount++;
		pLeaseWalker->tExpiry = tNow+(tLease*1000);
	} else {
		pNewLease = static_cast<RSIP_LEASE_RECORD*>( DNMalloc( sizeof( *pNewLease ) ) );
		if(pNewLease){
			pNewLease->dwRefCount = 1;
			pNewLease->ftcp_udp   = ftcp_udp;
			pNewLease->tExpiry    = tNow+(tLease*1000);
			pNewLease->bindid     = bindid;
			pNewLease->port		  = port;
			pNewLease->rport      = rport;
			pNewLease->addrV4     = addrV4;
			pNewLease->pNext	  = m_pRsipLeaseRecords;
			m_pRsipLeaseRecords= pNewLease;
		} else {
			DPF(0,"rsip: Couldn't allocate new lease block for port %x\n",port);
		}
	}

	Unlock();
}

/*=============================================================================

	CRsip::RemoveLease - removes a lease record from our list of leases

    Description:

    Parameters:

    Return Values:

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::RemoveLease"

void	CRsip::RemoveLease( DWORD bindid )
{
	PRSIP_LEASE_RECORD pLeaseWalker, pLeasePrev;

	DPF( 7, "==>rsipRemoveLease bindid %d\n", bindid );

	Lock();

	pLeaseWalker = m_pRsipLeaseRecords;
	pLeasePrev=(PRSIP_LEASE_RECORD)&m_pRsipLeaseRecords; //sneaky.

	while(pLeaseWalker){
		if(pLeaseWalker->bindid==bindid){
			--pLeaseWalker->dwRefCount;
			if(!pLeaseWalker->dwRefCount){
				// link over it
				pLeasePrev->pNext=pLeaseWalker->pNext;
				DPF( 7, "rsipRemove: removing bindid %d\n", bindid );
				DNFree(pLeaseWalker);
			} else {
				DPF( 7, "rsipRemove: refcount on bindid %d is %d\n", bindid, pLeaseWalker->dwRefCount );
			}
			break;
		}
		pLeasePrev=pLeaseWalker;
		pLeaseWalker=pLeaseWalker->pNext;
	}

	Unlock();

	DPF( 7, "<==rsipRemoveLease bindid %d\n", bindid );
}

/*=============================================================================

	CRsip::PortExtend - checks if port leases needs extension and extends
					 them if necessary

    Description:

    Parameters:

    Return Values:

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::PortExtend"

void	CRsip::PortExtend( const DWORD time )
{
	PRSIP_LEASE_RECORD pLeaseWalker;
	DWORD tExtend;
	HRESULT hr;


	Lock();

	pLeaseWalker = m_pRsipLeaseRecords;
	while(pLeaseWalker){

		if((int)(pLeaseWalker->tExpiry - time) < 180000){
			// less than 2 minutes left on lease.
			hr=ExtendPort(pLeaseWalker->bindid, &tExtend);			
			if(hr != DPN_OK){
				// this binding is now gone!
				DPF(0,"Couldn't renew lease on bindid %d, port %x\n",pLeaseWalker->bindid, pLeaseWalker->port);
			} else {
				pLeaseWalker->tExpiry=time+(tExtend*1000);
				DPF( 8, "rsip: Extended Lease of Port %x by %d seconds\n", pLeaseWalker->bindid, tExtend );
				DNASSERT(tExtend > 180);
			}
		}
		pLeaseWalker=pLeaseWalker->pNext;
	}

	Unlock();
}


/*=============================================================================

	CRsip::FindCacheEntry

    Description:

    Parameters:

    Return Values:

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::FindCacheEntry"

PADDR_ENTRY CRsip::FindCacheEntry( BOOL ftcp_udp, DWORD addr, WORD port)
{
	PADDR_ENTRY pAddrWalker;

	pAddrWalker = m_pAddrEntry;

	while(pAddrWalker){
		if(pAddrWalker->ftcp_udp == ftcp_udp &&
		   pAddrWalker->port     == port     &&
		   pAddrWalker->addr     == addr
		)
		{
			// if he looked it up, give it another minute to time out.
			pAddrWalker->tExpiry=timeGetTime()+60*1000;
			// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
			DPF( 8, "Returning Cache Entry Addr:Port (%p:%d)  Alias Addr:(Port %p:%d)\n",
				pAddrWalker->addr,
				p_htons(pAddrWalker->port),
				pAddrWalker->raddr,
				p_htons(pAddrWalker->rport));
			break;
		}
		pAddrWalker=pAddrWalker->pNext;
	}

	return pAddrWalker;


}

/*=============================================================================

	CRsip::AddCacheEntry - adds a cache entry or updates timeout on existing one.

    Description:

    	Adds an address mapping from public realm to local realm (or the
    	lack of such a mapping) to the cache of mappings.

    Parameters:

    Return Values:

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::AddCacheEntry"

void	CRsip::AddCacheEntry( BOOL ftcp_udp, DWORD addr, WORD port, DWORD raddr, WORD rport)
{
	ADDR_ENTRY	*pAddrWalker;
	ADDR_ENTRY	*pNewAddr;
	DWORD		tNow;


	tNow=timeGetTime();

	// First see if we already have a lease for this port;
	Lock();

	// first make sure there isn't already a lease for this port
	pAddrWalker = m_pAddrEntry;
	while(pAddrWalker){
		if(pAddrWalker->ftcp_udp == ftcp_udp &&
		   pAddrWalker->port     == port     &&
		   pAddrWalker->addr     == addr
		)
		{
			break;
		}
		pAddrWalker=pAddrWalker->pNext;
	}

	if(pAddrWalker){
		pAddrWalker->tExpiry = tNow+(60*1000); // keep for 60 seconds or 60 seconds from last reference
	} else {
		pNewAddr = static_cast<ADDR_ENTRY*>( DNMalloc( sizeof( *pNewAddr ) ) );
		if(pNewAddr){
			// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers and handles.
			DPF( 8, "Added Cache Entry Addr:Port (%p:%d)  Alias Addr:(Port %p:%d)\n", addr, p_htons( port ), raddr, p_htons( rport ) );
			pNewAddr->ftcp_udp   = ftcp_udp;
			pNewAddr->tExpiry    = tNow+(60*1000);
			pNewAddr->port		 = port;
			pNewAddr->addr		 = addr;
			pNewAddr->rport      = rport;
			pNewAddr->raddr      = raddr;
			pNewAddr->pNext	     = m_pAddrEntry;
			m_pAddrEntry		 = pNewAddr;
		} else {
			DPF(0,"rsip: Couldn't allocate new lease block for port %x\n",port);
		}
	}

	Unlock();
}


/*=============================================================================

	CRsip::CacheClear - checks if cached mappings are old and deletes them.

    Description:

    Parameters:

    Return Values:

-----------------------------------------------------------------------------*/
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::CacheClear"

void CRsip::CacheClear( const DWORD time )
{
	PADDR_ENTRY pAddrWalker, pAddrPrev;


	Lock();

	pAddrWalker = m_pAddrEntry;
	pAddrPrev=(PADDR_ENTRY)&m_pAddrEntry; //sneaky.

	while(pAddrWalker){

		if((int)(pAddrWalker->tExpiry - time) < 0){
			// cache entry expired.
			pAddrPrev->pNext=pAddrWalker->pNext;
			// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers and handles.
			DPF( 7, "rsipRemove: removing cached address entry %p\n", pAddrWalker );
			DNFree(pAddrWalker);
		} else {
			pAddrPrev=pAddrWalker;
		}	
		pAddrWalker=pAddrPrev->pNext;
	}

	Unlock();
}

//**********************************************************************
// ------------------------------
// CRsip::RsipTimerComplete - Rsip timer job has completed
//
// Entry:		Timer result code
//				Context
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::RsipTimerComplete"

void	CRsip::RsipTimerComplete( const HRESULT hResult, void *const pContext )
{
	CRsip	*pThisRsip;

	
	DNASSERT( pContext != NULL );
	pThisRsip = static_cast<CRsip*>( pContext );

	pThisRsip->DecRef();
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CRsip::RsipTimerFunction - Rsip timer job needs service
//
// Entry:		Context
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::RsipTimerFunction"

void	CRsip::RsipTimerFunction( void *const pContext )
{
	CRsip	*pThisRsip;
	DWORD	dwTime;


	DNASSERT( pContext != NULL );
	dwTime = timeGetTime();
	pThisRsip = static_cast<CRsip*>( pContext );
	pThisRsip->PortExtend( dwTime );
	pThisRsip->CacheClear( dwTime );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// CRsip::ReturnSelfToPool - return this object to the pool
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define	DPF_MODNAME "CRsip::ReturnSelfToPool"

void	CRsip::ReturnSelfToPool( void )
{
	Deinitialize();
	ReturnRsip( this );
}
//**********************************************************************

