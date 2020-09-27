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
// ********************************* Direct 3D ******************************
//
//  Module: nvCelsiusStateHelp.h
//      Constants, macros, and prototypes for nvCelsiusStateHelp.cpp
//
// **************************************************************************
//
//  History:
//       Craig Duttweiler        27Sep99         created
//
// **************************************************************************

#ifndef _NVCELSIUSSTATEHELP_H
#define _NVCELSIUSSTATEHELP_H

#if (NVARCH >= 0x010)

// combiner program hash table stuff

#define D3DTA_FLAGMASK           (D3DTA_COMPLEMENT | D3DTA_ALPHAREPLICATE)
#define NVTSSARGMASK(dwArg)      ((dwArg & D3DTA_SELECTMASK) | ((dwArg & D3DTA_FLAGMASK) >> 1))

#define CELSIUSCPTABLE_INDEXMASK 0x7f  // should be <= 0xff
#define CELSIUSCPTABLE_SIZE      (CELSIUSCPTABLE_INDEXMASK+1)

typedef struct __CELSIUSCOMBINERPROGRAM {
    // tags
    DWORD dwTSSMask0;
    DWORD dwTSSMask1;
    // data
    DWORD dwStateFlags;
    DWORD dwColorICW[2];
    DWORD dwColorOCW[2];
    DWORD dwAlphaICW[2];
    DWORD dwAlphaOCW[2];
    DWORD dwTexUnitMapping[2];
    DWORD dwNumActiveStages;
    // link to resolve hash collisions
    struct __CELSIUSCOMBINERPROGRAM *pNext;
} CELSIUSCOMBINERPROGRAM, *PCELSIUSCOMBINERPROGRAM;

//---------------------------------------------------------------------------

// prototypes

void  explut (float n, float *l, float *m);
DWORD nvCelsiusBuildTSSMask                   (PNVD3DCONTEXT pContext, DWORD dwStage);
BOOL  nvCelsiusCombinersSetFromHashTable      (PNVD3DCONTEXT pContext, DWORD dwTSSMask0, DWORD dwTSSMask1);
void  nvCelsiusCombinersAddProgramToHashTable (PCELSIUSCOMBINERPROGRAM pProgram);
BOOL  nvCelsiusCombinersAddToHashTable        (PNVD3DCONTEXT pContext, DWORD dwTSSMask0, DWORD dwTSSMask1);
DWORD nvCelsiusValidateTextureStageState      (LPD3DHAL_VALIDATETEXTURESTAGESTATEDATA);

#endif  // NVARCH >= 0x010

#endif  // _NVCELSIUSSTATEHELP_H

