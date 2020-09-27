/*==========================================================================
 *
 *  Copyright (C) 1999-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		SPData.h
 *  Content:	Global information for the DNSerial service provider in class
 *				format.
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	03/15/1999	jtk		Derived from Locals.h
 *  03/22/20000	jtk		Updated with changes to interface names
 ***************************************************************************/

#ifndef __SPDATA_H__
#define __SPDATA_H__

//**********************************************************************
// Constant definitions
//**********************************************************************

#define	DEFAULT_ADDRESS_BUFFER_SIZE	151

//
// enumeration of the states the SP can be in
//
typedef enum
{
	SPSTATE_UNKNOWN = 0,		// uninitialized state
	SPSTATE_UNINITIALIZED = 0,	// uninitialized state
	SPSTATE_INITIALIZED,		// service provider has been initialized
	SPSTATE_CLOSING				// service provider is closing
} SPSTATE;

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

// forward structure and class references
class	CCommandData;
class	CEndpoint;
class	CIPEndpoint;
class	CIPAddress;
// BUGBUG: [mgere] [xbox] Removed RSIP
//class	CRsip;
class	CSocketAddress;
class	CSocketPort;
class	CThreadPool;
class	CWriteIOData;
typedef	enum	_ENDPOINT_TYPE		ENDPOINT_TYPE;
typedef	enum	_GATEWAY_BIND_TYPE	GATEWAY_BIND_TYPE;
typedef	struct	_SPRECEIVEDBUFFER	SPRECEIVEDBUFFER;
typedef	struct	_THREAD_POOL_JOB	THREAD_POOL_JOB;
typedef	void	JOB_FUNCTION( THREAD_POOL_JOB *const pJobInfo );
typedef	void	DNADDRESS, *PDNADDRESS;

//**********************************************************************
// Class definitions
//**********************************************************************

//
// class for information used by the provider
//
class	CSPData
{	
	STDNEWDELETE

	public:
		CSPData();
		~CSPData();
		
		DWORD	AddRef( void ) { return DNInterlockedIncrement( &m_lRefCount ); }
		
		DWORD	DecRef( void )
		{
			DWORD	dwReturn;
			
				
			dwReturn = DNInterlockedDecrement( &m_lRefCount );
			if ( dwReturn == 0 )
			{
				//
				// WARNING, the following function deletes this object!!!
				//
				DestroyThisObject();
			}

			return	dwReturn;
		}

		#undef DPF_MODNAME
		#define	DPF_MODNAME "CSPData::ObjectAddRef"
		void	ObjectAddRef( void )
		{
			AddRef();
			
			Lock();
			if ( DNInterlockedIncrement( &m_lObjectRefCount ) == 1 )
			{
				DNASSERT( m_hShutdownEvent != NULL );
				if ( ResetEvent( m_hShutdownEvent ) == FALSE )
				{
					DWORD	dwError;


					dwError = GetLastError();
					DPF( 0, "Failed to reset shutdown event!" );
					DisplayErrorCode( 0, dwError );
				}
			}

			Unlock();
		}

		#undef DPF_MODNAME
		#define	DPF_MODNAME "CSPData::ObjectDecRef"
		void	ObjectDecRef( void )
		{
			Lock();

			if ( DNInterlockedDecrement( &m_lObjectRefCount ) == 0 )
			{
				if ( SetEvent( m_hShutdownEvent ) == FALSE )
				{
					DWORD	dwError;


					dwError = GetLastError();
					DPF( 0, "Failed to set shutdown event!" );
					DisplayErrorCode( 0, dwError );
				}
			}
			
			Unlock();
			
			DecRef();
		}
		

		HRESULT	Initialize( const CLSID *const pClassID,
							IDP8ServiceProviderVtbl *const pVtbl );
		void	Shutdown( void );
		void	Deinitialize( void );

		void	SetCallbackData( const SPINITIALIZEDATA *const pInitData );

		void	Lock( void ) { DNEnterCriticalSection( &m_Lock ); }
		void	Unlock( void ) { DNLeaveCriticalSection( &m_Lock ); }

		const SPSTATE	GetState( void ) const { return m_SPState; }

		#undef DPF_MODNAME
		#define DPF_MODNAME "CSPData::SetState"
		void SetState( const SPSTATE NewState )
		{
			DNASSERT( ( NewState == SPSTATE_UNINITIALIZED ) ||
					  ( NewState == SPSTATE_INITIALIZED ) ||
					  ( NewState == SPSTATE_CLOSING ) );

			m_SPState = NewState;
		}

		BOOL	IsRsipServer( void ) const { return m_fIsRsipServer; }
		const SOCKADDR	*GetPublicSocketAddress( void ) const
		{
			const SOCKADDR	*pReturn;
			

			pReturn = NULL;
			if ( IsRsipServer() != FALSE )
			{
				pReturn = &m_PublicSocketAddress;
			}

			return	pReturn;
		}

		CThreadPool	*GetThreadPool( void ) const { return m_pThreadPool; }

		#undef DPF_MODNAME
		#define DPF_MODNAME "CSPData::SetThreadPool"
		void	SetThreadPool( CThreadPool *const pThreadPool )
		{
			DNASSERT( ( m_pThreadPool == NULL ) || ( pThreadPool == NULL ) );
			m_pThreadPool = pThreadPool;
		}


