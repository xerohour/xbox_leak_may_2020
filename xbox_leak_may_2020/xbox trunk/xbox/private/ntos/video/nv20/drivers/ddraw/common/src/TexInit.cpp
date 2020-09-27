/***************************************************************************\
* Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.            *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
* Module: TexInit.cpp                                                       *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           11/16/98                                         *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

extern vpSurfaces*          pMySurfaces;
extern NvNotification*      nvMyVPNotifiers;


void initTextureSurf(void) {
    U032    error;

    // We're going to stuff things into our channel which are NV4 only
    // since the channel is an NV3 channel we have to type cast it to NV4
    // KevinK says this works fine...
    Nv04ChannelPio* nvPtr = (Nv04ChannelPio*)pMySurfaces->pVPChanPtr;

    // allocate the objects and the notifiers
    //   the contexts we have to do on the fly in ring0


    // allocate the notifier for the swiz class
    error = NvRmAllocContextDma(  (GLOBDATAPTR)->ROOTHANDLE,
        MY_SWIZ_NOTIFIER,
        NV01_CONTEXT_DMA,
        DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
        DRF_DEF(OS03, _FLAGS, _COHERENCY, _UNCACHED),
        (PVOID)(((U032)nvMyVPNotifiers) + NUM077OFFSET*sizeof(NvNotification)),
        (sizeof( NvNotification)*NUM077NOTIFIER  - 1)
   );
    checkNvAllocArchError(error);

    // allocate the notifer for the 2dswized surface class
    error = NvRmAllocContextDma(  (GLOBDATAPTR)->ROOTHANDLE,
        MY_2DSWIZD_NOTIFIER,
        NV01_CONTEXT_DMA,
        DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
        DRF_DEF(OS03, _FLAGS, _COHERENCY, _UNCACHED),
        (PVOID)(((U032)nvMyVPNotifiers) + NUM052OFFSET*sizeof(NvNotification)),
        (sizeof( NvNotification)*NUM052NOTIFIER  - 1)
   );
    checkNvAllocArchError(error);

   // allocate the swizzler object
    error = NvRmAllocObject(  (GLOBDATAPTR)->ROOTHANDLE,
                                MY_VPE_CHANNEL,
                                MY_SWIZ_OBJECT,
                                        NV04_CONTEXT_SURFACE_SWIZZLED
                                );

    checkNvAllocArchError(error);
    // allocate the swizzled surface
    error = NvRmAllocObject(  (GLOBDATAPTR)->ROOTHANDLE,
                                MY_VPE_CHANNEL,
                                MY_2DSWIZD_OBJECT,
                                        NV4_STRETCHED_IMAGE_FROM_CPU
                                );
    checkNvAllocArchError(error);

    //setup subchannels
    // subchannel 3-swiz 4-conv
    nvPtr->subchannel[3].SetObject  = MY_SWIZ_OBJECT;
    nvPtr->subchannel[4].SetObject = MY_2DSWIZD_OBJECT;

    // plug the notifiers in
    nvPtr->subchannel[3].nv04ScaledImageFromMemory.SetContextDmaNotifies = MY_SWIZ_NOTIFIER;
    nvPtr->subchannel[4].nv04ContextSurfaceSwizzled.SetContextDmaNotifies = MY_2DSWIZD_NOTIFIER;
    //
    // entire frame buffer context stolen from VPE stuff
    nvPtr->subchannel[3].nv04ScaledImageFromMemory.SetContextDmaImage = MY_IMAGE0_BUFFER_CONTEXT;
    // I think this is the correct color format...
    nvPtr->subchannel[3].nv04ScaledImageFromMemory.SetContextSurface = MY_2DSWIZD_OBJECT;
    // entire frame buffer context stolen from VPE stuff

}


void DestroyTextureSurf(void) {
    U032    error;

    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, (GLOBDATAPTR)->ROOTHANDLE, MY_SWIZ_NOTIFIER);
    checkNvAllocArchError(error);
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, (GLOBDATAPTR)->ROOTHANDLE, MY_2DSWIZD_NOTIFIER);
    checkNvAllocArchError(error);
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, MY_VPE_CHANNEL, MY_SWIZ_OBJECT);
    checkNvAllocArchError(error);
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, MY_VPE_CHANNEL, MY_2DSWIZD_OBJECT);
    checkNvAllocArchError(error);

}

#endif  // NVARCH >= 0x04
