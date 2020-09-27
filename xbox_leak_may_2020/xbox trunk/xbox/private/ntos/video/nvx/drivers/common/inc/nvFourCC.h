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
//  Content:    FOURCC codes used in the nVidia drivers
//
//  Note that this file is meant to be used across ALL operating systems.
//  Do not create any dependency on non-nVidia code.
//
// **************************************************************************
#ifndef _NVFOURCC_H_
#define _NVFOURCC_H_

#include "nvTypes.h"

/* MMIO macros */
#ifndef mmioFOURCC
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                                \
                ( (NvU32)(NvU8)(ch0) | ( (NvU32)(NvU8)(ch1) << 8 ) |    \
                ( (NvU32)(NvU8)(ch2) << 16 ) | ( (NvU32)(NvU8)(ch3) << 24 ) )
#endif // mmioFOURCC

/* FOURCC video formats we support */
#ifndef FOURCC_UYVY
#define FOURCC_UYVY                        mmioFOURCC('U','Y','V','Y')
#endif
#ifndef FOURCC_YUY2
#define FOURCC_YUY2                        mmioFOURCC('Y','U','Y','2')
#endif
#ifndef FOURCC_UYNV
#define FOURCC_UYNV                        mmioFOURCC('U','Y','N','V')
#endif
#ifndef FOURCC_YUNV
#define FOURCC_YUNV                        mmioFOURCC('Y','U','N','V')
#endif
#ifndef FOURCC_YV12
#define FOURCC_YV12                        mmioFOURCC('Y','V','1','2')
#endif
#ifndef FOURCC_NVDS
#define FOURCC_NVDS                        mmioFOURCC('N','V','D','S')
#endif
#ifndef FOURCC_NVMC
#define FOURCC_NVMC                        mmioFOURCC('N','V','M','C')
#endif
#ifndef FOURCC_NV12
#define FOURCC_NV12                        mmioFOURCC('N','V','1','2')
#endif
#ifndef FOURCC_NVID
#define FOURCC_NVID                        mmioFOURCC('N','V','I','D')
#endif
#ifndef FOURCC_NVSP
#define FOURCC_NVSP                        mmioFOURCC('N','V','S','P')
#endif
#ifndef FOURCC_420i
#define FOURCC_420i                        mmioFOURCC('4','2','0','i')
#endif
#ifndef FOURCC_IF09
#define FOURCC_IF09                        mmioFOURCC('I','F','0','9')
#endif
#ifndef FOURCC_YVU9
#define FOURCC_YVU9                        mmioFOURCC('Y','V','U','9')
#endif
#ifndef FOURCC_IV32
#define FOURCC_IV32                        mmioFOURCC('I','V','3','2')
#endif
#ifndef FOURCC_IV31
#define FOURCC_IV31                        mmioFOURCC('I','V','3','1')
#endif
#ifndef FOURCC_RAW8
#define FOURCC_RAW8                        mmioFOURCC('R','A','W','8')
#endif
#ifndef FOURCC_RGB0
#define FOURCC_RGB0                        0 // ?? @mjl@
#endif
#ifndef FOURCC_UBAD
#define FOURCC_UBAD                        mmioFOURCC('U','B','A','D')
#endif

/* FOURCC formats we support for 3d textures */
#ifndef FOURCC_NVT0
#define FOURCC_NVT0                        mmioFOURCC('N','V','T','0')
#endif
#ifndef FOURCC_NVT1
#define FOURCC_NVT1                        mmioFOURCC('N','V','T','1')
#endif
#ifndef FOURCC_NVT2
#define FOURCC_NVT2                        mmioFOURCC('N','V','T','2')
#endif
#ifndef FOURCC_NVT3
#define FOURCC_NVT3                        mmioFOURCC('N','V','T','3')
#endif
#ifndef FOURCC_NVT4
#define FOURCC_NVT4                        mmioFOURCC('N','V','T','4')
#endif
#ifndef FOURCC_NVT5
#define FOURCC_NVT5                        mmioFOURCC('N','V','T','5')
#endif
#ifndef FOURCC_NVT6
#define FOURCC_NVT6                        mmioFOURCC('N','V','T','6')
#endif
#ifndef FOURCC_NVT7
#define FOURCC_NVT7                        mmioFOURCC('N','V','T','7')
#endif
#ifndef FOURCC_NVT8
#define FOURCC_NVT8                        mmioFOURCC('N','V','T','8')
#endif
#ifndef FOURCC_NVT9
#define FOURCC_NVT9                        mmioFOURCC('N','V','T','9')
#endif
#ifndef FOURCC_NVS0
#define FOURCC_NVS0                        mmioFOURCC('N','V','S','0')
#endif
#ifndef FOURCC_NVS1
#define FOURCC_NVS1                        mmioFOURCC('N','V','S','1')
#endif
#ifndef FOURCC_NVS2
#define FOURCC_NVS2                        mmioFOURCC('N','V','S','2')
#endif
#ifndef FOURCC_NVS3
#define FOURCC_NVS3                        mmioFOURCC('N','V','S','3')
#endif
#ifndef FOURCC_NVS4
#define FOURCC_NVS4                        mmioFOURCC('N','V','S','4')
#endif
#ifndef FOURCC_NVS5
#define FOURCC_NVS5                        mmioFOURCC('N','V','S','5')
#endif
#ifndef FOURCC_NVS6
#define FOURCC_NVS6                        mmioFOURCC('N','V','S','6')
#endif
#ifndef FOURCC_NVS7
#define FOURCC_NVS7                        mmioFOURCC('N','V','S','7')
#endif
#ifndef FOURCC_NVS8
#define FOURCC_NVS8                        mmioFOURCC('N','V','S','8')
#endif
#ifndef FOURCC_NVS9
#define FOURCC_NVS9                        mmioFOURCC('N','V','S','9')
#endif
#ifndef FOURCC_NVHU
#define FOURCC_NVHU                        mmioFOURCC('N','V','H','U')
#endif
#ifndef FOURCC_NVHS
#define FOURCC_NVHS                        mmioFOURCC('N','V','H','S')
#endif
#ifndef FOURCC_DXT1
#define FOURCC_DXT1                        mmioFOURCC('D','X','T','1') // Compressed Texture
#endif
#ifndef FOURCC_DXT2
#define FOURCC_DXT2                        mmioFOURCC('D','X','T','2')
#endif
#ifndef FOURCC_DXT3
#define FOURCC_DXT3                        mmioFOURCC('D','X','T','3')
#endif
#ifndef FOURCC_DXT4
#define FOURCC_DXT4                        mmioFOURCC('D','X','T','4')
#endif
#ifndef FOURCC_DXT5
#define FOURCC_DXT5                        mmioFOURCC('D','X','T','5')
#endif


// Note!! The number below is UNRELATED to the number of fourcc's defined above.  
// This is the number that will be USED by the driver. This should be redone! @mjl@

/* total number of fourcc formats we support */
#define NV_MAX_FOURCC_REGULAR (10 + 12)     // 10 ddraw/video, 12 texture

#ifdef DXT_SUPPORT
#define NV_MAX_FOURCC_COMPRESSED 5          // 5 compressed texture
#else
#define NV_MAX_FOURCC_COMPRESSED 0
#endif

#define NV_MAX_FOURCC                       (NV_MAX_FOURCC_REGULAR+NV_MAX_FOURCC_COMPRESSED)

#endif // _NVFOURCC_H_