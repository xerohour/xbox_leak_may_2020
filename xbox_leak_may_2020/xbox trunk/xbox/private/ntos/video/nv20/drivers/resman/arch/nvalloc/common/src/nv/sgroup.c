 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1999 NVIDIA, Corp.  All rights reserved.        *|
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

/******************************************************************************
*
*   File: sgroup.c
*
*   Description:
*       This file contains the core implementation of the SWAP GROUP and
*   SWAP BARRIER extensions for SGI.
*
******************************************************************************/

#include <nvrm.h>
#include <os.h>
#include <nvos.h>
#include <nvhw.h>

#define SWAP_MAX_CHANNELS    32
#define SWAP_MAX_GROUPS       5
#define SWAP_INVALID_CHANNEL -1
#define SWAP_INVALID_DEVICE  -1

#define SWAP_MASK(chid)                  (1<<(chid))
#define SWAP_MASK_ADD_CHANNEL(sg,chid)    \
                              ((sg)->channel_mask |= SWAP_MASK(chid))
#define SWAP_MASK_REMOVE_CHANNEL(sg,chid) \
    ((sg)->channel_mask &= ~SWAP_MASK(chid));\
    ((sg)->ready_mask  &= ~SWAP_MASK(chid))
#define SWAP_MASK_CHANNEL_READY(sg,chid)  ((sg)->ready_mask |= SWAP_MASK(chid))
#define SWAP_MASK_SWAP_READY(sg)         ((sg)->ready_mask == (sg)->channel_mask)

static VOID RmSwapCallback(VOID *);
RM_STATUS RmSwapAddGroup(PHWINFO, U032, U032, U032, U032);
RM_STATUS RmSwapAddBarrier(PHWINFO, U032, U032);
RM_STATUS RmSwapRemoveGroup(PHWINFO, U032, U032, U032, U032);
RM_STATUS RmSwapRemoveBarrier(PHWINFO, U032);


/*******
 * Helper Functions
 *******/

static U032
GetChID(
    U032 hClient,
    U032 hDevice,
    U032 hChannel
)
{
    PCLI_DEVICE_INFO deviceList;
    U032 i, j;

    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO, "NVRM: looking for chid:"); 
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:       client 0x%x", hClient); 
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:       channel 0x%x", hChannel);


    if (CliSetClientContext(hClient) == FALSE)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: no client, returning -1\n");
        return (U032) SWAP_INVALID_CHANNEL;
    }

    if ((deviceList = CliGetDeviceList()) == NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: no device, returning -1\n");
        return (U032) SWAP_INVALID_CHANNEL;
    }

    for (i = 0; i < NUM_DEVICES; i++)
    {       
        if (deviceList[i].InUse && (deviceList[i].Handle == hDevice) && deviceList[i].DevFifoList)
        {
            for (j = 0; j < MAX_FIFOS; j++)
            {
                if (deviceList[i].DevFifoList[j].Handle == hChannel)
                {
                    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, 
                        "NVRM: returning ", deviceList[i].DevFifoList[j].ChID);
                    return deviceList[i].DevFifoList[j].ChID;
                }
            }
        }
    }
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: no chid, returning -1\n");
    return (U032) SWAP_INVALID_CHANNEL;
}

static U032
GetDevID(PHWINFO pDev)
{
    U032 i;

    for (i = 0; i < MAX_INSTANCE; i++)
        if (pDev == NvDBPtr_Table[i])
            return i;

    return SWAP_INVALID_DEVICE;
}

static PHWINFO
GetPDev(U032 DevID)
{
    if (DevID >= MAX_INSTANCE)
        return NULL;
    return NvDBPtr_Table[DevID];
}

static U032
GetClientDevID(U032 hClient, U032 hDevice)
{
    PCLI_DEVICE_INFO deviceList;
    U032 i;

    if (CliSetClientContext(hClient) == FALSE)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: no client, returning -1\n");
        return (U032) SWAP_INVALID_CHANNEL;
    }

    if ((deviceList = CliGetDeviceList()) == NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: no device, returning -1\n");
        return (U032) SWAP_INVALID_CHANNEL;
    }

    for (i = 0; i < NUM_DEVICES; i++)
    {       
        if (deviceList[i].InUse && (deviceList[i].Handle == hDevice))
            return deviceList[i].DevID;
    }

    return SWAP_INVALID_DEVICE; 
}

