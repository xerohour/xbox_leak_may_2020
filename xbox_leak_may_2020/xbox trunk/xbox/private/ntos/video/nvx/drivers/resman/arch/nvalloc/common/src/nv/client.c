/******************************************************************************
*
*   File:  client.c
*
*   Description:
*       This file contains the functions necessary to manage NV Resource
*   Manager client information.
*
*
*   THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
*   NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
*   IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
*
******************************************************************************/

// TO DO: ClientInstance is a global!!!!  Can we rely on it's being SMP safe??
//        i.e. can two IOCTLs be processing simultaneously?

// This file is chip independent AND OS independent

#include <nvrm.h>
#include <client.h>
#include <nvos.h>
#include <nvarch.h>

//
// Return a "unique" client handle by incrementing our clientHandle variable.
// We cannot let this freely range over all possible values, since it would
// eventually match an already existing handle (looping after 1MB hopefully
// will be enough).
//
static U032 clientHandle = 0xc1d00000;

RM_STATUS CliAddClient(
    U032 *phClient,
    U032 hClass
)
{
    U032 i;

    // verify class is a root object
    switch (hClass)
    {
        case NV01_ROOT:
        case NV01_ROOT_USER:
            break;

        default:
            return RM_ERR_BAD_CLASS;

    }

    // register the client
    for (i = 1; i < NUM_CLIENTS; i++)
    {
        if ( ! clientInfo[i].InUse)
        {
            // create a unique client handle
            clientHandle = ((clientHandle + 1) % 0x100000) ? (clientHandle + 1) : 0xc1d00000;
            // mark it busy asap
            clientInfo[i].InUse         = TRUE;

            // initialize the client
            clientInfo[i].Handle           = clientHandle;
            clientInfo[i].Class            = hClass;
            clientInfo[i].ProcID           = i;
            clientInfo[i].ProcContext      = 0;
            clientInfo[i].FifoCount        = 0;
            clientInfo[i].CliDmaList       = NULL;
            clientInfo[i].CliMemoryList    = NULL;
            clientInfo[i].CliEventList     = NULL;
            clientInfo[i].CliHeapOwnerList = NULL;    //added by Chuck Moidel

            // set the client instance and return the client handle
            CliSetClientInstance(i);
            *phClient = clientInfo[i].Handle;

            return RM_OK;
        }
    }

    return RM_ERR_INSUFFICIENT_RESOURCES;

} // end of CliAddClient()

BOOL CliDelClient()
{
    clientInfo[ClientInstance].InUse = FALSE;

    return TRUE;

} // end of CliDelClient()

BOOL CliGetClientHandle(
    U032 *phClient
)
{
    if (clientInfo[ClientInstance].InUse == TRUE)
    {
        *phClient = clientInfo[ClientInstance].Handle;
        return TRUE;
    }

    return FALSE;

} // end of CliGetClientHandle()

BOOL CliGetClientIndex(
    U032 hClient,
    U032 *pIndex
)
{
    U032 i;

    for (i = 1; i < NUM_CLIENTS; i++)
    {
        if (clientInfo[i].InUse && clientInfo[i].Handle == hClient)
        {
            *pIndex = i;
            return TRUE;
        }
    }

    return FALSE;

} // end of CliGetClientIndex()

BOOL CliSetClientContext(
    U032 hClient
)
{
    U032 i;

    for (i = 1; i < NUM_CLIENTS; i++)
    {
        if (clientInfo[i].InUse && clientInfo[i].Handle == hClient)
        {
            ClientInstance = i;
            return TRUE;
        }
    }

    return FALSE;

} // end of CliSetClientContext()

BOOL CliSetClientInstance(
    U032 instance
)
{
    if (instance < NUM_CLIENTS)
    {
        ClientInstance = instance;
        return TRUE;
    }

    return TRUE;

} // end of CliSetClientInstance()

BOOL CliSetClientAppHandle(
    U032  hClient,
    void *appHandle
)
{
    U032 i;

    for (i = 1; i < NUM_CLIENTS; i++)
    {
        if (clientInfo[i].InUse && clientInfo[i].Handle == hClient)
        {
            clientInfo[i].AppHandle = appHandle;
            return TRUE;
        }
    }

    return FALSE;

} // end of CliSetClientAppHandle()

BOOL CliGetClientAppHandle(
    U032   hClient,
    void **appHandle
)
{
    U032 i;

    for (i = 1; i < NUM_CLIENTS; i++)
    {
        if (clientInfo[i].InUse && clientInfo[i].Handle == hClient)
        {
            *appHandle = clientInfo[i].AppHandle;
            return TRUE;
        }
    }

    return FALSE;

} // end of CliGetClientAppHandle()


RM_STATUS CliAddDevice(
    U032 hClient,
    U032 hDevice,
    U032 hClass
)
{
    U032 client, device;

    // validate client
    if (!CliGetClientHandle(&client) || client != hClient)
    {
        return RM_ERR_BAD_OBJECT_PARENT;
    }

    // validate device
    if (!CliIsUniqueHandle(hDevice, FALSE))
    {
        return RM_ERR_BAD_OBJECT_HANDLE;
    }

    // validate class
    if (!CliGetPhysicalDevice(hClass, &device))
    {
        return RM_ERR_BAD_CLASS;
    }

    // make sure we have a device list
    if ( ! clientInfo[ClientInstance].CliDeviceList)
        if ( ! CliMakeDeviceList(ClientInstance, NUM_DEVICES, &clientInfo[ClientInstance].CliDeviceList))
            goto failed;

    // initialize the device
    if ( ! clientInfo[ClientInstance].CliDeviceList[device].InUse)
    {
        clientInfo[ClientInstance].CliDeviceList[device].InUse          = TRUE;
        clientInfo[ClientInstance].CliDeviceList[device].Handle         = hDevice;
        clientInfo[ClientInstance].CliDeviceList[device].Client         = hClient;
        clientInfo[ClientInstance].CliDeviceList[device].Class          = hClass;
        clientInfo[ClientInstance].CliDeviceList[device].DevMemoryList  = NULL;
        clientInfo[ClientInstance].CliDeviceList[device].AppHandle      = NULL;
        clientInfo[ClientInstance].CliDeviceList[device].DevFifoList    = NULL;
        return RM_OK;
    }

 failed:
    return RM_ERR_INSUFFICIENT_RESOURCES;

} // end of CliAddDevice()


