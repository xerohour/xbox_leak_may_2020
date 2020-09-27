
// FILE:			library\hardware\osd\generic\osddefs.h
// AUTHOR:		D.Heidrich
// COPYRIGHT:	(c) 1999 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		24.08.99
//
// PURPOSE:		General definitions for OSD.
//
// HISTORY:

#ifndef OSDDEFS_H
#define OSDDEFS_H

#include "library\common\prelude.h"
#include "library\common\hooks.h"
#include "library\hardware\video\generic\vidtypes.h"



struct OSDStandardChangeMsg {
	VideoStandard newStandard;
	};

MKHOOK(OSDStandardChange, OSDStandardChangeMsg);

typedef OSDStandardChangeHook *OSDStandardChangeHookPtr;



#endif
