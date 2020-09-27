/******************************************************************************
*
*	Module: nvapi.c
*
*   Description:
*       This module is the main entry module into the NV Architecture Driver.
*
*
*   THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
*   NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
*   IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
*
******************************************************************************/

#include <nvrm.h>
#include <nv32.h>
#include <nv_ref.h>
#include <nvhw.h>
#include <rmfail.h>

#include <state.h>
#include <class.h>
#include <vblank.h>
#include <dma.h>
#include <event.h>
#include <fifo.h>
#include <fb.h>
#include <gr.h>
#include <heap.h>
#include <tmr.h>
#include <mc.h>
#include <client.h>

#include <os.h>
#include <nvos.h>
#include <nvarch.h>
#include <smp.h>

PCLIENTINFO clientInfo;
U032        ClientInstance;
#ifdef DEBUG
PCLIENTINFO pClient;
U032 clientSize = sizeof(CLIENTINFO);
#endif

// NV4+ passes around an element of this table instead of using a global index
PHWINFO     NvDBPtr_Table[MAX_INSTANCE]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

RMINFO      NvRmInfo;

// globals used by the OS specific calls
U032        nvInfoSelector      = 0;
U032        clientInfoSelector  = 0;
U032        rmInService;
U032        osInService;
U032        rmInSafeService;
U032        rmInCallback;

// NV RM API functions

/*
NV01_ALLOC_ROOT
    NVOS01_PARAMETERS:
        NvV32 hObjectNew
        NvV32 hClass
        NvV32 status
*/
VOID Nv01AllocRoot(
    NVOS01_PARAMETERS *pArgs
)
{
    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO, "NVRM: Allocating root...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:    class:", ARG(hClass));

    ARG(status) = RmAllocClient(
        &ARG(hObjectNew),
        ARG(hClass)
    );
    if (ARG(status) == NVOS01_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:   client:", ARG(hObjectNew));
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: ...root object allocation complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: ...root object allocation *FAILED*\n");
    }

} // end of Nv01AllocRoot()

/*
NV01_ALLOC_DEVICE
    NVOS06_PARAMETERS:
        NvV32 hObjectParent
        NvV32 hObjectNew
        NvV32 hClass
        NvP64 szName
        NvV32 status
*/
VOID Nv01AllocDevice(
    NVOS06_PARAMETERS *pArgs
)
{
    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO, "NVRM: Allocating device...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:   client:", ARG(hObjectParent));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:   device:", ARG(hObjectNew));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:    class:", ARG(hClass));

    ARG(status) = RmAllocDevice(
        ARG(hObjectParent),
        ARG(hObjectNew),
        ARG(hClass),
#if _WIN32_WINNT >= 0x0400
        (char*)ARG_P064(szName)
#else
        (char*)ARG(szName)
#endif
    );
    if (ARG(status) == NVOS06_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: ...device allocation complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: ...device allocation *FAILED*\n");
    }

} // end of Nv01AllocDevice()

/*
NV01_ALLOC_CONTEXT_DMA
    NVOS03_PARAMETERS:
        NvV32 hObjectParent
        NvV32 hObjectNew
        NvV32 hClass
        NvV32 flags
        NvP64 pBase
        NvU64 limit
        NvV32 status
*/
VOID Nv01AllocContextDma(
    NVOS03_PARAMETERS *pArgs
)
{
    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO, "NVRM: Allocating DMA context...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:    client:", ARG(hObjectParent));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:   context:", ARG(hObjectNew));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:     class:", ARG(hClass));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:     flags:", ARG(flags));
    DBG_PRINT_STRING_PTR  (DEBUGLEVEL_TRACEINFO, "NVRM:     pBase:", (VOID*)ARG_P064(pBase));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:     limit:", (U032)ARG_U064(limit));

    ARG(status) = RmAllocContextDma(
        ARG(hObjectParent),
        ARG(hObjectNew),
        ARG(hClass),
        ARG(flags),
        ARG_SELECTOR(pBase),
        (VOID *)ARG_P064(pBase),
        (U032)ARG_U064(limit)
    );
    if (ARG(status) == NVOS03_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: ...DMA context allocation complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: ...DMA context allocation *FAILED*\n");
    }

} // end of Nv01AllocContextDma()

/*
NV01_ALLOC_CHANNEL_PIO
    NVOS04_PARAMETERS:
        NvV32 hRoot;
        NvV32 hObjectParent
        NvV32 hObjectNew
        NvV32 hClass
        NvV32 hObjectError
        NvP64 pChannel
        NvV32 flags
        NvV32 status
*/
VOID Nv01AllocChannelPio(
    NVOS04_PARAMETERS *pArgs
)
{
    VOID *channel;

    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM: Allocating PIO channel...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    client:", ARG(hRoot));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    device:", ARG(hObjectParent));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   channel:", ARG(hObjectNew));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     class:", ARG(hClass));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     error:", ARG(hObjectError));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     flage:", ARG(flags));

    // allocate the PIO channel
    ARG(status) = RmAllocChannelPio(
        ARG(hRoot),
        ARG(hObjectParent),
        ARG(hObjectNew),
        ARG(hClass),
        ARG(hObjectError),
        &channel,
        ARG(flags)
    );
    ARG_P064(pChannel) = (NvP64_VALUE_T)channel;
    if (ARG(status) == NVOS04_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...PIO channel allocation complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...PIO channel allocation *FAILED*\n");
    }

} // end of Nv01AllocChannelPio()

/*
NV03_ALLOC_CHANNEL_DMA
    NVOS07_PARAMETERS:
        NvV32 hRoot;
        NvV32 hObjectParent
        NvV32 hObjectNew
        NvV32 hClass
        NvV32 hObjectError
        NvV32 hObjectBuffer
        NvU32 offset
        NvP64 pControl
        NvV32 status
*/
VOID Nv03AllocChannelDma(
    NVOS07_PARAMETERS *pArgs
)
{
    VOID *control;

    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM: Allocating DMA channel...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:      client:", ARG(hRoot));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:      device:", ARG(hObjectParent));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     channel:", ARG(hObjectNew));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:       class:", ARG(hClass));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   error ctx:", ARG(hObjectError));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    data ctx:", ARG(hObjectBuffer));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:      offset:", ARG(offset));

    // allocate DMA channel
    ARG(status) = RmAllocChannelDma(
        ARG(hRoot),
        ARG(hObjectParent),
        ARG(hObjectNew),
        ARG(hClass),
        ARG(hObjectError),
        ARG(hObjectBuffer),
        ARG(offset),
        &control
    );
    ARG_P064(pControl) = (NvP64_VALUE_T)control;
    if (ARG(status) == NVOS07_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...DMA channel allocation complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...DMA channel allocation *FAILED*\n");
    }

} // end of Nv03AllocChannelDma()

/*
NV01_ALLOC_MEMORY
        NVOS02_PARAMETERS:
        NvV32 hRoot;       
        NvV32 hObjectParent;
        NvV32 hObjectNew;
        NvV32 hClass;
        NvV32 flags;
        NvP64 pMemory;
        NvU64 pLimit;
        NvV32 status;
*/
VOID Nv01AllocMemory(
    NVOS02_PARAMETERS *pArgs
)
{
    VOID *memory;
    U032 limit;

    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM: Allocating memory...\n");
    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM: Allocating memory...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    client:", ARG(hRoot));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    parent:", ARG(hObjectParent));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    memory:", ARG(hObjectNew));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     class:", ARG(hClass));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     flags:", ARG(flags));
    DBG_PRINT_STRING_PTR  (DEBUGLEVEL_TRACEINFO,  "NVRM:   pMemory:", (VOID*)ARG_P064(pMemory));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     limit:", (U032)ARG_U064(pLimit));

    // allocate memory

    memory = (VOID *)ARG_P064(pMemory);
    limit = (U032)ARG_U064(pLimit);

    ARG(status) = RmAllocMemory(
        ARG(hRoot),
        ARG(hObjectParent),
        ARG(hObjectNew),
        ARG(hClass),
        ARG(flags),
        &memory,
        &limit
    );

    ARG_P064(pMemory) = (NvP64_VALUE_T)memory;
    ARG_U064(pLimit) = (NvU64_VALUE_T)limit;

    if (ARG(status) == NVOS02_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...memory allocation complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...memory allocation *FAILED*\n");
    }

} // end of Nv01AllocMemory()

/*
NV01_ALLOC_DEVICE
    NVOS05_PARAMETERS
        NvV32 hRoot;
        NvV32 hObjectParent;
        NvV32 hObjectNew;
        NvV32 hClass;
        NvV32 status;
*/
VOID Nv01AllocObject(
    NVOS05_PARAMETERS *pArgs
)
{
    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM: Allocating object...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    client:", ARG(hRoot));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   channel:", ARG(hObjectParent));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    object:", ARG(hObjectNew));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     class:", ARG(hClass));

    ARG(status) = RmAllocObject(
        ARG(hRoot),
        ARG(hObjectParent),
        ARG(hObjectNew),
        ARG(hClass)
    );
    if (ARG(status) == NVOS05_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...object allocation complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...object allocation *FAILED*\n");
    }

} // end of Nv01AllocObject()

/*
NV04_ALLOC
    NVOS21_PARAMETERS;
        NvV32 hRoot;
        NvV32 hObjectParent;
        NvV32 hObjectNew;
        NvV32 hClass;
        NvP64 pAllocParms;
        NvV32 status;
*/
VOID Nv04Alloc(
    NVOS21_PARAMETERS *pArgs
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: Allocating object...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    client:", ARG(hRoot));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   channel:", ARG(hObjectParent));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    object:", ARG(hObjectNew));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     class:", ARG(hClass));

    ARG(status) = RmAlloc(
        ARG(hRoot),
        ARG(hObjectParent),
        ARG(hObjectNew),
        ARG(hClass),
        (VOID *) ARG_P064(pAllocParms)
    );
    if (ARG(status) == NVOS21_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...allocation complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...allocation *FAILED*\n");
    }

} // end of Nv04Alloc()

/*
NV01_ALLOC_EVENT
    NVOS10_PARAMETERS:
        NvV32 hRoot;
        NvV32 hObjectParent;
        NvV32 hObjectNew;
        NvV32 hClass;
        NvV32 index;
        NvU64 hEvent;
        NvV32 status;
*/
VOID Nv01AllocEvent(
    NVOS10_PARAMETERS *pArgs
)
{
    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM: Allocating Event...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Client:", ARG(hRoot));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Parent:", ARG(hObjectParent));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Object:", ARG(hObjectNew));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Class:",  ARG(hClass));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Index:",  ARG(index));
    DBG_PRINT_STRING_VAL64(DEBUGLEVEL_TRACEINFO,  "NVRM:   Event:",  ARG(hEvent));

    ARG(status) = RmAllocEvent(
        ARG(hRoot),
        ARG(hObjectParent),
        ARG(hObjectNew),
        ARG(hClass),
        ARG(index),
        ARG(hEvent)
    );
    if (ARG(status) == NVOS10_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...event allocation complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...event allocation *FAILED*\n");
    }

} // end of Nv01AllocEvent()

/*
NV01_FREE
    NVOS00_PARAMETERS:
        NvV32 hRoot;
        NvV32 hObjectParent;
        NvV32 hObjectOld;
        NvV32 status;
*/
VOID Nv01Free(
    NVOS00_PARAMETERS *pArgs
)
{
    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM: Freeing object...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   client:", ARG(hRoot));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   parent:", ARG(hObjectParent));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   object:", ARG(hObjectOld));

    ARG(status) = RmFree(
        ARG(hRoot),
        ARG(hObjectParent),
        ARG(hObjectOld)
    );
    if (ARG(status) == NVOS00_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...object free complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...object free *FAILED*\n");
    }

} // end of Nv01Free()

/*
NV03_DMA_FLOW_CONTROL
    NVOS08_PARAMETERS:
        NvV32 hChannel
        NvV32 flags
        NvU32 put
        NvU32 get
        NvV32 status
*/
VOID Nv03DmaFlowControl(
    NVOS08_PARAMETERS *pArgs
)
{
    // This function is not valid on NV4 and later devices.
    ARG(status) = NVOS08_STATUS_ERROR_BAD_OBJECT_PARENT;
    if (ARG(status) == NVOS08_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...DMA channel flow control complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...DMA channel flow control *FAILED*\n");
    }
} // end of Nv3DmaFlowControl()

/*
NV01_ARCH_HEAP
    NVOS11_PARAMETERS;
        NvV32 hRoot;
        NvV32 hObjectParent;
        NvU32 function;
        NvU32 owner;
        NvU32 type;
        NvU32 depth;
        NvU32 width;
        NvU32 height;
        NvS32 pitch;
        NvU32 offset;
        NvU32 size;
        NvP64 address;
        NvU64 limit;
        NvU32 total;
        NvU32 free;
        NvV32 status;
*/
VOID Nv03ArchHeap(
    NVOS11_PARAMETERS *pArgs
)
{
    VOID *address;
#if DBG
    // allow for the disabling of the enormous amount of spew from heap manipulations
    U032 saveDebuglevel = cur_debuglevel;
    //cur_debuglevel = DEBUGLEVEL_ERRORS;
#endif
    
	DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM: Arch Heap...\n");
	DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Client:", ARG(hRoot));
	DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Device:", ARG(hObjectParent));

    ARG(status) = RmArchHeap(
        ARG(hRoot),
        ARG(hObjectParent),
        ARG(function),
        ARG(owner),
        ARG(type),
        ARG(depth),
        ARG(width),
        ARG(height),
        &ARG(pitch),
        &ARG(offset),
        &ARG(size),
        &address,
        &ARG(limit),
        &ARG(total),
        &ARG(free)
    );
#if _WIN32_WINNT >= 0x0400
    ARG_P064(address) = (NvP64_VALUE_T)address;
#else
    ARG(address) = (NvU32)address;
#endif
    if (ARG(status) == NVOS11_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...Arch Heap complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...Arch Heap *FAILED*\n");
    }

#if DBG
     cur_debuglevel = saveDebuglevel;
#endif

} // end of Nv03ArchHeap()