BOOL CliDelDevice(
    U032 hDevice
)
{
    PCLI_DEVICE_INFO pDevice;

    if (CliGetDeviceInfo(hDevice, &pDevice))
    {
        // delete the dynamically allocated device fifo list
        if (pDevice->DevFifoList)
        {
            osFreeMem(pDevice->DevFifoList);
            pDevice->DevFifoList = NULL;
        }

        // mark the device as unused
        pDevice->InUse = FALSE;
        return TRUE;
    }

    return FALSE;

} // end of CliDelDevice()


// allocate a single client's device info
BOOL CliMakeDeviceList(
    U032              client_instance,
    U032              num_devices,
    PCLI_DEVICE_INFO *ppDeviceList
)
{
    PCLI_DEVICE_INFO pdevice;
	
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: make new device list\n");

    if (RM_OK != osAllocMem((VOID **)ppDeviceList, sizeof(CLI_DEVICE_INFO) * num_devices))
        return FALSE;

    for (pdevice = *ppDeviceList; pdevice < *ppDeviceList + num_devices; pdevice++)
    {
        pdevice->InUse       = FALSE;
        pdevice->AppID       = client_instance;
        pdevice->AppHandle   = (void *) 0;
        pdevice->DevID       = (U032)(pdevice - *ppDeviceList);
        pdevice->DevFifoList = NULL;
    }

    return TRUE;
}


BOOL CliGetDeviceInfo(
    U032 hDevice,
    PCLI_DEVICE_INFO *ppDeviceInfo
)
{
    PCLI_DEVICE_INFO deviceList;
    U032 i;

    if ((deviceList = CliGetDeviceList()) != NULL)
    {
        for (i = 0; i < NUM_DEVICES; i++)
        {
            if (deviceList[i].InUse && deviceList[i].Handle == hDevice)
            {
                *ppDeviceInfo = &deviceList[i];
                return TRUE;
            }
        }
    }

    return FALSE;

} // end of CliGetDeviceInfo()

PCLI_DEVICE_INFO CliGetDeviceList()
{
    return clientInfo[ClientInstance].CliDeviceList;

} // end of CliGetDeviceList()

BOOL CliGetDeviceHandle(
    U032 hFifo,
    U032 *phDevice
)
{
    PCLI_DEVICE_INFO deviceList;
    U032 i, j;

    // get the device handle based on one of its fifo handles
    if ((deviceList = CliGetDeviceList()) != NULL)
    {
        for (i = 0; i < NUM_DEVICES; i++)
        {
            if (deviceList[i].InUse && deviceList[i].DevFifoList)
            {
                for (j = 0; j < MAX_FIFOS; j++)
                {
                    if (deviceList[i].DevFifoList[j].InUse &&
                        (deviceList[i].DevFifoList[j].Handle == hFifo))
                    {
                        *phDevice = deviceList[i].Handle;
                        return TRUE;
                    }
                }
            }
        }
    }

    return FALSE;
}

BOOL CliSetDeviceContext(
    U032 hDevice,
    PHWINFO* ppDev
)
{
    PCLI_DEVICE_INFO pDevice;

    if (CliGetDeviceInfo(hDevice, &pDevice))
    {
        // For NT there is no global DeviceInstance variable.
        U032 deviceInstance;

        if (CliGetPhysicalDevice(pDevice->Class, &deviceInstance))
        {
            *ppDev = NvDBPtr_Table[deviceInstance];
            osEnsureDeviceEnabled((PHWINFO)*ppDev);
            return TRUE;
        }
    }

    return FALSE;

} // end of CliSetDeviceContext()

BOOL CliSetDeviceAppHandle(
    U032  hDevice,
    void *appHandle
)
{
    PCLI_DEVICE_INFO pDevice;

    if (CliGetDeviceInfo(hDevice, &pDevice))
    {
        pDevice->AppHandle = appHandle;
        return TRUE;
    }

    return FALSE;

} // end of CliSetDeviceAppHandle()

BOOL CliGetDeviceAppHandle(
    U032   hDevice,
    void **appHandle
)
{
    PCLI_DEVICE_INFO pDevice;

    if (CliGetDeviceInfo(hDevice, &pDevice))
    {
        *appHandle = pDevice->AppHandle;
        return TRUE;
    }

    return FALSE;

} // end of CliSetDeviceAppHandle()


