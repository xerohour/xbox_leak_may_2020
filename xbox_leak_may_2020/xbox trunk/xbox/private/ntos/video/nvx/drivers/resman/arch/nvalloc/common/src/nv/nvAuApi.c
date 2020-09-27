/******************************************************************************
*
*   Module: nvauapi.c
*
*   Description:
*       This module is the main entry module into the NV Architecture Audio Driver.
*
*
*   THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
*   NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
*   IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
*
******************************************************************************/
#include <nvarm.h>
#include <nv32.h>
#include <nvos.h>
// #include <nvarch.h>

#include <aos.h>
#include <AHal.h>

PHWINFO_COMMON_FIELDS	pNvHWInfo[MAX_AUDIO_INSTANCE]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

// device instance utilities
PHWINFO_COMMON_FIELDS
RmGetAudioDeviceInstance(U032 uRef)
{
    U032 uCount;
    AUDIO_REF aRef;
    
    aRef.uValue = uRef;
    
    for (uCount = 0; uCount < MAX_AUDIO_INSTANCE; uCount++)
    {
        if ((pNvHWInfo[uCount]) && 
            (pNvHWInfo[uCount]->uDevType == aRef.field.type) &&
            (pNvHWInfo[uCount]->uRevisionID == aRef.field.revID))
        {
            // found a device
            return pNvHWInfo[uCount];
        }
    }

    aosDbgPrintString(DEBUGLEVEL_WARNINGS, "NVARM: RmGetAudioDeviceInstance - instance not found\n");
    return NULL;
}


PHWINFO_COMMON_FIELDS
RmAllocAudioDeviceInstance(U032 uType, U008 uRevId)
{
    AUDIO_REF aRef;
    PHWINFO_COMMON_FIELDS pDev;
    
    aRef.field.type = uType;
    aRef.field.revID = uRevId;
    // client id-- don't care
    
    pDev = RmGetAudioDeviceInstance(aRef.uValue);
    
    if (!pDev)
    {
        U032 uCount;
        RM_STATUS rmStatus;
        
        for (uCount = 0; uCount < MAX_AUDIO_INSTANCE; uCount++)
        {
            // find a free index
            if (pNvHWInfo[uCount] == NULL)
                break;
        }
        
        if (uCount == MAX_AUDIO_INSTANCE)
        {
            aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: RmAllocAudioDeviceInstance - MAX_INSTANCE exceeded\n");
            return NULL;
        }
        
        // allocate a new one
        rmStatus = aosAllocateMem(sizeof(HWINFO_COMMON_FIELDS), 
                                    ALIGN_DONT_CARE,
                                    (VOID **)&pNvHWInfo[uCount], 
                                    NULL);
        
        if (rmStatus != RM_OK)
        {
            aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: RmAllocAudioDeviceInstance - mem alloc fails\n");
            return NULL;
        }
        
        pDev = pNvHWInfo[uCount];
        
        // fill the structure up
        pDev->uDevType = uType;
        pDev->uRevisionID = uRevId;
        pDev->uInstanceCounter = 0;
    }
    
    // increment the instance counter
    pDev->uInstanceCounter++;

    return pDev;
}

VOID
RmFreeAudioDeviceInstance(PHWINFO_COMMON_FIELDS pDev)
{
    // look for the pDev in the array
    U032 uCount;
    
    // check the ref count..
    if (pDev->uInstanceCounter > 1)
    {
        pDev->uInstanceCounter--;
        return;
    }
    
    for (uCount = 0; uCount < MAX_AUDIO_INSTANCE; uCount++)
    {
        if (pNvHWInfo[uCount] == pDev)
            break;
    }
    
    if (uCount == MAX_AUDIO_INSTANCE)
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: RmFreeAudioDeviceInstance - instance could not be located\n");
        return;
    }
    
    pNvHWInfo[uCount] = NULL;
    aosFreeMem(pDev);
}