/*
NV01_CONFIG_VERSION:
    NVOS12_PARAMETERS:
        NvV32 hClient;
        NvV32 hDevice;
        NvV32 version;
        NvV32 status;
*/
VOID Nv01ConfigVersion(
    NVOS12_PARAMETERS *pArgs
)
{
    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM: Config Version...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Client:", ARG(hClient));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Device:", ARG(hDevice));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Device:", ARG(version));

    ARG(status) = RmConfigVersion(
        ARG(hClient),
        ARG(hDevice),
        &ARG(version)
    );
    if (ARG(status) == NVOS12_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...Config Version complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...Config Version *FAILED*\n");
    }

} // end of Nv01ConfigVersion()

/*
NV01_CONFIG_GET:
    NVOS13_PARAMETERS:
      NvV32 hClient;
      NvV32 hDevice;
      NvV32 index;
      NvV32 value;
      NvV32 status;
*/
VOID Nv01ConfigGet(
    NVOS13_PARAMETERS *pArgs
)
{
    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM: Config Get...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Client:", ARG(hClient));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Device:", ARG(hDevice));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Index:", ARG(index));

    // get a specific configuration value
    ARG(status) = RmConfigGet(
        ARG(hClient),
        ARG(hDevice),
        ARG(index),
        &ARG(value)
    );
    if (ARG(status) == NVOS13_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...Config Get complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...Config Get *FAILED*\n");
    }

} // end of Nv01ConfigGet()

/*
NV01_CONFIG_SET:
    NVOS14_PARAMETERS:
      NvV32 hClient;
      NvV32 hDevice;
      NvV32 index;
      NvV32 oldValue;
      NvV32 newValue;
      NvV32 status;
*/
VOID Nv01ConfigSet(
    NVOS14_PARAMETERS *pArgs
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: Setting Configuration...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Client:", ARG(hClient));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Device:", ARG(hDevice));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     Index:", ARG(index));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     Value:", ARG(newValue));

    // set a specific configuration value
    ARG(status) = RmConfigSet(
        ARG(hClient),
        ARG(hDevice),
        ARG(index),
        &ARG(oldValue),
        ARG(newValue)
    );
    if (ARG(status) == NVOS14_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...Config Set complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...Config Set *FAILED*\n");
    }

} // end of Nv01ConfigSet()

/*
NV01_CONFIG_UPDATE:
    NVOS15_PARAMETERS:
        NvV32 hClient;
        NvV32 hDevice;
        NvV32 flags;
        NvV32 status;
*/
VOID Nv01ConfigUpdate(
    NVOS15_PARAMETERS *pArgs
)
{
    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM: Config Update...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Client:", ARG(hClient));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Device:", ARG(hDevice));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Flags:", ARG(flags));

    ARG(status) = RmConfigUpdate(
        ARG(hClient),
        ARG(hDevice),
        ARG(flags)
    );
    if (ARG(status) == NVOS15_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...Config Update complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...Config Update *FAILED*\n");
    }

} // end of Nv01ConfigUpdate()

/*
NV04_CONFIG_GET_EX:
    NVOS_CONFIG_GET_EX_PARAMS:
        NvV32 hClient;
        NvV32 hDevice;
        NvV32 index;
        NvP64 paramStructPtr;
        NvU32 paramSize;
        NvV32 status;
*/
VOID Nv04ConfigGetEx(
    NVOS_CONFIG_GET_EX_PARAMS *pArgs
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: config getEx...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Client:", ARG(hClient));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Device:", ARG(hDevice));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     Index:", ARG(index));
#if _WIN32_WINNT >= 0x0400
    DBG_PRINT_STRING_PTR  (DEBUGLEVEL_TRACEINFO,  "NVRM:  Parm Ptr:", (VOID*)ARG_P064(paramStructPtr));
#else
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:  Parm Ptr:", ARG(paramStructPtr));
#endif
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM: Parm size:", ARG(paramSize));

    ARG(status) = RmConfigGetEx(
        ARG(hClient),
        ARG(hDevice),
        ARG(index),
#if _WIN32_WINNT >= 0x0400
        (VOID*)ARG_P064(paramStructPtr),
#else
        (VOID*)ARG(paramStructPtr),
#endif
        ARG(paramSize)
    );
    if (ARG(status) == NVOS_CGE_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...config getEx complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...config getEx *FAILED*\n");
    }

} // end of Nv04ConfigGetEx()

/*
NV04_CONFIG_SET_EX:
    NVOS_CONFIG_SET_EX_PARAMS:
        NvV32 hClient;
        NvV32 hDevice;
        NvV32 index;
        NvP64 paramStructPtr;
        NvU32 paramSize;
        NvV32 status;
*/
VOID Nv04ConfigSetEx(
    NVOS_CONFIG_SET_EX_PARAMS *pArgs
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: config SetEx...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Client:", ARG(hClient));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Device:", ARG(hDevice));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     Index:", ARG(index));
#if _WIN32_WINNT >= 0x0400
    DBG_PRINT_STRING_PTR  (DEBUGLEVEL_TRACEINFO,  "NVRM:  Parm Ptr:", (VOID*)ARG_P064(paramStructPtr));
#else
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:  Parm Ptr:", ARG(paramStructPtr));
#endif
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM: Parm size:", ARG(paramSize));

    ARG(status) = RmConfigSetEx(
        ARG(hClient),
        ARG(hDevice),
        ARG(index),
#if _WIN32_WINNT >= 0x0400
        (VOID*)ARG_P064(paramStructPtr),
#else
        (VOID*)ARG(paramStructPtr),
#endif
        ARG(paramSize)
    );
    if (ARG(status) == NVOS_CSE_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...config SetEx complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...config SetEx *FAILED*\n");
    }

} // end of Nv04ConfigSetEx()

/*
NV01_INTERRUPT:
    NVOS09_PARAMETERS:
        NvV32 hClient;
        NvV32 hDevice;
        NvV32 status;
*/

VOID Nv01Interrupt(
    NVOS09_PARAMETERS *pArgs
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: interrupt...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Client:", ARG(hClient));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Device:", ARG(hDevice));


    // service an interrupt
    ARG(status) = RmInterrupt(
        ARG(hClient),
        ARG(hDevice)
    );
    if (ARG(status) == NVOS09_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...interrupt service complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...interrupt service *FAILED*\n");
    }

} // end of Nv01Interrupt()

/*
NV04_RING0_CALLBACK:
    NVRM_RING0CALLBACK_PARAMS:
        NvV32 hClient;
        NvV32 hDevice;
        NvP64 functionPtr;
        NvU32 param1;
        NvU32 param2;
        NvV32 status;
    typedef NvU32 (*RING0CALLBACKPROC)(NvU32, NvU32);
*/
VOID NvRing0Callback(
    NVRM_RING0CALLBACK_PARAMS *pArgs
)
{
#if _WIN32_WINNT >= 0x0400
    RING0CALLBACKPROC callbackFct = (RING0CALLBACKPROC)ARG_P064(functionPtr);
#else
    RING0CALLBACKPROC callbackFct = ARG(functionPtr);
#endif

    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM: Ring 0 Callback...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Client:", ARG(hClient));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Device:", ARG(hDevice));
#if _WIN32_WINNT >= 0x0400
    DBG_PRINT_STRING_PTR  (DEBUGLEVEL_TRACEINFO,  "NVRM:     func:", (VOID*)ARG_P064(functionPtr));
#else
    DBG_PRINT_STRING_PTR  (DEBUGLEVEL_TRACEINFO,  "NVRM:     func:", (VOID*)ARG(functionPtr));
#endif
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   param1:", ARG(param1));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   param2:", ARG(param2));

    ARG(status) = callbackFct(
        ARG(param1),
        ARG(param2)
    );
    if (ARG(status) == NVOS16_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...Ring 0 Callback complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...Ring 0 Callback *FAILED*\n");
    }

} // end of NvRing0Callback()

/*
NV04_I2C_ACCESS
    NVOS_I2C_ACCESS_PARAMS:
        NvV32 hClient;
        NvV32 hDevice;
        NvP64 paramStructPtr;
        NvU32 paramSize;
        NvV32 status;
*/
VOID Nv04I2CAccess(
    NVOS_I2C_ACCESS_PARAMS *pArgs
)
{
    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM: I2C Access...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Client:", ARG(hClient));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Device:", ARG(hDevice));
#if _WIN32_WINNT >= 0x0400
    DBG_PRINT_STRING_PTR  (DEBUGLEVEL_TRACEINFO,  "NVRM:     func:", (VOID*)ARG_P064(paramStructPtr));
#else
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     func:", ARG(paramStructPtr));
#endif
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Device:", ARG(paramSize));

    ARG(status) = RmI2CAccess(
        ARG(hClient),
        ARG(hDevice),
#if _WIN32_WINNT >= 0x0400
        (VOID*)ARG_P064(paramStructPtr)
#else
        (VOID*)ARG(paramStructPtr)
#endif
    );
    if (ARG(status) == NVOS_I2C_ACCESS_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...I2C Access complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...I2C Access *FAILED*\n");
    }

} // end of Nv04I2CAccess()

/*
NV04_DIRECT_METHOD_CALL
    NVOS1D_PARAMETERS:
        NvV32 hRoot;
        NvV32 hObjectParent;
        NvV32 hObjectOld;
        NvV32 method;
        NvV32 data;
        NvV32 status;
*/
VOID Nv04DirectMethodCall(
    NVOS1D_PARAMETERS *pArgs
)
{
    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM: Performing Direct Method Call...\n");
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   client:", ARG(hRoot));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   channel:", ARG(hObjectParent));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   object:", ARG(hObjectOld));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   method:", ARG(method));
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   data:", ARG(data));

    ARG(status) = RmDirectMethodCall(
        ARG(hRoot),
        ARG(hObjectParent),
        ARG(hObjectOld),
        ARG(method),
        ARG(data)
    );
    if (ARG(status) == NVOS1D_STATUS_SUCCESS)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM: ...DMC complete\n");
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: ...DMC *FAILED*\n");
    }

} // end of Nv04DirectMethodCall()

NvV32 RmAllocClient(
    U032 *phClient,
    U032 hClass
)
{
    RM_STATUS rmStatus = RM_OK;

    // register a new client and return the client handle
    rmStatus = CliAddClient(phClient, hClass);

    return RmArchStatus(rmStatus, NV01_ALLOC_ROOT);

} // end of RmAllocClient

NvV32 RmAllocDevice(
    NvV32 hClient,
    NvV32 hDevice,
    NvV32 hClass,
    char *szName
)
{
    RM_STATUS rmStatus = RM_OK;
    PCLI_DMA_INFO pDmaInfo;
    PDMAOBJECT pDmaObject;
    PHWINFO pDev;

    // set the client context
    if (!CliSetClientContext(hClient))
    {
        return RmArchStatus(RM_ERR_BAD_OBJECT_PARENT, NV01_ALLOC_DEVICE);
    }

    // The caller has to either pass a valid device class (NV01_DEVICE_0 through NV01_DEVICE_7)
    // or pass NV03_DEVICE_XX as the hClass and pass the name of the device in szName.
    rmStatus = osDeviceNameToDeviceClass(szName, &hClass);
    if (!RM_SUCCESS(rmStatus))
    {
        return RmArchStatus(RM_ERR_BAD_CLASS, NV01_ALLOC_DEVICE);
    }

    // add new device to client and set the device context
    rmStatus = CliAddDevice(hClient, hDevice, hClass);
    if (!RM_SUCCESS(rmStatus))
    {
        return RmArchStatus(rmStatus, NV01_ALLOC_DEVICE);
    }
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        return RmArchStatus(RM_ERR_BAD_OBJECT_HANDLE, NV01_ALLOC_DEVICE);
    }

    // register all DMA contexts with the device
    for (pDmaInfo = CliGetDmaList(); pDmaInfo; pDmaInfo = pDmaInfo->Next)
    {
        rmStatus = dmaRegisterToDevice(pDev, pDmaInfo, &pDmaObject);
        if (!RM_SUCCESS(rmStatus))
        {
            return RmArchStatus(rmStatus, NV01_ALLOC_DEVICE);
        }
    }

    return RmArchStatus(rmStatus, NV01_ALLOC_DEVICE);

} // end of RmAllocDevice