RM_STATUS CliAddDeviceFifo(
    PHWINFO pDev,
    U032 hDevice,
    U032 hFifo,
    U032 hClass,
    U032 hErrorCtx,
    U032 hDataCtx,
    U032 offset,
    U032 flags,
    U032 chID,
    VOID* pChannel
)
{
    PCLI_DEVICE_INFO pDevice;

    // validate device
    if (!CliGetDeviceInfo(hDevice, &pDevice))
    {
        return RM_ERR_BAD_OBJECT_PARENT;
    }

    // validate fifo
    if ( ! CliIsUniqueHandle(hFifo, FALSE))
    {
        return RM_ERR_BAD_OBJECT_HANDLE;
    }

    // make sure we have a fifo list
    // allocate MAX_FIFOS worth because some of the code in here walks the
    //     list without having access to NUM_FIFOS for a particular card.
    //     (NUM_FIFOS is a macro defined in terms of pDev)
    if ( ! pDevice->DevFifoList)
        if ( ! CliMakeDeviceFifoList(ClientInstance,
                                     pDevice->DevID,
                                     MAX_FIFOS,
                                     &pDevice->DevFifoList))
            return RM_ERR_INSUFFICIENT_RESOURCES;

    // add fifo info to the given device's fifo
    if ( ! pDevice->DevFifoList[chID].InUse)
    {
        pDevice->DevFifoList[chID].Handle           = hFifo;
        pDevice->DevFifoList[chID].Device           = hDevice;
        pDevice->DevFifoList[chID].Class            = hClass;
        pDevice->DevFifoList[chID].ErrorDmaContext  = hErrorCtx;
        pDevice->DevFifoList[chID].DataDmaContext   = hDataCtx;
        pDevice->DevFifoList[chID].Selector         = 0;
        pDevice->DevFifoList[chID].Flat             = pChannel;
        pDevice->DevFifoList[chID].FifoPtr          = &pDev->DBfifoTable[chID];
        pDevice->DevFifoList[chID].heventWait       = 0;
        pDevice->DevFifoList[chID].hwndNotify       = 0;
        pDevice->DevFifoList[chID].hwndError        = 0;
        pDevice->DevFifoList[chID].msgNotify        = 0;
        pDevice->DevFifoList[chID].msgError         = 0;
        pDevice->DevFifoList[chID].lpSysCallback    = 0;
        pDevice->DevFifoList[chID].eventNotify      = 0;

        pDevice->DevFifoList[chID].InUse            = TRUE;
        return RM_OK;
    }

    return RM_ERR_INSUFFICIENT_RESOURCES;

} // end of CliAddDeviceFifo()

BOOL CliDelDeviceFifo(
    U032 hDevice,
    U032 hFifo
)
{
    PCLI_FIFO_INFO pFifo;

    if (CliGetDeviceFifoInfo(hDevice, hFifo, &pFifo))
    {
        pFifo->InUse = FALSE;
        return TRUE;
    }

    return FALSE;

} // end of CliDelDeviceFifo()

// allocate a single device's fifo info

BOOL CliMakeDeviceFifoList(
    U032              client_instance,
    U032              device_instance,
    U032              num_fifos,
    PCLI_FIFO_INFO   *ppFifoList
)
{
    PCLI_FIFO_INFO pfifo;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: make new fifo list\n");

    if (RM_OK != osAllocMem((VOID **)ppFifoList, sizeof(CLI_FIFO_INFO) * num_fifos))
        return FALSE;

    for (pfifo = *ppFifoList; pfifo < *ppFifoList + num_fifos; pfifo++)
    {
        pfifo->InUse = FALSE;
        pfifo->AppID = client_instance;
        pfifo->AppHandle = (void *) 0;
        pfifo->DevID = device_instance;
        pfifo->ChID = (U032)(pfifo - *ppFifoList);

        pfifo->Selector = 0;
        pfifo->Handle = 0;
        pfifo->Device = 0;
        pfifo->Class = 0;
        pfifo->ErrorDmaContext = 0;
        pfifo->DataDmaContext = 0;
        pfifo->FifoPtr = 0;
        pfifo->Flat = 0;
        pfifo->heventWait = 0;
        pfifo->hwndNotify = 0;
        pfifo->hwndError = 0;
        pfifo->msgNotify = 0;
        pfifo->msgError = 0;
        pfifo->lpSysCallback = 0;
        pfifo->eventNotify = 0;
        pfifo->pDmaObject = 0;
    }

    return TRUE;
}

BOOL CliGetDeviceFifoInfo(
    U032 hDevice,
    U032 hFifo,
    PCLI_FIFO_INFO *ppFifoInfo
)
{
    PCLI_FIFO_INFO fifoList;
    U032 i;

    if (CliGetDeviceFifoList(hDevice, &fifoList) && fifoList)
    {
        for (i = 0; i < MAX_FIFOS; i++)
        {
            if (fifoList[i].InUse && (fifoList[i].Handle == hFifo))
            {
                *ppFifoInfo = &fifoList[i];
                return TRUE;
            }
        }
    }

    return FALSE;

} // end of CliGetDeviceFifoInfo()


// CliGetDeviceFifoList() could return a NULL list on TRUE
BOOL CliGetDeviceFifoList(
    U032 hDevice,
    PCLI_FIFO_INFO *pFifoList
)
{
    PCLI_DEVICE_INFO pDevice;

    if (CliGetDeviceInfo(hDevice, &pDevice))
    {
        *pFifoList = pDevice->DevFifoList;
        return TRUE;
    }

    return FALSE;

} // end of CliGetDeviceFifoList()

