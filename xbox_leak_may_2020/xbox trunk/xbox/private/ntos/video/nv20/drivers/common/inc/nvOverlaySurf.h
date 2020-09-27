 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/

#ifndef _NVOVERLAYSURF_H_
#define _NVOVERLAYSURF_H_

#define NVDBG_LEVEL_OVERLAY_INFO 0x80000000

#ifdef DEBUG

#define NV_DBG_SHOW_FOURCC()                                                    \
    char fourCCStr[5];                                                          \
    fourCCStr[0] = (char)pSurf->lpGbl->ddpfSurface.dwFourCC & 0xFF;             \
    fourCCStr[1] = (char)(pSurf->lpGbl->ddpfSurface.dwFourCC >> 8) & 0xFF;      \
    fourCCStr[2] = (char)(pSurf->lpGbl->ddpfSurface.dwFourCC >> 16) & 0xFF;     \
    fourCCStr[3] = (char)(pSurf->lpGbl->ddpfSurface.dwFourCC >> 24) & 0xFF;     \
    fourCCStr[4] = 0;                                                           \
    DPF_LEVEL (NVDBG_LEVEL_OVERLAY_INFO, "FOURCC %s allocation: size=%u bytes", \
               &fourCCStr, dwBlockSize );

#else // NOT DEBUG

#define NV_DBG_SHOW_FOURCC()

#endif // DEBUG

// external functions

extern DWORD VppCreateFourCCSurface(Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pSurf, LPDDRAWI_DIRECTDRAW_GBL pDDGbl, DWORD dwExtraSize, DWORD dwExtraNumSurfaces, DWORD dwBlockHeight);
extern void VppCalcExtraSurfaceSpace(Vpp_t *pVpp, LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl, NvU32 displayHeight, NvU32 dwRequestedSurfaces, NvU32 *dwExtraSize, NvU32 *dwExtraNumSurfaces, NvU32 *dwBlockHeight);
#endif // _DDSURF4CC_H_