NvV32 RmAllocContextDma(
    U032 hClient,
    U032 hDma,
    U032 hClass,
    U032 flags,
    U032 selector,
    VOID* base,
    U032 limit
)
{
    RM_STATUS rmStatus = RM_OK;
    PCLI_DMA_INFO pDmaInfo;
    PCLI_DEVICE_INFO deviceList = 0;
    PCLI_FIFO_INFO fifoList;
    PDMAOBJECT pDmaObject;
    U032 i, j, hDeviceWithFrameBufferCtx = 0;
    PHWINFO pDev;

    // A limit of 0 is not valid.
    if (limit == 0)
    {
        return NVOS03_STATUS_ERROR_BAD_LIMIT;
    }

    // set the client context
    if (!CliSetClientContext(hClient))
    {
        return RmArchStatus(RM_ERR_BAD_OBJECT_PARENT, NV01_ALLOC_CONTEXT_DMA);
    }

    // add DMA context to client
    rmStatus = CliAddDma(hClient, hDma, hClass, flags, selector, base, limit, &pDmaInfo);
    if (!RM_SUCCESS(rmStatus))
    {
        return RmArchStatus(rmStatus, NV01_ALLOC_CONTEXT_DMA);
    }

    // TO DO: USE FLAGS FOR CREATION??

    // lock down DMA pages -- zero length is not an error for legacy reasons
    if (pDmaInfo->DescLimit != 0)
    {
        // validate DMA buffer address, searching each device's FB
        if ((deviceList = CliGetDeviceList()) == NULL)
        {
            RmFreeContextDma(hClient, hDma);
            return RmArchStatus(RM_ERR_BAD_OBJECT_PARENT, NV01_ALLOC_CONTEXT_DMA);
        }
        for (i = 0; i < NUM_DEVICES; i++)
        {
            if (deviceList[i].InUse)
            {
                if (!CliSetDeviceContext(deviceList[i].Handle, &pDev))
                {
                    RmFreeContextDma(hClient, hDma);
                    return RmArchStatus(RM_ERR_BAD_OBJECT_PARENT, NV01_ALLOC_CONTEXT_DMA);
                }
                rmStatus = osGetAddressInfo(
                    pDev,
                    0 /* ChId */,
                    pDmaInfo->DescSelector,
                    &pDmaInfo->DescOffset,
                    pDmaInfo->DescLimit,
                    &pDmaInfo->DescAddr,
                    &pDmaInfo->AddressSpace
                    );

                if (!RM_SUCCESS(rmStatus))
                {
                    RmFreeContextDma(hClient, hDma);
                    return RmArchStatus(RM_ERR_BAD_OBJECT_HANDLE, NV01_ALLOC_CONTEXT_DMA);
                }
                if (pDmaInfo->AddressSpace != ADDR_UNKNOWN)
                {
                    // found the buffer -- remember the device, if the context describes a frame buffer
                    hDeviceWithFrameBufferCtx = (pDmaInfo->AddressSpace == ADDR_FBMEM) ? deviceList[i].Handle : 0;
                    break;
                }
            }
        }

        // calculate page table values
        pDmaInfo->BufferSize = pDmaInfo->DescLimit + 1;
        osCalculatePteAdjust(pDev, pDmaInfo->AddressSpace, pDmaInfo->DescAddr, &pDmaInfo->PteAdjust);
        pDmaInfo->PteLimit   = pDmaInfo->DescLimit; // pDmaInfo->PteAdjust + pDmaInfo->BufferSize;
        pDmaInfo->PteCount   = (pDmaInfo->PteLimit + pDmaInfo->PteAdjust + RM_PAGE_SIZE - 1) >> RM_PAGE_SHIFT;
        rmStatus = osAllocMem((VOID **)&(pDmaInfo->PteArray), pDmaInfo->PteCount * 4);
        if (!RM_SUCCESS(rmStatus))
        {
            RmFreeContextDma(hClient, hDma);
            return RmArchStatus(RM_ERR_OPERATING_SYSTEM, NV01_ALLOC_CONTEXT_DMA);
        }

        // lock DMA buffer
        rmStatus = osLockUserMem(
            pDev,
            INVALID_CHID,
            pDmaInfo->DescSelector,
            pDmaInfo->DescOffset,
            pDmaInfo->DescAddr,
            &pDmaInfo->AddressSpace,
            pDmaInfo->DescLimit,
            pDmaInfo->PteCount,
            pDmaInfo->PteArray,
            &pDmaInfo->BufferBase,
            &pDmaInfo->LockHandle
        );
        if (!RM_SUCCESS(rmStatus))
        {
            RmFreeContextDma(hClient, hDma);
            return RmArchStatus(RM_ERR_OPERATING_SYSTEM, NV01_ALLOC_CONTEXT_DMA);
        }
    }

    // register and bind DMA context
    for (i = 0; i < NUM_DEVICES; i++)
    {
        if (deviceList && deviceList[i].InUse)
        {
            // set the device context
            if (!CliSetDeviceContext(deviceList[i].Handle, &pDev))
            {
                rmStatus = RM_ERR_BAD_OBJECT_PARENT;
                break;
            }

            // only allow the device with a DMA context in its FB to be registered as such
            if(deviceList[i].Handle == hDeviceWithFrameBufferCtx)
              pDmaInfo->AddressSpace = ADDR_FBMEM;
            
            // register DMA context with the current device
            if (!RM_SUCCESS(rmStatus = dmaRegisterToDevice(pDev, pDmaInfo, &pDmaObject)))
            {
                break;
            }

            // bind DMA context to all FIFOs of the current device
            if (!CliGetDeviceFifoList(deviceList[i].Handle, &fifoList))
            {
                rmStatus = RM_ERR_BAD_OBJECT_PARENT;
                break;
            }
            for (j = 0; j < NUM_FIFOS; j++)
            {
                if (fifoList && fifoList[j].InUse)
                {
                    if (!RM_SUCCESS(rmStatus = dmaBindToChannel(pDev, pDmaObject, j)))
                    {
                        break;
                    }
                }
            }
            if (!RM_SUCCESS(rmStatus))
            {
                break;
            }
        }
    }
    if (!RM_SUCCESS(rmStatus))
    {
        RmFreeContextDma(hClient, hDma);
    }

    return RmArchStatus(rmStatus, NV01_ALLOC_CONTEXT_DMA);

} // end of RmAllocContextDma()

NvV32 RmAllocChannelPio(
    U032 hClient,
    U032 hDevice,
    U032 hPioChannel,
    U032 hClass,
    U032 hErrorContext,
    VOID** ppChannel,
    U032 flags
)
{
    RM_STATUS rmStatus = RM_OK;

    rmStatus = RmAllocChannel(
        hClient,
        hDevice,
        hPioChannel,
        hClass,
        hErrorContext,
        0,          // no DMA context
        0,          // no DMA push initial offset
        ppChannel,
        flags
    );

    return RmArchStatus(rmStatus, NV01_ALLOC_CHANNEL_PIO);

} // end of RmAllocChannelPio()

NvV32 RmAllocChannelDma(
    U032 hClient,
    U032 hDevice,
    U032 hDmaChannel,
    U032 hClass,
    U032 hErrorContext,
    U032 hDataContext,
    U032 offset,
    VOID** ppControl
)
{
    RM_STATUS rmStatus = RM_OK;

    rmStatus = RmAllocChannel(
        hClient,
        hDevice,
        hDmaChannel,
        hClass,
        hErrorContext,
        hDataContext,
        offset,
        ppControl,
        0           // no flags
    );

    return RmArchStatus(rmStatus, NV03_ALLOC_CHANNEL_DMA);

} // end of RmAllocChannelDma()

RM_STATUS RmAllocChannel(
    U032 hClient,
    U032 hDevice,
    U032 hChannel,
    U032 hClass,
    U032 hErrorContext,
    U032 hDataContext,
    U032 offset,
    VOID** ppChannel,
    U032 flags
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 chID;
    U032 length = CHANNEL_LENGTH;
    PDMAOBJECT pDmaObject;
    PCLI_DMA_INFO pDmaInfo;
    U032 clientClass;
    PHWINFO pDev;

    // set the client context
    if (!CliSetClientContext(hClient))
    {
        return RM_ERR_OPERATING_SYSTEM;
    }

    // set the correct device context
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        CliDelDeviceFifo(hDevice, hChannel);
        return RM_ERR_INSUFFICIENT_RESOURCES;
    }

    // allocate the channel based on hDataContext
    if (hDataContext == 0)
    {
        // allocate a PIO channel
        rmStatus = fifoAlloc(pDev, hClass, &chID);
    }
    else
    {
        // get context dma object
        if (RM_SUCCESS(rmStatus = dmaFindContext(pDev, hDataContext, hClient, &pDmaObject)))
        {
            // allocate a DMA channel
            rmStatus = fifoAllocDma(pDev, hClass, &chID, pDmaObject);
        }
    }

    if (!RM_SUCCESS(rmStatus))
    {
        return rmStatus;
    }

    if (!CliGetClass(NV01_NULL_OBJECT, hClient, &clientClass))
    {
        return RM_ERR_OPERATING_SYSTEM;
    }

    // map the channel
    switch (hClass)
    {
        // special case the nv3 dma channel
        case NV03_CHANNEL_DMA:
            *ppChannel = NULL;
            break;
        default:
        {
            rmStatus = osMapFifo(pDev, clientClass, chID, ppChannel);
            break;
        }
    }
    if (!RM_SUCCESS(rmStatus))
    {
        fifoFree(pDev, chID);
        //CliDelDeviceFifo(hDevice, hChannel);
        return rmStatus;
    }
    //pFifoInfo->Flat = *ppChannel;

    // add fifo to device fifo list
    rmStatus = CliAddDeviceFifo(
        pDev,
        hDevice,
        hChannel,
        hClass,
        hErrorContext,
        hDataContext,
        offset,
        flags,
        chID,
        *ppChannel
    );
    if (!RM_SUCCESS(rmStatus))
    {
        osUnmapMemory(pDev, clientClass, *ppChannel, length);
        fifoFree(pDev, chID);
        return rmStatus;
    }

    // bind all DMA contexts of the client to the FIFO of the current device
    for (pDmaInfo = CliGetDmaList(); pDmaInfo; pDmaInfo = pDmaInfo->Next)
    {
        // When the device was allocated, the dma context was registered to it.
        // Find the dmaObject created at that time.
        rmStatus = dmaFindContext(pDev, pDmaInfo->Handle, hClient, &pDmaObject);
        if (!RM_SUCCESS(rmStatus))
        {
            RmFreeChannel(hClient, hDevice, hChannel);
            return RmArchStatus(rmStatus, NV01_ALLOC_DEVICE);
        }
        rmStatus = dmaBindToChannel(pDev, pDmaObject, chID);
        if (!RM_SUCCESS(rmStatus))
        {
            RmFreeChannel(hClient, hDevice, hChannel);
            return RmArchStatus(rmStatus, NV01_ALLOC_DEVICE);
        }
    }

    rmStatus = 0;

    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM:   logical address:", *ppChannel);

    return rmStatus;

} // end of RmAllocChannel()

NvV32 RmAllocMemory(
    U032 hClient,
    U032 hParent,
    U032 hMemory,
    U032 hClass,
    U032 flags,
    VOID** pAddress,
    U032* pLimit
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 trueLength;
    U032 instance = 0;
    BOOL addDeviceMemory = TRUE;
    PHWINFO pDev;
    VOID *pMemData = NULL;

    // set the client context
    if (!CliSetClientContext(hClient))
    {
        return RmArchStatus(RM_ERR_OPERATING_SYSTEM, NV01_ALLOC_MEMORY);
    }

    // set the device context
    if (!CliSetDeviceContext(hParent, &pDev))
    {
        return RmArchStatus(RM_ERR_BAD_OBJECT_PARENT, NV01_ALLOC_MEMORY);
    }

    switch (hClass)
    {
        case NV01_MEMORY_SYSTEM:

            rmStatus = RmAllocSystemMemory(pDev, hClient, pAddress, pLimit, &pMemData, flags);
            if (RM_SUCCESS(rmStatus))
            {
                rmStatus = CliAddMemory(hClient, hMemory, hClass, flags, *pAddress, *pLimit + 1, pMemData);
                if (!RM_SUCCESS(rmStatus))
                {
                    RmFreeSystemMemory(pDev, hMemory);
                }
            }
            break;


        // modified by Chuck Moidel
        case NV01_MEMORY_LOCAL_PRIVILEGED:
            #ifndef NV3
            RM_ASSERT(0);  //NV01_MEMORY_LOCAL_PRIVILEGED should only be used by NV3 and before
            #endif
            /*
            // the length returned is the offset into instance memory, but the
            // true length is registered with the client.
            rmStatus = RmAllocInstanceMemory(pDev, hClient, pAddress, pLimit, &trueLength, &instance);
            if (RM_SUCCESS(rmStatus))
            {
               rmStatus = CliAddDeviceMemory(hParent, hMemory, hClass, *pAddress, trueLength, instance, 0);
               // is this right to return the status of the Free rather than the failure of CliAddDeviceMemory??
               if (!RM_SUCCESS(rmStatus))
               {
                   rmStatus = RmFreeInstanceMemory(pDev, hClient, hParent, hMemory);
               }
            }
            */
            break;

        /*
        ATTENTION!
        RmAllocMemory(NV01_MEMORY_LOCAL_USER) and RmAlloc(NV01_MEMORY_LOCAL USER) do VERY different things.
        For more information see nvarch.h  - added by Charles Moidel
        */

        case NV01_MEMORY_LOCAL_USER: /* please note that this function does not allocate ANY framebuffer memory.
                                        This function only allocates and returns a dumb linear mapping to the entire 
                                        framebuffer.  -C.Moidel
                                     */

            // if the address is non-NULL, only return the length -- don't add as device memory
            addDeviceMemory = !*pAddress;
            rmStatus = RmAllocFrameBufferMapping(pDev, hClient, pAddress, pLimit);
            trueLength = *pLimit;

            // add the device memory info to the client database
            if (RM_SUCCESS(rmStatus) && addDeviceMemory)
            {
                rmStatus = CliAddDeviceMemory(hParent, hMemory, hClass, *pAddress, trueLength, instance, 0, 0);
                if (!RM_SUCCESS(rmStatus))
                {
                    rmStatus = RmFreeFrameBuffer(pDev, hClient, hParent, hMemory);
                }
            }
            break;

        default:
            rmStatus = RM_ERR_BAD_CLASS;
            break;
    }

    return RmArchStatus(rmStatus, NV01_ALLOC_MEMORY);

} // end of RmAllocMemory()