RM_STATUS CliAddDeviceMemory(
    U032 hDevice,
    U032 hMemory,
    U032 hClass,
    VOID *address,
    U032 length,
    U032 instance,
    U032 heapOwner,
    U032 version      //see CLI_MEMORY_INFO definition for more info on version  -C.Moidel
)
{
    PCLI_DEVICE_INFO pDevice;
    PCLI_MEMORY_INFO pMemory;

    // validate client
    if (!CliGetDeviceInfo(hDevice, &pDevice))
    {
        return RM_ERR_BAD_OBJECT_PARENT;
    }

    // validate device
    if (!CliIsUniqueHandle(hMemory, FALSE))
    {
        return RM_ERR_BAD_OBJECT_HANDLE;
    }

    // validate class
    switch (hClass)
    {
        case NV01_MEMORY_LOCAL_PRIVILEGED:
        case NV01_MEMORY_LOCAL_USER:
            break;

        default:
            return RM_ERR_BAD_CLASS;

    }

    //TODO: Validate HeapOwner based on version     -C.Moidel

    // allocate a new memory info struct and add to the device
    if (RM_SUCCESS(osAllocMem((void **) &pMemory, sizeof(CLI_MEMORY_INFO))))
    {
        // link in the new device memory mapping
        pMemory->Next = pDevice->DevMemoryList;
        pMemory->Prev = NULL;
        pDevice->DevMemoryList = pMemory;
        if (pMemory->Next)
        {
            pMemory->Next->Prev = pMemory;
        }

        // initialize the device
        pMemory->Handle     = hMemory;
        pMemory->Parent     = hDevice;
        pMemory->Class      = hClass;
        pMemory->Address    = address;
        pMemory->Length     = length;
        pMemory->Instance   = instance;
        pMemory->HeapOwner  = heapOwner;
        pMemory->version    = version;
        return RM_OK;
    }

    return RM_ERR_INSUFFICIENT_RESOURCES;

} // end of CliAddDeviceMemory()

BOOL CliDelDeviceMemory(
    U032 hDevice,
    U032 hMemory
)
{
    PCLI_MEMORY_INFO pMemory;
    PCLI_DEVICE_INFO pDevice;

    if (CliGetDeviceMemoryInfo(hDevice, hMemory, &pMemory))
    {
        // fixup prev link
        if (pMemory->Prev)
        {
            pMemory->Prev->Next = pMemory->Next;
        }
        else
        {
            // head of list, so modify the list head link
            if (!CliGetDeviceInfo(hDevice, &pDevice))
            {
                return FALSE;
            }
            pDevice->DevMemoryList = pMemory->Next;
        }

        // fixup next link
        if (pMemory->Next)
        {
            pMemory->Next->Prev = pMemory->Prev;
        }

        // free the list element
        osFreeMem(pMemory);

        return TRUE;
    }

    return FALSE;

} // end of CliDelDeviceMemory()

//
// This is currently used only by heapPurge to delete DeviceMemory based
// only on the owner.
//
BOOL CliDelDeviceHeapOwner(
    U032 hDevice,
    U032 heapOwner
)
{
    PCLI_MEMORY_INFO pMemory;
    BOOL retval = FALSE;

    for (pMemory = CliGetDeviceMemoryList(hDevice); pMemory; )
    {
        if (pMemory->HeapOwner == heapOwner)
        {
            U032 hMemory = pMemory->Handle;

            pMemory = pMemory->Next;                // get Next, before we delete it
            CliDelDeviceMemory(hDevice, hMemory);

            retval = TRUE;
        } else
            pMemory = pMemory->Next;
    }

    return retval;

} // end of CliDelDeviceHeapOwner

BOOL CliGetDeviceMemoryInfo(
    U032 hDevice,
    U032 hMemory,
    PCLI_MEMORY_INFO *ppMemoryInfo
)
{
    PCLI_MEMORY_INFO pMemory;

    for (pMemory = CliGetDeviceMemoryList(hDevice); pMemory; pMemory = pMemory->Next)
    {
        if (pMemory->Handle == hMemory)
        {
            *ppMemoryInfo = pMemory;
            return TRUE;
        }
    }

    return FALSE;

} // end of CliGetDeviceMemoryInfo()

PCLI_MEMORY_INFO CliGetDeviceMemoryList(
    U032 hDevice
)
{
    PCLI_DEVICE_INFO pDevice;
    PCLI_MEMORY_INFO memoryList = NULL;

    if (CliGetDeviceInfo(hDevice, &pDevice))
    {
        memoryList = pDevice->DevMemoryList;
    }

    return memoryList;

} // end of CliGetDeviceMemoryList()

