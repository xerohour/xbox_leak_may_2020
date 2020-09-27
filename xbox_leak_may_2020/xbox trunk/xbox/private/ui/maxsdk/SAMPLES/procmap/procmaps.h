/**********************************************************************
 *<
	FILE: procmaps.h

	DESCRIPTION:

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __PROCMAPS__H
#define __PROCMAPS__H

#include "max.h"
#include "imtl.h"
#include "stdmat.h"
#include "texutil.h"

extern HINSTANCE hInstance;

extern ClassDesc* GetPlanetDesc();
extern ClassDesc* GetStuccoDesc();
extern ClassDesc* GetSplatDesc();
extern ClassDesc* GetWaterDesc();
extern ClassDesc* GetSpeckleDesc();
extern ClassDesc* GetSmokeDesc();

extern TCHAR *GetString(int id);

#define PLANET_CLASS_ID 0x46396cf1
#define WATER_CLASS_ID 0x7712634e
#define SMOKE_CLASS_ID 0xa845e7c
#define SPECKLE_CLASS_ID 0x62c32b8a
#define SPLAT_CLASS_ID 0x90b04f9
#define STUCCO_CLASS_ID 0x9312fbe

#endif
