 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/***************************** Object Manager *****************************\
*                                                                           *
* Module: CONTROL.C                                                         *
*     Control methods are implemented in this module.                       *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <vblank.h>
#include <fifo.h>
#include <gr.h>
#include <os.h>
#include <nv32.h>
#include "control.h"
#include "nvhw.h"

//
// FIFO database.
//

//---------------------------------------------------------------------------
//
//  Control methods.
//
//---------------------------------------------------------------------------


CTRLMETHOD NvControlMethods[] =
{
    {mthdSetObject,                 0x0000, 0x0003}
#ifdef OBSOLETE_FUNCTIONS
    {mthdSetStallNotifier,          0x0050, 0x0053},
    {mthdStallChannel,              0x0054, 0x0057}
#endif // OBSOLETE_FUNCTIONS
};

// Class representing information for channel control operations
CTRLCLASS NvControlClass =
{
    NvControlMethods,
    sizeof(NvControlMethods) / sizeof(CTRLMETHOD)
};


// Implementation of the control "Set Object" method
RM_STATUS mthdSetObject
(
    PHWINFO     pDev, 
    U032        ChID,
    U032        SubChannel,
    PCTRLMETHOD Method, 
    U032        Offset, 
    V032        Data
)
{
    U032    Context;
    POBJECT Object;
#ifdef KJK
    U032    CacheChID;
#endif

    if (Data == NV1_NULL_OBJECT)
    {
        //
        // Set to the NULL object.
        //
        pDev->DBfifoTable[ChID].SubchannelContext[SubChannel] = NULL;
    }
    else if (fifoSearchObject(pDev, Data, ChID, &Object) == RM_OK)
    {
    
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: SetObject on subchannel: ", SubChannel);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "                      Object#: ", Object->Name);
    
        //
        // Set this as the current object for this ChID/SubChannel.
        //
        pDev->DBfifoTable[ChID].SubchannelContext[SubChannel] = Object;

        //
        // Set the current subchannel for this object, per this SetObject
        //
        Object->Subchannel = SubChannel;
        
        //
        // Update the fifo context for this subchannel back to a software object
        //
        Context = REG_RD32(NV_PFIFO_CACHE1_ENGINE);
        Context &= ~(0x03 << (SubChannel*4));
        REG_WR32(NV_PFIFO_CACHE1_ENGINE, Context);
    }
    else
    {
        //
        // Object not found.
        //
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: SetObject on invalid object: ", Data);
        DBG_BREAKPOINT();
        pDev->DBfifoTable[ChID].SubchannelContext[SubChannel] = NULL;
    }
    
#ifdef KJK    
    //
    // Do the hash function.  Update the appropriate channel context.
    //
    fifoHashEntry(pDev->DBfifoTable[ChID].ObjectStack[SubChannel][0], ChID, &Context);
    CacheChID = REG_RD_DRF(_PFIFO, _CACHE1_PUSH1, _CHID);
    if (CacheChID == ChID)
    {
        REG_WR32(NV_PFIFO_CACHE1_CTX(SubChannel), Context);
        REG_WR32(NV_PFIFO_CACHE1_PULL1, DRF_DEF(_PFIFO, _CACHE1_PULL1, _CTX,       _DIRTY));
    }
    else
    {
        U032  *CtxtPtr = (U032 *)pDev->Pram.FifoContextAddr;
        MEM_WR32((U032)(CtxtPtr + (ChID * 8) + SubChannel), Context);
    }
#endif // KJK
    
    return(RM_OK);
}

#ifdef OBSOLETE_FUNCTIONS
// Implementation of the control "Set Stall Notifier" method
RM_STATUS mthdSetStallNotifier
(
    PHWINFO     pDev, 
    U032        ChID,
    U032        SubChannel,
    PCTRLMETHOD Method, 
    U032        Offset, 
    V032        Data
)
{
    RM_STATUS status;

    if (!(REG_RD32(NV_PFIFO_MODE) & (1 << ChID))) 
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: SetStallNotifier not valid for PIO channel: ", ChID);
        DBG_BREAKPOINT();
        pDev->DBfifoTable[ChID].StallNotifier = NULL;
        return RM_ERR_ILLEGAL_ACTION;
    }

    status = dmaValidateObjectName(pDev, Data,
                                   ChID,
                                   &(pDev->DBfifoTable[ChID].StallNotifier));
    if (status) {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Invalid notifier for SetStallNotifier: ", Data);
        DBG_BREAKPOINT();
        pDev->DBfifoTable[ChID].StallNotifier = NULL;
        return RM_ERR_BAD_OBJECT;
    }

    return RM_OK;
}


// Implementation of the control "Stall Channel" method
RM_STATUS mthdStallChannel
(
    PHWINFO     pDev, 
    U032        ChID,
    U032        SubChannel,
    PCTRLMETHOD Method, 
    U032        Offset, 
    V032        Data
)
{
    RM_STATUS status;
    U032      GetPtr;

    if (!(REG_RD32(NV_PFIFO_MODE) & (1 << ChID))) 
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: StallChannel not valid for PIO channel: ", ChID);
        DBG_BREAKPOINT();
        pDev->DBfifoTable[ChID].StallNotifier = NULL;
        return RM_ERR_ILLEGAL_ACTION;
    }

    // Remember the current position of the PutPtr.
    pDev->DBfifoTable[ChID].Put = REG_RD32(NV_PFIFO_CACHE1_PUT);

    // Make the Put the same as Get. The getptr will be incremented to skip 
    // over the stall channel method. Account for that when setting the putptr.
    GetPtr     = REG_RD32(NV_PFIFO_CACHE1_GET) >> 2;
    GetPtr++;
    REG_WR_DRF_NUM(_PFIFO, _CACHE1_PUT, _ADDRESS, GetPtr);

    // Mark this channel as stalled.
    pDev->DBfifoTable[ChID].IsStalledPendingVblank = TRUE;

    // Write the stall notifier.
    if (pDev->DBfifoTable[ChID].StallNotifier) 
    {
        status = notifyFillNotifierArray(pDev, pDev->DBfifoTable[ChID].StallNotifier, 
                                         0 /* info16 */, 0 /* info32 */, 
                                         Data,
                                         0 /* index */);
    }

    return RM_OK;
}

