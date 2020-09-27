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

#ifndef _ITvOutCaps_HPP_
#define _ITvOutCaps_HPP_

#ifdef INITGUIDS
#include <initguid.h>
#endif

//Use Guidgen.exe to generate new unique GUIDs.
//{F1506F60-896F-11d0-9445-008029E64916}
DEFINE_GUID(IID_ITvOutCaps, 0xF1506F60, 0x896F, 0x11d0, 0x94, 0x45, 0x00, 0x80, 0x29, 0xE6, 0x49, 0x16);

#undef INTERFACE
#define INTERFACE ITvOutCaps

DECLARE_INTERFACE_(ITvOutCaps, IUnknown)
{
 	//IUnknown members;
 	STDMETHOD(QueryInterface) (THIS_ REFIID, LPVOID*) PURE;
 	STDMETHOD_(ULONG, AddRef) (THIS) PURE;
 	STDMETHOD_(ULONG, Release) (THIS) PURE;

	// Returns the number of ATI display devices in the systems (typically 1)
	STDMETHOD( GetNumCards )		(THIS_ LPDWORD lpdwNumCards)PURE;

	// Returns the current version # of the driver in standard microsoft format
	STDMETHOD( GetDriverVersion )	(THIS_ LPDWORD lpdwDriverVersion)PURE;

	// Returns the current version # of ImpacTV chip on the specified card
	// returns 0 for no TV Out installed, 1 for ImpacTV I, and 2 for ImpacTV II
	STDMETHOD( GetTVOutVersion )	(THIS_ WORD wCard, LPDWORD lpdwTVOutVersion)PURE;

	// Returns a bitfield of TV standards supported on a specific card including
	// 1 for NTSC, 2 for PAL, all other bits reserved
	STDMETHOD( GetTVOutStandards )	(THIS_ WORD wCard, LPDWORD lpdwStandards)PURE;

	// Returns BOOL TRUE if the specified card supports macrovision
	STDMETHOD( IsMacrovisionSupported )(THIS_ WORD wCard, LPBOOL lpbHasMacrovision)PURE;

	// Used to query for supported TV Out resolutions on a specific card.
	// Pass indexes incrementing from 0. Each time <lpIsGoodMode> retunrs
	// TRUE, <lprcMode> will contain a new rectangle for a supported screen
	// size. When <lpIsGoodMode> returns FALSE, no additional sizes are supported.
	STDMETHOD( GetSupportedMode )	(THIS_ WORD wCard, WORD wIndex, LPRECT lprcMode, LPBOOL lpbIsGoodMode)PURE;

 	//ITvOutCaps members
};  typedef ITvOutCaps *LPITvOutCaps;

#endif //_ITvOutCaps_HPP_