RM_STATUS RmAllocSystemMemory(
    PHWINFO pDev,
    U032    hClient,
    VOID**   pAddress,
    U032*   pLimit,
    VOID**  pMemData,
    U032    flags
)
{
    RM_STATUS rmStatus = RM_OK;
    U032      MemoryType, Contiguous, PageCount;
    U032      Cache = 0;
    U032      clientClass;

    if (!CliGetClass(NV01_NULL_OBJECT, hClient, &clientClass))
    {
        return RM_ERR_BAD_OBJECT_PARENT;
    }

    //
    // Allocate system memory
    //
    // 4k Page sized allocation?  This is the page size default for arch work.
    // Remember that the given length is actually a limit (size-1)
    //
    if ((*pLimit + 1) & 0xFFF)
    {
        //
        // Not a page request.  Don't support for now.
        //
        rmStatus = NVOS03_STATUS_ERROR_BAD_LIMIT;
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: *** Invalid limit: not page aligned\n");
        return rmStatus;
    }
    else
    {
        //
        // Page sized request
        //
        // Convert arch constants into RM constants.
        //
        PageCount = (*pLimit + 1) >> 12;

        if (DRF_VAL(OS02, _FLAGS, _LOCATION, flags) == NVOS02_FLAGS_LOCATION_AGP)
            MemoryType = NV_MEMORY_TYPE_AGP;
        else
            MemoryType = NV_MEMORY_TYPE_SYSTEM;

        if (DRF_VAL(OS02, _FLAGS, _PHYSICALITY, flags) == NVOS02_FLAGS_PHYSICALITY_CONTIGUOUS)
            Contiguous = NV_MEMORY_CONTIGUOUS;
        else
            Contiguous = NV_MEMORY_NONCONTIGUOUS;

        if (DRF_VAL(OS02, _FLAGS, _COHERENCY, flags) == NVOS02_FLAGS_COHERENCY_UNCACHED)
            Cache = NV_MEMORY_UNCACHED;
        else if (DRF_VAL(OS02, _FLAGS, _COHERENCY, flags) == NVOS02_FLAGS_COHERENCY_CACHED)
            Cache = NV_MEMORY_DEFAULT;
        else if (DRF_VAL(OS02, _FLAGS, _COHERENCY, flags) == NVOS02_FLAGS_COHERENCY_WRITE_COMBINE)
            Cache = NV_MEMORY_WRITECOMBINED;
        else if (DRF_VAL(OS02, _FLAGS, _COHERENCY, flags) == NVOS02_FLAGS_COHERENCY_WRITE_THROUGH)
            Cache = NV_MEMORY_WRITETHRU;
        else if (DRF_VAL(OS02, _FLAGS, _COHERENCY, flags) == NVOS02_FLAGS_COHERENCY_WRITE_PROTECT)
            Cache = NV_MEMORY_WRITEPROTECT;
        else if (DRF_VAL(OS02, _FLAGS, _COHERENCY, flags) == NVOS02_FLAGS_COHERENCY_WRITE_BACK)
            Cache = NV_MEMORY_WRITEBACK;

        rmStatus = osAllocPages(pDev,
                                pAddress,
                                PageCount,
                                MemoryType,
                                Contiguous,
                                Cache,
                                clientClass,
                                pMemData);

        if (!RM_SUCCESS(rmStatus))
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: *** Cannot allocate pages\n");
            rmStatus = NVOS02_STATUS_ERROR_BAD_FLAGS;
            return rmStatus;
        }
    }

    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO, "NVRM: after system memory allocation:\n");
    DBG_PRINT_STRING_PTR  (DEBUGLEVEL_TRACEINFO, "NVRM:   pMemory:", *pAddress);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:     limit:", *pLimit);

    return rmStatus;

} // end of RmAllocSystemMemory()

RM_STATUS RmAllocInstanceMemory(
    PHWINFO pDev,
    U032 hClient,
    VOID** pAddress,
    U032* pLength,
    U032* pTrueLength,
    U032* pInstance
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 instMemOffset, clientClass;
    // U032 instanceMemoryPciAddress = INSTANCE_MEMORY_BASE;

    // get the class of client
    if (!CliGetClass(NV01_NULL_OBJECT, hClient, &clientClass))
    {
        rmStatus = RM_ERR_BAD_OBJECT_PARENT;
    }

    // NOTE: never allocate cursor memory for client of type "user"
    // TO DO: this is not generalized -- for NV3 legacy reasons, only a kernel client can alloc inst mem
    // validate the client class
    if (clientClass != NV01_ROOT)
    {
        return RM_ERR_BAD_OBJECT_PARENT;
    }

    // allocate instance memory for the cursor aligned on 2K boundary
    rmStatus = fbAllocInstMemAlign(pDev, pInstance, *pLength >> 4, 0x800 >> 4);
    if (!RM_SUCCESS(rmStatus))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: *** Cannot allocate instance for cursor\n");
        return rmStatus;
    }
    instMemOffset = *pInstance << 4;

    // map the memory into the client's context
    rmStatus = osMapInstanceMemory(pDev, clientClass, instMemOffset, *pLength, pAddress);
    if (!RM_SUCCESS(rmStatus))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: *** Cannot map instance memory for cursor\n");
        return rmStatus;
    }

    // return the offset from the top of instance memory in pLength, and the true length in pTrueLength
    // TODO: eliminate this when the miniport does not talk to HW
    *pTrueLength = *pLength;
    *pLength = instMemOffset;

    DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO, "NVRM:  after cursor allocation:\n");
    DBG_PRINT_STRING_PTR  (DEBUGLEVEL_TRACEINFO, "NVRM:   pMemory:", *pAddress);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:     limit:", *pLength);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:  instance:", *pInstance);

    return rmStatus;

} // end of RmAllocInstanceMemory()

RM_STATUS RmAllocFrameBufferMapping(
    PHWINFO pDev,
    U032 hClient,
    VOID** pAddress,
    U032* pLength
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 clientClass;

    // get the class of client
    if (!CliGetClass(NV01_NULL_OBJECT, hClient, &clientClass))
    {
        rmStatus = RM_ERR_BAD_OBJECT_PARENT;
    }

    // map frame buffer into the client's context -- only map if null is passed in as address
    *pLength = pDev->Framebuffer.HalInfo.Limit[0];
    if (*pAddress == NULL)
    {
        rmStatus = osMapFrameBuffer(pDev, clientClass, 0, *pLength, pAddress);
        if (!RM_SUCCESS(rmStatus))
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  "NVRM: *** Cannot map frame buffer\n");
            return rmStatus;
        }
    }

    DBG_PRINT_STRING_PTR  (DEBUGLEVEL_TRACEINFO, "NVRM:   address:", *pAddress);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:    length:", *pLength);

    return rmStatus;

} // end of RmAllocFrameBufferMapping()

NvV32 RmAllocObject(
    U032 hClient,
    U032 hChannel,
    U032 hObject,
    U032 hClass
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 hDevice;
    PCLI_FIFO_INFO pFifo;
    POBJECT pObject;
    PHWINFO pDev;

    // set the client context
    if (!CliSetClientContext(hClient))
    {
        //return RmArchStatus(RM_ERR_BAD_CLIENT, NV01_ALLOC_OBJECT);
        return RmArchStatus(RM_ERR_OPERATING_SYSTEM, NV01_ALLOC_OBJECT);
    }

    // find and set the device context
    if (!CliGetDeviceHandle(hChannel, &hDevice))
    {
        return RmArchStatus(RM_ERR_BAD_OBJECT_PARENT, NV01_ALLOC_OBJECT);
    }
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        return RmArchStatus(RM_ERR_BAD_OBJECT_PARENT, NV01_ALLOC_OBJECT);
    }

    // get the class object
    //nv->subchannel[BLIT_SUBCHANNEL].SetObject = NV_VIDEO_COLORMAP;
    if (!CliGetDeviceFifoInfo(hDevice, hChannel, &pFifo))
    {
        return RmArchStatus(RM_ERR_BAD_OBJECT_PARENT, NV01_ALLOC_OBJECT);
    }
    if (hClass == NV01_NULL_OBJECT)
    {
        return RmArchStatus(RM_ERR_BAD_OBJECT_HANDLE, NV01_ALLOC_OBJECT);
    }

    rmStatus = fifoSearchObject(pDev, hClass, pFifo->ChID, &pObject);
    if (!RM_SUCCESS(rmStatus))
    {
        return RmArchStatus(RM_ERR_BAD_CLASS, NV01_ALLOC_OBJECT);
    }

    // create the device given its class object
    //nv->subchannel[BLIT_SUBCHANNEL].nvClass.Create = DD_COLORMAP;
    rmStatus = mthdCreate(pDev, pObject, 0, 0, hObject, NULL);
    if (!RM_SUCCESS(rmStatus))
    {
        return RmArchStatus(RM_ERR_INSUFFICIENT_RESOURCES, NV01_ALLOC_OBJECT);
    }

    return RmArchStatus(rmStatus, NV01_ALLOC_OBJECT);

} // end of RmAllocObject()

NvV32 RmAllocEvent
(
    U032 hClient,
    U032 hUser,
    U032 hObject,
    U032 hClass,
    U032 index,
    U064 hEvent
)
{
    RM_STATUS rmStatus = RM_OK;
    PCLI_EVENT_INFO pEventInfo;
    POBJECT pUser;
    U032 hChannel;

    // set client context
    if (!CliSetClientContext(hClient))
    {
        return NVOS10_STATUS_ERROR_OPERATING_SYSTEM;
    }

    // add event to client and parent object
    rmStatus = CliAddEvent(hClient, hUser, hObject, hClass, &pEventInfo);
    if (RM_SUCCESS(rmStatus))
    {
        CliFindObject(hUser, &pUser, &hChannel);
        rmStatus = eventSetObjectEventNotify(pUser, hObject, index, hClass, hEvent);
        if (!RM_SUCCESS(rmStatus))
        {
            CliDelEvent(hObject);
        }
    }

    return RmArchStatus(rmStatus, NV01_ALLOC_EVENT);

} // end of RmAllocEvent()




NvV32 RmAlloc(
    U032 hClient,
    U032 hParent,
    U032 hObject,
    U032 hClass,
    VOID* pAllocParms
)
{
    RM_STATUS rmStatus = RM_OK;

    //special case for Client Allocation - added by Chuck Moidel
    if (hClass == NV01_ROOT)
    {
        NVOS01_PARAMETERS *pArgs = (NVOS01_PARAMETERS*)pAllocParms;
        ARG(status) = RmAllocClient(&ARG(hObjectNew),ARG(hClass));
        return ARG(status);
    }
    
    // set the client context
    if (!CliSetClientContext(hClient))
    {
        return RmArchStatus(RM_ERR_BAD_CLIENT, NV04_ALLOC);
    }

    // allocate the object based on its class
    switch(hClass)
    {
        //case NV01_NULL_OBJECT:
        case NV01_ROOT:  //should be handled above!! -C.Moidel
            RM_ASSERT(hClass);
            return RmArchStatus(RM_ERR_BAD_CLASS, NV04_ALLOC);

        case NV01_MEMORY_SYSTEM:




        //case NV01_MEMORY_LOCAL_PRIVILEGED:  //shouldn't be using this in NV4+   -C.Moidel

        /*
        ATTENTION!
        RmAllocMemory(NV01_MEMORY_LOCAL_USER) and RmAlloc(NV01_MEMORY_LOCAL USER) do VERY different things.
        For more information see nvarch.h  - added by Charles Moidel
        */
        case NV01_MEMORY_LOCAL_USER:      /*This is used to allocate blocks from the Framebuffer Heap!   -C.Moidel */



        case NV04_HEAP_OWNER:             //added by Chuck Moidel


        case NV01_EVENT:
        case NV01_CONTEXT_DMA:
        case NV01_DEVICE_0:
        case NV01_DEVICE_1:
        case NV01_DEVICE_2:
        case NV01_DEVICE_3:
        case NV01_DEVICE_4:
        case NV01_DEVICE_5:
        case NV01_DEVICE_6:
        case NV01_DEVICE_7:
        case NV03_CHANNEL_PIO:
        case NV04_CHANNEL_PIO:
        case NV03_CHANNEL_DMA:
        case NV04_CHANNEL_DMA:
            return RmArchStatus(RM_ERR_BAD_OBJECT_HANDLE, NV04_ALLOC);

        case NVDE_DEBUGGER:
        case NVDE_REGISTER_DEBUGGER:
        case NVDE_UNREGISTER_DEBUGGER:
        case NVDE_READ_MEMORY:
        case NVDE_WRITE_MEMORY:
        case NVDE_TEST:
        case NVDE_GET_EVENT_INFORMATION:
            rmStatus = RmDebugEx(hClient, hObject, hClass, pAllocParms);
            break;

        default:
            rmStatus = RmAllocObjectEx(hParent, hObject, hClass, pAllocParms);
            break;
    }

    return RmArchStatus(rmStatus, NV04_ALLOC);

} // end of RmAlloc()

