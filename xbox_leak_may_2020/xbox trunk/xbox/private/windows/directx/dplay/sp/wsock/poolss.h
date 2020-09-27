/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Pools.h
 *  Content:	Pool functions
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	01/20/2000	jtk		Derived from utils.h
 ***************************************************************************/

#ifndef __POOLS_H__
#define __POOLS_H__

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
// forward structure references
//
class	CAdapterEntry;
class	CCommandData;
class	CIPAddress;
class	CIPEndpoint;
// BUGBUG: [mgere] [xbox] Removed RSIP
//class	CRsip;
class	CSocketPort;
class	CSPData;
class	CThreadPool;

typedef	struct	_ENDPOINT_POOL_CONTEXT	ENDPOINT_POOL_CONTEXT;


//**********************************************************************
// Variable definitions
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************

BOOL	InitializePools( void );
void	DeinitializePools( void );

CAdapterEntry	*CreateAdapterEntry( void );
void	ReturnAdapterEntry( CAdapterEntry *const pAdapterEntry );

CCommandData	*CreateCommand( void );
void	ReturnCommand( CCommandData *const pCommandData );

CIPAddress	*CreateIPAddress( void );
void	ReturnIPAddress( CIPAddress *const pIPAddress );


CIPEndpoint		*CreateIPEndpoint( ENDPOINT_POOL_CONTEXT *pContext );

// BUGBUG: [mgere] [xbox] Removed RSIP
//CRsip	*CreateRsip( void );
//void	ReturnRsip( CRsip *const pRsipObject );

CSocketPort	*CreateSocketPort( void );
void	ReturnSocketPort( CSocketPort *const pSocketPort );

CThreadPool	*CreateThreadPool( void );
void	ReturnThreadPool( CThreadPool *const pThreadPool );

#endif	// __POOLS_H__
