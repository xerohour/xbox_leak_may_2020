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
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),p
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
//  Module: nvprecomp.h
//     headers used frequently by D3D drivers for all architectures.
//     all are included via this file to enable pre-compiling.
//
// **************************************************************************
//
//  History:
//       Craig Duttweiler    (bertrem)   01Mar99     created
//
// **************************************************************************

#ifndef _NVPRECOMP_H
#define _NVPRECOMP_H

#include <math.h>

#include "CompileControl.h"

// include compile-time switches
#include "nvSwitches.h"

// System Stuff
//#include <windows.h>
#include <stdlib.h>
//#include <conio.h>
#ifndef WINNT
    //#include <windows.h>
    //#include <assert.h>
    //#include <wingdi.h>

    #include "ddrawi.h"
    #include "d3dhal.h"
    #include "d3d.h"
    #include "dx95type.h"
    #include "dmemmgr.h"
#else // WINNT
    #define  __NTDDKCOMP__
    //#include <d3dtypes.h>
    //#include <d3d8.h>
    //#include <ddrawint.h>
    #include <devioctl.h>
    //#include <ntddvdeo.h>
#endif

#define D3D_OK S_OK

//#include "ddkmmini.h"
// Shared NV files
#include "nvreg.h"
//#include "nvtypes.h"
//#include "nv32.h"

#ifndef WINNT
    #include "nvwin32.h"
#endif // !WINNT

#include "ddrvmem.h"
#include "nvProcMan.h"
#include "ddmini.h"
#include "nvrmapi.h"
#include "nvrmarch.inc"
#include "nvddobj.h"
#include "nvcm.h"
#include "nvos.h"
#include "nvPriv.h"

#ifdef WINNT
    #include "memstruc.h"
    #include "nvEscDef.h"
    #include "nvntioctl.h"
#endif // WINNT

#include "surfaces.h"
#include "nvstat.h"
#include "escape.h"
#include "nvPM.h"
#include "nvCapture.h"

// Local NV common files
// *** order sensitive ***
#include "d3dConst.h"
#include "nvDMac.h"
#include "nvD3DMac.h"
#include "nvPusher.h"
#include "nvSemaphore.h"
#include "nvFlipper.h"
#include "nvDefVB.h"

#ifdef KPFS
    #include "nvKelvinProgram.h"
#endif

#include "nvContext.h"
#include "nvTranslate.h"
#include "nvSurface.h"
#include "nvLight.h"
#include "nvBlockManager.h"
#include "nvPalette.h"
#include "nvBlit.h"
#include "nvTex.h"
#include "nvTexManage.h"
#include "nvVB.h"
#include "nv4Tables.h"
#include "nvSwiz.h"
#include "nvheap.h"
#include "d3dDebug.h"
#include "nvvxmac.h"
#include "nvtexfmt.h"
#include "nvmocomp.h"
#include "nvEnable.h"
#include "ddFlip.h"
#include "nvClear.h"
#include "ddVideo.h"
#include "DDHal.h"
#include "nvVideoPort.h"
#include "ddSurface.h"
#include "nvLogo.h"
#include "d3dMath.h"
#include "nvBenchmark.h"
#include "nvAccess.h"   //FILE abstraction



#if (NVARCH >= 0x020)
    #include "nvSuperTri.h"
    #ifndef KPFS
    #include "nvKelvinProgram.h"
    #endif
    #include "nvKelvinTables.h"
    #include "nvKelvinState.h"
    #include "nvKelvinStateHelp.h"
    #include "nvKelvinAA.h"
#endif

#if (NVARCH >= 0x010)
    #include "nvCelsiusTables.h"
    #include "nvCelsiusState.h"
    #include "nvCelsiusStateHelp.h"
    #include "nvCelsiusNvTSS.h"
    #include "nvCelsiusAA.h"
#endif // NVARCH >= 0x010

#include "d3dinc.h"
#include "nvVShad.h"
#include "nvPShad.h"
#include "nvdinc.h"
#include "nvproto.h"
#include "global.h"
#include "nvObjectLists.h"
//#include "ddCapture.h"
#include "nvComp.h"
#include "nvDP2Help.h"
#include "nvdp2ops.h"
#include "nvFormats.h"
#include "nvSurf.h"
//#include "nvPal.h"
#include "nvObject.h"
#include "nvRefCount.h"
//#include "nvTimer.h"
#include "nvStereo.h"
//#include "PopupAgent.h"

// DO NOT include x86 in the precompiled headers.  It redefines things that affect some variable
// names and thereby causes hard-to-understand compile problems.
//#include "x86.h"

// preventive logic
#undef GlobalLock
#define GlobalLock      __this_is_an_unsupported_legacy_operation__do_not_use__
#undef GlobalUnlock
#define GlobalUnlock    __this_is_an_unsupported_legacy_operation__do_not_use__

#endif // _NVPRECOMP_H

