//******************************Module*Header***********************************
// 
// Module Name: NV3PAL.C
// 
// Copyright (c) 1992-1996 Microsoft Corporation
// 
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"

#ifdef NV3
#include "oldnv332.h"
#define NV32_INCLUDED // Prevents re-inclusion of real nv32 header
#include "driver.h"

#include "nvsubch.h"
#include "nvalpha.h"

//******************************************************************************
// Extern declarations
//******************************************************************************

//******************************************************************************
//
//  Function:   NV3SetPalette
//
//  Routine Description:
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//  Return Value:
//
//      None.
//
//******************************************************************************


VOID NV3SetPalette(
PDEV *ppdev,
ULONG start, 
ULONG length)

    {
    Nv3ChannelPio *nv = (Nv3ChannelPio *) ppdev->pjMmBase;
    USHORT FreeCount = ppdev->NVFreeCount;

    //**************************************************************************
    // Setup the palette via PIO
    //**************************************************************************

    while (FreeCount < 4*4)
        FreeCount = NvGetFreeCount(nv, 0);
    FreeCount -= 4*4;

    nv->subchannel[0].SetObject = DD_COLORMAP;
    nv->subchannel[0].nvVideoColorMap.ColormapDirtyStart = start;
    nv->subchannel[0].nvVideoColorMap.ColormapDirtyLength = length;
    nv->subchannel[0].SetObject = DD_IMAGE_FROM_CPU;

    //**************************************************************************
    // Update the cached free count
    //**************************************************************************

    ppdev->NVFreeCount = FreeCount;
    }

#endif // NV3