RM_STATUS CliAddDma(
    U032 hClient,
    U032 hDma,
    U032 hClass,
    U032 flags,
    U032 selector,
    VOID* base,
    U032 limit,
    PCLI_DMA_INFO *ppDma
)
{
    U032 client;
    PCLI_DMA_INFO pDma;
    U032 access, physicality, locked, coherency;

    // validate client
    if (!CliGetClientHandle(&client) || client != hClient)
    {
        return RM_ERR_BAD_OBJECT_PARENT;
    }

    // validate handle
    if (!CliIsUniqueHandle(hDma, FALSE))
    {
        return RM_ERR_BAD_OBJECT_HANDLE;
    }

    // validate class
    switch (hClass)
    {
        case NV01_CONTEXT_DMA:
            break;

        default:
            return RM_ERR_BAD_CLASS;
    }

    // validate the flags
    switch (flags >> DRF_SHIFT(NVOS03_FLAGS_ACCESS) & DRF_MASK(NVOS03_FLAGS_ACCESS))
    {
        case NVOS03_FLAGS_ACCESS_READ_WRITE:
            access = NV01_CONTEXT_DMA_IN_MEMORY;
            break;

        case NVOS03_FLAGS_ACCESS_READ_ONLY:
            access = NV01_CONTEXT_DMA_FROM_MEMORY;
            break;

        case NVOS03_FLAGS_ACCESS_WRITE_ONLY:
            access = NV01_CONTEXT_DMA_TO_MEMORY;
            break;

        default:
            return RM_ERR_BAD_FLAGS;
    }
    switch (flags >> DRF_SHIFT(NVOS03_FLAGS_PHYSICALITY) & DRF_MASK(NVOS03_FLAGS_PHYSICALITY))
    {
        case NVOS03_FLAGS_PHYSICALITY_CONTIGUOUS:
            physicality = NVOS03_FLAGS_PHYSICALITY_CONTIGUOUS;
            break;

        case NVOS03_FLAGS_PHYSICALITY_NONCONTIGUOUS:
            physicality = NVOS03_FLAGS_PHYSICALITY_NONCONTIGUOUS;
            break;

        default:
            return RM_ERR_BAD_FLAGS;
    }
    switch (flags >> DRF_SHIFT(NVOS03_FLAGS_LOCKED) & DRF_MASK(NVOS03_FLAGS_LOCKED))
    {
        case NVOS03_FLAGS_LOCKED_ALWAYS:
            locked = NVOS03_FLAGS_LOCKED_ALWAYS;
            break;

        case NVOS03_FLAGS_LOCKED_IN_TRANSIT:
            locked = NVOS03_FLAGS_LOCKED_IN_TRANSIT;
            break;

        default:
            return RM_ERR_BAD_FLAGS;
    }
    switch (flags >> DRF_SHIFT(NVOS03_FLAGS_COHERENCY) & DRF_MASK(NVOS03_FLAGS_COHERENCY))
    {
        case NVOS03_FLAGS_COHERENCY_UNCACHED:
            coherency = NVOS03_FLAGS_COHERENCY_UNCACHED;
            break;

        case NVOS03_FLAGS_COHERENCY_CACHED:
            coherency = NVOS03_FLAGS_COHERENCY_CACHED;
            break;

        default:
            return RM_ERR_BAD_FLAGS;
    }

    // allocate a new dmaInfo and add the device to the client
    if (RM_SUCCESS(osAllocMem((void **) &pDma, sizeof(CLI_DMA_INFO))))
    {
        // link in the new DMA
        pDma->Next = clientInfo[ClientInstance].CliDmaList;
        pDma->Prev = NULL;
        clientInfo[ClientInstance].CliDmaList = pDma;
        if (pDma->Next)
        {
            pDma->Next->Prev = pDma;
        }

        // initialize the device
        pDma->Handle        = hDma;
        pDma->Client        = hClient;
        pDma->Class         = hClass;
        pDma->Flags         = flags;
        pDma->Access        = access;
        pDma->Physicality   = physicality;
        pDma->Locked        = locked;
        pDma->Coherency     = coherency;
        pDma->Valid         = FALSE;
        pDma->DescSelector  = selector;
        pDma->DescOffset    = base;
        pDma->DescLimit     = limit;
        pDma->DescAddr      = 0;
        pDma->BufferBase    = 0;
        pDma->BufferSize    = 0;
        pDma->LockHandle    = 0;
        pDma->PteCount      = 0;
        pDma->PteAdjust     = 0;
        pDma->PteLimit      = 0;
        pDma->PteArray      = 0;
        pDma->AddressSpace  = ADDR_UNKNOWN;
        *ppDma = pDma;

        return RM_OK;
    }

    return RM_ERR_INSUFFICIENT_RESOURCES;

} // end of CliAddDma()

BOOL CliDelDma(
    U032 hDma
)
{
    PCLI_DMA_INFO pDma;

    if (CliGetDmaInfo(hDma, &pDma))
    {
        // fixup prev link
        if (pDma->Prev)
        {
            pDma->Prev->Next = pDma->Next;
        }
        else
        {
            // head of list, so modify the list head link
            clientInfo[ClientInstance].CliDmaList = pDma->Next;
        }

        // fixup the next link
        if (pDma->Next)
        {
            pDma->Next->Prev = pDma->Prev;
        }

        // free the list element
        osFreeMem(pDma);

        return TRUE;
    }

    return FALSE;

} // end of CliDelDma()

BOOL CliGetDmaInfo(
    U032 hDma,
    PCLI_DMA_INFO *ppDmaInfo
)
{
    PCLI_DMA_INFO pDma;

    for (pDma = CliGetDmaList(); pDma; pDma = pDma->Next)
    {
        if (pDma->Handle == hDma)
        {
            *ppDmaInfo = pDma;
            return TRUE;
        }
    }

    return FALSE;

} // end of CliGetDmaInfo()

PCLI_DMA_INFO CliGetDmaList()
{
    return clientInfo[ClientInstance].CliDmaList;

} // end of CliGetDmaList()

RM_STATUS CliAddMemory(
    U032 hClient,
    U032 hMemory,
    U032 hClass,
    U032 flags,
    VOID* address,
    U032 length,
    VOID* memdata       // private per-alloc data
)
{
    U032 client;
    PCLI_MEMORY_INFO pMemory;

    // validate client
    if (!CliGetClientHandle(&client) || client != hClient)
    {
        return RM_ERR_BAD_OBJECT_PARENT;
    }

    // validate memory description
    if (!CliIsUniqueHandle(hMemory, FALSE))
    {
        return RM_ERR_BAD_OBJECT_HANDLE;
    }

    // validate class
    switch (hClass)
    {
        case NV01_MEMORY_SYSTEM:
            break;

        default:
            return RM_ERR_BAD_CLASS;

    }

    // allocate a new memory description and add the device to the client
    if (RM_SUCCESS(osAllocMem((void **) &pMemory, sizeof(CLI_MEMORY_INFO))))
    {
        // link in the new system memory mapping
        pMemory->Next = clientInfo[ClientInstance].CliMemoryList;
        pMemory->Prev = NULL;
        clientInfo[ClientInstance].CliMemoryList = pMemory;
        if (pMemory->Next)
        {
            pMemory->Next->Prev = pMemory;
        }

        // initialize the memory description
        pMemory->Handle     = hMemory;
        pMemory->Parent     = hClient;
        pMemory->Class      = hClass;
        pMemory->Address    = address;
        pMemory->Length     = length;
        pMemory->Type       = DRF_VAL(OS02, _FLAGS, _LOCATION, flags);
        pMemory->Physicality = DRF_VAL(OS02, _FLAGS, _PHYSICALITY, flags);
        pMemory->Coherency  = DRF_VAL(OS02, _FLAGS, _COHERENCY, flags);
        pMemory->MemData    = memdata;

        return RM_OK;
    }

    return RM_ERR_INSUFFICIENT_RESOURCES;

} // end of CliAddMemory()