static U032
GetErrorDmaContext(
    U032 hClient,
    U032 hDevice,
    U032 hChannel
)
{
    PCLI_DEVICE_INFO deviceList;
    U032 i, j;

    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO, "NVRM: looking for chid:"); 
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:       client 0x%x", hClient); 
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:       channel 0x%x", hChannel);


    if (CliSetClientContext(hClient) == FALSE)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: no client, returning -1\n");
        return (U032) SWAP_INVALID_CHANNEL;
    }

    if ((deviceList = CliGetDeviceList()) == NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: no device, returning -1\n");
        return (U032) SWAP_INVALID_CHANNEL;
    }

    for (i = 0; i < NUM_DEVICES; i++)
    {       
        if (deviceList[i].InUse && (deviceList[i].Handle == hDevice) && deviceList[i].DevFifoList)
        {
            for (j = 0; j < MAX_FIFOS; j++)
            {
                if (deviceList[i].DevFifoList[j].Handle == hChannel)
                {
                    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, 
                        "NVRM: returning ", deviceList[i].DevFifoList[j].ErrorDmaContext);
                    return deviceList[i].DevFifoList[j].ErrorDmaContext;
                }
            }
        }
    }
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: no chid, returning -1\n");
    return (U032) SWAP_INVALID_CHANNEL;
}


/*
 * All swap clients are sorted by device.
 * A database of devices is shared among swap groups. A swap group has a 
 * linked list of device pointers that point to the devices in that group.
 *
 * Since a device can have channels in multiple swap groups, the device
 * pointer contains masks specificing which channels belong to that 
 * particular swap group.
 *
 * A channel can only belong to one swap group at a time, so the group id
 * of the bound group is stored in the channel structure.
 *
 */

struct swap_channel_entry_t {
    U032 valid;
    U032 GroupID;
    U032 hClient;
    U032 hErrorDma;   /* to write notifier */
    PDMAOBJECT pDma;
    U032 Put;
};

struct swap_device_entry_t {
    void *pDev;
    struct swap_channel_entry_t channels[SWAP_MAX_CHANNELS];
};

struct dev_ptr_t {
    U032 DevID;
    struct swap_device_entry_t *dev;
    U032 num_channels;
    U032 channel_mask;              /* mask of channels in this group */
    U032 ready_mask;                /* mask of channels ready to swap */
    struct dev_ptr_t *next;
};

struct swap_group_entry_t {
    U032 valid;
    U032 GroupID;
    U032 BarrierID;
    struct dev_ptr_t *devices;
};

// databases
static struct swap_group_entry_t swap_group_db[SWAP_MAX_GROUPS] = {};

// array of pointers to active devices..
static struct swap_device_entry_t *swap_device_db[MAX_INSTANCE] = {};


/********
 * Swap Database Functions
 ********/

static int
dbChannelValidClient(U032 hClient, U032 DevID, U032 ChID)
{
    if (swap_device_db[DevID] == NULL)
        return 0;

    if (swap_device_db[DevID]->channels[ChID].valid &&
        swap_device_db[DevID]->channels[ChID].hClient == hClient)
    {
        return 1;
    }
    return 0;
}

static struct swap_channel_entry_t *
dbChannelFind(U032 DevID, U032 ChID)
{
    // make sure the device is valid
    if (swap_device_db[DevID] == NULL)
        return NULL;

    // make sure the channel was allocated
    if (!swap_device_db[DevID]->channels[ChID].valid)
    {
        return NULL;
    }

    // return the channel
    return (&swap_device_db[DevID]->channels[ChID]);
}

static struct swap_channel_entry_t *
dbChannelAlloc(U032 DevID, U032 ChID)
{
    // we allocate devices as needed to save space..
    if (swap_device_db[DevID] == NULL)
    {
        if (RM_OK != osAllocMem( (void **) &swap_device_db[DevID], 
            sizeof(struct swap_device_entry_t)))
        {
            return NULL;
        }

        // zero the structure out
        osMemSet(swap_device_db[DevID], 0, 
            sizeof(struct swap_device_entry_t));

        // set the device number
        swap_device_db[DevID]->pDev = GetPDev(DevID);
    }

    // make sure this channel isn't already allocated
    if (swap_device_db[DevID]->channels[ChID].valid)
    {
        return NULL;
    }

    // return the channel
    return (&swap_device_db[DevID]->channels[ChID]);
}
    

