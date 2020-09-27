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
* Module: FIFOOBJ.C                                                         *
*   The FIFO objects are managed in this module.  All modifications to the  *
*   object tree are handled here.  The object tree is a balanced tree       *
*   implementation.                                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <btree.h>
#include <class.h>
#include <fifo.h>
#include <os.h>
#include "nvhw.h"

//
// Fifo object routines.
//

RM_STATUS fifoAddObject
(
	PHWINFO pDev,
    POBJECT Object
)
{
    RM_STATUS status;
    PNODE     node;

    DBG_VAL_PTR(Object);
    status = btreeSearch(Object->Name, &node, pDev->DBfifoTable[Object->ChID].ObjectTree);
    if (status == RM_OK)
        return (RM_ERR_INSERT_DUPLICATE_NAME);
    Object->Node.Value = Object->Name;
    Object->Node.Data  = Object;
    status = btreeInsert(&(Object->Node), &(pDev->DBfifoTable[Object->ChID].ObjectTree));
    if (status == RM_OK)
    {
        //
        // Balance tree after every 16th object addition.
        //
        if ((++pDev->DBfifoTable[Object->ChID].ObjectCount & 0x0F) == 0x00)
            btreeBalance(&(pDev->DBfifoTable[Object->ChID].ObjectTree));
    }
    return (status);    
}

// jsw...
// 
// cwj: we don't add the DMA context object to the fifoTable
// in NV4 (not called from RmBindDmaContext).
//
//RM_STATUS fifoAddDmaObject
//(
//	PHWINFO pDev,
//    POBJECT Object,
//	U032 ChID
//)
//{
//    RM_STATUS status;
//    PNODE     node;
//
//    DBG_VAL_PTR(Object);
//    //status = btreeSearch(Object->Name, &node, pDev->DBfifoTable[Object->ChID].ObjectTree);
//    status = btreeSearch(Object->Name, &node, pDev->DBfifoTable[ChID].ObjectTree);
//    if (status == RM_OK)
//        return (RM_ERR_INSERT_DUPLICATE_NAME);
//    Object->Node.Value = Object->Name;
//    Object->Node.Data  = Object;
//    //status = btreeInsert(&(Object->Node), &(pDev->DBfifoTable[Object->ChID].ObjectTree));
//    status = btreeInsert(&(Object->Node), &(pDev->DBfifoTable[ChID].ObjectTree));
//    if (status == RM_OK)
//    {
//        //
//        // Balance tree after every 16th object addition.
//        //
//        //if ((++pDev->DBfifoTable[Object->ChID].ObjectCount & 0x0F) == 0x00)
//        if ((++pDev->DBfifoTable[ChID].ObjectCount & 0x0F) == 0x00)
//            //btreeBalance(&(pDev->DBfifoTable[Object->ChID].ObjectTree));
//            btreeBalance(&(pDev->DBfifoTable[ChID].ObjectTree));
//    }
//    return (status);    
//}

RM_STATUS fifoDelObject
(
	PHWINFO pDev,
    POBJECT Object
)
{
    RM_STATUS status;

    DBG_VAL_PTR(Object);
    status = btreeDelete(Object->Node.Value, &(pDev->DBfifoTable[Object->ChID].ObjectTree));
    if (status == RM_OK)
        pDev->DBfifoTable[Object->ChID].ObjectCount--;

    if (pDev->DBfifoTable[Object->ChID].LastIllegalMthdObject == Object)
    {
        pDev->DBfifoTable[Object->ChID].LastIllegalMthdObject = NULL;
    }
    return (status);    
}

