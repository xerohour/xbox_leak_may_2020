/**********************************************************************
 *<
	FILE: pack1.h

	DESCRIPTION:

	CREATED BY: Rolf Berteig

	HISTORY:

 *>	Copyright (c) 1996, All Rights Reserved.
 **********************************************************************/

#ifndef __PACK1__H
#define __PACK1__H

#include "Max.h"
#include "resource.h"

TCHAR *GetString(int id);

extern ClassDesc* GetEditTriObjectDesc();

extern HINSTANCE hInstance;
extern int enabled;

#endif
