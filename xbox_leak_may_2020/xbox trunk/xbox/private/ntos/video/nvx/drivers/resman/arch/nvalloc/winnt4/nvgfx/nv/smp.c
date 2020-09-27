/******************************************************************************
*
*   Module: smp.c
*
*   Description:
*       This module contains SMP-safe versions of calls to functions that
*   contain critical code.
*
*       Each SMP-safe wrapper has the same name as the function it replaces
*   prepended with "smp".  Each requires a macro, to be included in those files
*   that call the "wrapped" functions, that replaces the call to the original
*   function with a call to the wrapper.  Currently, the header file that
*   contains these macros is called "smp.h".  The wrapper function packages
*   up the args into a single, custom arg struct, then calls
*   VideoPortSynchronizeExecution(), specifying a pointer to the arg struct
*   and a callback function.  The callback has the same name as the original
*   function prepended with "smpCallback".  The callback calls the original
*   function with the correct members of the arg struct.
*
*
*   THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
*   NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
*   IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
*
******************************************************************************/

#include <miniport.h>
#include <ntddvdeo.h>
#include <video.h>
#include <nvrm.h>
#include <os.h>

// external prototypes
RM_STATUS fifoUpdateObjectInstance(
    PHWINFO pDev,
    POBJECT Object,
    U032    ChID,
    U032    Instance
);
RM_STATUS fifoDeleteObjectInstance(
    PHWINFO pDev,
    POBJECT Object,
    U032    ChID
);
RM_STATUS fifoResetChannelState(
    PHWINFO pDev,
    U032 ChID
);
RM_STATUS fifoDeleteSubchannelContextEntry(
    PHWINFO pDev,
    POBJECT Object
);
RM_STATUS nvHalFbSetAllocParameters(
    PHWINFO pDev,
    PFBALLOCINFO pFbAllocInfo
);
RM_STATUS nvHalFbFree(
    PHWINFO pDev,
    U032 hwResId
);
VOID VBlankDeleteCallback(
    PHWINFO pDev,
    U032 Head,
    PVBLANKCALLBACK Callback
);
RM_STATUS classDirectSoftwareMethod
(
    PHWINFO pDev,
    POBJECT Object,
    U032    Offset,
    V032    Data
);
// local prototypes
BOOLEAN smpCallbackFifoUpdateObjectInstance(PVOID object);
BOOLEAN smpCallbackFifoDeleteObjectInstance(PVOID object);
BOOLEAN smpCallbackFifoResetChannelState(PVOID object);
BOOLEAN smpCallbackFifoDeleteSubchannelContextEntry(PVOID object);
BOOLEAN smpCallbackMcService(PVOID object);
BOOLEAN smpCallbackNvHalFbSetAllocParameters(PVOID object);
BOOLEAN smpCallbackNvHalFbFree(PVOID object);
BOOLEAN smpCallbackVBlankDeleteCallback(PVOID object);
BOOLEAN smpCallbackClassDirectSoftwareMethod(PVOID object);

// arg strucs
typedef struct
{
    PHWINFO pDev;
    POBJECT Object;
    U032    ChID;
    U032    Instance;
} ARGS_FifoUpdateObjectInstance, *PARGS_FifoUpdateObjectInstance;
typedef struct
{
    PHWINFO pDev;
    POBJECT Object;
    U032    ChID;
} ARGS_FifoDeleteObjectInstance, *PARGS_FifoDeleteObjectInstance;
typedef struct
{
    PHWINFO pDev;
    U032    ChID;
} ARGS_FifoResetChannelState, *PARGS_FifoResetChannelState;
typedef struct
{
    PHWINFO pDev;
    POBJECT Object;
} ARGS_FifoDeleteSubchannelContextEntry, *PARGS_FifoDeleteSubchannelContextEntry;
typedef struct
{
    PHWINFO pDev;
} ARGS_McService, *PARGS_McService;
typedef struct
{
    PHWINFO pDev;
    PFBALLOCINFO pFbAllocInfo;
} ARGS_NvHalFbSetAllocParameters, *PARGS_NvHalFbSetAllocParameters;
typedef struct
{
    PHWINFO pDev;
    U032    hwResId;
} ARGS_NvHalFbFree, *PARGS_NvHalFbFree;
typedef struct
{
    PHWINFO pDev;
    U032 Head;
    PVBLANKCALLBACK Callback;
} ARGS_VBlankDeleteCallback, *PARGS_VBlankDeleteCallback;
typedef struct
{
    PHWINFO pDev;
    POBJECT Object;
    U032    Offset;
    V032    Data;
} ARGS_ClassDirectSoftwareMethod, *PARGS_ClassDirectSoftwareMethod;

