//******************************************************************************
//
// Module Name:
//
//     NV1.C
//
// Abstract:
//
//     Implements all NV1 specific routines
//
// Environment:
//
//     kernel mode only
//
// Notes:
//
// Revision History:
//
//     This driver was adapted from the S3 Display driver
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996,1997  NVidia Corporation. All Rights Reserved
//
//******************************************************************************

#include "precomp.h"
#include "driver.h"
#include "nv32.h"

    #include "ddminint.h"
#ifdef NVD3D
    #include "dx95type.h"
    #include "nvntd3d.h"
    #include "ddmini.h"
#endif
#include "nv1c_ref.h"

#include "nvsubch.h"
#include "nvalpha.h"
// Temporary fix for Compile error...ask!
#define NvGetFreeCount(NN, CH)\
                 (NN)->subchannel[CH].control.Free


//******************************Public*Routine**********************************
//
// Function: NV1FifoIsBusy
//
// Routine Description:
//
//             This function has been pulled out of DDBLT so that
//             we can single step thru DDBLT.  The problem is that
//             WinDbg tries to display the values of local variables
//             when tracing through functions.  However, READING from
//             the NV User Channel (*nv) will cause the system to lock
//             and is not allowed.  So to make debugging easier, this
//             code has been separated.  We can put it back in later for
//             better performance.
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

ULONG NV1FifoIsBusy(PDEV *ppdev)

    {
    Nv3ChannelPio           *nv;
    USHORT              FreeCount;

    nv  = (Nv3ChannelPio*) ppdev->pjMmBase;

    //**************************************************************************
    // Get current Free Count.  If the FIFO is absolutely full, and we
    // can't send another command, then return TRUE.
    //**************************************************************************

    FreeCount = NvGetFreeCount(nv, BLIT_SUBCHANNEL);

    if (FreeCount >= NV_GUARANTEED_FIFO_SIZE)
        return(TRUE);
    else
        return(FALSE);

    }