RM_STATUS 
RmAllocAudioMemory(VOID *pParam)
{
    AUDIO_ALLOC_MEMORY	*pAllocParam = (AUDIO_ALLOC_MEMORY *) pParam;
    
    return aosAllocateMem(pAllocParam->uSize,
                        pAllocParam->uAlignMask,
                        (VOID **)&(pAllocParam->pLinearAddress),
                        NULL);
}

RM_STATUS 
RmFreeAudioMemory(VOID *pParam)
{
    AUDIO_FREE_MEMORY *pFreeParam = (AUDIO_FREE_MEMORY *) pParam;
    
    aosFreeMem((VOID *)pFreeParam->pLinearAddress);
    
    return RM_OK;
}

// utility stuff
NvV32 
RmArchStatus(RM_STATUS rmStatus, U032 uArchFunc)
{
    // todo
    return (NvV32) rmStatus;
}

// exported to the calls
NvV32 
NvAuAlloc(U032 hClass, VOID* pAllocParms)
{
    PHWINFO_COMMON_FIELDS   pDev;
    RM_STATUS               rmStatus = RM_ERROR;
    
    switch(hClass)
    {
    case NV01_MEMORY_AUDIO:
        aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: NvAuAlloc - NV01_MEMORY_AUDIO ... start\n");
        rmStatus = RmAllocAudioMemory(pAllocParms);
        break;
        
    case NV01_DEVICE_AUDIO:
        {
            AUDIO_INIT_DEVICE *pParam = (AUDIO_INIT_DEVICE *)pAllocParms;
            U008 uClientID = 0;
            AUDIO_REF aRef;
            MCP1_CLIENT_INFO clientInfo;
            
            aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: NvAuAlloc - NV01_DEVICE_AUDIO ... start\n");

            if (!pParam)
            {
                aosDbgPrintString(DEBUGLEVEL_WARNINGS, "NVARM: NvAuAlloc - bad param list\n");
                rmStatus = RM_ERR_BAD_OBJECT_BUFFER;
                break;
            }
            
            // allocate a device reference
            // note (WDM specific) - NVMM is not initialized at this point
            // but mem allocation is fine, since it does not ask for physically
            // contiguous memory
            pDev = RmAllocAudioDeviceInstance(hClass, (U008) pParam->uRevisionID);
            
            while(pDev)
            {
                // if it's the first instance then init the device etc
                if (pDev->uInstanceCounter == 1)
                {
                    rmStatus = RmInitAudioDevice((VOID *)pParam->pResList,
                                                (VOID *)pParam->pDevObj,
                                                pDev);
                    
                    if (rmStatus == RM_OK)
                    {
                        rmStatus = InitAudioHal(pParam->uRevisionID, pDev);

                        if (rmStatus == RM_OK)
                            rmStatus = AllocDevice(pDev);
                    }
                }

                break;
            };
            
            if (rmStatus != RM_OK)
            {
                aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: NvAuAlloc - NV01_DEVICE_AUDIO - 1 of the calls failed\n");
                break; 
            }
            
            clientInfo.pFn = pParam->pISRFn;
            clientInfo.Mask.uValue = pParam->IntrMask.uValue;
            clientInfo.pContext = (VOID *)pParam->pServiceContext;
            
            // everything done.. now add the client
            rmStatus = DeviceAddClient(pDev, &clientInfo, &uClientID);
            
            // form a context to be returned
            aRef.field.type = hClass;
            aRef.field.revID = pParam->uRevisionID;
            aRef.field.client = uClientID;
            
            pParam->uDeviceRef = aRef.uValue;
            break;
        }
        
    case NV01_CONTEXT_DMA_AUDIO:
        {
            // get the pDev
            APU_AP_ALLOC_CONTEXT_DMA *pParams = (APU_AP_ALLOC_CONTEXT_DMA *)pAllocParms;

            aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: NvAuAlloc - NV01_CONTEXT_DMA_AUDIO ... start\n");

            pDev = RmGetAudioDeviceInstance(pParams->uDeviceRef);
            if (!pDev)
                rmStatus = RM_ERR_BAD_OBJECT_HANDLE;
            else
                rmStatus = AllocContextDma((PHWINFO_COMMON_FIELDS)pDev, pAllocParms);
            
            break;
        }
        
        // audio object calls
    case NV1B_AUDIO_OBJECT:
        {
            APU_OBJECT *pParams = (APU_OBJECT *)pAllocParms;

            aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: NvAuAlloc - NV1B_AUDIO_OBJECT ... start\n");

            pDev = RmGetAudioDeviceInstance(pParams->uDeviceRef);
            if (!pDev)
                rmStatus = RM_ERR_BAD_OBJECT_HANDLE;
            else
                rmStatus = AllocObject((PHWINFO_COMMON_FIELDS)pDev, pAllocParms);
            
            break;
        }
        
    default:
        aosDbgPrintString(DEBUGLEVEL_WARNINGS, "NVARM: NvAuAlloc - bad handle\n");
        rmStatus = RM_ERR_BAD_CLASS;
        break;
    }
    
    aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: NvAuAlloc ... end\n");
    return RmArchStatus(rmStatus, NV01_ALLOC_DEVICE);
}