		//
		// functions to manage the socket port list
		//
		void	LockSocketPortData( void ) { DNEnterCriticalSection( &m_SocketPortDataLock ); }
		void	UnlockSocketPortData( void ) { DNLeaveCriticalSection( &m_SocketPortDataLock ); }
		HRESULT	BindEndpoint( CEndpoint *const pEndpoint,
							  IDirectPlay8Address *const pDeviceAddress,
							  const CSocketAddress *const pSocketAddress,
							  const GATEWAY_BIND_TYPE GatewayBindType );
		void	UnbindEndpoint( CEndpoint *const pEndpoint );

		//
		// address pool functions
		//
		CSocketAddress	*GetNewAddress( void );
		void			ReturnAddress( CSocketAddress *const pAddress );

		//
		// endpoint pool management
		//
		CEndpoint	*GetNewEndpoint( void );
		CEndpoint	*EndpointFromHandle( const HANDLE hEndpoint );
		void		CloseEndpointHandle( CEndpoint *const pEndpoint );
		CEndpoint	*GetEndpointAndCloseHandle( const HANDLE hEndpoint );

		//
		// generic address functions
		//
		#undef DPF_MODNAME
		#define DPF_MODNAME "CSPData::GetBroadcastHostAddress"
		IDirectPlay8Address *GetBroadcastHostAddress( void ) const
		{
			DNASSERT( m_pBroadcastAddress != NULL );
			return	m_pBroadcastAddress;
		}

		#undef DPF_MODNAME
		#define DPF_MODNAME "GetListenDeviceAddress"
		IDirectPlay8Address *GetListenDeviceAddress( void ) const
		{
			DNASSERT( m_pListenAddress != NULL );
			IDirectPlay8Address_AddRef( m_pListenAddress );
			return	m_pListenAddress;
		}

		#undef DPF_MODNAME
		#define DPF_MODNAME "CSPData::GetGenericDeviceAddress"
		IDirectPlay8Address	 *GetGenericDeviceAddress( void ) const
		{
			DNASSERT( m_pGenericAddress != NULL );
			IDirectPlay8Address_AddRef( m_pGenericAddress );
			return	m_pGenericAddress;
		}

//		CRsip	*RsipModule( void ) const { return m_pRsip; }
//		void	SetRsipModule( CRsip *const pRsip )
//		{
//		    DNASSERT( ( m_pRsip == NULL ) || ( pRsip == NULL ) );
//		    m_pRsip = pRsip;
//		}

		IDP8SPCallback	*DP8SPCallbackInterface( void ) { return reinterpret_cast<IDP8SPCallback*>( m_InitData.pIDP ); }
		IDP8ServiceProvider	*COMInterface( void ) { return reinterpret_cast<IDP8ServiceProvider*>( &m_COMInterface ); }

		#undef DPF_MODNAME
		#define DPF_MODNAME "CSPData::SPDataFromCOMInterface"
		static	CSPData	*SPDataFromCOMInterface( IDP8ServiceProvider *const pCOMInterface )
		{
			DNASSERT( pCOMInterface != NULL );
			DBG_CASSERT( sizeof( BYTE* ) == sizeof( pCOMInterface ) );
			DBG_CASSERT( sizeof( CSPData* ) == sizeof( BYTE* ) );
			return	reinterpret_cast<CSPData*>( &reinterpret_cast<BYTE*>( pCOMInterface )[ -OFFSETOF( CSPData, m_COMInterface ) ] );
		}

		void	SetWinsockBufferSizeOnAllSockets( const INT iBufferSize );

	private:
		DNCRITICAL_SECTION	m_Lock;					// lock
		volatile LONG		m_lRefCount;			// reference count
		volatile LONG		m_lObjectRefCount;		// reference count of outstanding objects (CEndpoint, CSocketPort, etc.)
		HANDLE				m_hShutdownEvent;		// handle for shutdown
		SPSTATE				m_SPState;				// what state is the SP in?
		SPINITIALIZEDATA	m_InitData;				// initialization data
		BOOL				m_fIsRsipServer;		// Boolean indication status of Rsip on this machine
		SOCKADDR			m_PublicSocketAddress;	// public socket address to be returned to user

		//
		// job management
		//
		CThreadPool		*m_pThreadPool;

		//
		// List of active adapters.  This encapsulates active device addresses
		// so sockets can be reused when possible and contains a CRsip interface
		// for each adapter (if applicable)
		//
		CBilink	m_ActiveAdapterList;

		//
		// default addresses
		//
		IDirectPlay8Address	*m_pBroadcastAddress;	// broadcast address for this protocol
		IDirectPlay8Address	*m_pListenAddress;		// listen address for this protocol
		IDirectPlay8Address	*m_pGenericAddress;		// generic address for this protocol (allows protocol
													// to choose address and port)

		CHandleTables			m_HandleTables;			// handle table for endpoints
		DNCRITICAL_SECTION		m_SocketPortDataLock;	// lock for socket port list
		CClassHash< CSocketPort*, const CSocketAddress* >	m_ActiveSocketPortList;	// list of active socket ports

		//
		// initialization state Booleans
		//
		BOOL	m_fHandleTableInitialized;
		BOOL	m_fLockInitialized;
		BOOL	m_fSocketPortDataLockInitialized;
		BOOL	m_fSocketPortListInitialized;
		BOOL	m_fInterfaceGlobalsInitialized;
		BOOL	m_fDefaultAddressesBuilt;

		struct
		{
			IDP8ServiceProviderVtbl	*m_pCOMVtbl;
		} m_COMInterface;

		HRESULT	BuildDefaultAddresses( void );
		void	FreeDefaultAddresses( void );
		void	DestroyThisObject( void );
		
		//
		// prevent unwarranted copies
		//
		CSPData( const CSPData & );
		CSPData& operator=( const CSPData & );
};

#undef DPF_MODNAME

#endif	// __SPDATA_H__