RM_STATUS RmAllocObjectEx(
    U032 hChannel,
    U032 hObject,
    U032 hClass,
    VOID* pAllocParms
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 hDevice;
    PCLI_FIFO_INFO pFifo;
    POBJECT pObject;
    PHWINFO pDev;

    // set the device context
    if (!CliGetDeviceHandle(hChannel, &hDevice))
    {
        return RM_ERR_BAD_OBJECT_PARENT;
    }
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        return RM_ERR_BAD_OBJECT_PARENT;
    }

    // get the class object
    if (!CliGetDeviceFifoInfo(hDevice, hChannel, &pFifo))
    {
        return RM_ERR_BAD_OBJECT_PARENT;
    }
    rmStatus = fifoSearchObject(pDev, hClass, pFifo->ChID, &pObject);
    if (!RM_SUCCESS(rmStatus))
    {
        return RM_ERR_BAD_CLASS;
    }

    // create the device given its class object
    rmStatus = mthdCreate(pDev, pObject, 0, 0, hObject, pAllocParms);
    if (!RM_SUCCESS(rmStatus))
    {
        return RM_ERR_INSUFFICIENT_RESOURCES;
    }

    return rmStatus;

} // end of RmAllocObjectEx()

// TO DO: clean up as much as possible despite any errors, for all free routines

NvV32 RmFree(
    U032 hClient,
    U032 hParent,
    U032 hObject
)
{
    U032 client;
    PCLI_DEVICE_INFO pDevice;
    PCLI_FIFO_INFO pFifo;
    PCLI_MEMORY_INFO pMemory;
    PCLI_DMA_INFO pDma;
    PCLI_EVENT_INFO pEvent;
    RM_STATUS rmStatus = RM_OK;

    // set the client context
    if (!CliSetClientContext(hClient))
    {
        return RmArchStatus(RM_ERR_OPERATING_SYSTEM, NV01_FREE);
    }

    // scan all object types
    if (CliGetClientHandle(&client) && client == hObject)
    {
        rmStatus = RmFreeClient(hClient);
    }
    else if (CliGetDeviceInfo(hObject, &pDevice))
    {
        rmStatus = RmFreeDevice(hClient, hObject);
    }
    else if (CliGetDeviceFifoInfo(hParent, hObject, &pFifo))
    {
        rmStatus = RmFreeChannel(hClient, hParent, hObject);
    }
    else if (CliGetDeviceMemoryInfo(hParent, hObject, &pMemory))
    {
        rmStatus = RmFreeMemory(hClient, hParent, pMemory->Class, hObject);
    }
    else if (CliGetDmaInfo(hObject, &pDma))
    {
        rmStatus = RmFreeContextDma(hClient, hObject);
    }
    else if (CliGetMemoryInfo(hObject, &pMemory))
    {
        rmStatus = RmFreeMemory(hClient, hParent, pMemory->Class, hObject);
    }
    else if (CliGetEventInfo(hObject, &pEvent))
    {
        rmStatus = RmFreeEvent(hClient, hObject);
    }
    else
    {
        rmStatus = RmFreeObject(hParent, hObject);
    }

    return RmArchStatus(rmStatus, NV01_FREE);

} // end of RmFree()

RM_STATUS RmFreeClient(
    U032 hClient
)
{
    PHWINFO pDev = (PHWINFO) 0;
    RM_STATUS rmStatus = RM_OK;
    PCLI_DMA_INFO pDmaInfo, nextDma;
    PCLI_DEVICE_INFO deviceList;
    PCLI_MEMORY_INFO pMemoryInfo, nextMem;
    U032 i;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM:     type: client\n");

    // free all of the devices of the client
    if ((deviceList = CliGetDeviceList()) != NULL)
    {
        for (i = 0; i < NUM_DEVICES; i++)
        {
            if (deviceList[i].InUse)
            {
                rmStatus |= RmFreeDevice(hClient, deviceList[i].Handle);
                // This patch is necessary in order to distinguish system memory resources
                // on PCI or on AGP. The architecture separates system memory resources from the
                // device, but in reality there is pDev dependence. This problem should be
                // addressed in the future. This patch could cause problems where a client has
                // alloc'd 2 devices(PCI and AGP) along with some system memory resource. The
                // current driver set does not implement this scenario but I would expect problems
                // in the near future.
                pDev = NvDBPtr_Table[i];
            }
        }
    }
    else
        rmStatus |= RM_ERR_BAD_OBJECT_HANDLE;

    // free all of the DMA contexts of the client
    nextDma = CliGetDmaList();
    while (nextDma)
    {
        pDmaInfo = nextDma;
        nextDma = nextDma->Next;
        rmStatus |= RmFreeContextDma(hClient, pDmaInfo->Handle);
    }

    // free all client system memory
    nextMem = CliGetMemoryList();
    while (nextMem)
    {
        pMemoryInfo = nextMem;
        nextMem = nextMem->Next;
        rmStatus |= RmFreeSystemMemory(pDev, pMemoryInfo->Handle);
    }

    // free the client
    if (!CliDelClient())
    {
        rmStatus |= RM_ERR_BAD_OBJECT_HANDLE;
    }

    return rmStatus;

} // end of RmFreeClient()

RM_STATUS RmFreeDevice(
    U032 hClient,
    U032 hDevice
)
{
    RM_STATUS rmStatus = RM_OK;
    PCLI_FIFO_INFO fifoList;
    PCLI_MEMORY_INFO pMemoryInfo;
    PCLI_DMA_INFO pDmaInfo;
    U032 i;
    PDMAOBJECT pDmaObject;
    PHWINFO pDev;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM:     type: device\n");

    // set the device context
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        return RM_ERR_BAD_OBJECT_HANDLE;
    }

    // if the client was created, but never had any devices successfully
    //   attached, we'll get here.  The client's device structure will have
    //   been created, but pDev will be NULL if the device was later found
    //   to be non-existent
    // Since NUM_FIFOS macro ref's pDev, we stay away
    if (pDev)
    {
        // free all of the FIFOs of the device
        if (CliGetDeviceFifoList(hDevice, &fifoList))
        {
            for (i = 0; i < NUM_FIFOS; i++)
            {
                if (fifoList && fifoList[i].InUse)
                {
                    rmStatus |= RmFreeChannel(hClient, hDevice, fifoList[i].Handle);
                }
            }
        }
    }

    // unregister all DMA contexts with the device
    for (pDmaInfo = CliGetDmaList(); pDmaInfo; pDmaInfo = pDmaInfo->Next)
    {
        if (RM_SUCCESS(dmaFindContext(pDev, pDmaInfo->Handle, hClient, &pDmaObject)))
        {
            rmStatus |= dmaUnregisterFromDevice(pDev, pDmaObject);
        }
    }

    // free all device memory
    while ((pMemoryInfo = CliGetDeviceMemoryList(hDevice)) != NULL)
    {
        rmStatus |= RmFreeMemory(hClient, hDevice, pMemoryInfo->Class, pMemoryInfo->Handle);
    }

    // free the device
    if (!CliDelDevice(hDevice))
    {
        rmStatus |= RM_ERR_BAD_OBJECT_HANDLE;
    }

    return rmStatus;

} // end of RmFreeDevice()

RM_STATUS RmFreeContextDma(
    U032 hClient,
    U032 hDma
)
{
    RM_STATUS rmStatus = RM_OK;
    PCLI_DEVICE_INFO deviceList;
    PCLI_FIFO_INFO fifoList;
    U032 i, j;
    PDMAOBJECT pDmaObject;
    PHWINFO pDev;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM:     type: DMA context\n");

    // unregister and unbind the DMA context
    if ((deviceList = CliGetDeviceList()) == NULL)
    {
        rmStatus |= RM_ERR_BAD_OBJECT_PARENT;
    }
    else
    {
        for (i = 0; i < NUM_DEVICES; i++)
        {
            if (deviceList[i].InUse)
            {
                // set the current device context
                if (!CliSetDeviceContext(deviceList[i].Handle, &pDev))
                {
                    rmStatus |= RM_ERR_BAD_OBJECT_PARENT;
                }

                // get a pointer to the dma object
                else if (RM_SUCCESS(rmStatus |= dmaFindContext(pDev, hDma, hClient, &pDmaObject)))
                {
                    // unbind DMA context from all FIFOs of the current device
                    if (!CliGetDeviceFifoList(deviceList[i].Handle, &fifoList))
                    {
                        rmStatus |= RM_ERR_BAD_OBJECT_PARENT;
                    }
                    else
                    {
                        for (j = 0; j < NUM_FIFOS; j++)
                        {
                            if (fifoList && fifoList[j].InUse)
                            {
                                rmStatus |= dmaUnbindFromChannel(pDev, pDmaObject, j);
                            }
                        }

                        // unregister DMA context with the current device
                        rmStatus |= dmaUnregisterFromDevice(pDev, pDmaObject);
                    }
                }
            }
        }
    }

    // TO DO:  ***** THIS SHOULD HAPPEN HERE, BUT UNTIL IT IS PROVED
    //               IT IS DONE IN DMA DELETE
    /*
    // unlock pages
    if (!CliGetDmaInfo(hDma, &pDmaInfo))
    {
        return RM_ERR_BAD_OBJECT_HANDLE;
    }
    rmStatus = osUnlockUserMem(
        INVALID_CHID,
        pDmaInfo->BufferBase,
        pDmaInfo->DescAddr,
        pDmaInfo->AddressSpace,
        pDmaInfo->DescLimit,
        pDmaInfo->PteCount,
        pDmaInfo->LockHandle,
        pDmaInfo->Class == NV_CONTEXT_DMA_TO_MEM ? TRUE : FALSE
    );
    if (!RM_SUCCESS(rmStatus))
    {
        return rmStatus;
    }
    */

    // delete the DMA context from the client
    if (!CliDelDma(hDma))
    {
        rmStatus |= RM_ERR_BAD_OBJECT_HANDLE;
    }

    return rmStatus;

} // end of RmFreeContextDma()

RM_STATUS RmFreeChannel(
    U032 hClient,
    U032 hDevice,
    U032 hFifo
)
{
    RM_STATUS rmStatus = RM_OK;
    PCLI_DMA_INFO pDmaInfo;
    PCLI_FIFO_INFO pFifoInfo;
    PDMAOBJECT pDmaObject;
    PHWINFO pDev;
    U032 hClass;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM:     type: channel\n");

    // set the device context
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        return RM_ERR_BAD_OBJECT_HANDLE;
    }

    // unbind all DMA contexts from the FIFO
    for (pDmaInfo = CliGetDmaList(); pDmaInfo; pDmaInfo = pDmaInfo->Next)
    {
        if (!RM_SUCCESS(rmStatus = dmaFindContext(pDev, pDmaInfo->Handle, hClient, &pDmaObject)))
        {
            return rmStatus;
        }
        if (!CliGetDeviceFifoInfo(hDevice, hFifo, &pFifoInfo))
        {
            return RM_ERR_BAD_OBJECT_HANDLE;
        }
        if (!RM_SUCCESS(rmStatus = dmaUnbindFromChannel(pDev, pDmaObject, pFifoInfo->ChID)))
        {
            return rmStatus;
        }
    }

    // unmap the channel
    if (!CliGetDeviceFifoInfo(hDevice, hFifo, &pFifoInfo))
    {
        return RM_ERR_BAD_OBJECT_HANDLE;
    }
    hClass = pFifoInfo->Class;

    // NV03_CHANNEL_DMA wasn't mapped into clients address space
    if (hClass != NV03_CHANNEL_DMA)
    {
        U032 clientClass;

        if (!CliGetClass(NV01_NULL_OBJECT, hClient, &clientClass))
        {
            return RM_ERR_OPERATING_SYSTEM;
        }
        osUnmapMemory(pDev, clientClass, (VOID *)pFifoInfo->Flat, CHANNEL_LENGTH);
    }

    // free the FIFO
    rmStatus = fifoFree(pDev, pFifoInfo->ChID);
    if (!RM_SUCCESS(rmStatus))
    {
        return rmStatus;
    }

    // delete the device fifo info
    if (!CliDelDeviceFifo(hDevice, hFifo))
    {
        return RM_ERR_BAD_OBJECT_HANDLE;
    }

    // remove any events associated with this channel from the client
    CliDelEventUser(hFifo, hClass);

    return rmStatus;

} // end of RmFreeChannel()


RM_STATUS RmFreeMemory
(
    U032 hClient,
    U032 hDevice,
    U032 hClass,
    U032 hMemory
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev;

    // set the device context
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        return RM_ERR_BAD_OBJECT_PARENT;
    }

    switch(hClass)
    {
        case NV01_MEMORY_SYSTEM:

            rmStatus = RmFreeSystemMemory(pDev, hMemory);

            // remove the system memory reference from the client
            if (!CliDelMemory(hMemory))
            {
                rmStatus |= RM_ERR_BAD_OBJECT_HANDLE;
            }
            break;

        case NV01_MEMORY_LOCAL_PRIVILEGED:
        case NV01_MEMORY_LOCAL_USER:

            // set the device context
            if (!CliSetDeviceContext(hDevice, &pDev))
            {
                rmStatus |= RM_ERR_BAD_OBJECT_PARENT;
            }

            // free the device memory
            switch(hClass)
            {
                case NV01_MEMORY_LOCAL_PRIVILEGED:
                    rmStatus |= RmFreeInstanceMemory(pDev, hClient, hDevice, hMemory);
                    break;

                case NV01_MEMORY_LOCAL_USER:
                    rmStatus |= RmFreeFrameBuffer(pDev, hClient, hDevice, hMemory);
                    break;
            }

            // remove the device memory reference from the client
            if (!CliDelDeviceMemory(hDevice, hMemory))
            {
                rmStatus |= RM_ERR_BAD_OBJECT_HANDLE;
            }

            break;
    }

    return rmStatus;

} // end of RmFreeMemory()

