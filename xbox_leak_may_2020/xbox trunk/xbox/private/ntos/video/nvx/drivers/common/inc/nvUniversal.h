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
#ifndef _NVUNIVERSAL_H_
#define _NVUNIVERSAL_H_

/*==========================================================================;
 *
 *  File:       NvUniversal.h
 *
 *  Content:    Defines, constants, macros, and miscellaneous constructs that
 *              are commonly needed across all operating system platforms.
 *              
 *              If the code is specific to Windows, or Mac, or Linux it doesn't
 *              go here.
 *              
 ***************************************************************************/

#ifndef NVARCH
#error NVARCH must be defined. Definition of NV3, NV4, NV10, etc. is obsolete.
ERROR! NVARCH must be defined. Definition of NV3, NV4, NV10, etc. is obsolete.
#endif

// TBD: have everyone use this from here... -mlavoie
#if 0
#if (NVARCH >= 0x04)
#define NV_IN_PROGRESS          0x8000
#else
#define NV_IN_PROGRESS          0xFF
#endif
#endif

// TBD: combine this with PERFDATA from D3D -mlavoie
// TBD: move device version (and other appropos stuff) into this
typedef struct NV_SystemInfo_s
{
    // 32 byte bank
    NvU32 dwCPUFeatureSet;
    NvU32 dwProcessorSpeed;         // In MHz
    NvU32 dwSystemMemory;           // MB of system memory, rounded to the nearest 8
    NvU32 dwNVClasses;
    NvU32 dwNVClasses1;
    NvU32 dwPad_0[3];
} NV_SystemInfo_t;

// nvD3DPerfData.dwCPUFeatureSet
#define CPU_FS_MMX              0x00000001      // ) do not change - some code
#define CPU_FS_KATMAI           0x00000002      //  )          relies on the macros to match these numbers
#define CPU_FS_3DNOW            0x00000004      // )
#define CPU_FS_HALFFASTWRITE    0x00000008      // camino with agp4x and 1/2 nv fast writes
#define CPU_FS_FULLFASTWRITE    0x00000010      // camino with agp4x and 1/1 nv fast writes
#define CPU_FS_WILLAMETTE       0x00000020      //  Pentium 4 support
#define CPU_FS_ATHLON           0x00000040      // AMD Athlon
#define CPU_FS_PREFETCH         0x00000080      // supports prefetchNTA, prefetchT0, movntq
#define CPU_FS_SFENCE           0x00001000      // has sfence support
#define CPU_FS_64BIT            0x00008000      // 64-bit back end (VANTA)

/* Shared Celsius class usage flags */
#define CELSIUSUSER_NONE                0  // celsius object is untouched and (almost) fully uninitialized
#define CELSIUSUSER_D3D                 1
#define CELSIUSUSER_DDRAW               2
#define CELSIUSUSER_DDRAW_VPP           3

/* Shared Kelvin class usage flags */
#define KELVINUSER_NONE                 0  // kelvin object is untouched and (almost) fully uninitialized
#define KELVINUSER_D3D                  1
#define KELVINUSER_DDRAW                2
#define KELVINUSER_DDRAW_VPP            3

// NV_SYSTEM_INFO.dwNVClasses
// ordered from "worst" to "best" within each class
#define NVCLASS_0055_DX6TRI     0x00000001
#define NVCLASS_0095_DX6TRI     0x00000002

#define NVCLASS_0054_DX5TRI     0x00000004
#define NVCLASS_0094_DX5TRI     0x00000008

#define NVCLASS_0056_CELSIUS    0x00000010
#define NVCLASS_0096_CELSIUS    0x00000020
#define NVCLASS_1196_CELSIUS    0x00000040

#define NVCLASS_0097_KELVIN     0x00000080

#define NVCLASS_0046_DAC        0x00000100
#define NVCLASS_0049_DAC        0x00000200
#define NVCLASS_0067_DAC        0x00000400
#define NVCLASS_007C_DAC        0x00000800

#define NVCLASS_0042_CTXSURF2D  0x00001000
#define NVCLASS_0062_CTXSURF2D  0x00002000

#define NVCLASS_0060_IDXIMAGE   0x00004000
#define NVCLASS_0064_IDXIMAGE   0x00008000

#define NVCLASS_0077_SCALEDIMG  0x00010000
#define NVCLASS_0063_SCALEDIMG  0x00020000
#define NVCLASS_0089_SCALEDIMG  0x00040000

#define NVCLASS_0038_DVDPICT    0x00100000
#define NVCLASS_0088_DVDPICT    0x00200000

#define NVCLASS_007A_OVERLAY    0x00400000

#define NVCLASS_0053_CTXSURF3D  0x01000000
#define NVCLASS_0093_CTXSURF3D  0x02000000

#define NVCLASS_004A_GDIRECT    0x04000000
#define NVCLASS_005E_SOLIDRECT  0x08000000

#define NVCLASS_0052_CTXSURFSWZ 0x10000000
#define NVCLASS_009E_CTXSURFSWZ 0x20000000

// class 0 collectives
#define NVCLASS_FAMILY_DXTRI    (NVCLASS_0094_DX5TRI  | NVCLASS_0054_DX5TRI  | NVCLASS_0095_DX6TRI | NVCLASS_0055_DX6TRI)
#define NVCLASS_FAMILY_CELSIUS  (NVCLASS_0056_CELSIUS | NVCLASS_0096_CELSIUS | NVCLASS_1196_CELSIUS)
#define NVCLASS_FAMILY_KELVIN   (NVCLASS_0097_KELVIN)

// NV_SYSTEM_INFO.dwNVClasses1
// ordered from "worst" to "best" within each class
#define NVCLASS1_006C_CHANNELDMA 0x00000001
#define NVCLASS1_006E_CHANNELDMA 0x00000002
#define NVCLASS1_206E_CHANNELDMA 0x00000004

// class 1 collectives
// none yet

#define SUB_CHANNEL(I) (I * 0x00002000) // Computes subchannel offset given a simple ndx 0-7

// Convert Microsoft Specific modifier (not available on all MS compilers either)
#if defined(__forceinline)
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE inline
#endif

#endif // _NVUNIVERSAL_H_
       
