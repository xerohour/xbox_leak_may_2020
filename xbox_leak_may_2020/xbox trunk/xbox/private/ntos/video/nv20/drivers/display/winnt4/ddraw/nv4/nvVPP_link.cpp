// **************************************************************************
//
//       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// **************************************************************************
//
//  Module: nvVPP_link.cpp
//      a pointer to a shared file
//
// **************************************************************************

// It would be best to move this head to a common header directory
// since it is in fact shared.  All we need is NV_ESC_SETGET_TWINVIEW_DATA.
#include "..\win9x\inc\escape.h"


#define PDEV_PTR() vpp.pDriverData->ppdev

#undef VPP_USE_EVENT_NOTIFIERS    // use event notifiers if this is defined, else use polling
// On NT4 the Ke*Event() routines are not available to the display driver.

#include "..\..\..\..\common\src\nvVPP.cpp"

// This function is only called to get AGP memory -- we don't have AGP on NT4, so we always fail
FLATPTR DDHAL32_VidMemAlloc (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, DWORD dwXDim, DWORD dwYDim)
{
    nvAssert(heap == AGP_HEAP);
    return ERR_DXALLOC_FAILED;
}

void DDHAL32_VidMemFree (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, FLATPTR ptr)
{
    nvAssert(heap == AGP_HEAP);
    nvAssert(FALSE); // Never called -- since we always fail AGP alloc
}