RM_STATUS RmFreeSystemMemory
(
    PHWINFO pDev,
    U032 hMemory
)
{
    RM_STATUS rmStatus = RM_OK;
    PCLI_MEMORY_INFO pMemoryInfo;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM:     type: system memory\n");

    if (!CliGetMemoryInfo(hMemory, &pMemoryInfo))
    {
        rmStatus = RM_ERR_BAD_OBJECT_HANDLE;
    }
    else
    {
        rmStatus = osFreePages(pDev,
                               &pMemoryInfo->Address,
                               pMemoryInfo->Type,
                               pMemoryInfo->Length >> RM_PAGE_SHIFT,
                               pMemoryInfo->MemData,
                               (pMemoryInfo->Coherency == NVOS02_FLAGS_COHERENCY_WRITE_COMBINE));
    }

    return rmStatus;

} // end of RmFreeSystemMemory()

RM_STATUS RmFreeInstanceMemory(
    PHWINFO pDev,
    U032 hClient,
    U032 hDevice,
    U032 hMemory
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 clientClass;
    PCLI_MEMORY_INFO pMemoryInfo;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM:     type: cursor memory\n");

    // get the class of client
    if (!CliGetClass(NV01_NULL_OBJECT, hClient, &clientClass))
    {
        rmStatus = RM_ERR_BAD_OBJECT_PARENT;
    }

    // free the instance memory
    else if (!CliGetDeviceMemoryInfo(hDevice, hMemory, &pMemoryInfo))
    {
        rmStatus = RM_ERR_BAD_OBJECT_HANDLE;
    }
    else
    {
        // unmap the memory
        osUnmapMemory(pDev, clientClass, pMemoryInfo->Address, pMemoryInfo->Length);

        // deallocate the instance memory held by the cursor
        rmStatus = fbFreeInstMem(pDev, pMemoryInfo->Instance, pMemoryInfo->Length >> 4);
    }

    return rmStatus;

} // end of RmFreeInstanceMemory()

RM_STATUS RmFreeFrameBuffer(
    PHWINFO pDev,
    U032 hClient,
    U032 hDevice,
    U032 hMemory
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 clientClass;
    PCLI_MEMORY_INFO pMemoryInfo;
    U032 memHandle;


    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM:     type: frame buffer\n");

    // get the class of client
    if (!CliGetClass(NV01_NULL_OBJECT, hClient, &clientClass))
    {
        rmStatus = RM_ERR_BAD_OBJECT_PARENT;
    }

    // free the frame buffer
    else if (!CliGetDeviceMemoryInfo(hDevice, hMemory, &pMemoryInfo))
    {
        rmStatus = RM_ERR_BAD_OBJECT_HANDLE;
    }
    else
    {
        // free the video memory based on how it was alloced ... a non-zero
        // HeapOwner indicates it was heapAlloc-ed.

        if (pMemoryInfo->HeapOwner)
        {
            DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM: Function: FREE\n");
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Owner:", pMemoryInfo->HeapOwner);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:   Offset:", pMemoryInfo->Handle);

            // memHandle is returned, but not needed ... the caller already has
            // the correct handle to pass to CliDelDeviceMemory, since it's
            // traversing the client DB.

            rmStatus = heapFree(
                pDev,
                (PHEAP)pDev->Framebuffer.HeapHandle,
                pMemoryInfo->HeapOwner,
                (U032)((U008*)pMemoryInfo->Address - (U008*)fbAddr),
                &memHandle);
        }
        else
        {
            // unmap the memory
            osUnmapMemory(pDev, clientClass, pMemoryInfo->Address, pMemoryInfo->Length);
        }
    }
    return rmStatus;

} // end of RmFreeFrameBuffer()

RM_STATUS RmFreeObject(
    U032 hChannel,
    U032 hObject
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 hDevice;
    PCLI_FIFO_INFO pFifo;
    OBJECT classObject;
    PHWINFO pDev;

    // check for debugger object
    if (NvRmInfo.Debugger.object)
        if (NvRmInfo.Debugger.object->Name == hObject)
            return NvdeFree(hChannel, hObject);

    // set the corresponding device context
    if (!CliGetDeviceHandle(hChannel, &hDevice))
    {
        return RM_ERR_BAD_OBJECT_PARENT;
    }
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        return RM_ERR_BAD_OBJECT_PARENT;
    }

    // contrive the class object
    if (!CliGetDeviceFifoInfo(hDevice, hChannel, &pFifo))
    {
        return RM_ERR_BAD_OBJECT_PARENT;
    }
    classObject.Name = NV01_NULL_OBJECT;
    classObject.ChID = pFifo->ChID;

    // destroy the object
    rmStatus = mthdDestroy(pDev, &classObject, 0, 0, hObject);
    if (!RM_SUCCESS(rmStatus))
    {
        return RM_ERR_BAD_OBJECT_HANDLE;
    }

    // remove this user object from client events
    CliDelEventUser(hObject, NV01_NULL_OBJECT);

    return rmStatus;

} // end of RmFreeObject()

RM_STATUS RmFreeEvent(
    U032 hClient,
    U032 hEvent
)
{
    RM_STATUS rmStatus = RM_OK;
    PCLI_EVENT_INFO pEvent;
    PCLI_EVENT_USER nextUser;
    POBJECT pUser;
    U032 hChannel;

    // delete the event from all parent objects and client
    if (CliGetEventInfo(hEvent, &pEvent))
    {
        for (nextUser = pEvent->UserList; nextUser; nextUser = nextUser->Next)
        {
            if (CliFindObject(nextUser->Handle, &pUser, &hChannel))
            {
                eventRemoveObjectEventNotify(pUser, hEvent);
            }
        }
        CliDelEvent(hEvent);
    }
    else
    {
        rmStatus = RM_ERR_BAD_OBJECT;
    }

    return rmStatus;

} // end of RmFreeEvent()

NvV32 RmArchHeap
(
    U032 hRoot,
    U032 hObjectParent,
    U032 function,
    U032 owner,
    U032 type,
    U032 depth,
    U032 width,
    U032 height,
    S032* pPitch,
    U032* pOffset,
    U032* pSize,
    VOID** pAddress,
    U032* pLimit,
    U032* pTotal,
    U032* pFree
)
{
    U032 tempPitch;
    RM_STATUS rmStatus = NVOS11_STATUS_SUCCESS;
    PHWINFO pDev;
    U032 memHandle;
#ifdef DEBUG
    U032 oldFree;
#endif

    // set the client context
    if (!CliSetClientContext(hRoot))
    {
        return RM_ERR_OPERATING_SYSTEM;
    }

    // set the correct device context
    if (!CliSetDeviceContext(hObjectParent, &pDev))
    {
        return RM_ERR_INSUFFICIENT_RESOURCES;
    }

    //
    // Heap manager for NV memory.
    //
    if ((owner == 0) || (owner == 0xFFFFFFFF))
    {
        return NVOS11_STATUS_ERROR_INVALID_OWNER;
    }

    //
    // Setup size for DEPTH_WIDTH_HEIGHT or TILED_PITCH_HEIGHT allocs
    //
    if (function == NVOS11_HEAP_ALLOC_DEPTH_WIDTH_HEIGHT)
    {
        U032 byteWidth;

        // For NV3, scanline alignment is 32 bytes.
        byteWidth = ((width * depth) + 7) >> 3;
        *pSize  = height * ((byteWidth + 31) & ~31);
    }
    else if (function == NVOS11_HEAP_ALLOC_TILED_PITCH_HEIGHT)
    {
        *pSize  = (height * *pPitch);
        tempPitch = *pPitch;
    }

    switch (function)
    {
        //
        // Alloc mem.
        //
        case NVOS11_HEAP_ALLOC_DEPTH_WIDTH_HEIGHT:
        case NVOS11_HEAP_ALLOC_SIZE:
            tempPitch = 0;          // pitch is valid only for TILED_PITCH_HEIGHT

        case NVOS11_HEAP_ALLOC_TILED_PITCH_HEIGHT:
            DBG_PRINT_STRING      (DEBUGLEVEL_TRACEINFO,  "NVRM:    Function: ALLOC\n");
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Type:  ", type);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Owner: ", owner);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Size:  ", *pSize);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Pitch: ", *pPitch);
            rmStatus =
                heapAlloc
                (
                    pDev,
                    (PHEAP)pDev->Framebuffer.HeapHandle,
                    owner,
                    pSize,
                    type,
                    height,
                    &tempPitch,
                    pOffset
                );
            if (rmStatus == RM_OK)
            {
                //
                // Set the mapping for the memory.
                //
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Allocated at Offset: ", *pOffset);
                if (tempPitch)
                {
                    // update pitch value
                    *pPitch = tempPitch;
                }
                *pAddress = (U008*)fbAddr + *pOffset;
                *pLimit = *pSize - 1;

                // add the heapAlloc to the Client DB
                rmStatus =
                    CliAddDeviceMemory
                    (
                        hObjectParent,
                        ((PHEAP)pDev->Framebuffer.HeapHandle)->memHandle, // generated handle
                        NV01_MEMORY_LOCAL_USER,
                        *pAddress,
                        *pSize,
                        0x0,
                        owner,
                        0           //version = 0 because the HeapOwner is just an ID without an allocated object behind it - C.Moidel
                    );
                if (rmStatus != RM_OK)
                {
                    // return the failed rmStatus from CliAddMemory not heapFree
                    (VOID)heapFree
                    (
                        pDev,
                        (PHEAP)pDev->Framebuffer.HeapHandle,
                        owner,
                        *pOffset,
                        &memHandle
                    );
                }
            }
            *pFree   = ((PHEAP)pDev->Framebuffer.HeapHandle)->free;
            *pTotal  = ((PHEAP)pDev->Framebuffer.HeapHandle)->total;

            break;

        //
        // Free mem.
        //
        case NVOS11_HEAP_FREE:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM:    Function: FREE\n");
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Type:", type);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Owner:", owner);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Offset:", *pOffset);

            //
            // OpenGL might try to call heapfree (via an ESCAPE call) before NT
            // ever gets the chance to call RmLoadState (in SetPowerState).
            // So to be safe, make sure the HeapHandle is not null.
            //

            if ( ((PHEAP)pDev->Framebuffer.HeapHandle) != NULL)
            {
                rmStatus =
                    heapFree
                    (
                        pDev,
                        (PHEAP)pDev->Framebuffer.HeapHandle,
                        owner,
                        *pOffset,
                        &memHandle  // return the memory handle used
                    );

                *pFree   = ((PHEAP)pDev->Framebuffer.HeapHandle)->free;
                *pTotal  = ((PHEAP)pDev->Framebuffer.HeapHandle)->total;

                if (!CliDelDeviceMemory(hObjectParent, memHandle))
                {
                    rmStatus |= RM_ERR_BAD_OBJECT_HANDLE;
                }
            }

            else
            {
                rmStatus = NVOS11_STATUS_ERROR_INVALID_HEAP;
            }

            break;

        //
        // Purge mem.
        //
        case NVOS11_HEAP_PURGE:

            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM:    Function: PURGE\n");
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Type:", type);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Owner:", owner);
#ifdef DEBUG
            oldFree = ((PHEAP)pDev->Framebuffer.HeapHandle)->free;
#endif
            rmStatus = heapPurge(pDev, (PHEAP)pDev->Framebuffer.HeapHandle, owner);
            *pFree   = ((PHEAP)pDev->Framebuffer.HeapHandle)->free;
            *pTotal  = ((PHEAP)pDev->Framebuffer.HeapHandle)->total;
#ifdef DEBUG
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Blocks puged:", *pFree-oldFree);
#endif
/*
#ifdef DEBUG
            if (!(*pFree-oldFree))
            {
                DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM:    You did a PURGE for NO REASON!\n");
            };
#endif
*/

            if (!CliDelDeviceHeapOwner(hObjectParent, owner))
            {
                rmStatus |= RM_ERR_BAD_OBJECT_HANDLE;
            }

            break;

        //
        // Info.
        //
        case NVOS11_HEAP_INFO:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM:    Function: INFO\n");
            rmStatus =
                heapInfo
                (
                    (PHEAP)pDev->Framebuffer.HeapHandle,
                    pFree,
                    pTotal,
                    pAddress,  // base of heap
                    pOffset,   // base of largest free block
                    pSize      // size of largest free block
                );
            if (rmStatus == RM_OK)
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     Free:", *pFree);
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Total:", *pTotal);
                DBG_PRINT_STRING_PTR  (DEBUGLEVEL_TRACEINFO,  "NVRM:     Base:", *pAddress);
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:  MaxFree:", *pSize);
            }

            break;

        //
        // Info with freed blocks.
        //
        // The offsets passed in are considered freed for calculation purposes, but
        // aren't actually freed. Passing -1 as an offset arg indicates offset doesn't
        // contain a valid arg (use the NVOS11_INVALID_BLOCK_FREE_OFFSET #define).
        //
        case NVOS11_HEAP_INFO_FREE_BLOCKS:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM:    Function: INFO FREE BLOCKS\n");
            rmStatus = 
                heapInfoFreeBlocks
                (
                    (PHEAP)pDev->Framebuffer.HeapHandle,
                    pFree,
                    pTotal,
                    *pOffset,   // offset of block to be considered free
                    *pLimit,    // offset of block to be considered free
                    pSize       // size of resulting largest free block
                );    
            if (rmStatus == RM_OK)
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     Free:", *pFree);
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Total:", *pTotal);
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:  MaxFree:", *pSize);
            }
            
            break;
            
        //
        // Destroy.
        //
        case NVOS11_HEAP_DESTROY:
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,  "NVRM:    Function: DESTROY\n");
            rmStatus = heapDestroy(pDev, (PHEAP)pDev->Framebuffer.HeapHandle);
            if (rmStatus == RM_OK)
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:     Free:",
                    ((PHEAP)pDev->Framebuffer.HeapHandle)->free);
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,  "NVRM:    Total:",
                    ((PHEAP)pDev->Framebuffer.HeapHandle)->total);
            }

            break;

        //
        // Invalid.
        //
        default:
            rmStatus = NVOS11_STATUS_ERROR_INVALID_FUNCTION;
            break;
    }

    return rmStatus;

} // end of RmArchHeap

