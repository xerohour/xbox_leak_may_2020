/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Enum_SP.h
 *  Content:    DirectNet SP/Adapter Enumeration
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *	01/15/00	mjn		Created
 *	04/08/00	mjn		Added DN_SPCrackEndPoint()
 *	05/01/00	mjn		Prevent unusable SPs from being enumerated.
 *	07/29/00	mjn		Added fUseCachedCaps to DN_SPEnsureLoaded()
 *	08/16/00	mjn		Removed DN_SPCrackEndPoint()
 *	08/20/00	mjn		Added DN_SPInstantiate(), DN_SPLoad()
 *				mjn		Removed fUseCachedCaps from DN_SPEnsureLoaded()
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#ifndef	__ENUM_SP_H__
#define	__ENUM_SP_H__

//**********************************************************************
// Constant definitions
//**********************************************************************

#ifndef	GUID_STRING_LENGTH
#define	GUID_STRING_LENGTH	((sizeof(GUID) * 2) + 2 + 4)
#endif
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

//
//	Enumeration
//
HRESULT DN_EnumSP(DIRECTNETOBJECT *const pdnObject,
				  const DWORD dwFlags,
				  const GUID *const lpguidApplication,
				  DPN_SERVICE_PROVIDER_INFO *const pSPInfoBuffer,
				  DWORD *const pcbEnumData,
				  DWORD *const pcReturned);

void DN_SPReleaseAll(DIRECTNETOBJECT *const pdnObject);

HRESULT DN_SPFindEntry(DIRECTNETOBJECT *const pdnObject,
					   const GUID *const pguidSP,
					   CServiceProvider **const ppSP);

HRESULT DN_SPInstantiate(DIRECTNETOBJECT *const pdnObject,
						 const GUID *const pguid,
						 CServiceProvider **const ppSP);

HRESULT DN_SPLoad(DIRECTNETOBJECT *const pdnObject,
				  const GUID *const pguid,
				  CServiceProvider **const ppSP);

HRESULT DN_SPEnsureLoaded(DIRECTNETOBJECT *const pdnObject,
						  const GUID *const pguid,
						  CServiceProvider **const ppSP);


#endif	// __ENUM_SP_H__
