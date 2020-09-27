/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       voice.h
 *  Content:    Direct Net Voice Transport Interface
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  01/17/00	rmt		Created
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#ifndef	__VOICE_H__
#define	__VOICE_H__

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

VOID    VoiceTarget_ClearTargetList( DIRECTNETOBJECT *pdnObject );
VOID    VoiceTarget_ClearExpandedTargetList(DIRECTNETOBJECT *pdnObject);
HRESULT VoiceTarget_AddExpandedTargetListEntry(DIRECTNETOBJECT *pdnObject, DPNID dpid);
void    VoiceTarget_AddIfNotAlreadyFound( DIRECTNETOBJECT *pdnObject, DPNID dpidID );
HRESULT VoiceTarget_ExpandTargetList(DIRECTNETOBJECT *pdnObject, DWORD nTargets, PDPNID pdvidTo);

//
// VTable for peer interface
//
//extern IDirectPlayVoiceTransportVtbl DN_VoiceTbl;

HRESULT Voice_Notify( DIRECTNETOBJECT *pObject, DWORD dwMsgID, DWORD_PTR dwParam1, DWORD_PTR dwParam2, DWORD dwObjectType = DVTRANSPORT_OBJECTTYPE_BOTH );
HRESULT Voice_Receive(DIRECTNETOBJECT *pObject, DVID dvidFrom, DVID dvidTo, LPVOID lpvMessage, DWORD dwMessageLen );


//**********************************************************************
// Function prototypes
//**********************************************************************

#endif	// __VOICE_H__
