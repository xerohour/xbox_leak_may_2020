 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

/****************************** FIFO Manager *******************************\
*                                                                           *
* Module: FIFO.C                                                            *
*   The FIFO is managed in this module.  Allocating and controlling the     *
*   FIFO hardware and servicing FIFO exceptions belong here.                *
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
#include "control.h"
#include "nvhw.h"
#include "smp.h"

#ifdef WIN32
#include "nvwin32.h"   // XXX this shouldn't be needed
#else
#define NV_OS_WRITE_THEN_AWAKEN	   0x00000001
#endif


#ifdef DEBUG
U032 findex = 0;
U032 fifolog[MAX_FIFOLOG_LENGTH];
#endif

//---------------------------------------------------------------------------
//
//  FIFO assignment and deassignment to a client.
//
//---------------------------------------------------------------------------

//
// Allocate a User FIFO and return its channel ID.
//
RM_STATUS fifoAlloc
(
    PHWINFO pDev,
    U032    ClassNum,
    U032   *ChannelID
)
{
    U032       i;
    U032       ChID;
    PCLASS     pClass;
    U032       instSize, instAlign, instance;
    RM_STATUS  status;

    //
    // Make sure this channel class is supported on this chip.
    //
    if (classGetClass(&pDev->classDB, ClassNum, &pClass) == RM_ERROR)
        return RM_ERR_BAD_CLASS;

    //
    // If this class requires additional instance memory, get it now.
    //
    if (pClass->ClassInstMalloc)
    {
        instSize = CLASSINSTMALLOC_FIELD(*pClass->ClassInstMalloc, _SIZE);
        instAlign = CLASSINSTMALLOC_FIELD(*pClass->ClassInstMalloc, _ALIGN);

        if (instAlign)
            status = fbAllocInstMemAlign(pDev, &instance, instSize, instAlign);
        else
            status = fbAllocInstMem(pDev, &instance, instSize);
        if (status)
            return (status);
    }
    else
    {
        instance = 0; instSize = 0;
    }

    //
    // Look for unused FIFO.
    //
    for (ChID = 0; ChID < NUM_FIFOS; ChID++)
    {
        if (!pDev->DBfifoTable[ChID].InUse)
        {
            pDev->DBfifoTable[ChID].ObjectCount             = 0;
            pDev->DBfifoTable[ChID].ObjectTree              = NULL;
            pDev->DBfifoTable[ChID].InUse                   = TRUE;
            pDev->DBfifoTable[ChID].IsStalledPendingVblank  = FALSE;
            pDev->DBfifoTable[ChID].StallNotifier           = NULL;
            pDev->DBfifoTable[ChID].Put                     = 0;
            pDev->DBfifoTable[ChID].LastIllegalMthdObject   = NULL;
            pDev->DBfifoTable[ChID].Instance                = instance;
            pDev->DBfifoTable[ChID].InstanceSize            = instSize;
            for (i=0; i<NUM_SUBCHANNELS; i++)
                pDev->DBfifoTable[ChID].SubchannelContext[i] = NULL;

            break;
        }
    }
    if (ChID == NUM_FIFOS)
    {
        FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: No allocatable FIFO available.\n\r"));
        if (instance) fbFreeInstMem(pDev, instance, instSize);
        return (RM_ERR_NO_FREE_FIFOS);
    }
    FIFO_PRINTF((DBG_LEVEL_INFO, "NVRM: Allocate FIFO #: 0x%x\n", ChID));
    //
    // Install CLASS objects into object tree.
    //
    // KJK This goes away once NvAllocObject() is implemented.
    //
    if (classCreateWellKnownObjects(pDev, ChID) != RM_OK)
    {
        FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: Unable to create well-known names!\n"));
        DBG_BREAKPOINT();
        pDev->DBfifoTable[ChID].InUse = FALSE;
        if (instance) fbFreeInstMem(pDev, instance, instSize);
        return (RM_ERR_CANT_CREATE_CLASS_OBJS);
    }

    osEnterCriticalCode(pDev);
    nvHalFifoAllocPio(pDev, ChID, instance);
    osExitCriticalCode(pDev);

    FIFO_PRINTF((DBG_LEVEL_INFO, "NVRM: fifoAllocate - CACHE1 currently mapped to ChID: 0x%x\n", REG_RD_DRF(_PFIFO, _CACHE1_PUSH1, _CHID)));

    *ChannelID = ChID;
    return (RM_OK);
}
//
// Allocate a User DMA FIFO and return its channel ID.
//
RM_STATUS fifoAllocDma
(
    PHWINFO pDev,
    U032    ClassNum,
    U032   *ChannelID,
    PDMAOBJECT pDmaObject
)
{
    U032       i;
    U032       ChID;
    U032       fetchTrigger, fetchSize, fetchRequests;
    U032       instSize, instAlign, instance;
    PCLASS     pClass;
    RM_STATUS  status;

    //
    // Make sure this channel class is supported on this chip.
    //
    if (classGetClass(&pDev->classDB, ClassNum, &pClass) == RM_ERROR)
        return RM_ERR_BAD_CLASS;

    //
    // If this class requires additional instance memory, get it now.
    //
    if (pClass->ClassInstMalloc)
    {
        instSize = CLASSINSTMALLOC_FIELD(*pClass->ClassInstMalloc, _SIZE);
        instAlign = CLASSINSTMALLOC_FIELD(*pClass->ClassInstMalloc, _ALIGN);

        if (instAlign)
            status = fbAllocInstMemAlign(pDev, &instance, instSize, instAlign);
        else
            status = fbAllocInstMem(pDev, &instance, instSize);
        if (status)
            return (status);
    }
    else
    {
        instance = 0; instSize = 0;
    }

    //
    // Get default fetch parameters.
    //
    if (osSetFifoDmaParams(pDev, &fetchTrigger, &fetchSize, &fetchRequests))
        return RM_ERR_OPERATING_SYSTEM;

    //
    // Look for unused FIFO.
    //
    for (ChID = 0; ChID < NUM_FIFOS; ChID++)
    {
        if (!pDev->DBfifoTable[ChID].InUse)
        {
            pDev->DBfifoTable[ChID].ObjectCount             = 0;
            pDev->DBfifoTable[ChID].ObjectTree              = NULL;
            pDev->DBfifoTable[ChID].InUse                   = TRUE;
            pDev->DBfifoTable[ChID].IsStalledPendingVblank  = FALSE;
            pDev->DBfifoTable[ChID].StallNotifier           = NULL;
            pDev->DBfifoTable[ChID].Put                     = 0;
            pDev->DBfifoTable[ChID].LastIllegalMthdObject   = NULL;
            for (i=0; i<NUM_SUBCHANNELS; i++)
                pDev->DBfifoTable[ChID].SubchannelContext[i] = NULL;
            pDev->DBfifoTable[ChID].Instance                = instance;
            pDev->DBfifoTable[ChID].InstanceSize            = instSize;
            break;
        }
    }
    if (ChID == NUM_FIFOS)
    {
        FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: No allocatable FIFO available.\n\r"));
        if (instance) fbFreeInstMem(pDev, instance, instSize);
        return (RM_ERR_NO_FREE_FIFOS);
    }
    FIFO_PRINTF((DBG_LEVEL_INFO, "NVRM: Allocate FIFO #: 0x%x\n", ChID));
    //
    // Install CLASS objects into object tree.
    //
    // KJK This goes away once NvAllocObject() is implemented.
    //
    if (classCreateWellKnownObjects(pDev, ChID) != RM_OK)
    {
        FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: Unable to create well-known names!\n"));
        DBG_BREAKPOINT();
        pDev->DBfifoTable[ChID].InUse = FALSE;
        if (instance) fbFreeInstMem(pDev, instance, instSize);
        return (RM_ERR_CANT_CREATE_CLASS_OBJS);
    }

    osEnterCriticalCode(pDev);
    status = nvHalFifoAllocDma(pDev, ChID, instance, pDmaObject->HalInfo.Instance, fetchTrigger, fetchSize, fetchRequests);
    osExitCriticalCode(pDev);
    if (status != RM_OK)
    {
        pDev->DBfifoTable[ChID].InUse = FALSE;
        if (instance) fbFreeInstMem(pDev, instance, instSize);
        return (status);
    }

    FIFO_PRINTF((DBG_LEVEL_INFO, "NVRM: fifoAllocateDma - CACHE1 currently mapped to ChID: 0x%x\n", REG_RD_DRF(_PFIFO, _CACHE1_PUSH1, _CHID)));

    *ChannelID = ChID;
    return (RM_OK);
}

//
// Free up a User FIFO to the available pool.
//
RM_STATUS fifoFree
(
    PHWINFO pDev,
    U032    ChID
)
{
    POBJECT           DeadObject;

    FIFO_PRINTF((DBG_LEVEL_INFO, "NVRM: Freeing FIFO on ChID: 0x%x\n", ChID));

    //
    // Reset the channel, so it's in an idle state and clear the hashTable
    // for this ChID.
    //
    fifoResetChannelState(pDev, ChID);

    //
    // Remove objects from object database.
    //
    while (pDev->DBfifoTable[ChID].ObjectTree)
    {
        //
        // Free up all remaining objects.
        //
        DeadObject = (POBJECT)pDev->DBfifoTable[ChID].ObjectTree->Data;
        fifoDelObject(pDev, DeadObject);
        //
        // Class specific delete procedure.
        //
        //DeadObject->Name = NV_ZOMBIE_OBJECT;
        classDestroy((VOID*)pDev, DeadObject);
    }

#ifdef DEBUG
    if (pDev->DBfifoTable[ChID].ObjectCount != 0)
    {
        FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: Object count not zero after deleting object tree!\n\r"));
        DBG_BREAKPOINT();
    }
#endif // DEBUG

    pDev->DBfifoTable[ChID].InUse = FALSE;

    //
    // Release any instance memory.  State in fifo object is
    // cleared when it's reallocated.
    //
    if (pDev->DBfifoTable[ChID].Instance != 0)
    {
        fbFreeInstMem(pDev, pDev->DBfifoTable[ChID].Instance, pDev->DBfifoTable[ChID].InstanceSize);
    }

    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  HASH table management.
//
//---------------------------------------------------------------------------

//
// Add entry into Hash Table.
//
RM_STATUS fifoAddHashEntry
(
    PHWINFO pDev,
    POBJECT Object,
    U032    ChID,
    U032    Instance
)
{
    U032 Hash, Entry, Limit;

    //
    // Query HAL for starting entry for this <handle,chid> pair.
    //
    nvHalFifoHashFunc(pDev, Object->Name, ChID, &Hash);

    //
    // Search table for free slot.
    //
    // We can't exceed the hardware's search limit and we also can't wrap
    // since there's a bug in current chips that prevents the hardware from
    // wrapping on a search.
    //
    Limit = Hash + pDev->Pram.HalInfo.HashSearch;
    if (Limit > pDev->Pram.HalInfo.HashDepth)
        Limit = pDev->Pram.HalInfo.HashDepth;
    for (Entry = Hash; Entry < Limit; Entry++)
    {
        //
        // Find first empty entry.
        //
        if (pDev->DBhashTable[Entry].Object == NULL)
            break;
    }

    if (pDev->DBhashTable[Entry].Object != NULL)
    {
        FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: FIFO Hash table is FULL!!\n\r"));
        return (RM_ERROR);
    }

    //
    // Add object to the Hash Table.
    //
    pDev->DBhashTable[Entry].Object = Object;
    pDev->DBhashTable[Entry].ChID   = ChID;

    nvHalFifoHashAdd(pDev, Entry, Object->Name, ChID, Instance,
                     ENGDECL_FIELD(*Object->Class->EngineDecl, _OBJRAMHT));

    return (RM_OK);
}
//
// Remove entry from Hash Table.
//
RM_STATUS fifoDelHashEntry
(
    PHWINFO pDev,
    POBJECT Object,
    U032    ChID
)
{
    U032              Entry;

    //
    // Skip hashing since we scan the whole table.
    //
    for (Entry = 0; Entry < pDev->Pram.HalInfo.HashDepth; Entry++)
    {
        if (pDev->DBhashTable[Entry].Object == Object && pDev->DBhashTable[Entry].ChID == ChID)
        {
            pDev->DBhashTable[Entry].Object = NULL;
            nvHalFifoHashDelete(pDev, Entry);
            return (RM_OK);
        }
    }
    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  FIFO exception handling.
//
//---------------------------------------------------------------------------

#define BENSWORK

V032 fifoService
(
    PHWINFO pDev
)
{
    RM_STATUS status;
    V032      intr;
    POBJECT   Object;
    FIFOEXCEPTIONDATA FifoExceptionData;
    U032 i;
    static U032 fifoIsrCounter;

#ifdef BENSWORK
    U032      isDebuggerPresent;
    U032      mustNotifyDebugger;

    isDebuggerPresent  = ((PRMINFO)pDev->pRmInfo)->Debugger.object != NULL;
    mustNotifyDebugger = 0;
#endif

    NVRM_TRACE1('FISR');
    // count fifo isrs as part of fifo fixup heuristic below
    fifoIsrCounter++;

    status = RM_OK;
    intr   = REG_RD32(NV_PFIFO_INTR_0);

    //
    // Turn off reassign for the duration.
    //
    FLD_WR_DRF_DEF(_PFIFO, _CACHES, _REASSIGN, _DISABLED);

    //
    // Get exception data from HAL.
    //
    nvHalFifoGetExceptionData(pDev, &FifoExceptionData);

    //
    // Here's where we'd bypass HAL fifo exception handling if needed.
    //

    //
    // Give HAL chance to service selected exceptions.
    //
    nvHalFifoService(pDev, intr, &FifoExceptionData);

    //
    // Refresh interrupt status value in case HAL cleared any exceptions.
    //
    intr = REG_RD32(NV_PFIFO_INTR_0);

    if (intr & DRF_DEF(_PFIFO, _INTR_0, _CACHE_ERROR, _PENDING))
    {
        //
        // Cache error handler.  We got here due to one of two issues.
        //
        // 1) The current object that was just hashed/routed is configured
        //    as a SW_ENGINE object
        // 2) There is no hash entry available for the current object
        //

        //
        // Disable the fifo
        //
        // Don't explicitly disable the puller -- it should already
        // be off by the time we get here.  Verify with an assert().
        //
        REG_WR_DRF_DEF(_PFIFO, _CACHE1_PULL0, _ACCESS,   _DISABLED);

        FIFOLOG(1, 0, 0);

        //
        // Clear condition.
        //
        REG_WR_DRF_DEF(_PFIFO, _INTR_0, _CACHE_ERROR,  _RESET);

        FIFO_PRINTF((DBG_LEVEL_INFO, "NVRM: FIFO Cache miss Channel: 0x%x\n", FifoExceptionData.ChID));
        FIFO_PRINTF((DBG_LEVEL_INFO, "                  sub channel: 0x%x\n", FifoExceptionData.SubChannel));
        FIFO_PRINTF((DBG_LEVEL_INFO, "                       method: 0x%x\n", FifoExceptionData.Method));
        FIFO_PRINTF((DBG_LEVEL_INFO, "                         data: 0x%x\n", FifoExceptionData.Data));
        FIFO_PRINTF((DBG_LEVEL_INFO, "                       reason: 0x%x\n", FifoExceptionData.Reason));

        //
        // Timeout
        //
        i = 0xFFFF;
        while (i && (FifoExceptionData.Reason & DRF_DEF(_PFIFO, _CACHE1_PULL0, _HASH_STATE, _BUSY)))
        {
            i--;
            FifoExceptionData.Reason     = REG_RD32(NV_PFIFO_CACHE1_PULL0);
        }

        if (FifoExceptionData.Reason & DRF_DEF(_PFIFO, _CACHE1_PULL0, _HASH, _FAILED))
        {
            U032 result;
            //
            // On IKOS, we sometimes see spurious hash errors (where looking manually we
            // find the hash entry). If so, clear FifoExceptionData.Reason and allow the HW
            // to retry.
            //
            nvHalFifoHashSearch(pDev, FifoExceptionData.Data, FifoExceptionData.ChID, &result);
            if (result) {
                FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: HW missed valid hash entry: 0x%x\n", FifoExceptionData.Data));
                FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: CACHE1 get ptr: 0x%x\n", FifoExceptionData.GetPtr));
                FifoExceptionData.Reason = 0x0;
            } else {
                //
                // Hash failure.  This had better be a master class.
                //
                FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: FIFO Cache Error due to hash failure\n"));
                FIFO_PRINTF((DBG_LEVEL_ERRORS, "                     channel: 0x%x\n", FifoExceptionData.ChID));
                FIFO_PRINTF((DBG_LEVEL_ERRORS, "                 sub channel: 0x%x\n", FifoExceptionData.SubChannel));
                FIFO_PRINTF((DBG_LEVEL_ERRORS, "                      offset: 0x%x\n", FifoExceptionData.Method));
                FIFO_PRINTF((DBG_LEVEL_ERRORS, "                        data: 0x%x\n", FifoExceptionData.Data));
            }
        }

        //
        // This captures any unexpected strangeness on IKOS. In particular make
        // sure the engine bits for objects in subchannels agree and that we are
        // not just experiencing a transient hash failure.
        //

        if (FifoExceptionData.Reason & DRF_DEF(_PFIFO, _CACHE1_PULL0, _DEVICE, _SOFTWARE))
        {
            U032 pull1, engine1;

            // If we find a disagreement in the register engine bits, clear the reason and retry
            pull1   = REG_RD_DRF(_PFIFO, _CACHE1_PULL1, _ENGINE);
            engine1 = (REG_RD32(NV_PFIFO_CACHE1_ENGINE) >> (FifoExceptionData.SubChannel * 4)) & 0xF;

            if (pull1 ^ engine1)
            {
                FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: CACHE1 PULL1/ENGINE differ in engine bits\n\r"));
                FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: CACHE1 get ptr: 0x%x\n", FifoExceptionData.GetPtr));
                DBG_BREAKPOINT();
                FifoExceptionData.Reason = 0x0;
            }
            else
            {
                // On nv11 (at least) and IKOS sometimes we get a transient hash failure.
                // Until this is understood just retry the item.  As a cheap trick to avoid looping
                // on this for whatever reason, make sure we take a few fifo interrupts between
                // successive invocations of the hack.

                // Have the chip retry if its really a hw class and we see the item in hash table.
                Object = pDev->DBfifoTable[FifoExceptionData.ChID].SubchannelContext[FifoExceptionData.SubChannel];
                if ((FifoExceptionData.Method >= 0x0100) && ! Object)
                {
                    static U032 lastHackedFifoIsrCounter;
                    U032 delta;

                    if (fifoIsrCounter > lastHackedFifoIsrCounter) delta = fifoIsrCounter - lastHackedFifoIsrCounter;
                    else delta = lastHackedFifoIsrCounter - fifoIsrCounter;
                    lastHackedFifoIsrCounter = fifoIsrCounter;

                    if (delta > 10)  // require a few calls to fifoService() between these retries
                    {
                        FIFO_PRINTF((DEBUGLEVEL_ERRORS, "NVRM: Should have been a HW method: 0x%x\n", FifoExceptionData.Method));
                        FIFO_PRINTF((DEBUGLEVEL_ERRORS, "NVRM: CACHE1 get ptr: 0x%x\n", FifoExceptionData.GetPtr));
                        FIFO_PRINTF((DEBUGLEVEL_ERRORS, "             channel: %d\n", FifoExceptionData.ChID));
                        FIFO_PRINTF((DEBUGLEVEL_ERRORS, "         sub channel: %d\n", FifoExceptionData.SubChannel));
                        FIFO_PRINTF((DEBUGLEVEL_ERRORS, "                data: 0x%x\n", FifoExceptionData.Data));
                        DBG_BREAKPOINT();
                        FifoExceptionData.Reason = 0x0;
                    }
                    else
                    {
                        FIFO_PRINTF((DEBUGLEVEL_ERRORS, "NVRM: was not transient hash failure: 0x%x\n", FifoExceptionData.Method));
                        FIFO_PRINTF((DEBUGLEVEL_ERRORS, "NVRM: CACHE1 get ptr: 0x%x\n", FifoExceptionData.GetPtr));
                        FIFO_PRINTF((DEBUGLEVEL_ERRORS, "             channel: %d\n", FifoExceptionData.ChID));
                        FIFO_PRINTF((DEBUGLEVEL_ERRORS, "         sub channel: %d\n", FifoExceptionData.SubChannel));
                        FIFO_PRINTF((DEBUGLEVEL_ERRORS, "                data: 0x%x\n", FifoExceptionData.Data));
                    }
                }
            }
        }

        if ((FifoExceptionData.Reason & DRF_DEF(_PFIFO, _CACHE1_PULL0, _DEVICE, _SOFTWARE))
            || (FifoExceptionData.Reason & DRF_DEF(_PFIFO, _CACHE1_PULL0, _HASH, _FAILED)))
        {
            //
            // Software method
            //
            FIFO_PRINTF((DBG_LEVEL_INFO, "NVRM: FIFO Cache miss due to software object\n\r"));

            if ((FifoExceptionData.ChID < NUM_FIFOS) && pDev->DBfifoTable[FifoExceptionData.ChID].InUse)
            {
                if (FifoExceptionData.Method < 0x0100)
                {
                    status = fifoSoftwareMethod(pDev, &FifoExceptionData);
                }
                else
                {
                    //
                    // First we need to find the object to send this method.
                    //
                    // Do this brute force.  Search the whole fifo tree for the object.
                    //
                    // Go find the minimum value (>0) and start there.  That way we're
                    // sure to visit every node.
                    //
                    Object = pDev->DBfifoTable[FifoExceptionData.ChID].SubchannelContext[FifoExceptionData.SubChannel];

                    //
                    // If we found a valid object, route the method
                    //
                    if (Object)
                    {
                        status = classSoftwareMethod(pDev, Object, FifoExceptionData.Method, FifoExceptionData.Data);

                        //if (status != RM_OK)
                        //    notifyDefaultError(pDev, Object, Method, Data, ChID, status);
                    }
                    else
                    {
                        FIFO_PRINTF((DEBUGLEVEL_ERRORS, "NVRM: No valid object in subchannel: %d\n", FifoExceptionData.SubChannel));
                        FIFO_PRINTF((DEBUGLEVEL_ERRORS, "NVRM:                       channel: %d\n", FifoExceptionData.ChID));
                        FIFO_PRINTF((DEBUGLEVEL_ERRORS, "                             offset: 0x%x\n", FifoExceptionData.Method));
                        FIFO_PRINTF((DEBUGLEVEL_ERRORS, "                               data: 0x%x\n", FifoExceptionData.Data));
                        DBG_BREAKPOINT();
                    }
                }

            }
            else
            {
                FIFO_PRINTF((DBG_LEVEL_INFO, "NVRM: Access to free channel: 0x%x\n", FifoExceptionData.ChID));
                FIFO_PRINTF((DBG_LEVEL_INFO, "                 sub channel: 0x%x\n", FifoExceptionData.SubChannel));
                FIFO_PRINTF((DBG_LEVEL_INFO, "                      offset: 0x%x\n", FifoExceptionData.Method));
                FIFO_PRINTF((DBG_LEVEL_INFO, "                        data: 0x%x\n", FifoExceptionData.Data));
            }

            //
            // Advance the FIFO get pointer.
            //
            FifoExceptionData.GetPtr++;
            REG_WR_DRF_NUM(_PFIFO, _CACHE1_GET, _ADDRESS, FifoExceptionData.GetPtr);    // no more gray coding
        }

        //
        // Clear the fifo's hash result
        //
        REG_WR32(NV_PFIFO_CACHE1_HASH, 0);

        //
        // Re-enable cache draining.
        //
        FIFOLOG(1, 1, 1);
        REG_WR_DRF_DEF(_PFIFO_, CACHE1_PULL0, _ACCESS,   _ENABLED);
    }

    if (intr & DRF_DEF(_PFIFO, _INTR_0, _RUNOUT_OVERFLOW, _PENDING))
    {
        //
        // Clear condition.
        //
        REG_WR_DRF_DEF(_PFIFO, _INTR_0, _RUNOUT_OVERFLOW, _RESET);
        //
        // RunOut overflow.  Bad news.
        //
        // KJK: Do we wish to attempt to recover from this error?  Maybe close this channel
        // and attempt to recover to other valid channels?
        //
        FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: ERROR = FIFO runout overflow! Bad News!!!\n\r 0x%x\n", intr));
        REG_WR_DRF_DEF(_PFIFO_, CACHE1_PULL0, _ACCESS, _DISABLED);
        FIFOLOG(1, 0, 0);
        status = RM_ERR_FIFO_RUNOUT_OVERFLOW;
    }

    if (intr & DRF_DEF(_PFIFO, _INTR_0, _DMA_PUSHER, _PENDING))
    {
        // create a write cycle to the FB (used as an analyzer trigger).
        fbAddr->Reg032[0] = 0xDEADCAFE;

        FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM: DMA Pusher encountered a parse error.\n\r"));
        FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM:     Push Error: 0x%x\n", REG_RD32(NV_PFIFO_CACHE1_DMA_STATE)));
        FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM:        Get Ptr: 0x%x\n", REG_RD32(NV_PFIFO_CACHE1_DMA_GET)));
        if (IsNV5orBetter(pDev)) {
            FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM:  Dwords Xfered: 0x%x\n", REG_RD32(NV_PFIFO_CACHE1_DMA_DCOUNT)));
            FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM:    Last jmp at: 0x%x\n", REG_RD32(NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW)));
            FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM:       Last cmd: 0x%x\n", REG_RD32(NV_PFIFO_CACHE1_DMA_RSVD_SHADOW)));
            FIFO_PRINTF((DBG_LEVEL_ERRORS, "NVRM:      Last data: 0x%x\n", REG_RD32(NV_PFIFO_CACHE1_DMA_DATA_SHADOW)));
        }
#ifdef BENSWORK // case 4
        // external debugger registered?
        if (isDebuggerPresent)
        {
            NvdeAppendEventForDebugger ((PNVDEOBJECT)(PNVDEOBJECT)((PRMINFO)pDev->pRmInfo)->Debugger.object,4,4,0,0,0,0,0,0);
            mustNotifyDebugger = 1;
        }
        else
        {
            DBG_BREAKPOINT();
        }
#else
        DBG_BREAKPOINT();
#endif

        //
        // Clear condition.
        //
        REG_WR_DRF_DEF(_PFIFO, _INTR_0, _DMA_PUSHER, _RESET);
        REG_WR32(NV_PFIFO_CACHE1_DMA_STATE, 0);

        //
        // The dma pusher has faulted while processing the push buffer stream.  This is
        // probably due to an invalid jump or bad opcode.
        //
        // Move the get pointer forward if necessary to skip this bad data
        //
        if (REG_RD32(NV_PFIFO_CACHE1_DMA_PUT) != REG_RD32(NV_PFIFO_CACHE1_DMA_GET))
        {
            U032 GetPtr;

            GetPtr = REG_RD32(NV_PFIFO_CACHE1_DMA_GET);
            REG_WR32(NV_PFIFO_CACHE1_DMA_GET, GetPtr+4);
            //
            // This somehow needs to account for rollover, but without parsing the data, I don't
            // know where to roll to.
            //
        }
    }

    if (intr & DRF_DEF(_PFIFO, _INTR_0, _DMA_PT, _PENDING))
    {
#ifdef TRAP_HOST_FIFO_SWITCHES
        //
        // We're now expect this intr as a way to know what
        // the host FIFO is doing.
        //
        extern U032 dma_inst[32];
        static U032 prev_chid;
        U032 FifoCtxtPtr, fc_data;

        FIFO_PRINTF((DBG_LEVEL_ERRORS,      "Prev ChID: 0x%x\n", FifoExceptionData.ChID));

        // First, clear out prev_chid FIFO context inst addr, so he faults again
        FifoCtxtPtr = pDev->halHwInfo.pPramHalInfo->FifoContextAddr + (prev_chid * 32);
        fc_data = REG_RD32((U032)(FifoCtxtPtr + SF_OFFSET(NV_RAMFC_DMA_INST)));
        fc_data &= 0xFFFF0000;    // mask off the inst addr
        REG_WR32((U032)(FifoCtxtPtr + SF_OFFSET(NV_RAMFC_DMA_INST)), fc_data);

        FIFO_PRINTF((DBG_LEVEL_ERRORS,      "New  ChID: 0x%x\n", FifoExceptionData.ChID));

        // load into HW the inst addr
        REG_WR32(NV_PFIFO_CACHE1_DMA_INSTANCE, dma_inst[FifoExceptionData.ChID]);

        // Force a reread of the DMA pusher address
        REG_WR32(NV_PFIFO_CACHE1_DMA_CTL, 0);
        prev_chid = FifoExceptionData.ChID;
#endif

        //
        // Bad news if we get this -- the fifo engine must have attempted to fetch an invalid
        // dma page.  We could attempt to reconstruct what was meant to happen, or we can
        // just barf for now.  I vote for barfing...
        //
        REG_WR_DRF_DEF(_PFIFO, _INTR_0, _DMA_PT, _RESET);
        FIFO_PRINTF((DEBUGLEVEL_USERERRORS, "NVRM: Invalid DMA pusher PTE fetch.\n\r"));
#ifdef BENSWORK // case 5
        // external debugger registered?
        if (isDebuggerPresent)
        {
            NvdeAppendEventForDebugger ((PNVDEOBJECT)((PRMINFO)pDev->pRmInfo)->Debugger.object,5,4,0,0,0,0,0,0);
            mustNotifyDebugger = 1;
        }
        else
        {
            DBG_BREAKPOINT();
        }
#else
        DBG_BREAKPOINT();
#endif
    }

    //
    // If the dma pusher is currently suspended, wait until the CACHE1 is empty
    // and then set it running again.
    //
    if (REG_RD_DRF(_PFIFO, _CACHE1_DMA_PUSH, _STATUS) == NV_PFIFO_CACHE1_DMA_PUSH_STATUS_SUSPENDED)
    {
        while (REG_RD_DRF(_PFIFO, _CACHE1_STATUS, _LOW_MARK) != NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY)
        {
            // If there's a pending FIFO intr, make this check after servicing it
            if (REG_RD32(NV_PFIFO_INTR_0))
                break;

            // Check if GE needs servicing
            if (REG_RD32(NV_PGRAPH_INTR))
                grService(pDev);

            // Check if vblank needs servicing (for NV15 HW flip)
            if (REG_RD32(NV_PMC_INTR_0) & (DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING) | DRF_DEF(_PMC, _INTR_0, _PCRTC2, _PENDING)))
                VBlank(pDev);
        }
        if (REG_RD_DRF(_PFIFO, _CACHE1_STATUS, _LOW_MARK) == NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY)
        {
            while (REG_RD_DRF(_PFIFO, _CACHES, _DMA_SUSPEND) == NV_PFIFO_CACHES_DMA_SUSPEND_BUSY)
			    ;

            while (REG_RD_DRF(_PFIFO, _CACHE1_DMA_PUSH, _STATE) == NV_PFIFO_CACHE1_DMA_PUSH_STATE_BUSY)
                ;

            FLD_WR_DRF_DEF(_PFIFO, _CACHE1_DMA_PUSH, _STATUS, _RUNNING);
        }
    }

#if 0
    //
    // Re-enable the dma pusher if this was a pusher channel
    //
    if (REG_RD32(NV_PFIFO_CACHE1_PUSH1) & DRF_DEF(_PFIFO, _CACHE1_PUSH1, _MODE, _DMA))
    {
        //
        // Wait til idle
        //
        while (REG_RD_DRF(_PFIFO, _CACHE1_DMA_PUSH, _STATE) == NV_PFIFO_CACHE1_DMA_PUSH_STATE_BUSY);

        //
        // Turn it back on (in the process will also clear DMA_PUSH_STATUS_SUSPENDED)
        //
        REG_WR_DRF_DEF(_PFIFO, _CACHE1_DMA_PUSH, _ACCESS, _ENABLED);
    }
#endif

    //
    // Re-enable FIFO CACHE1 if all conditions cleared.
    //
    if (REG_RD32(NV_PFIFO_INTR_0) == 0x00000000)
    {
#ifdef BENSWORK
        if (mustNotifyDebugger)
        {
            osNotifyEvent(pDev, ((PRMINFO)pDev->pRmInfo)->Debugger.object, 0, 0, 0, RM_OK, NV_OS_WRITE_THEN_AWAKEN);
        }
        else
        {
#endif
        FIFOLOG(1, 1, 1);
        REG_WR_DRF_DEF(_PFIFO_, CACHE1_PULL0, _ACCESS, _ENABLED);
        FLD_WR_DRF_DEF(_PFIFO, _CACHES, _REASSIGN, _ENABLED);
#ifdef BENSWORK
        }
#endif
    }

    intr = REG_RD32(NV_PFIFO_INTR_0);
    //
    // See if there is yet another FIFO exception to be handled.
    // Just need to make sure that "intr" is non-zero in order to
    // get more servicing done.
    //
    intr |= REG_RD_DRF(_PFIFO, _DEBUG_0, _CACHE_ERROR0);
    NVRM_TRACE2('fisr', intr);
    return intr;
}

//
// Emulate the device access in software.
//
RM_STATUS fifoSoftwareMethod
(
    PHWINFO pDev,
    PFIFOEXCEPTIONDATA pFifoExceptionData
)
{
    U032 i;

    // Dispatch the requested software method
    for (i = 0; i < NvControlClass.MethodMax; i++)
        {
            if ((pFifoExceptionData->Method >= NvControlClass.Method[i].Low)
            &&  (pFifoExceptionData->Method <  NvControlClass.Method[i].High))
            {

                return(NvControlClass.Method[i].Proc(pDev,
                                                     pFifoExceptionData->ChID,
                                                     pFifoExceptionData->SubChannel,
                                                     &(NvControlClass.Method[i]),
                                                     pFifoExceptionData->Method,
                                                     pFifoExceptionData->Data));
            }
        }
    //
    // Method not found
    //
    FIFO_PRINTF((DEBUGLEVEL_USERERRORS, "NVRM: invalid control access "));
    FIFO_PRINTF((DEBUGLEVEL_USERERRORS, "              offset: 0x%x\n", pFifoExceptionData->Method));
    FIFO_PRINTF((DEBUGLEVEL_USERERRORS, "                data: 0x%x\n", pFifoExceptionData->Data));
    return (RM_ERR_FIFO_BAD_ACCESS);
}
