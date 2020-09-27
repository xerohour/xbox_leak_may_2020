/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       IPEndpt.h
 *  Content:	IP endpoint
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	01/20/99	jtk		Created
 *	05/11/99	jtk		Split out to make a base class
 ***************************************************************************/

#ifndef __IP_ENDPOINT_H__
#define __IP_ENDPOINT_H__

//**********************************************************************
// Constant definitions
//**********************************************************************

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

//**********************************************************************
// Variable definitions
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Class definition
//**********************************************************************

class	CIPEndpoint : public CEndpoint
{
	STDNEWDELETE

	public:
		//
		// we need a virtual destructor to guarantee we call destructors in base classes
		//
		CIPEndpoint();
		~CIPEndpoint();

		//
		// pool functions
		//
		BOOL	PoolAllocFunction( ENDPOINT_POOL_CONTEXT *pContext );
		BOOL	PoolInitFunction( ENDPOINT_POOL_CONTEXT *pContext );
		void	PoolReleaseFunction( void );
		void	PoolDeallocFunction( void );
		void	ReturnSelfToPool( void );

		#undef DPF_MODNAME
		#define DPF_MODNAME "CIPEndpoint::SetOwningPool"
		void	SetOwningPool( CLockedContextFixedPool< CIPEndpoint, ENDPOINT_POOL_CONTEXT* > *pOwningPool )
		{
			DNASSERT( ( m_pOwningPool == NULL ) || ( pOwningPool == NULL ) );
			m_pOwningPool = pOwningPool;
		}

	protected:

	private:
		CLockedContextFixedPool< CIPEndpoint, ENDPOINT_POOL_CONTEXT* >	*m_pOwningPool;
		
		CIPAddress	m_IPAddress;
		
		//
		// make copy constructor and assignment operator private and unimplemented
		// to prevent illegal copies from being made
		//
		CIPEndpoint( const CIPEndpoint & );
		CIPEndpoint& operator=( const CIPEndpoint & );
};

#undef DPF_MODNAME

#endif	// __IP_ENDPOINT_H__

