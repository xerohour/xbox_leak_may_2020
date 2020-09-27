/*==========================================================================
 *
 *  Copyright (C) 1999 - 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Rsip.h
 *  Content:	Realm Specific IP Support header
 *
 *  History:
 *  Date		By		Reason
 *  ====		==		======
 *  12/7/99		rlamb	Original
 *  12/8/99     aarono  added function prototypes for rsip support in SP
 *  03/16/2000	johnkan	Converted to class for DPlay 8 Winsock
 *
 ***************************************************************************/


#ifndef __RSIP_H__
#define __RSIP_H__


//**********************************************************************
// Constant definitions
//**********************************************************************

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

//
// forward references
//
class	CThreadPool;
typedef	struct	_RSIP_RESPONSE_INFO		RSIP_RESPONSE_INFO;
typedef struct	_RSIP_LEASE_RECORD		RSIP_LEASE_RECORD;
typedef struct	_ADDR_ENTRY				ADDR_ENTRY;

//**********************************************************************
// Variable definitions
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Class definitions
//**********************************************************************

class	CRsip
{
	STDNEWDELETE

	public:
		CRsip();
		~CRsip();

		void	Lock( void ) { DNEnterCriticalSection( &m_Lock ); }
		void	Unlock( void ) { DNLeaveCriticalSection( &m_Lock ); }

		void	AddRef( void ) { DNInterlockedIncrement( &m_iRefCount ); }
		void	DecRef( void )
		{
			if ( DNInterlockedDecrement( &m_iRefCount ) == 0 )
			{
				ReturnSelfToPool();
			}
		}

		BOOL	Initialize( CThreadPool *const pThreadPool,
							const SOCKADDR *const pBaseSocketAddress,
							const BOOL fIsRsipServer );

		void	Deinitialize( void );

		BOOL	RsipIsRunningOnThisMachine( SOCKADDR *const pPublicSocketAddress );
		
		HRESULT	AssignPort( const BOOL fTCP_UDP,
							const WORD wPort,
							SOCKADDR *const pSocketAddress,
							DWORD *const pdwBindID );	// gets address on RSIP box
		
		HRESULT FreePort( const DWORD dwBindID );		// when port is done
		
		HRESULT QueryLocalAddress( const BOOL fTCP_UDP,
								   const SOCKADDR *const pQueryAddress,
								   SOCKADDR *const pLocalAddress) ;	// called to map remote to local
		
		HRESULT ListenPort( const BOOL fTCP_UDP,
							const WORD wPort,
							SOCKADDR *const pListenAddress,
							DWORD *const pBindID );		// called for ROD

	protected:

	private:
		DNCRITICAL_SECTION		m_Lock;
		volatile	LONG		m_iRefCount;
		volatile	SOCKET  	m_sRsip;
		volatile	BOOL		m_fTimerJobSubmitted;

		SOCKADDR_IN		m_saddrGateway;
		static LONG		m_MsgID;		// NOTE: there is only one instance of this for ALL CRsip objects!!!
		DWORD			m_ClientID;

		RSIP_LEASE_RECORD	*m_pRsipLeaseRecords;	// list of leases.
		ADDR_ENTRY        	*m_pAddrEntry;			// cache of mappings.
		DWORD 		 		m_tuRetry;				// microseconds starting retry time.
		CThreadPool			*m_pThreadPool;			// thread pool
		BOOL				m_fLockInitialized;		// Boolean indicating initialization of lock	

		// Local Functions
		HRESULT	FindGateway( const UINT myip, char *const gwipaddr);
		HRESULT	Register( void );
		HRESULT	Deregister( void );
		
		HRESULT	ExchangeAndParse( PCHAR pRequest,
								  const UINT cbReq,
								  RSIP_RESPONSE_INFO *const pRespInfo,
								  const DWORD messageid,
								  const BOOL bConnect,
								  SOCKADDR *const pRecvSocketAddress );
		
		HRESULT	Parse( CHAR *pBuf, DWORD cbBuf, RSIP_RESPONSE_INFO *const pRespInfo );
		HRESULT ExtendPort( const DWORD Bindid, DWORD *const ptExtend );
		void	RemoveLease( const DWORD dwBindID );
		void	AddLease( const DWORD dwBindID, const BOOL fTCP_UDP, const DWORD addrV4, const WORD lport, const WORD port, const DWORD tLease);
		RSIP_LEASE_RECORD	*FindLease( const BOOL fTCP_UDP, const WORD port );
		void	AddCacheEntry( const BOOL fTCP_UDP, const DWORD addr, const WORD port, const DWORD raddr, const WORD rport);
		ADDR_ENTRY	*FindCacheEntry( const BOOL fTCP_UDP, const DWORD addr, const WORD port);
		
		
		static void		RsipTimerComplete( const HRESULT hCompletionCode, void *const pContext );	
		static void		RsipTimerFunction( void *const pContext );
		void 	PortExtend( const DWORD dwTime );		// every 2 minutes
		void 	CacheClear( const DWORD dwTime );		// every 2 minutes
		
		void	ReturnSelfToPool( void );

		// prevent unwarranted copies
		CRsip( const CRsip & );
		CRsip& operator=( const CRsip & );

};

#endif /* __RSIP_H__ */

