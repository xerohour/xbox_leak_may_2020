/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Common.h
 *  Content:    DirectNet common code header file
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *	10/08/99	jtk		Created
 *	01/14/00	mjn		Added pvUserContext to DN_Host
 *	01/23/00	mjn		Added DN_DestroyPlayer and DNTerminateSession
 *	01/28/00	mjn		Added DN_ReturnBuffer
 *	02/01/00	mjn		Added DN_GetCaps, DN_SetCaps
 *	02/15/00	mjn		Implement INFO flags in SetInfo and return context in GetInfo
 *	02/17/00	mjn		Implemented GetPlayerContext and GetGroupContext
 *	02/17/00	mjn		Reordered parameters in EnumServiceProviders,EnumHosts,Connect,Host
 *	02/18/00	mjn		Converted DNADDRESS to IDirectPlayAddress8
 *  03/17/00    rmt     Moved caps funcs to caps.h/caps.cpp
 *	04/06/00	mjn		Added DN_GetHostAddress()
 *	04/19/00	mjn		Changed DN_SendTo to accept a range of DPN_BUFFER_DESCs and a count
 *	06/23/00	mjn		Removed dwPriority from DN_SendTo()
 *	06/25/00	mjn		Added DNUpdateLobbyStatus()
 *  07/09/00	rmt		Bug #38323 - RegisterLobby needs a DPNHANDLE parameter.
 *	07/30/00	mjn		Added hrReason to DNTerminateSession()
 *	08/15/00	mjn		Added hProtocol tp DNRegisterWithDPNSVR()
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#ifndef	__COMMON_H__
#define	__COMMON_H__

//**********************************************************************
// Constant definitions
//**********************************************************************

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

typedef	struct	_PROTOCOL_ENUM_DATA PROTOCOL_ENUM_DATA;

typedef	struct	_PROTOCOL_ENUM_RESPONSE_DATA PROTOCOL_ENUM_RESPONSE_DATA;

//**********************************************************************
// Variable definitions
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************


HRESULT DNTerminateSession(DIRECTNETOBJECT *const pdnObject,
						   const HRESULT hrReason);

HRESULT DNGetMaxEnumFrameSize(DIRECTNETOBJECT *const pdnObject,
							  DWORD *const pdwEnumFrameSize);

HRESULT DNAddRefLock(DIRECTNETOBJECT *const pdnObject);

void DNDecRefLock(DIRECTNETOBJECT *const pdnObject);

STDMETHODIMP DN_DumpNameTable(PVOID pInterface,char *const Buffer);

#endif	// __COMMON_H__