NvV32
NvAuFree
(
	U032 hClient,
    U032 hClass,
    U032 uDeviceRef,
    VOID *pParam
)
{
    RM_STATUS rmStatus;

    // find the pDev
    PHWINFO_COMMON_FIELDS pDev = RmGetAudioDeviceInstance(uDeviceRef);

    if (!pDev)
        rmStatus = RM_ERR_BAD_OBJECT_HANDLE;
    else
    {
        switch(hClass)
        {
        case NV01_DEVICE_AUDIO:
        case NV01_DEVICE_EXTERNAL_CODEC:
            {

                // remove client
                AUDIO_REF aRef;
                aRef.uValue = uDeviceRef;

                aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: NvAuFree ... NV01_DEVICE start\n");
                
                DeviceRemoveClient(pDev, (U008) aRef.field.client);
                
                // check if this is the last instance
                if (pDev->uInstanceCounter == 1)
                {
                    aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: NvAuFree ... NV01_DEVICE freeing device object...\n");
                    rmStatus = FreeDevice(pDev);
                    
                    aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: NvAuFree ... NV01_DEVICE freeing HAL...\n");
                    DestroyAudioHal(pDev);
                    
                    aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: NvAuFree ... NV01_DEVICE shutting down device...\n");
                    RmShutdownAudioDevice(pDev);
                    
                    aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: NvAuFree ... NV01_DEVICE removing instance...\n");
                    RmFreeAudioDeviceInstance(pDev);
                }
            }
            break;
            
        case NV01_CONTEXT_DMA_AUDIO:
        case NV01_CONTEXT_DMA_EXTERNAL_CODEC:
            aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: NvAuFree ...  NV01_CONTEXT_DMA start\n");
            rmStatus = FreeContextDma(pDev, pParam);
            break;

        case NV01_EXTERNAL_CODEC_OBJECT:
        case NV1B_AUDIO_OBJECT:
            aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: NvAuFree ... one of the audio objects..  start\n");
            rmStatus = FreeObject(pDev, pParam);
            break;
            
        case NV01_MEMORY_AUDIO:
            aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: NvAuFree ... NV01_MEMORY_AUDIO start\n");
            rmStatus = RmFreeAudioMemory(pParam);
            break;
            
        default:
            aosDbgPrintString(DEBUGLEVEL_WARNINGS, "NVARM: NvAuFree ... Bad class handle\n");
            rmStatus = RM_ERR_BAD_OBJECT_PARENT;
            break;
        }
    }
    
    aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: NvAuFree ... end\n");

    return RmArchStatus(rmStatus, NV01_FREE);
}
