/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Utils.h
 *  Content:	Utilitiy functions
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	11/25/98	jtk		Created
 ***************************************************************************/

#ifndef __UTILS_H__
#define __UTILS_H__

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
class	CPackedBuffer;
// BUGBUG: [mgere] [xbox] Removed RSIP
//class	CRsip;
class	CSPData;
class	CThreadPool;

//**********************************************************************
// Variable definitions
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************

BOOL	InitProcessGlobals( void );
void	DeinitProcessGlobals( void );

HRESULT	CreateSPData( CSPData **const ppSPData,
					  const CLSID *const pClassID,
					  IDP8ServiceProviderVtbl *const pVtbl );

HRESULT	InitializeInterfaceGlobals( CSPData *const pSPData );
void	DeinitializeInterfaceGlobals( CSPData *const pSPData );

//HRESULT	ApplyDeviceAddressDefaults( IDirectPlay8Address *const pInputAddress,
//									IDirectPlay8Address *const pDefaultAddress,
//									IDirectPlay8Address **const ppModifiedDeviceAddress );

/* BUGBUG: [mgere] [xbox] Removed -- multiple adapters not supported on xbox
HRESULT	AddNetworkAdapterToBuffer( CPackedBuffer *const pPackedBuffer,
								   const char *const pAdapterName,
								   const GUID *const pAdapterGUID );
*/

#endif	// __UTILS_H__