// critical code wrappers

// critical code wrapper for fifoUpdateObjectInstance()
RM_STATUS smpFifoUpdateObjectInstance
(
    PHWINFO pDev,
    POBJECT Object,
    U032    ChID,
    U032    Instance
)
{
    BOOLEAN bStatus;
    ARGS_FifoUpdateObjectInstance args;

    // load the arg struct
    args.pDev = pDev;
    args.Object = Object;
    args.ChID = ChID;
    args.Instance = Instance;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: Synchronizing smpFifoUpdateObjectInstance...\n");

    bStatus = VideoPortSynchronizeExecution(
        pDev->DBmpDeviceExtension,
        VpMediumPriority,
        smpCallbackFifoUpdateObjectInstance,
        (PVOID)&args
    );

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...smpFifoUpdateObjectInstance unsynchronized\n");

    return (bStatus) ? RM_OK : RM_ERROR;

} // end of smpFifoUpdateObjectInstance()

RM_STATUS smpFifoDeleteObjectInstance
(
    PHWINFO pDev,
    POBJECT Object,
    U032    ChID
)
{
    BOOLEAN bStatus;
    ARGS_FifoDeleteObjectInstance args;

    // load the arg struct
    args.pDev = pDev;
    args.Object = Object;
    args.ChID = ChID;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: Synchronizing smpFifoDeleteObjectInstance...\n");

    bStatus = VideoPortSynchronizeExecution(
        pDev->DBmpDeviceExtension,
        VpMediumPriority,
        smpCallbackFifoDeleteObjectInstance,
        (PVOID)&args
    );

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...smpFifoDeleteObjectInstance unsynchronized\n");

    return (bStatus) ? RM_OK : RM_ERROR;

} // end of smpFifoDeleteObjectInstance()

// critical code wrapper for fifoResetChannelState()
RM_STATUS smpFifoResetChannelState
(
    PHWINFO pDev,
    U032    ChID
)
{
    BOOLEAN bStatus;
    ARGS_FifoResetChannelState args;

    // load the arg struct
    args.pDev = pDev;
    args.ChID = ChID;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: Synchronizing smpFifoResetChannelState...\n");

    bStatus = VideoPortSynchronizeExecution(
        pDev->DBmpDeviceExtension,
        VpMediumPriority,
        smpCallbackFifoResetChannelState,
        (PVOID)&args
    );

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...smpFifoResetChannelState unsynchronized\n");

    return (bStatus) ? RM_OK : RM_ERROR;

} // end of smpFifoResetChannelState()

// critical code wrapper for fifoDeleteSubchannelContextEntry()
RM_STATUS smpFifoDeleteSubchannelContextEntry
(
    PHWINFO pDev,
    POBJECT Object
)
{
    BOOLEAN bStatus;
    ARGS_FifoDeleteSubchannelContextEntry args;

    // load the arg struct
    args.pDev = pDev;
    args.Object = Object;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: Synchronizing smpFifoDeleteSubchannelContextEntry...\n");

    bStatus = VideoPortSynchronizeExecution(
        pDev->DBmpDeviceExtension,
        VpMediumPriority,
        smpCallbackFifoDeleteSubchannelContextEntry,
        (PVOID)&args
    );

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...smpFifoDeleteSubchannelContextEntry unsynchronized\n");

    return (bStatus) ? RM_OK : RM_ERROR;

} // end of smpFifoDeleteSubchannelContextEntry()

