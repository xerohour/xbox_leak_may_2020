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


extern ClassDesc* GetPathCtrlDesc();
extern ClassDesc* GetEulerCtrlDesc();
extern ClassDesc* GetLocalEulerCtrlDesc();
extern ClassDesc* GetExprPosCtrlDesc();
extern ClassDesc* GetExprP3CtrlDesc();
extern ClassDesc* GetExprFloatCtrlDesc();
extern ClassDesc* GetExprScaleCtrlDesc();
extern ClassDesc* GetExprRotCtrlDesc();
extern ClassDesc* GetFloatNoiseDesc();
extern ClassDesc* GetPositionNoiseDesc();
extern ClassDesc* GetPoint3NoiseDesc();
extern ClassDesc* GetRotationNoiseDesc();
extern ClassDesc* GetScaleNoiseDesc();
extern ClassDesc* GetBoolControlDesc();
extern ClassDesc* GetIPosCtrlDesc();
extern ClassDesc* GetAttachControlDesc();
extern ClassDesc* GetIPoint3CtrlDesc();
extern ClassDesc* GetIColorCtrlDesc();
extern ClassDesc* GetLinkCtrlDesc();
extern ClassDesc* GetFollowUtilDesc();
extern ClassDesc* GetSurfCtrlDesc();
extern ClassDesc* GetLODControlDesc();
extern ClassDesc* GetLODUtilDesc();
extern ClassDesc* GetIScaleCtrlDesc(); // mjm 9.15.98

TCHAR *GetString(int id);
extern HINSTANCE hInstance;

#endif

