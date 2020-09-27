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

#ifndef _TVmmed_HPP_
#define _TVmmed_HPP_

#ifdef INITGUIDS
#include <initguid.h>
#endif

//Use Guidgen.exe to generate new unique GUIDs.
//{F1506F62-896F-11d0-9445-008029E64916}
DEFINE_GUID(IID_ITVOutMultiMedia, 0xF1506F62, 0x896F, 0x11d0, 0x94, 0x45, 0x00, 0x80, 0x29, 0xE6, 0x49, 0x16);

#undef INTERFACE
#define INTERFACE ITVOutMultiMedia

DECLARE_INTERFACE_(ITVOutMultiMedia, IUnknown)
{
 	//IUnknown members;
 	STDMETHOD(QueryInterface) (THIS_ REFIID, LPVOID*) PURE;
 	STDMETHOD_(ULONG, AddRef) (THIS) PURE;
 	STDMETHOD_(ULONG, Release) (THIS) PURE;


	// Returns TRUE in <lpbIsTrue> if multimedia (digital passthrough)
	// mode is currently enabled, FALSE otherwise.
	STDMETHOD( IsMultimediaEnabled)		(THIS_ WORD wCard, LPBOOL lpbIsTrue) PURE;

	// Returns TRUE if the analog cross-bar passthough (BPX feature) is
	// currently enabled, FALSE otherwise. This is not supported on any
	// current hardware
	STDMETHOD( IsAnalogPassThruEnabled)	(THIS_ WORD wCard, LPBOOL lpbIsTrue) PURE;

	// Call to enable or disable multimedia (digital passthough) mode.
	STDMETHOD( SetMultimediaMode)		(THIS_ WORD wCard, BOOL	bOn) PURE;

	// Call to enable or disable analog passthough mode. Does nothing
	// on cards which do not support analog passthrough.
	STDMETHOD( SetAnalogPassThruMode)	(THIS_ WORD wCard, BOOL	bOn) PURE;

	//ITVOutMultiMedia members
};  typedef ITVOutMultiMedia *LPITVOutMultiMedia;

#endif //_TVmmed_HPP_