// critical code wrapper for nvHalFbSetAllocParameters()
RM_STATUS smpNvHalFbSetAllocParameters
(
    PHWINFO pDev,
    PFBALLOCINFO pFbAllocInfo
)
{
    RM_STATUS rmStatus;
    BOOLEAN bStatus;    
    ARGS_NvHalFbSetAllocParameters args;

    //
    // This wrapper only deploys if we have hardware resources
    // associated with the specified block of local memory.
    //
    if (pFbAllocInfo->hwResId)
    {
        // load the arg struct
        args.pDev = pDev;
        args.pFbAllocInfo = pFbAllocInfo;

        //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: Synchronizing smpNvHalFbSetAllocParameters...\n");

        bStatus = VideoPortSynchronizeExecution(
            pDev->DBmpDeviceExtension,
            VpMediumPriority,
            smpCallbackNvHalFbSetAllocParameters,
            (PVOID)&args
            );

        //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...smpNvHalFbSetAllocParameters unsynchronized\n");

        rmStatus = (bStatus) ? RM_OK : RM_ERROR;
    }
    else
    {
        //
        // No MP issues in this case...
        //
        rmStatus = nvHalFbSetAllocParameters(pDev, pFbAllocInfo);
    }

    return (rmStatus);

} // end of smpNvHalFbSetAllocParameters()

// critical code wrapper for nvHalFbFree()
RM_STATUS smpNvHalFbFree
(
    PHWINFO pDev,
    U032 hwResId
)
{
    RM_STATUS rmStatus;
    BOOLEAN bStatus;    
    ARGS_NvHalFbFree args;

    //
    // This wrapper only deploys if we have hardware resources
    // associated with the specified block of local memory.
    //
    if (hwResId)
    {
        // load the arg struct
        args.pDev = pDev;
        args.hwResId = hwResId;

        //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: Synchronizing smpNvHalFbFree...\n");

        bStatus = VideoPortSynchronizeExecution(
            pDev->DBmpDeviceExtension,
            VpMediumPriority,
            smpCallbackNvHalFbFree,
            (PVOID)&args
            );

        //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...smpNvHalFbFree unsynchronized\n");
        rmStatus = (bStatus) ? RM_OK : RM_ERROR;
    }
    else
    {
        //
        // No MP issues in this case...
        //
        rmStatus = nvHalFbFree(pDev, hwResId);
    }

    return (rmStatus);

} // end of smpNvHalFbFree()

// critical code wrapper for VBlankDeleteCallback()
RM_STATUS smpVBlankDeleteCallback
(
    PHWINFO pDev,
    U032 Head,
    PVBLANKCALLBACK Callback
)
{
    BOOLEAN bStatus;    
    ARGS_VBlankDeleteCallback args;

    // load the arg struct
    args.pDev = pDev;
    args.Head = Head;
    args.Callback = Callback;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: Synchronizing smpVBlankDeleteCallback...\n");

    bStatus = VideoPortSynchronizeExecution(
        pDev->DBmpDeviceExtension,
        VpMediumPriority,
        smpCallbackVBlankDeleteCallback,
        (PVOID)&args
    );

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...smpVBlankDeleteCallback unsynchronized\n");

    return (bStatus) ? RM_OK : RM_ERROR;

} // end of smpVBlankDeleteCallback()

// critical code wrapper for ClassDirectSoftwareMethod()
RM_STATUS smpClassDirectSoftwareMethod
(
    PHWINFO pDev,
    POBJECT Object,
    U032 Offset,
    U032 Data
)
{
    BOOLEAN bStatus;    
    ARGS_ClassDirectSoftwareMethod args;

    // load the arg struct
    args.pDev = pDev;
    args.Object = Object;
    args.Offset = Offset;
    args.Data = Data;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: Synchronizing smpClassDirectSoftwareMethod...\n");

    bStatus = VideoPortSynchronizeExecution(
        pDev->DBmpDeviceExtension,
        VpMediumPriority,
        smpCallbackClassDirectSoftwareMethod,
        (PVOID)&args
    );

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...smpClassDirectSoftwareMethod unsynchronized\n");

    return (bStatus) ? RM_OK : RM_ERROR;

} // end of smpClassDirectSoftwareMethod()

// critical code callbacks

// critical code callback for fifoUpdateObjectInstance()
BOOLEAN smpCallbackFifoUpdateObjectInstance(PVOID pvArgs)
{
    RM_STATUS rmStatus;
    PARGS_FifoUpdateObjectInstance pArgs = (PARGS_FifoUpdateObjectInstance)pvArgs;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: calling synchronized fifoUpdateObjectInstance...\n");

    rmStatus = fifoUpdateObjectInstance(
        pArgs->pDev,
        pArgs->Object,
        pArgs->ChID,
        pArgs->Instance
    );

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...returned from synchronized fifoUpdateObjectInstance\n");

    return (rmStatus == RM_OK) ? TRUE : FALSE;
    
} // end of smpCallbackFifoUpdateObjectInstance()

