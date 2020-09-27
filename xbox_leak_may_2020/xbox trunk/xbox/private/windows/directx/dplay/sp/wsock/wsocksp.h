/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	   WSockSP.h
 *  Content:	declaration of DN Winsock SP functions
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	10/26/98	jwo		Created it.
 ***************************************************************************/

#ifndef __WSOCKSP_H__
#define __WSOCKSP_H__

//**********************************************************************
// Constant definitions
//**********************************************************************

//**********************************************************************
// Macro definitions
//**********************************************************************

#define VALID_SP_PTR( ptr ) ( ptr && !IsBadWritePtr( ptr, sizeof( DNSPI_DNSP_INT )))
#define VALID_INIT_PTR( ptr ) ( ptr && !IsBadWritePtr( ptr, sizeof( SPINITDATA )))
#define VALID_ENUMQUERY_PTR( ptr ) ( ptr && !IsBadWritePtr( ptr, sizeof( SPENUMQUERYDATA )))
#define VALID_ENUMRESPOND_PTR( ptr ) ( ptr && !IsBadWritePtr( ptr, sizeof( SPENUMRESPONDDATA )))
#define VALID_CONNECT_PTR( ptr ) ( ptr && !IsBadWritePtr( ptr, sizeof( SPCONNECTDATA )))
#define VALID_DISCONNECT_PTR( ptr ) ( ptr && !IsBadWritePtr( ptr, sizeof( SPDISCONNECTDATA )))
#define VALID_LISTEN_PTR( ptr ) ( ptr && !IsBadWritePtr( ptr, sizeof( SPLISTENDATA )))
#define VALID_SENDDATA_PTR( ptr ) ( ptr && !IsBadWritePtr( ptr, sizeof( SPSENDDATA )))
#define VALID_CREATEGROUP_PTR( ptr ) ( ptr && !IsBadWritePtr( ptr, sizeof( SPCREATEGROUPDATA )))
#define VALID_ADDTOGROUP_PTR( ptr ) ( ptr && !IsBadWritePtr( ptr, sizeof( SPADDTOGROUPDATA )))
#define VALID_REMOVEFROMGROUP_PTR( ptr ) ( ptr && !IsBadWritePtr( ptr, sizeof( SPREMOVEFROMGROUPDATA )))
#define VALID_DELETEGROUP_PTR( ptr ) ( ptr && !IsBadWritePtr( ptr, sizeof( SPDELETEGROUPDATA )))

//**********************************************************************
// Structure definitions
//**********************************************************************

//**********************************************************************
// Variable definitions
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************
STDAPI DNSP_Initialize( IDP8ServiceProvider*, SPINITIALIZEDATA* );
STDMETHODIMP_(ULONG) DNSP_AddRef( IDP8ServiceProvider* pDNSP );
STDMETHODIMP_(ULONG) DNSP_Release( IDP8ServiceProvider* pDNSP );
STDMETHODIMP DNSP_Connect( IDP8ServiceProvider*, SPCONNECTDATA* );
STDMETHODIMP DNSP_Disconnect( IDP8ServiceProvider*, SPDISCONNECTDATA* );
STDMETHODIMP DNSP_Listen( IDP8ServiceProvider*, SPLISTENDATA* );
STDMETHODIMP DNSP_EnumQuery( IDP8ServiceProvider*, SPENUMQUERYDATA* );
STDMETHODIMP DNSP_EnumRespond( IDP8ServiceProvider*, SPENUMRESPONDDATA* );
STDMETHODIMP DNSP_SendData( IDP8ServiceProvider*, SPSENDDATA* );
STDMETHODIMP DNSP_CancelCommand( IDP8ServiceProvider*, HANDLE, DWORD );
STDMETHODIMP DNSP_Close( IDP8ServiceProvider* );
STDMETHODIMP DNSP_CreateGroup( IDP8ServiceProvider*, SPCREATEGROUPDATA* );
STDMETHODIMP DNSP_DeleteGroup( IDP8ServiceProvider*, SPDELETEGROUPDATA* );
STDMETHODIMP DNSP_AddToGroup( IDP8ServiceProvider*, SPADDTOGROUPDATA* );
STDMETHODIMP DNSP_RemoveFromGroup( IDP8ServiceProvider*, SPREMOVEFROMGROUPDATA* );
STDMETHODIMP DNSP_GetCaps( IDP8ServiceProvider*, SPGETCAPSDATA* );
STDMETHODIMP DNSP_SetCaps( IDP8ServiceProvider*, SPSETCAPSDATA* );
STDMETHODIMP DNSP_ReturnReceiveBuffers( IDP8ServiceProvider*, SPRECEIVEDBUFFER* );
STDMETHODIMP DNSP_GetAddressInfo( IDP8ServiceProvider*, SPGETADDRESSINFODATA* );
STDMETHODIMP DNSP_IsApplicationSupported( IDP8ServiceProvider*, SPISAPPLICATIONSUPPORTEDDATA* );
STDMETHODIMP DNSP_ProxyEnumQuery( IDP8ServiceProvider*, SPPROXYENUMQUERYDATA* );

#endif	// __WSOCKSP_H__
