////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _TVStat_HPP_
#define _TVStat_HPP_

#ifdef INITGUIDS
#include <initguid.h>
#endif

//Use Guidgen.exe to generate new unique GUIDs.
//{F1506F61-896F-11d0-9445-008029E64916}
DEFINE_GUID(IID_ITVOutStatus, 0xF1506F61, 0x896F, 0x11d0, 0x94, 0x45, 0x00, 0x80, 0x29, 0xE6, 0x49, 0x16);

#undef INTERFACE
#define INTERFACE ITVOutStatus

DECLARE_INTERFACE_(ITVOutStatus, IUnknown)
{
 	//IUnknown members;
 	STDMETHOD(QueryInterface) (THIS_ REFIID, LPVOID*) PURE;
 	STDMETHOD_(ULONG, AddRef) (THIS) PURE;
 	STDMETHOD_(ULONG, Release) (THIS) PURE;


	// Returns BOOL TRUE in <lpbIsTrue> if TV Out currently detects
	// a connected device
	STDMETHOD( IsTVConnected )			(THIS_ WORD wCard, LPBOOL lpbIsTrue) PURE;

	// Returns BOOL TRUE in <lpbIsTrue> if TV Out is currently enabled.
	STDMETHOD( IsTVEnabled )			(THIS_ WORD wCard, LPBOOL lpbIsTrue) PURE;

	// Returns BOOL TRUE in <lpbIsTrue> if analog passthough is currently enabled.
	STDMETHOD( IsAnalogPassThruEnabled ) (THIS_ WORD wCard, LPBOOL lpbIsTrue) PURE;

	// Returns BOOL TRUE in <lpbIsTrue> if multimedia mode
	// (digital passthough) is currently enabled.
	STDMETHOD( IsMultmediaEnabled )		(THIS_ WORD wCard, LPBOOL lpbIsTrue) PURE;

	// Returns the currently active standard in <lpdwStandard>. This value
	// is a single bit from those defined for the GetTVOutStandards method.
	STDMETHOD( GetCurrentStandard )		(THIS_ WORD wCard, LPDWORD lpdwStandard) PURE;

 	//ITVOutStatus members
};  typedef ITVOutStatus *LPITVOutStatus;

#endif //_TVStat_HPP_