// jsw...
//
// cwj: wasn't added above, so no need to delete it.
//
//RM_STATUS fifoDelDmaObject
//(
//	PHWINFO pDev,
//    POBJECT Object,
//	U032 ChID
//)
//{
//    RM_STATUS status;
//
//    DBG_VAL_PTR(Object);
//    //status = btreeDelete(Object->Node.Value, &(pDev->DBfifoTable[Object->ChID].ObjectTree));
//    status = btreeDelete(Object->Node.Value, &(pDev->DBfifoTable[ChID].ObjectTree));
//    if (status == RM_OK)
//        //pDev->DBfifoTable[Object->ChID].ObjectCount--;
//        pDev->DBfifoTable[ChID].ObjectCount--;
//    return (status);    
//}

RM_STATUS fifoSearchObject
(
	PHWINFO  pDev,
    U032     Name,
    U032     ChID,
    POBJECT *Object
)
{
    RM_STATUS status;
    PNODE     node;
    
    status = btreeSearch(Name, &node, pDev->DBfifoTable[ChID].ObjectTree);
    if (status == RM_OK)
    {
        *Object = node->Data;
        DBG_VAL_PTR(*Object);
    }
    return (status);    
}

//
// Update an objects' context
//
RM_STATUS fifoUpdateObjectInstance
(
	PHWINFO     pDev,
    POBJECT     Object,
    U032        ChID,
    U032        Instance
)
{
    RM_STATUS status;

    FIFO_PRINTF((DBG_LEVEL_INFO, "NVRM: Updating FIFO context on object: 0x%x\n", Object->Name));
    FIFO_PRINTF((DBG_LEVEL_INFO, "                                 ChID: 0x%x\n", ChID));
    FIFO_PRINTF((DBG_LEVEL_INFO, "                             instance: 0x%x\n", Instance));

    osEnterCriticalCode(pDev);
    //
    // Remove the object from the hash table.
    //
    fifoDelHashEntry(pDev, Object, ChID);
    //
    // Add it back.
    //
    status = fifoAddHashEntry(pDev, Object, ChID, Instance);
    osExitCriticalCode(pDev);
    return (status);
}

RM_STATUS fifoDeleteObjectInstance
(
	PHWINFO pDev,
	POBJECT Object,
	U032    ChID    
)
{
    FIFO_PRINTF((DBG_LEVEL_INFO, "NVRM: Deleting FIFO context on object: 0x%x\n", Object->Name));
    FIFO_PRINTF((DBG_LEVEL_INFO, "                                 ChID: 0x%x\n", ChID));

    osEnterCriticalCode(pDev);
    //
    // Remove the object from the hash table.
    //
    fifoDelHashEntry(pDev, Object, ChID);
    osExitCriticalCode(pDev);
    return (RM_OK);
}

//
// Reset the channel, so it's in an idle state and clear the hash.
// This is called as part of fifoFree.
//
RM_STATUS fifoResetChannelState
(
    PHWINFO pDev,
    U032    ChID
)
{
    U032 Entry;

    //
    // Reset the channel, so it's in an idle state.
    //
    osEnterCriticalCode(pDev);
    nvHalFifoFree(pDev, ChID, pDev->DBfifoTable[ChID].Instance);

    //
    // Clear the hash table for this ChID.
    //
    for (Entry = 0; Entry < pDev->Pram.HalInfo.HashDepth; Entry++)
    {
        if (pDev->DBhashTable[Entry].Object && pDev->DBhashTable[Entry].ChID == ChID)
        {
            // clear software entry
            pDev->DBhashTable[Entry].Object = NULL;
            // clear hardware entry
            nvHalFifoHashDelete(pDev, Entry);
        }
    }
    osExitCriticalCode(pDev);

    return (RM_OK);
}

//
// Clear out the SubchannelContext
//
RM_STATUS fifoDeleteSubchannelContextEntry
(
    PHWINFO pDev,
    POBJECT Object
)
{
    U032 i;
    U032 ChID = Object->ChID;

    for (i = 0; i < NUM_SUBCHANNELS; i++)
    {
        if (pDev->DBfifoTable[ChID].SubchannelContext[i] == Object)
        {
            pDev->DBfifoTable[ChID].SubchannelContext[i] = NULL;
        }
    }

    return (RM_OK);
}
