// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
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
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
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
//  Module: nvPalette.cpp
//   HW Palette routines.
//
//  History:
//      Daniel Rohrer   9/20/99
//
/********************************************************************/

#include "nvprecomp.h"
#pragma hdrstop

// IDirectDraw interface function callback through HAL
// Microsoft says they won't ever give us this.. callback despite
// the fact that it is exposed the ddrawi.h
DWORD __stdcall CreatePalette32(LPDDHAL_CREATEPALETTEDATA lpCreatePaletteData)
{
    dbgTracePush("CreatePalette32");
    assert(0);
    dbgTracePop();

    return (DDHAL_DRIVER_HANDLED);
}

// IDirectDrawPalette interface function callback through HAL
// set's the palette entries for the current palette
// Microsoft says they won't ever give us this.. callback despite
// the fact that it is exposed the ddrawi.h
DWORD __stdcall SetEntries32(LPDDHAL_SETENTRIESDATA lpDDSetEntriesData)
{
    dbgTracePush("SetEntries32");
    assert(0);
    dbgTracePop();

    return (DDHAL_DRIVER_HANDLED);
}

// IDirectDrawSurface interface function callback through HAL
// binds a palette object to the surface in question --
// Microsoft says they won't ever give us this.. callback despite
// the fact that it is exposed the ddrawi.h
DWORD __stdcall SetPalette32(LPDDHAL_SETPALETTEDATA lpDDSetPaletteData)
{
    dbgTracePush("SetPalette32");
    assert(0);
    dbgTracePop();

    return (DDHAL_DRIVER_HANDLED);
}

// This function is defined in the callback struct unlike the GetEntries but isn't
// described in the DX7 Help files, I'm assuming this call isn't for the general user
// but is a notification from Microsoft that we should clean up after ourselves in
// any data structures we might have lying around.
// Microsoft says they won't ever give us this.. callback despite
// the fact that it is exposed the ddrawi.h
DWORD __stdcall DestroyPalette32(LPDDHAL_DESTROYPALETTEDATA lpDDDestroyPaletteData)
{
    dbgTracePush("DestroyPalette32");
    assert(0);
    dbgTracePop();

    return (DDHAL_DRIVER_HANDLED);
}