BOOLEAN smpCallbackFifoDeleteObjectInstance(PVOID pvArgs)
{
    RM_STATUS rmStatus;
    PARGS_FifoDeleteObjectInstance pArgs = (PARGS_FifoDeleteObjectInstance)pvArgs;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: calling synchronized fifoDeleteObjectInstance...\n");

    rmStatus = fifoDeleteObjectInstance(
        pArgs->pDev,
        pArgs->Object,
        pArgs->ChID
    );

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...returned from synchronized fifoDeleteObjectInstance\n");

    return (rmStatus == RM_OK) ? TRUE : FALSE;
    
} // end of smpCallbackFifoDeleteObjectInstance()

BOOLEAN smpCallbackFifoResetChannelState(PVOID pvArgs)
{
    RM_STATUS rmStatus;
    PARGS_FifoResetChannelState pArgs = (PARGS_FifoResetChannelState)pvArgs;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: calling synchronized fifoResetChannelState...\n");

    rmStatus = fifoResetChannelState(
        pArgs->pDev,
        pArgs->ChID
    );

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...returned from synchronized fifoResetChannelState\n");

    return (rmStatus == RM_OK) ? TRUE : FALSE;

} // end of smpCallbackFifoResetChannelState()

BOOLEAN smpCallbackFifoDeleteSubchannelContextEntry(PVOID pvArgs)
{
    RM_STATUS rmStatus;
    PARGS_FifoDeleteSubchannelContextEntry pArgs = (PARGS_FifoDeleteSubchannelContextEntry)pvArgs;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: calling synchronized fifoDeleteSubchannelContextEntry...\n");

    rmStatus = fifoDeleteSubchannelContextEntry(
        pArgs->pDev,
        pArgs->Object
    );

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...returned from synchronized fifoDeleteSubchannelContextEntry\n");

    return (rmStatus == RM_OK) ? TRUE : FALSE;

} // end of smpCallbackFifoDeleteSubchannelContextEntry()

BOOLEAN smpCallbackNvHalFbSetAllocParameters(PVOID pvArgs)
{
    RM_STATUS rmStatus;
    PARGS_NvHalFbSetAllocParameters pArgs = (PARGS_NvHalFbSetAllocParameters)pvArgs;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: calling synchronized nvHalFbSetAllocParameters...\n");

    rmStatus = nvHalFbSetAllocParameters(pArgs->pDev, pArgs->pFbAllocInfo);

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...returned from synchronized nvHalFbSetAllocParameters\n");

    return (rmStatus == RM_OK) ? TRUE : FALSE;

} // end of smpCallbackNvHalFbSetAllocParameters

BOOLEAN smpCallbackNvHalFbFree(PVOID pvArgs)
{
    RM_STATUS rmStatus;
    PARGS_NvHalFbFree pArgs = (PARGS_NvHalFbFree)pvArgs;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: calling synchronized nvHalFbFree...\n");

    rmStatus = nvHalFbFree(pArgs->pDev, pArgs->hwResId);

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...returned from synchronized nvHalFbFree\n");

    return (rmStatus == RM_OK) ? TRUE : FALSE;

} // end of smpCallbackNvHalFbFree

BOOLEAN smpCallbackVBlankDeleteCallback(PVOID pvArgs)
{
    PARGS_VBlankDeleteCallback pArgs = (PARGS_VBlankDeleteCallback)pvArgs;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: calling synchronized VBlankDeleteCallback...\n");

    VBlankDeleteCallback(pArgs->pDev, pArgs->Head, pArgs->Callback);

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...returned from synchronized VBlankDeleteCallback\n");

    return RM_OK;

} // end of smpCallbackVBlankDeleteCallback

BOOLEAN smpCallbackClassDirectSoftwareMethod(PVOID pvArgs)
{
    PARGS_ClassDirectSoftwareMethod pArgs = (PARGS_ClassDirectSoftwareMethod)pvArgs;
    RM_STATUS rmStatus;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: calling synchronized classDirectSoftwareMethod...\n");

    rmStatus = classDirectSoftwareMethod(pArgs->pDev, pArgs->Object, pArgs->Offset, pArgs->Data);

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "SMP: ...returned from synchronized classDirectSoftwareMethod\n");

    return (rmStatus == RM_OK) ? TRUE : FALSE;

} // end of smpCallbackClassDirectSoftwareMethod

// end of smp.c

