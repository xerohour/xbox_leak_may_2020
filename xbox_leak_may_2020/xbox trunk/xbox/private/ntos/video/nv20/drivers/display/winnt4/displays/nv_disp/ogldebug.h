#ifndef _OGLDEBUG_H
#define _OGLDEBUG_H
//******************************Module*Header***********************************
// Module Name: ogldebug.h
//
// special debug code for ogl server part of display driver
//
// Copyright (c) 2000, NVIDIA, Corporation.  All rights reserved.
//
// THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
// NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
// IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
//
//******************************************************************************

#if DBG
//******************************************************************************
#ifndef _DEBUG_H
#error ogldebug.h needs debug.h!
#endif

// DrvClipChanged-WOC-Flags stuff
char *szDbg_WOC_flGet(ULONG fl);
#define DBG_PRINT_WOC_FL(lvl,str,pfn,fl) vDbg_PrintDBG_NAMEINDEXfl(lvl,str,szDbg_WOC_flGet,fl)

//
//******************************************************************************
#else  // DBG
//******************************************************************************
// In a free build remove all debug defines
//

#define DBG_PRINT_WOC_FL(lvl,str,pfn,fl)


//******************************************************************************
#endif // DBG
#endif // _OGLDEBUG_H