static VOID
dbChannelFree(U032 DevID, U032 ChID)
{
    // make sure the device is valid
    if (swap_device_db[DevID] == NULL)
        return;

    // make sure the channel was allocated
    if (!swap_device_db[DevID]->channels[ChID].valid)
        return;

    osMemSet(&(swap_device_db[DevID]->channels[ChID]), 0, 
        sizeof(struct swap_channel_entry_t));
}

static struct swap_group_entry_t *
dbGroupFind(U032 GroupID)
{
    U032 i;

    for (i = 0; i < SWAP_MAX_GROUPS; i++)
    {
        if (swap_group_db[i].valid && 
            swap_group_db[i].GroupID == GroupID)
            return (&swap_group_db[i]);
    }
    return NULL;
}

static struct swap_group_entry_t *
dbGroupAlloc(U032 GroupID)
{
    U032 i;

    for (i = 0; i < SWAP_MAX_GROUPS; i++)
    {
        if (!swap_group_db[i].valid)
            return (&swap_group_db[i]);
    }
    return NULL;
}
    

static VOID
dbGroupFree(U032 GroupID)
{
    U032 i;

    for (i = 0; i < SWAP_MAX_GROUPS; i++)
    {
        if (swap_group_db[i].valid && 
            swap_group_db[i].GroupID == GroupID)
        {
            osMemSet(&swap_group_db[i], 0, 
                sizeof(struct swap_group_entry_t));
        }
    }
    return;
}


static struct swap_group_entry_t *
dbGroupFindBarrier(U032 BarrierID)
{
    int i;

    for (i = 0; i < SWAP_MAX_GROUPS; i++)
    {
        // use pDev to make sure the group is valid.. can BarrierID be '0'?
        if (swap_group_db[i].valid && swap_group_db[i].BarrierID == BarrierID)
        {
            return &swap_group_db[i];
        }
    }
    return NULL;
}

/********
 * Swap Group Member Functions
 ********/

static VOID sgRemoveBarrier(struct swap_group_entry_t *sg);

static struct dev_ptr_t *
sgAllocDevPtr(struct swap_group_entry_t *sg, U032 DevID)
{
    struct dev_ptr_t *tmp, *newdev;

    if (RM_OK != osAllocMem( (void **) &newdev, sizeof(struct dev_ptr_t)))
        return NULL;

    // zero the new structure out
    osMemSet(newdev, 0, sizeof(struct dev_ptr_t));

    if ( (sg->devices == NULL) || (sg->devices[0].DevID > DevID))
    {
        // new head device
        newdev->next = sg->devices;
        sg->devices = newdev;
    } else {
        // look for either the end of the list or a larger devID
        tmp = sg->devices;
        while ( (tmp) && (tmp->next) && (tmp->next->DevID > DevID))
            tmp = tmp->next;

        newdev->next = tmp->next;
        tmp->next = newdev;
    }

    newdev->dev          = swap_device_db[DevID];
    newdev->DevID        = DevID;
    newdev->channel_mask = 0;
    newdev->ready_mask   = 0;
    newdev->num_channels = 0;

    return newdev;
}

static void
sgFreeDevPtr(struct swap_group_entry_t *sg, U032 DevID)
{
    struct dev_ptr_t *tmp, *prev;

    if (sg->devices == NULL)
        return;

    prev = tmp = sg->devices;
    while (tmp && tmp->DevID != DevID)
    {
        prev = tmp;
        tmp = tmp->next;
    }

    if (prev == tmp)
        sg->devices = tmp->next;
    else
        prev->next = tmp->next;

    osFreeMem(tmp);

    return;
}

static struct dev_ptr_t *
sgFindDevPtr(struct swap_group_entry_t *sg, U032 DevID)
{
    struct dev_ptr_t *tmp = sg->devices;

    while ( (tmp) && (tmp->DevID != DevID) )
        tmp = tmp->next;

    return tmp;
}

static VOID
sgInit(struct swap_group_entry_t *sg, PHWINFO pDev, U032 GroupID)
{
    sg->GroupID = GroupID;
    sg->BarrierID    = 0;
    sg->devices      = NULL;
    sg->valid = 1;
}