NvV32 RmConfigVersion
(
    U032 hClient,
    U032 hDevice,
    U032* pVersion
)
{
    PHWINFO pDev;

    // set the client context
    if (!CliSetClientContext(hClient))
    {
        return RM_ERR_OPERATING_SYSTEM;
    }

    // set the correct device context
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        return RM_ERR_INSUFFICIENT_RESOURCES;
    }

    // call the kernel for help
    //*pVersion = stateConfigVersion(pDev);
    *pVersion = 0x01000000;

    return NVOS12_STATUS_SUCCESS;

} // end of RmConfigVersion

NvV32 RmConfigGet
(
    U032 hClient,
    U032 hDevice,
    U032 index,
    U032* pValue
)
{
    RM_STATUS rmStatus = NVOS13_STATUS_SUCCESS;
    PHWINFO pDev;

    // set the client context
    if (!CliSetClientContext(hClient))
    {
        return NVOS13_STATUS_ERROR_BAD_CLIENT;
    }

    // set the correct device context
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        return NVOS13_STATUS_ERROR_BAD_DEVICE;
    }

    // call the kernel for help
    rmStatus = stateConfigGet(pDev, index, pValue);
    if (!RM_SUCCESS(rmStatus))
    {
        rmStatus = NVOS13_STATUS_ERROR_BAD_INDEX;
    }

    return rmStatus;

} // end of RmConfigGet()

NvV32 RmConfigSet
(
    U032 hClient,
    U032 hDevice,
    U032 index,
    U032* pOldValue,
    U032 newValue
)
{
    RM_STATUS rmStatus = NVOS14_STATUS_SUCCESS;
    PHWINFO pDev;

    // set the client context
    if (!CliSetClientContext(hClient))
    {
        return NVOS14_STATUS_ERROR_BAD_CLIENT;
    }

    // set the correct device context
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        return NVOS14_STATUS_ERROR_BAD_DEVICE;
    }

    // access the kernel state
    rmStatus = stateConfigSet(pDev, index, newValue, pOldValue);
    if (!RM_SUCCESS(rmStatus))
    {
        rmStatus = NVOS14_STATUS_ERROR_BAD_INDEX;
    }

    return rmStatus;

} // end of RmConfigSet()

NvV32 RmConfigUpdate
(
    U032 hClient,
    U032 hDevice,
    U032 flags
)
{
    RM_STATUS rmStatus = NVOS15_STATUS_SUCCESS;
    PHWINFO pDev;

    // set the client context
    if (!CliSetClientContext(hClient))
    {
        return RM_ERR_OPERATING_SYSTEM;
    }

    // set the correct device context
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        return RM_ERR_INSUFFICIENT_RESOURCES;
    }

    // call the kernel for help
    //stateConfigUpdate(pDev, flags);

    return rmStatus;

} // end of RmConfigUpdate

NvV32 RmConfigGetEx
(
    U032 hClient,
    U032 hDevice,
    U032 index,
    VOID* pParms,
    U032 parmsSize
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev;

    // set the client context
    if (!CliSetClientContext(hClient))
    {
        return NVOS_CGE_STATUS_ERROR_BAD_CLIENT;
    }

    // set the correct device context
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        return NVOS_CGE_STATUS_ERROR_BAD_DEVICE;
    }

    // call the kernel for help
    rmStatus =
        stateConfigGetEx
        (
            pDev,
            index,
            pParms,
            parmsSize
        );

    return RmArchStatus(rmStatus, NV04_CONFIG_GET_EX);

} // end of RmConfigGetEx

NvV32 RmConfigSetEx
(
    U032 hClient,
    U032 hDevice,
    U032 index,
    VOID* pParms,
    U032 parmsSize
)
{
    RM_STATUS rmStatus = RM_OK;
    PHWINFO pDev;

    // set the client context
    if (!CliSetClientContext(hClient))
    {
        return NVOS_CSE_STATUS_ERROR_BAD_CLIENT;
    }

    // set the correct device context
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        return NVOS_CSE_STATUS_ERROR_BAD_DEVICE;
    }

    // call the kernel for help
    rmStatus =
        stateConfigSetEx
        (
            pDev,
            index,
            pParms,
            parmsSize
        );

    return RmArchStatus(rmStatus, NV04_CONFIG_SET_EX);

} // end of RmConfigSetEx

NvV32 RmInterrupt
(
    U032 hClient,
    U032 hDevice
)
{
    PHWINFO pDev;

    // set the client context
    if (!CliSetClientContext(hClient))
    {
        return NVOS09_STATUS_ERROR_BAD_CLIENT;
    }

    // set the correct device context
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        return NVOS09_STATUS_ERROR_BAD_DEVICE;
    }

    // call the os ISR service
    osIsr(pDev);

    return NVOS09_STATUS_SUCCESS;

} // end of RmInterrupt()

RM_STATUS RmAllocDeviceInstance
(
    U032 *pInstance
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 instance, i, instanceSize;

    // allocate the next available device instance
    for (instance = 0; instance < MAX_INSTANCE; instance++)
    {
        if (NvDBPtr_Table[instance] == NULL)
        {
            // allocate and init the device information structure
            instanceSize = sizeof(HWINFO);
            rmStatus = osAllocMem((VOID **)&NvDBPtr_Table[instance], instanceSize);
            if (RM_SUCCESS(rmStatus))
            {
                // clear out the new instance
                for (i = 0; i < instanceSize; i++)
                {
                     ((U008 *)(NvDBPtr_Table[instance]))[i] = 0;
                }
                
                // set the instance self-reference in the device instance
                NvDBPtr_Table[instance]->DBdevInstance = instance;

                // return the instance
                *pInstance = instance;

                // setup the pointer to the global NvRmInfo
                NvDBPtr_Table[instance]->pRmInfo = (VOID_PTR) &NvRmInfo;
            }
            break;
        }
    }
    if (instance == MAX_INSTANCE)
    {
        rmStatus = RM_ERROR;
    }

    return rmStatus;

}

NvV32 RmArchStatus(RM_STATUS rmStatus, U032 archFct)
{
    switch (archFct)
    {
        case NV01_FREE:
            switch (rmStatus)
            {
                case RM_OK:
                    return NVOS00_STATUS_SUCCESS;
                case RM_ERR_OPERATING_SYSTEM:
                    return NVOS00_STATUS_ERROR_OPERATING_SYSTEM;
                case RM_ERR_BAD_OBJECT_PARENT:
                    return NVOS00_STATUS_ERROR_BAD_OBJECT_PARENT;
                case RM_ERR_BAD_OBJECT_HANDLE:
                    return NVOS00_STATUS_ERROR_BAD_OBJECT_OLD;
                case RM_ERR_OBJECT_IN_USE:
                    return NVOS00_STATUS_ERROR_OBJECT_IN_USE;
                case RM_ERR_OBJECT_HAS_CHILDERN:
                    return NVOS00_STATUS_ERROR_OBJECT_HAS_CHILDERN;
                default:
                    return rmStatus;
            }
        
        case NV01_ROOT_USER:
        case NV01_ALLOC_ROOT:
            switch (rmStatus)
            {
                case RM_OK:
                    return NVOS01_STATUS_SUCCESS;
                case RM_ERR_OPERATING_SYSTEM:
                    return NVOS01_STATUS_ERROR_OPERATING_SYSTEM;
                case RM_ERR_BAD_OBJECT_PARENT:
                    return NVOS01_STATUS_ERROR_BAD_OBJECT_PARENT;
                case RM_ERR_BAD_OBJECT_HANDLE:
                    return NVOS01_STATUS_ERROR_BAD_OBJECT_NEW;
                case RM_ERR_BAD_CLASS:
                    return NVOS01_STATUS_ERROR_BAD_CLASS;
                case RM_ERR_INSUFFICIENT_RESOURCES:
                    return NVOS01_STATUS_ERROR_INSUFFICIENT_RESOURCES;
                default:
                    return rmStatus;
            }

        case NV01_ALLOC_MEMORY:
            switch (rmStatus)
            {
                case RM_OK:
                    return NVOS02_STATUS_SUCCESS;
                case RM_ERR_OPERATING_SYSTEM:
                    return NVOS02_STATUS_ERROR_OPERATING_SYSTEM;
                case RM_ERR_BAD_OBJECT_PARENT:
                    return NVOS02_STATUS_ERROR_BAD_OBJECT_PARENT;
                case RM_ERR_BAD_OBJECT_HANDLE:
                    return NVOS02_STATUS_ERROR_BAD_OBJECT_NEW;
                case RM_ERR_BAD_CLASS:
                    return NVOS02_STATUS_ERROR_BAD_CLASS;
                case RM_ERR_INSUFFICIENT_RESOURCES:
                    return NVOS02_STATUS_ERROR_INSUFFICIENT_RESOURCES;
                default:
                    return rmStatus;
            }

        case NV01_ALLOC_CONTEXT_DMA:
            switch (rmStatus)
            {
                case RM_OK:
                    return NVOS03_STATUS_SUCCESS;
                case RM_ERR_OPERATING_SYSTEM:
                    return NVOS03_STATUS_ERROR_OPERATING_SYSTEM;
                case RM_ERR_BAD_OBJECT_PARENT:
                    return NVOS03_STATUS_ERROR_BAD_OBJECT_PARENT;
                case RM_ERR_BAD_OBJECT_HANDLE:
                    return NVOS03_STATUS_ERROR_BAD_OBJECT_NEW;
                case RM_ERR_BAD_CLASS:
                    return NVOS03_STATUS_ERROR_BAD_CLASS;
                case RM_ERR_BAD_FLAGS:
                    return NVOS03_STATUS_ERROR_BAD_FLAGS;
                case RM_ERR_BAD_BASE:
                    return NVOS03_STATUS_ERROR_BAD_BASE;
                case RM_ERR_BAD_LIMIT:
                    return NVOS03_STATUS_ERROR_BAD_LIMIT;
                case RM_ERR_PROTECTION_FAULT:
                    return NVOS03_STATUS_ERROR_PROTECTION_FAULT;
                case RM_ERR_MULTIPLE_MEMORY_TYPES:
                    return NVOS03_STATUS_ERROR_MULTIPLE_MEMORY_TYPES;
                case RM_ERR_INSUFFICIENT_RESOURCES:
                    return NVOS03_STATUS_ERROR_INSUFFICIENT_RESOURCES;
                default:
                    return rmStatus;
            }

        case NV01_ALLOC_CHANNEL_PIO:
            switch (rmStatus)
            {
                case RM_OK:
                    return NVOS04_STATUS_SUCCESS;
                case RM_ERR_OPERATING_SYSTEM:
                    return NVOS04_STATUS_ERROR_OPERATING_SYSTEM;
                case RM_ERR_BAD_OBJECT_PARENT:
                    return NVOS04_STATUS_ERROR_BAD_OBJECT_PARENT;
                case RM_ERR_BAD_OBJECT_HANDLE:
                    return NVOS04_STATUS_ERROR_BAD_OBJECT_NEW;
                case RM_ERR_BAD_CLASS:
                    return NVOS04_STATUS_ERROR_BAD_CLASS;
                case RM_ERR_BAD_OBJECT_ERROR:
                    return NVOS04_STATUS_ERROR_BAD_OBJECT_ERROR;
                case RM_ERR_BAD_FLAGS:
                    return NVOS04_STATUS_ERROR_BAD_FLAGS;
                case RM_ERR_INSUFFICIENT_RESOURCES:
                case RM_ERR_NO_FREE_FIFOS:
                    return NVOS04_STATUS_ERROR_INSUFFICIENT_RESOURCES;
                default:
                    return rmStatus;
            }

        case NV01_ALLOC_OBJECT:
            switch (rmStatus)
            {
                case RM_OK:
                    return NVOS05_STATUS_SUCCESS;
                case RM_ERR_OPERATING_SYSTEM:
                    return NVOS05_STATUS_ERROR_OPERATING_SYSTEM;
                case RM_ERR_BAD_OBJECT_PARENT:
                    return NVOS05_STATUS_ERROR_BAD_OBJECT_PARENT;
                case RM_ERR_BAD_OBJECT_HANDLE:
                    return NVOS05_STATUS_ERROR_BAD_OBJECT_NEW;
                case RM_ERR_BAD_CLASS:
                    return NVOS05_STATUS_ERROR_BAD_CLASS;
                case RM_ERR_INSUFFICIENT_RESOURCES:
                    return NVOS05_STATUS_ERROR_INSUFFICIENT_RESOURCES;
                default:
                    return rmStatus;
            }

        case NV04_ALLOC:
            switch (rmStatus)
            {
                case RM_OK:
                    return NVOS21_STATUS_SUCCESS;
                case RM_ERR_OPERATING_SYSTEM:
                    return NVOS21_STATUS_ERROR_OPERATING_SYSTEM;
                case RM_ERR_BAD_CLIENT:
                    return NVOS21_STATUS_ERROR_BAD_ROOT;
                case RM_ERR_BAD_OBJECT_PARENT:
                    return NVOS21_STATUS_ERROR_BAD_OBJECT_PARENT;
                case RM_ERR_BAD_OBJECT_HANDLE:
                    return NVOS21_STATUS_ERROR_BAD_OBJECT_NEW;
                case RM_ERR_BAD_CLASS:
                    return NVOS21_STATUS_ERROR_BAD_CLASS;
                case RM_ERR_INSUFFICIENT_RESOURCES:
                    return NVOS21_STATUS_ERROR_INSUFFICIENT_RESOURCES;
                default:
                    return rmStatus;
            }

        case NV01_ALLOC_DEVICE:
            switch (rmStatus)
            {
                case RM_OK:
                    return NVOS06_STATUS_SUCCESS;
                case RM_ERR_OPERATING_SYSTEM:
                    return NVOS06_STATUS_ERROR_OPERATING_SYSTEM;
                case RM_ERR_BAD_OBJECT_PARENT:
                    return NVOS06_STATUS_ERROR_BAD_OBJECT_PARENT;
                case RM_ERR_BAD_OBJECT_HANDLE:
                    return NVOS06_STATUS_ERROR_BAD_OBJECT_NEW;
                case RM_ERR_BAD_CLASS:
                    return NVOS06_STATUS_ERROR_BAD_CLASS;
                case RM_ERR_INSUFFICIENT_RESOURCES:
                    return NVOS06_STATUS_ERROR_INSUFFICIENT_RESOURCES;
                default:
                    return rmStatus;
            }

        case NV03_ALLOC_CHANNEL_DMA:
            switch (rmStatus)
            {
                case RM_OK:
                    return NVOS07_STATUS_SUCCESS;
                case RM_ERR_OPERATING_SYSTEM:
                    return NVOS07_STATUS_ERROR_OPERATING_SYSTEM;
                case RM_ERR_BAD_OBJECT_PARENT:
                    return NVOS07_STATUS_ERROR_BAD_OBJECT_PARENT;
                case RM_ERR_BAD_OBJECT_HANDLE:
                    return NVOS07_STATUS_ERROR_BAD_OBJECT_NEW;
                case RM_ERR_BAD_CLASS:
                    return NVOS07_STATUS_ERROR_BAD_CLASS;
                case RM_ERR_BAD_OBJECT_ERROR:
                    return NVOS07_STATUS_ERROR_BAD_OBJECT_ERROR;
                case RM_ERR_BAD_OBJECT_BUFFER:
                    return NVOS07_STATUS_ERROR_BAD_OBJECT_BUFFER;
                case RM_ERR_BAD_OFFSET:
                    return NVOS07_STATUS_ERROR_BAD_OFFSET;
                case RM_ERR_INSUFFICIENT_RESOURCES:
                case RM_ERR_NO_FREE_FIFOS:
                    return NVOS07_STATUS_ERROR_INSUFFICIENT_RESOURCES;
                default:
                    return rmStatus;
            }

        case NV04_CONFIG_GET_EX:
            switch (rmStatus)
            {
                case RM_OK:
                    return NVOS_CGE_STATUS_SUCCESS;
                case RM_ERR_OPERATING_SYSTEM:
                    return NVOS_CGE_STATUS_ERROR_OPERATING_SYSTEM;
                case CONFIG_GETEX_BAD_INDEX:
                    return NVOS_CGE_STATUS_ERROR_BAD_INDEX;
                case RM_ERR_INSUFFICIENT_RESOURCES:
                    return NVOS_CGE_STATUS_ERROR_INSUFFICIENT_RESOURCES;
                case CONFIG_GETEX_BAD_PARAM:
                    return NVOS_CGE_STATUS_ERROR_BAD_PARAM_STRUCT;
            }

        case NV04_CONFIG_SET_EX:
            switch (rmStatus)
            {
                case RM_OK:
                    return NVOS_CSE_STATUS_SUCCESS;
                case RM_ERR_OPERATING_SYSTEM:
                    return NVOS_CSE_STATUS_ERROR_OPERATING_SYSTEM;
                case CONFIG_SETEX_BAD_INDEX:
                    return NVOS_CSE_STATUS_ERROR_BAD_INDEX;
                case RM_ERR_INSUFFICIENT_RESOURCES:
                    return NVOS_CSE_STATUS_ERROR_INSUFFICIENT_RESOURCES;
                case CONFIG_SETEX_BAD_PARAM:
                    return NVOS_CSE_STATUS_ERROR_BAD_PARAM_STRUCT;
            }

        default:
            return rmStatus;
    }

} // end of RmArchStatus()

