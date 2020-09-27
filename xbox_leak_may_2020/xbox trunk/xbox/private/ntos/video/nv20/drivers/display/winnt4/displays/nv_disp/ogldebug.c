//******************************Module*Header***********************************
// Module Name: ogldebug.c
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

#include "precomp.h"
#include "driver.h"

#if DBG
#include "ogldebug.h"

//*************************************************************************
// DrvClipChanged-WOC-Flags stuff:
//                                                        
// szDbg_WOC_flGet
//   return string exactly matching fl
// aDbg_WOC_fl                                             
//   array defining all flags                             
//*************************************************************************
DBG_NAMEINDEX aDbg_WOC_fl[] =
{
    DBG_MAKE_NAMEINDEX(WOC_RGN_CLIENT_DELTA ),
    DBG_MAKE_NAMEINDEX(WOC_RGN_CLIENT       ),
    DBG_MAKE_NAMEINDEX(WOC_RGN_SURFACE_DELTA),
    DBG_MAKE_NAMEINDEX(WOC_RGN_SURFACE      ),
    DBG_MAKE_NAMEINDEX(WOC_CHANGED          ),
    DBG_MAKE_NAMEINDEX(WOC_DELETE           ),
    #if (_WIN32_WINNT >= 0x0500)
    DBG_MAKE_NAMEINDEX(WOC_DRAWN            ),
    DBG_MAKE_NAMEINDEX(WOC_SPRITE_OVERLAP   ),
    DBG_MAKE_NAMEINDEX(WOC_SPRITE_NO_OVERLAP),
    #endif
};

char *szDbg_WOC_flGet(ULONG fl)
{
    return (DBG_GETDBG_NAMEINDEX(aDbg_WOC_fl, fl));
}

#endif // DBG
// End of ogldebug.c