static RM_STATUS
sgAddChannel(struct swap_group_entry_t *sg, U032 DevID, U032 ChID)
{
    struct dev_ptr_t *devptr;

    nv_printf("Adding channel %d:%d to group %d\n", 
        DevID, ChID, sg->GroupID);

    devptr = sgFindDevPtr(sg, DevID);
    if (devptr == NULL)
    {
        devptr = sgAllocDevPtr(sg, DevID);
        if (devptr == NULL)
            return RM_ERROR;
    }

    if (!devptr->dev->channels[ChID].valid)
    {
        return RM_ERROR;
    }

    SWAP_MASK_ADD_CHANNEL(devptr, ChID);
    devptr->num_channels++;

    return RM_OK;
}

static VOID
sgSwapIfReady(struct swap_group_entry_t *sg)
{
    struct dev_ptr_t *tmp;

    // nothing to swap
    if (sg->devices == NULL)
    {
        return;
    }


    // if any device isn't ready, bail out
    tmp = sg->devices;
    while (tmp)
    {
        if (!SWAP_MASK_SWAP_READY(tmp))
        {
            return;
        }
        tmp = tmp->next;
    }

    // if this is part of a barrier, call the barrier function
    // to handle things, otherwise call the swap function to 
    // write everything
    if (sg->BarrierID)
        osSwapBarrier(sg->BarrierID, RmSwapCallback, (VOID*) sg);
    else
        RmSwapCallback((VOID*) sg);
}

static RM_STATUS
sgRemoveChannel(struct swap_group_entry_t *sg, U032 DevID, U032 ChID)
{
    struct dev_ptr_t *devptr;

    nv_printf("Removing channel %d:%d from group %d\n", 
        DevID, ChID, sg->GroupID);

    devptr = sgFindDevPtr(sg, DevID);
    if (devptr == NULL)
    {
        return RM_ERROR;
    }

    // remove channel from masks, and see if we're ready to swap now
    SWAP_MASK_REMOVE_CHANNEL(devptr, ChID);
    devptr->num_channels--;

    if (devptr->num_channels <= 0)
        sgFreeDevPtr(sg, DevID);

    if (sg->devices == 0)
    {
        if (sg->BarrierID)
            sgRemoveBarrier(sg);
        dbGroupFree(sg->GroupID);
        return RM_OK;
    }

    sgSwapIfReady(sg);

    return RM_OK;
}

static VOID
sgChannelReady(struct swap_group_entry_t *sg, U032 DevID, U032 ChID)
{
    struct dev_ptr_t *tmp;

    nv_printf("marking channel %d:%d as ready\n", DevID, ChID);

    if ((tmp = sgFindDevPtr(sg, DevID)) == NULL)
        return;

    SWAP_MASK_CHANNEL_READY(tmp, ChID);
    sgSwapIfReady(sg);
}

static VOID
sgAddBarrier(struct swap_group_entry_t *sg, U032 BarrierID)
{
    sg->BarrierID = BarrierID;
    osInitSwapBarrier(BarrierID);
}

static VOID
sgRemoveBarrier(struct swap_group_entry_t *sg)
{
    osRemoveSwapBarrier(sg->BarrierID);
    sg->BarrierID = 0;
}

/********
 * Swap Channel Member Functions
 ********/

static VOID
scInit(struct swap_channel_entry_t *sc, 
       U032 hClient, 
       U032 hDevice, 
       U032 hChannel, 
       U032 GroupID
)
{
    sc->hClient   = hClient;
    sc->Put       = 0;
    sc->valid     = 1;
    sc->GroupID   = GroupID;
    sc->hErrorDma = GetErrorDmaContext(hClient, hDevice, hChannel);
    sc->pDma      = NULL;
}

static VOID
scPrepareForSwap(struct swap_channel_entry_t *sc, PHWINFO pDev, U032 Put)
{
    sc->Put = Put;

    // the dma object may not be valid when we init the channel,
    // but probably is now. Let's track it down to make the swap
    // itself go faster
    dmaFindContext(pDev, sc->hErrorDma, sc->hClient, &sc->pDma);
}


/********
 * Exported RmSwap Functions
 ********/

RM_STATUS
RmSwapInit(
    PHWINFO pDev
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "initializing swap extension\n");
    /* anything else needed? */
    osMemSet(swap_group_db, 0x0, sizeof(struct swap_group_entry_t) * 
        SWAP_MAX_GROUPS);
    osMemSet(swap_device_db, 0x0, sizeof(struct swap_device_entry_t *) * 
        MAX_INSTANCE);

    return RM_OK;
}