RM_STATUS mthdEnableStalledChannels
(
    PHWINFO   pDev,
    RM_STATUS status
)
{
    U032 i, currentCh, pushPending;
    BOOL isDma;
    // Check all DMA channels other than the current channel that are in use 
    // (The stalled channel cannot be the current one.)
    currentCh = REG_RD_DRF(_PFIFO, _CACHE1_PUSH1, _CHID); 
    for (i = 0; i < NUM_FIFOS; i++)
    {
        isDma = REG_RD32(NV_PFIFO_MODE) & (1 << i);
        if (pDev->DBfifoTable[i].InUse && isDma && i != currentCh && 
            pDev->DBfifoTable[i].IsStalledPendingVblank)
        {
            // This channel was stalled pending a vblank.
            // Update the put in instance memory and mark this channel as push pending.
            REG_WR32((0x714000 + i * 32), pDev->DBfifoTable[i].Put);
            pushPending = REG_RD32(NV_PFIFO_DMA);
            pushPending |= (1 << i);
            REG_WR32(NV_PFIFO_DMA, pushPending);
            // Write the stall notifier.
            if (pDev->DBfifoTable[i].StallNotifier) 
            {
                notifyFillNotifierArray(pDev, pDev->DBfifoTable[i].StallNotifier, 
                                        0 /* info16 */, 0 /* info32 */, 
                                        status,
                                        0 /* index */);
            }

            pDev->DBfifoTable[i].IsStalledPendingVblank = FALSE;
        }
    }

    return RM_OK;
}
#endif // OBSOLETE_FUNCTIONS
