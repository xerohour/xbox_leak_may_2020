/**********************************************************************
 *<
	FILE: ctrl.h

	DESCRIPTION:

	CREATED BY: Rolf Berteig

	HISTORY: created 13 June 1995

	         added independent scale controller (ScaleXYZ)
			   mjm - 9.15.98

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __CTRL__H
#define __CTRL__H

#include "Max.h"
#include "resource.h"


extern ClassDesc* GetMasterBlockDesc();
extern ClassDesc* GetBlockControlDesc();
extern ClassDesc* GetSlaveFloatDesc();
extern ClassDesc* GetSlavePosDesc();
extern ClassDesc* GetSlavePoint3Desc();
extern ClassDesc* GetSlaveRotationDesc();
extern ClassDesc* GetSlaveScaleDesc();
extern ClassDesc* GetControlContainerDesc();
TCHAR *GetString(int id);
extern HINSTANCE hInstance;

#endif