/* let 'er rip! */
static VOID
RmSwapCallback(
    VOID *data
)
{
    struct swap_group_entry_t *sg = (struct swap_group_entry_t *) data;
    struct dev_ptr_t *devtmp;
    U032 i, ChID;


    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"RmSwapCallback: 0x%x", data);

    if ((devtmp = sg->devices) == 0)
        return;

#if 0
    // XXX do I even need this??
    /* first, make sure the Pusher can't context switch on us */
    REG_WR_DRF_DEF(_PFIFO, _CACHES, _REASSIGN, _DISABLED);
    while (REG_RD_DRF(_PFIFO, _CACHES, _DMA_SUSPEND) ==
        NV_PFIFO_CACHES_DMA_SUSPEND_BUSY)
        ;
#endif

    while (devtmp)
    {
        PHWINFO pDev = devtmp->dev->pDev;
        U032 DevID = GetDevID(pDev);

        ChID = 0;
        for (i = 0; i < devtmp->num_channels; i++)
        {
            struct swap_channel_entry_t *sc;

            // find the next ChID in the channel mask
            while (!(devtmp->channel_mask & (1<<ChID)))
                ChID++;

            // assume all channels exist for now
            sc = dbChannelFind(DevID, ChID);

            /* write through our mapping to the user's dma put */
            REG_WR32(NV_USER_DMA_PUT(ChID, 0), sc->Put);

            // assume dma object is good, the app will hang otherwise
            notifyFillNotifier(pDev, sc->pDma, 0, 0, RM_OK);

            // increment past the current channel
            ChID++;
        }
        devtmp->ready_mask = 0;
        devtmp = devtmp->next;
    }

#if 0
    REG_WR_DRF_DEF(_PFIFO, _CACHES, _REASSIGN, _ENABLED);
#endif

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "RmSwapCallback: done with swap callback\n");
}


RM_STATUS
RmSwapClient(
    PHWINFO pDev,
    U032    ChID,
    U032    Put
)
{
    struct swap_channel_entry_t *sc = NULL;
    struct swap_group_entry_t  *sg = NULL;
    U032 DevID;

    nv_printf("SwapClient\n");

    if ((DevID = GetDevID(pDev)) == SWAP_INVALID_DEVICE)
    {
        return RM_ERROR;
    }

    sc = dbChannelFind(DevID, ChID);
    if (sc == NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, 
            "Attempting to swap non-registered client\n");
        return RM_ERROR;
    }

    sg = dbGroupFind(sc->GroupID);
    if (sg == NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,
            "Attempting to swap non-registered group\n");
        return RM_ERROR;
    }

    sc->Put = Put;
    scPrepareForSwap(sc, pDev, Put);

    /* indicate we're ready to go */
    sgChannelReady(sg, DevID, ChID);

    sgSwapIfReady(sg);

    return RM_OK;
}

RM_STATUS
RmSwapAddGroup(
    PHWINFO pDev,
    U032    hClient,
    U032    hDevice,
    U032    hChannel,
    U032    GroupID
)
{
    struct swap_channel_entry_t *sc = NULL;
    struct swap_group_entry_t *sg = NULL;
    U032 ChID, DevID;


    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: SwapAddGroup");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM: client 0x%x", hClient);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM: channel 0x%x", hChannel);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM: group 0x%x", GroupID);

    sg = dbGroupFind(GroupID);

    if (sg == NULL)
    {
        sg = dbGroupAlloc(GroupID);
        if (sg == NULL)
            return RM_ERROR /* SYS RESOURCES */;

        sgInit(sg, pDev, GroupID);
    }

    ChID = GetChID(hClient, hDevice, hChannel);
    if (ChID == SWAP_INVALID_CHANNEL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Couldn't find ChID");
        return RM_ERROR /* INVALID CLIENT? */;
    }

    DevID = GetClientDevID(hClient, hDevice);
    if (DevID == SWAP_INVALID_DEVICE)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Couldn't find DevID");
        return RM_ERROR /* INVALID DEVICE? */;
    }

    nv_printf("AddGroup: add channel %d:%d to group %d\n", DevID, ChID, GroupID);

    sc = dbChannelAlloc(DevID, ChID);
    if (sc == NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Channel already allocated?");
        return RM_ERROR /* INVALID CLIENT? */;
    }

    /* store client data */
    scInit(sc, hClient, hDevice, hChannel, GroupID);

    /* store group data */
    sgAddChannel(sg, DevID, ChID);

    return RM_OK;
}


