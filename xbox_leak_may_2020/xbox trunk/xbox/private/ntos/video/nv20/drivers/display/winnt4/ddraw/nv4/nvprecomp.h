 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2001 NVIDIA, Corporation.  All rights reserved.      *|
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
#include <math.h>   // For sin/cos
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <windef.h>
#include <winerror.h>
#include <wingdi.h>
#include <winbase.h>
extern "C" {
#include <winddi.h>
};
#include <ntddvdeo.h>

#include "nvTypes.h"
#include "CompileControl.h"

#include "memstruc.h"

// These constants were taken from nvDD.h.  They should come from a shared directory. @mjl@
#define NV_PRESCALE_OVERLAY_X           1
#define NV_PRESCALE_OVERLAY_Y           2
#define NV_PRESCALE_OVERLAY_FIELD       4
#define NV_ALPHA_1_032          ((U032)0xFF000000)
#if (NVARCH >= 0x04)
#define NV_ALPHA_1_016          ((U032)0xFFFF0000)
#else
#define NV_ALPHA_1_016          ((U032)0x00008000)
#endif
#define NV_ALPHA_1_008          ((U032)0x0000FF00)
#define NV_MAX_Y_CLIP                   0x00000FFF
#define NV_MAX_X_CLIP                   0x00000FFF
#define NV_PFIFO_DMA                                     0x00002508 // @mjl@ make sure we map these objects!
#define NV_PFIFO_CACHE1_PUSH1                            0x00003204
#define NV_PFIFO_CACHE1_PUSH1_MODE_DMA_BIT               0x00000100
#define NV_PFIFO_CACHE1_DMA_PUSH                         0x00003220
#define NV_PFIFO_CACHE1_DMA_PUSH_BUFFER_EMPTY_BIT        0x00000100

#include "nvPusher.h"

#include "nv32.h"

// Note: ddminint.h defines GLOBALDATA (and much else) but shared code (anything including this file)
//   will get GLOBALDATA from a different place (win9x/inc/DDMINI.H)
//
#include "ddminint.h"
#include "driver.h" // This is an NT4 specific header despite the name... - @mjl@

#include "nvDbg.h"
#include "nvUtil.h"
#include "nv4subch.h"

#define AllocIPM(size) EngAllocMem(FL_ZERO_MEMORY,size,ALLOC_TAG)
#define FreeIPM(ptr) EngFreeMem(ptr)

#define VIDMEM_ADDR(fpVidMem)    ((fpVidMem) + (ULONG) PDEV_PTR()->pjFrameBufbase)
#define AGPMEM_ADDR(fpVidMem)    ((ULONG) fpVidMem + (ULONG) PDEV_PTR()->pDriverData->GARTLinearBase)
#define VIDMEM_OFFSET(fpVidMem)  (fpVidMem)

// These came from dx95types.h which is not in the dx path being used for NT4
typedef PDD_SURFACE_LOCAL LPDDRAWI_DDRAWSURFACE_LCL;
typedef PDD_SURFACE_GLOBAL LPDDRAWI_DDRAWSURFACE_GBL;
typedef PDD_SURFACE_GLOBAL LPDDRAWI_DDRAWSURFACE_GBL;
typedef PDD_SURFACE_GLOBAL LPDDRAWI_DDRAWSURFACE_GBL;
typedef PDD_DIRECTDRAW_GLOBAL LPDDRAWI_DIRECTDRAW_GBL;

// From win9x/inc32/winnt & winbase headers
// Needed for nvVPP and nvPriv
#define STATUS_WAIT_0       ((DWORD   )0x00000000L)    
#define WAIT_OBJECT_0       ((STATUS_WAIT_0 ) + 0 )

#include "Nvcm.h" // Configuration Manager

#define NvRmConfigGetEx(hCli, hDevice, index, pParams, paramSize) \
        NvConfigGetEx(PDEV_PTR()->hDriver, hCli, hDevice, index, pParams, paramSize)
#define NvRmConfigGet(hClient, hDevice, index, pValue) \
        NvConfigGet(PDEV_PTR()->hDriver, hClient, hDevice, index, pValue)
#define NvRmAllocObject(hCli, hChan, hObj, hCls) \
        NvAllocObject(PDEV_PTR()->hDriver, hCli, hChan, hObj, hCls)
#define NvRmAlloc(hCli, hChan, hObj, hCls, parms) \
        NvAlloc(PDEV_PTR()->hDriver, hCli, hChan, hObj, hCls, parms)
#define NvRmFree(hClient, hParent, hObj) \
        NvFree(PDEV_PTR()->hDriver, hClient, hParent, hObj)
#define NvRmAllocContextDma(hClient, hDma, hClass, flags, base, limit) \
        NvAllocContextDma(PDEV_PTR()->hDriver, hClient, hDma, hClass, flags, base, limit)
#define NvRmAllocChannelDma(hClient, hDev, hChan, hClass, hErrorCtx, hDataCtx, offset, ppChan) \
        NvAllocChannelDma(PDEV_PTR()->hDriver, (hClient), hDev, (hChan), (hClass), (hErrorCtx), (hDataCtx), (offset), (ppChan))

#include "nvapi.h" // For NvConfigGetEx (and possibly other stuff)

#include "winioctl.h"
#include "nvntioctl.h"

#include "nvos.h"
#include "nvReg.h"

// This comes from DDPRIV and is needed by VPP -> resolve later -@mjl@
#define NVS2VF_SP_SUBRECT       0x00000002

#define DXVA_OVERLAY_WAS_BOBBED     0x00000010 // Yet another thing to move from ddmini.h -@mjl@

#define NV_TEXTURE_OFFSET_ALIGN            255

#define AGP_HEAP 1
#define ERR_DXALLOC_FAILED 1
FLATPTR DDHAL32_VidMemAlloc (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, DWORD dwXDim, DWORD dwYDim);
void DDHAL32_VidMemFree (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, FLATPTR ptr);


#define NVDBG_LEVEL_ERROR               0x80000000 // Temp -- until dbg flags are placed properly @mjl@
#define NVDBG_LEVEL_INFO                0x40000000