NvV32 RmI2CAccess
(
    U032  hClient,
    U032  hDevice,
    VOID*  pCtrl
)
{
    RM_STATUS   rmStatus;
    PHWINFO pDev;
    U032        Head = 0;   // TO DO: specify which head
    // set the client context
    if (!CliSetClientContext(hClient))
    {
        return NVOS_I2C_ACCESS_STATUS_ERROR_BAD_CLIENT;
    }

    // set the correct device context
    // also checking for NULL pDev.
    if (!CliSetDeviceContext(hDevice, &pDev) || !pDev)
    {
        return NVOS_I2C_ACCESS_STATUS_ERROR_BAD_DEVICE;
    }

    // call the kernel for help
    rmStatus = i2cAccess(pDev, Head, pCtrl);

    return rmStatus;

} // end of RmI2CAccess()

NvV32 RmDirectMethodCall
(
    U032 hClient,
    U032 hChannel,
    U032 hObject,
    U032 offset,
    U032 data
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 hDevice;
    PCLI_FIFO_INFO pFifo;
    POBJECT pObject;
    PHWINFO pDev;

    // TO DO: it is necessary to be very restrictive as to which methods can be called -- is there a better test?
    // check for valid methods
    switch (offset)
    {
        case NV046_SET_CONTEXT_DMA_NOTIFIES:
        case NV046_SET_CONTEXT_DMA_LUT(0):
        case NV046_SET_LUT_OFFSET(0):
        case NV046_SET_LUT_FORMAT(0):
        case NV046_SET_CURSOR_POINT:
        case NV046_SET_DAC_IMAGE_SIZE(0):
        case NV046_SET_DAC_HORIZONTAL_BLANK(0):
        case NV046_SET_DAC_HORIZONTAL_SYNC(0):
        case NV046_SET_DAC_VERTICAL_BLANK(0):
        case NV046_SET_DAC_VERTICAL_SYNC(0):
        case NV046_SET_DAC_TOTAL_SIZE(0):
        case NV046_SET_DAC_PIXEL_CLOCK(0):
        case NV046_SET_DAC_FORMAT(0):
            break;

        default:
            return NVOS1D_STATUS_ERROR_BAD_METHOD;
    }

    // set the client context
    if (!CliSetClientContext(hClient))
    {
        return NVOS1D_STATUS_ERROR_OPERATING_SYSTEM;
    }

    // set the corresponding device context
    if (!CliGetDeviceHandle(hChannel, &hDevice))
    {
        return NVOS1D_STATUS_ERROR_BAD_OBJECT_PARENT;
    }
    if (!CliSetDeviceContext(hDevice, &pDev))
    {
        return NVOS1D_STATUS_ERROR_BAD_OBJECT_PARENT;
    }

    // get the object
    if (!CliGetDeviceFifoInfo(hDevice, hChannel, &pFifo))
    {
        return NVOS1D_STATUS_ERROR_BAD_OBJECT_PARENT;
    }
    rmStatus = fifoSearchObject(pDev, hObject, pFifo->ChID, &pObject);
    if (!RM_SUCCESS(rmStatus))
    {
        return NVOS1D_STATUS_ERROR_BAD_OBJECT_OLD;
    }

    // call the method directly and safely
    rmStatus = classDirectSoftwareMethod(pDev, pObject, offset, data);
    if (!RM_SUCCESS(rmStatus))
    {
        return NVOS1D_STATUS_ERROR_BAD_DATA;
    }

    return NVOS1D_STATUS_SUCCESS;

} // end of RmDirectMethodCall()

/*
 * Control various debugging facilities in the resource manager
 */

VOID Nv01DebugControl(
    NVOS20_PARAMETERS * configParams
)
{
    RM_STATUS rmStatus = NVOS20_STATUS_SUCCESS;

    switch (configParams->command)
    {
        case NVOS20_COMMAND_RM_FAILURE_ENABLE:
            RM_FAILURE_ENABLE();
            break;

        case NVOS20_COMMAND_RM_FAILURE_DISABLE:
            RM_FAILURE_DISABLE();
            break;
    }

    configParams->status = rmStatus;
} // end of RmDebugControl

RM_STATUS RmDebugEx(
	U032 hClient,
    U032 hObject,
	U032 hClass,
    VOID* pAllocParms
)
{
    RM_STATUS status = RM_OK;

    switch (hClass)
    {
        case NVDE_DEBUGGER:
        {
            status = NvdeAlloc(hObject, pAllocParms);
            break;
        }
        case NVDE_REGISTER_DEBUGGER:
        {
            if (hClient == 0)
                return NVDE_STATUS_ERROR_BAD_ARGUMENT;
            if (NvRmInfo.Debugger.Handle != 0)
                return NVDE_STATUS_ERROR_STATE_IN_USE;

            NvRmInfo.Debugger.Handle = hClient;
            break;
        }
        case NVDE_UNREGISTER_DEBUGGER:
        {
            if (NvRmInfo.Debugger.Handle != hClient)
                return NVDE_STATUS_ERROR_BAD_ARGUMENT;
            NvRmInfo.Debugger.Handle = 0;
            break;
        }
        case NVDE_READ_MEMORY:
        {
            NVDE_READ_MEMORY_PARAMETERS *pNvDEParams;
            U008 *in, *out;
            U032 i;

            if (NvRmInfo.Debugger.Handle != hClient)
                return NVDE_STATUS_ERROR_BAD_ARGUMENT;

            //
            // Not bothering with osCopyin/osCopyout here because
            // this code will soon be moved to the DMC interface.
            // 
            // bdw: we need 32-bit xfers to be atomic - please retain
            //      this notion when we port this.
            //
            pNvDEParams = (NVDE_READ_MEMORY_PARAMETERS *)pAllocParms;
            
            in  = (U008 *)pNvDEParams->Data;
            out = (U008 *)pNvDEParams->Address;
            i   = pNvDEParams->Size;
            while (i >= 4)
            {
                *(U032*)in = *(U032*)out;
                in  += 4;
                out += 4;
                i   -= 4;
            }
            while (i)
            {
                *in = *out;
                in  += 1;
                out += 1;
                i   -= 1;
            }
            break;
        }
        case NVDE_WRITE_MEMORY:
        {
            NVDE_WRITE_MEMORY_PARAMETERS *pNvDEParams;
            U008 *in, *out;
            U032 i;

            if (NvRmInfo.Debugger.Handle != hClient)
                return NVDE_STATUS_ERROR_BAD_ARGUMENT;

            //
            // Not bothering with osCopyin/osCopyout here because
            // this code will soon be moved to the DMC interface.
            // 
            // bdw: we need 32-bit xfers to be atomic - please retain
            //      this notion when we port this.
            //
            pNvDEParams = (NVDE_WRITE_MEMORY_PARAMETERS *)pAllocParms;
            
            in  = (U008 *)pNvDEParams->Address;
            out = (U008 *)pNvDEParams->Data;
            i   = pNvDEParams->Size;
            while (i >= 4)
            {
                *(U032*)in = *(U032*)out;
                in  += 4;
                out += 4;
                i   -= 4;
            }
            while (i)
            {
                *in = *out;
                in  += 1;
                out += 1;
                i   -= 1;
            }
            break;
        }
        case NVDE_TEST:
        {
            NVDE_TEST_PARAMETERS *pNvDEParams = (NVDE_TEST_PARAMETERS *)pAllocParms;
            NVDE_TEST_FUNC        pfn         = (NVDE_TEST_FUNC)pNvDEParams->CallBackAddress;

            (pfn)();

            break;
        }
        case NVDE_GET_EVENT_INFORMATION:
        {
            NVDE_GET_EVENT_INFORMATION_PARAMETERS *pNvDEParams = (NVDE_GET_EVENT_INFORMATION_PARAMETERS*)pAllocParms;
            U032 *s, *d;
            int  i;

            pNvDEParams->Count = ((PNVDEOBJECT)NvRmInfo.Debugger.object)->EventBufferCount;
            s = &((PNVDEOBJECT)NvRmInfo.Debugger.object)->EventBuffer[0].code;
            d = &pNvDEParams->Event[0][0];
            for (i = 0; i < NVDE_MAX_EVENTS * sizeof(NVDEEVENT) / 4; i++)
            {
                *d = *s;
                s++;
                d++;
            }

            // reset events
            ((PNVDEOBJECT)NvRmInfo.Debugger.object)->EventBufferCount = 0;

            break;
        }
    }

    return status;

} // end of RmDebugEx()

// end of nvapi.c