RM_STATUS
RmSwapAddBarrier(
    PHWINFO pDev,
    U032    GroupID,
    U032    BarrierID
)
{
    struct swap_group_entry_t *sg = NULL;

    sg = dbGroupFind(GroupID);

    if (sg == NULL)
    {
        return RM_ERROR /* BAD_GROUP */;
    }

    sgAddBarrier(sg, BarrierID);

    return RM_OK;
}


RM_STATUS
RmSwapRemoveGroup(
    PHWINFO pDev,
    U032    hClient,
    U032    hDevice,
    U032    hChannel,
    U032    GroupID
)
{
    struct swap_group_entry_t *sg = NULL;
    U032 ChID, DevID;

    sg = dbGroupFind(GroupID);
    if (sg == NULL)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,
            "RemoveSwapGroup: invalid group id 0x%x", GroupID);
        return RM_ERROR /* BAD_GROUP */;
    }

    ChID = GetChID(hClient, hDevice, hChannel);
    if (ChID == SWAP_INVALID_CHANNEL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, 
            "RemoveSwapGroup: invalid group or channel");
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,
            "    client : 0x%x", hClient);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,
            "    channel: 0x%x", hChannel);
        return RM_ERROR /* BAD_GROUP */;
    }

    DevID = GetClientDevID(hClient, hDevice);
    if (ChID == SWAP_INVALID_DEVICE)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "Couldn't find DevID");
        return RM_ERROR /* INVALID DEVICE? */;
    }

    sgRemoveChannel(sg, DevID, ChID);

    dbChannelFree(DevID, ChID);

    return RM_OK;
}

RM_STATUS
RmSwapRemoveBarrier(
    PHWINFO pDev,
    U032    BarrierID
)
{
    struct swap_group_entry_t *sg = NULL;

    sg = dbGroupFindBarrier(BarrierID);

    if (sg == NULL)
    {
        return RM_ERROR /* BAD_GROUP */;
    }

    sgRemoveBarrier(sg);

    return RM_OK;
}


RM_STATUS
RmSwapExt(
    PHWINFO pDev,
    U032    hClient,
    U032    hDevice,
    U032    hChannel,
    U032    flags,
    U032    group_id,
    U032    barrier_id
)
{
    RM_STATUS rmStatus = RM_ERROR;

    if (DRF_VAL(OS2A, _FLAGS, _SWAP, flags) == NVOS2A_FLAGS_SWAP_GROUP)
    {
        if (DRF_VAL(OS2A, _FLAGS, _ACTION, flags) ==
            NVOS2A_FLAGS_ACTION_ADD)
                rmStatus = RmSwapAddGroup(pDev, hClient, hDevice, hChannel, group_id);
        else

        if (DRF_VAL(OS2A, _FLAGS, _ACTION, flags) ==
            NVOS2A_FLAGS_ACTION_REMOVE)
                rmStatus = RmSwapRemoveGroup(pDev, hClient, hDevice, hChannel, group_id);
    } else

    if (DRF_VAL(OS2A, _FLAGS, _SWAP, flags) == NVOS2A_FLAGS_SWAP_BARRIER)
    {
        if (DRF_VAL(OS2A, _FLAGS, _ACTION, flags) ==
            NVOS2A_FLAGS_ACTION_ADD)
                rmStatus = RmSwapAddBarrier(pDev, group_id, barrier_id);

        if (DRF_VAL(OS2A, _FLAGS, _ACTION, flags) ==
            NVOS2A_FLAGS_ACTION_REMOVE)
                rmStatus = RmSwapRemoveBarrier(pDev, barrier_id);
    } else
    /* unknown swap extension */
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: unknown Swap Extension\n");

    return rmStatus;
}

RM_STATUS
RmSwapClientGone(
    U032    hClient
)
{
    U032 DevID, ChID;

    for (DevID = 0; DevID < MAX_INSTANCE; DevID++)
    {
        for (ChID = 0; ChID < SWAP_MAX_CHANNELS; ChID++)
        {
            if (dbChannelValidClient(hClient, DevID, ChID))
            {
                struct swap_channel_entry_t *sc;
                struct swap_group_entry_t *sg;

                sc = dbChannelFind(DevID, ChID);
                if (sc == NULL)
                    return RM_ERROR;

                sg = dbGroupFind(sc->GroupID);
                if (sg == NULL)
                    return RM_ERROR;

                sgRemoveChannel(sg, DevID, ChID);

                dbChannelFree(DevID, ChID);
            }
        }
    }
    return RM_OK;
}