BOOL CliDelMemory(
    U032 hMemory
)
{
    PCLI_MEMORY_INFO pMemory;

    if (CliGetMemoryInfo(hMemory, &pMemory))
    {
        // fixup prev link
        if (pMemory->Prev)
        {
            pMemory->Prev->Next = pMemory->Next;
        }
        else
        {
            // head of list, so modify the list head link
            clientInfo[ClientInstance].CliMemoryList = pMemory->Next;
        }

        // fixup the next link
        if (pMemory->Next)
        {
            pMemory->Next->Prev = pMemory->Prev;
        }

        pMemory->MemData = NULL;

        // free the list element
        osFreeMem(pMemory);

        return TRUE;
    }

    return FALSE;

} // end of CliDelMemory()

BOOL CliGetMemoryInfo(
    U032 hMemory,
    PCLI_MEMORY_INFO *ppMemoryInfo
)
{
    PCLI_MEMORY_INFO pMemory;

    for (pMemory = CliGetMemoryList(); pMemory; pMemory = pMemory->Next)
    {
        if (pMemory->Handle == hMemory)
        {
            *ppMemoryInfo = pMemory;
            return TRUE;
        }
    }

    return FALSE;

} // end of CliGetMemoryInfo()


// which memory chunk spans a given linear address?
BOOL CliGetMemoryInfoForLinAddr(
    VOID *address,
    PCLI_MEMORY_INFO *ppMemoryInfo
)
{
    PCLI_MEMORY_INFO pMemory;

    for (pMemory = CliGetMemoryList(); pMemory; pMemory = pMemory->Next)
    {
        // NOTE: pMemory->length is really length-1
        if ((address >= pMemory->Address) &&
            (address <= (VOID*)((U008*)pMemory->Address + pMemory->Length)))
        {
            *ppMemoryInfo = pMemory;
            return TRUE;
        }
    }

    return FALSE;

} // end of CliGetMemoryInfoForLinAddr()


PCLI_MEMORY_INFO CliGetMemoryList()
{
    return clientInfo[ClientInstance].CliMemoryList;

} // end of CliGetMemoryList()

RM_STATUS CliAddEvent(
    U032 hClient,
    U032 hObject,
    U032 hEvent,
    U032 hClass,
    PCLI_EVENT_INFO* ppEventInfo
)
{
    RM_STATUS rmStatus = RM_OK;
    POBJECT pObject;
    PCLI_EVENT_INFO pEvent;
    PCLI_EVENT_USER pUser;
    U032 hChannel;

    // validate parent
    if (!CliFindObject(hObject, &pObject, &hChannel))
    {
        return RM_ERR_BAD_OBJECT_PARENT;
    }

    // validate handle -- disregard existing event handles
    if (!CliIsUniqueHandle(hEvent, TRUE))
    {
        return RM_ERR_BAD_OBJECT_HANDLE;
    }

    // validate class
    switch (hClass)
    {
        case NV01_EVENT_KERNEL_CALLBACK:
        case NV01_EVENT_WIN32_EVENT:
            break;

        default:
            return RM_ERR_BAD_CLASS;

    }

    // return a pointer to the client event list element
    if (!CliGetEventInfo(hEvent, &pEvent))
    {
        // allocate a new event and add to the client
        rmStatus = osAllocMem((void **) &pEvent, sizeof(CLI_EVENT_INFO));
        if (RM_SUCCESS(rmStatus))
        {
            // link in the new event
            pEvent->Next = clientInfo[ClientInstance].CliEventList;
            pEvent->Prev = NULL;
            clientInfo[ClientInstance].CliEventList = pEvent;
            if (pEvent->Next)
            {
                pEvent->Next->Prev = pEvent;
            }

            // initialize the event
            pEvent->Handle      = hEvent;
            pEvent->Class       = hClass;
            pEvent->UserList    = NULL;
        }
    }
    else
    {
        // validate the user object's uniqueness for non-trivial lists
        for (pUser = pEvent->UserList; pUser; pUser = pUser->Next)
        {
            if (pUser->Handle == hObject)
            {
                return RM_ERR_BAD_OBJECT_HANDLE;
            }
        }
    }

    // add the next user
    if (RM_SUCCESS(rmStatus))
    {
        rmStatus = osAllocMem((void **) &pUser, sizeof(CLI_EVENT_USER));
        if (RM_SUCCESS(rmStatus))
        {
            // link in the new event's user
            pUser->Next = pEvent->UserList;
            pUser->Prev = NULL;
            pEvent->UserList = pUser;
            if (pUser->Next)
            {
                pUser->Next->Prev = pUser;
            }

            // initialize the new user
            pUser->Handle   = hObject;
            pUser->Channel  = hChannel;

            // set the return value
            *ppEventInfo = pEvent;
        }
    }

    return rmStatus;

} // end of CliAddEvent()

BOOL CliDelEvent(
    U032 hEvent
)
{
    PCLI_EVENT_INFO pEvent;
    PCLI_EVENT_USER nextUser;

    // remove the event from the client database
    if (CliGetEventInfo(hEvent, &pEvent))
    {
        // free up user list
        nextUser = pEvent->UserList;
        while (pEvent->UserList)
        {
            pEvent->UserList = nextUser->Next;
            osFreeMem(nextUser);
        }

        // fixup next link
        if (pEvent->Prev)
        {
            pEvent->Prev->Next = pEvent->Next;
        }
        else
        {
            // head of list, so modify the list head link
            clientInfo[ClientInstance].CliEventList = pEvent->Next;
        }

        // fixup the prev link
        if (pEvent->Next)
        {
            pEvent->Next->Prev = pEvent->Prev;
        }

        // free the list element
        osFreeMem(pEvent);

        return TRUE;
    }

    return FALSE;

} // end of CliDelEvent()

