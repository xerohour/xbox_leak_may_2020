/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Caps.h
 *  Content:    DirectPlay8 Caps routines
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *	03/17/00	rmt		Created
 *  03/25/00    rmt     Changed Get/SetActualSPCaps so takes interface instead of obj
 *	08/20/00	mjn		DNSetActualSPCaps() uses CServiceProvider object instead of GUID
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#ifndef	__CAPS_H__
#define	__CAPS_H__

//**********************************************************************
// Constant definitions
//**********************************************************************

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

class CServiceProvider;

typedef struct _DN_SP_CAPS_ENTRY {
    GUID        guidSP;
    DPN_SP_CAPS dpnCaps;
    CBilink      blList;
} DN_SP_CAPS_ENTRY, *PDN_SP_CAPS_ENTRY;

//**********************************************************************
// Variable definitions
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************

HRESULT DN_InitSPCapsList( DIRECTNETOBJECT *pdnObject );
HRESULT DN_FreeSPCapsList( DIRECTNETOBJECT *pdnObject );

HRESULT DNGetCachedSPCaps( DIRECTNETOBJECT *pdnObject, const GUID * const guidProvider, PDPN_SP_CAPS *ppCaps );
HRESULT DNSetCachedSPCaps( DIRECTNETOBJECT *pdnObject, const GUID * const guidProvider, const DPN_SP_CAPS * const pCaps );
HRESULT DNSetActualSPCaps( DIRECTNETOBJECT *pdnObject, CServiceProvider *const pSP, const DPN_SP_CAPS * const pCaps );
HRESULT DNGetActualSPCaps( DIRECTNETOBJECT *pdnObject, const GUID * const pguidSP, PDPN_SP_CAPS pCaps );


#endif	// __CONNECT_H__