BOOL CliDelEventUser
(
    U032 hUser,
    U032 hClass
)
{
    PCLI_EVENT_INFO nextEvent;
    PCLI_EVENT_USER nextUser, pUser;
    U032 hEvent;

    // remove user from all events
    nextEvent = CliGetEventList();
    while (nextEvent)
    {
        nextUser = nextEvent->UserList;
        while (nextUser)
        {
            if (CliGetEventUserHandle(nextUser, hClass) == hUser)
            {
                // fixup next link
                if (nextUser->Prev)
                {
                    nextUser->Prev->Next = nextUser->Next;
                }
                else
                {
                    // head of list, so modify the list head link
                    nextEvent->UserList = nextUser->Next;
                }

                // fixup the prev link
                if (nextUser->Next)
                {
                    nextUser->Next->Prev = nextUser->Prev;
                }

                // free the list element
                pUser = nextUser;
                nextUser = nextUser->Next;
                osFreeMem(pUser);
            }
            else
            {
                nextUser = nextUser->Next;
            }
        }

        // remove the event if user list is empty
        if (nextEvent->UserList == NULL)
        {
            hEvent = nextEvent->Handle;
            nextEvent = nextEvent->Next;
            CliDelEvent(hEvent);
        }
        else
        {
            nextEvent = nextEvent->Next;
        }
    }

    return TRUE;

}

U032 CliGetEventUserHandle
(
    PCLI_EVENT_USER pUser,
    U032 hClass
)
{
    switch (hClass)
    {
        // object
        case NV01_NULL_OBJECT:
            return pUser->Handle;

        // channel
        default:
            return pUser->Channel;
    }

} // end of CliGetEventUserHandle()

BOOL CliGetEventInfo(
    U032 hEvent,
    PCLI_EVENT_INFO* ppEventInfo
)
{
    PCLI_EVENT_INFO pEvent;

    for (pEvent = CliGetEventList(); pEvent; pEvent = pEvent->Next)
    {
        if (pEvent->Handle == hEvent)
        {
            *ppEventInfo = pEvent;
            return TRUE;
        }
    }

    return FALSE;

} // end of CliGetEventInfo()

PCLI_EVENT_INFO CliGetEventList()
{
    return clientInfo[ClientInstance].CliEventList;

} // end of CliGetEventList()

BOOL CliFindObject(
    U032 hObject,
    POBJECT* ppObject,
    U032* phChannel
)
{
    BOOL found = FALSE;
    PCLI_DEVICE_INFO deviceList;
    PCLI_FIFO_INFO fifoList;
    U032 device, fifo;
    PHWINFO pDev;

    deviceList = CliGetDeviceList();
    if ( ! deviceList)
        goto done;

    // search thru all channels of all devices for the object
    for (device = 0; device < NUM_DEVICES; device++)
    {
        if (deviceList[device].InUse)
        {
            if (CliSetDeviceContext(deviceList[device].Handle, &pDev))
            {
                if (CliGetDeviceFifoList(deviceList[device].Handle, &fifoList)
                    && fifoList)
                {
                    for (fifo = 0; fifo < NUM_FIFOS; fifo++)
                    {
                        if (fifoList[fifo].InUse)
                        {
                            if (RM_OK == fifoSearchObject(pDev,
                                                          hObject,
                                                          fifoList[fifo].ChID,
                                                          ppObject))
                            {
                                found = TRUE;
                                *phChannel = fifoList[fifo].Handle;
                                goto done;
                            }
                        }
                    }
                }
            }
        }
    }

 done:
    return found;

} // end of CliFindObject()

BOOL CliGetClass(
    U032 hParent,
    U032 hObject,
    U032 *phClass
)
{
    U032 client;
    PCLI_DEVICE_INFO pDevice;
    PCLI_FIFO_INFO pFifo;
    PCLI_MEMORY_INFO pMemory;
    PCLI_DMA_INFO pDma;
    PCLI_EVENT_INFO pEvent;

    // scan all object types

    if (CliGetClientHandle(&client) && client == hObject)
    {
        *phClass = clientInfo[ClientInstance].Class;
    }
    else if (CliGetDeviceInfo(hObject, &pDevice))
    {
        *phClass = pDevice->Class;
    }
    else if (CliGetDeviceFifoInfo(hParent, hObject, &pFifo))
    {
        *phClass = pFifo->Class;
    }
    else if (CliGetDeviceMemoryInfo(hParent, hObject, &pMemory))
    {
        *phClass = pMemory->Class;
    }
    else if (CliGetDmaInfo(hObject, &pDma))
    {
        *phClass = pDma->Class;
    }
    else if (CliGetMemoryInfo(hObject, &pMemory))
    {
        *phClass = pMemory->Class;
    }
    else if (CliGetEventInfo(hObject, &pEvent))
    {
        *phClass = pEvent->Class;
    }
    else
    {
        return FALSE;
    }

    return TRUE;

} // end of CliGetClass()

BOOL CliIsUniqueHandle(U032 hObject, BOOL omitEvents)
{
    U032 i, j;
    PCLI_DEVICE_INFO deviceList;
    PCLI_FIFO_INFO fifoList;
    PCLI_DMA_INFO nextDma;
    PCLI_MEMORY_INFO nextMemory;
    PCLI_EVENT_INFO nextEvent;
    POBJECT pObject;
    U032 hChannel;

    // handle is unique if it is non-null or does not exist in the client
    if (hObject == NV01_NULL_OBJECT)
        return FALSE;

    if ((deviceList = CliGetDeviceList()) != NULL)
    {
        for (i = 0; i < NUM_DEVICES; i++)
        {
            if (deviceList[i].InUse)
            {
                if (deviceList[i].Handle == hObject)
                    return FALSE;

                if (CliGetDeviceFifoList(deviceList[i].Handle, &fifoList) && fifoList)
                {
                    for (j = 0; j < MAX_FIFOS; j++)
                    {
                        if (fifoList[j].InUse)
                        {
                            if (fifoList[j].Handle == hObject)
                                return FALSE;
                        }
                    }
                }
            }
        }
    }

    for (nextDma = CliGetDmaList(); nextDma; nextDma = nextDma->Next)
    {
        if (nextDma->Handle == hObject)
        {
            return FALSE;
        }
    }
    for (nextMemory = CliGetMemoryList(); nextMemory; nextMemory = nextMemory->Next)
    {
        if (nextMemory->Handle == hObject)
        {
            return FALSE;
        }
    }
    for (nextEvent = CliGetEventList(); nextEvent; nextEvent = nextEvent->Next)
    {
        if (nextEvent->Handle == hObject)
        {
            return FALSE;
        }
    }
    if (CliFindObject(hObject, &pObject, &hChannel))
    {
        return FALSE;
    }

    return TRUE;

} // end of CliIsUniqueHandle()

BOOL CliGetPhysicalDevice(
    U032 logicalDevice,
    U032 *physicalDevice
)
{
    switch (logicalDevice)
    {
        case NV01_DEVICE_0:
            *physicalDevice = 0;
            break;

        case NV01_DEVICE_1:
            *physicalDevice = 1;
            break;

        case NV01_DEVICE_2:
            *physicalDevice = 2;
            break;

        case NV01_DEVICE_3:
            *physicalDevice = 3;
            break;

        case NV01_DEVICE_4:
            *physicalDevice = 4;
            break;

        case NV01_DEVICE_5:
            *physicalDevice = 5;
            break;

        case NV01_DEVICE_6:
            *physicalDevice = 6;
            break;

        case NV01_DEVICE_7:
            *physicalDevice = 7;
            break;

        default:
            return FALSE;
    }

    return TRUE;

} // end of CliGetPhysicalDevice()

// Given a Channel ID return the Client handle
//
// Currently, this is routine is only called at intr time from
// dmaValidateObjectName, so dmaFindContext has the client handle
// to retrieve the correct DmaObject from the global DmaList. The
// DeviceInstance has already been set.
//
RM_STATUS CliGetClientHandleFromChID(
    PHWINFO pDev,
    U032 ChID,
    U032 *phClient
)
{
    U032 i, j, k, physnum;

    PCLI_FIFO_INFO fifoList;

    *phClient = 0;

    for (i = 0; i < NUM_CLIENTS; i++)
    {
        if (!clientInfo[i].InUse)
            continue;

        if (!clientInfo[i].CliDeviceList)
            continue;

        // scan the client's devices
        for (j = 0; j < NUM_DEVICES; j++)
        {
            if (!clientInfo[i].CliDeviceList[j].InUse)
                continue;

            // look for the DeviceInstance in DeviceList
            CliGetPhysicalDevice(clientInfo[i].CliDeviceList[j].Class, &physnum);
            if (physnum != devInstance)
                continue;

            // see if any "inuse" fifos match this channel ID
            fifoList = clientInfo[i].CliDeviceList[j].DevFifoList;
            if ( ! fifoList)
                continue;

            for (k = 0; k < MAX_FIFOS; k++)
            {
                if (fifoList[k].InUse && (fifoList[k].ChID == ChID))
                {
                    // find the one corresponding to largest client id.
                    // this avoids returning a stale client id in case
                    // of a previous client (using this device/channel)
                    // not being properly freed up.
                    if (clientInfo[i].Handle > *phClient)
                        *phClient = clientInfo[i].Handle;
                }
            }
        }
    }

    if (*phClient != 0)
        return RM_OK;

    return RM_ERR_BAD_OBJECT_ERROR;

} // end of CliGetClientHandleFromChID()

//
// Initialize the client database.
//
RM_STATUS initClientInfo(
    VOID
)
{
    RM_STATUS rmStatus;
    U032 i;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: initClientInfo\n");

    // allocate table
    rmStatus = osAllocMem((VOID **)&clientInfo, sizeof(CLIENTINFO) * NUM_CLIENTS);
    if (!RM_SUCCESS(rmStatus))
        return rmStatus;

    // initialize the client database
    for (i = 0; i < NUM_CLIENTS; i++)
    {
        clientInfo[i].InUse = FALSE;
        clientInfo[i].AppID = i;
        clientInfo[i].AppHandle = (void *) 0;
        clientInfo[i].CliDeviceList = NULL;
        clientInfo[i].CliHeapOwnerList = NULL;         //added by Chuck Moidel
    }

    return RM_OK;
}

//
// Tear down a clientInfo by free'ing all the memory allocated to it.
//
RM_STATUS destroyClientInfo(
    VOID
)
{
    U032 i, d;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: destroyClientInfo\n");

    if ( ! clientInfo)
        return RM_OK;

    for (i = 0; i < NUM_CLIENTS; i++)
    {
        if ( ! clientInfo[i].CliDeviceList)
            continue;

        for (d = 0; d < NUM_DEVICES; d++)
        {
            if ( ! clientInfo[i].CliDeviceList[d].DevFifoList)
                continue;

            osFreeMem(clientInfo[i].CliDeviceList[d].DevFifoList);
            clientInfo[i].CliDeviceList[d].DevFifoList = NULL;
        }
        osFreeMem(clientInfo[i].CliDeviceList);
        clientInfo[i].CliDeviceList = NULL;
    }

    osFreeMem(clientInfo);
    clientInfo = NULL;

    return RM_OK;
}

// end of client.c

