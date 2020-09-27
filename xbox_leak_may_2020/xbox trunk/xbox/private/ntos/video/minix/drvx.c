/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    drvx.c

Abstract:

    This module contains the code to initialize the video hardware

--*/

#include "minixp.h"
#include <nvntioctl.h>
#include "nv32.h"
#include "nvddobj.h"
#include <nvos.h>

//
// Sends an IOCTL to the miniport by calling MxDeviceIoControl.  To use this macro
// your shoud have these 'status' and 'BytesReturned' locals defined and CleanupAndExit label
//

#define IOCTL_CMD(Code, InBuf, InLen, OutBuf, OutLen) { \
            status = MxDeviceIoControl(Code, InBuf, InLen, OutBuf, OutLen, &BytesReturned); \
            if (!NT_SUCCESS(status)) goto CleanupAndExit; }

//
// Helper macro to check status of the return call
//
#define CHECKSTATUS(x) { status = (x); if (!NT_SUCCESS(status)) goto CleanupAndExit; }
  

extern PMINIX_DRIVER_EXTENSION MxDriverExtension;



typedef struct _VIDEOMODEDEF
{
    USHORT  Mode;
    USHORT  Width;
    USHORT  Height;
    UCHAR   Depth;
    UCHAR   Refresh;

} VIDEOMODEDEF, *PVIDEOMODEDEF;


VIDEOMODEDEF g_VideoModeDef[] =
{
#define DEF_VIDEOMODE(Mode, Width, Height, Depth, Refresh) {Mode, Width, Height, Depth, Refresh},
#include "modes.h"
#undef DEF_VIDOEMODE
};






BOOLEAN
NVStartIO(
    PVOID HwDeviceExtension,
    PVIDEO_REQUEST_PACKET RequestPacket
    );



VOID
pVideoPortMapToNtStatus(
    IN PSTATUS_BLOCK StatusBlock
    );


//
// Nvidia object ids
//

#define DD_DEVICE_OBJECT_HANDLE                     (0x00002002)
#define DD_FRAME_BUFFER_OBJECT_HANDLE               (0x000C0006)

#define DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM          (0x00001022)

//
// Nvidia alloc context dma flags
//
#define ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR                    0x00000000
#define ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY                0x00000001
#define ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY               0x00000002

#define ALLOC_CTX_DMA_FLAGS_CONTIGUOUS_PHYSICALITY          0x00000000
#define ALLOC_CTX_DMA_FLAGS_NONCONTIGUOUS_PHYSICALITY       0x00000010

#define ALLOC_CTX_DMA_FLAGS_LOCKED_ALWAYS                   0x00000000
#define ALLOC_CTX_DMA_FLAGS_LOCKED_IN_TRANSIT               0x00000100

#define ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED              0x00000000
#define ALLOC_CTX_DMA_FLAGS_COHERENCY_CACHED                0x00001000

#define ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED                   ALLOC_CTX_DMA_FLAGS_LOCKED_ALWAYS + ALLOC_CTX_DMA_FLAGS_COHERENCY_CACHED



NTSTATUS
MxDeviceIoControl(
    IN ULONG IoControlCode,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    OUT PULONG BytesReturned
    )

/*++

Routine Description:

    This routine builds an vrp and calls miniport start io function directly.

Arguments:

Return Value:

    Status of the operation

--*/

{
    NTSTATUS status;
    KEVENT event;
    STATUS_BLOCK StatusBlock;
    PHW_DEVICE_EXTENSION HwDeviceExtension;
    PVIDEO_DEVICE_EXTENSION VideoDeviceExtension;
    VIDEO_REQUEST_PACKET vrp;


    //
    // we only support METHOD_BUFFERED
    //

    if ((IoControlCode & 3) != METHOD_BUFFERED) {
        return STATUS_UNSUCCESSFUL;
    }


    HwDeviceExtension = &(MxDriverExtension->HwExtension);
    VideoDeviceExtension = &(MxDriverExtension->VideoExtension);

    KeInitializeEvent(&event, SynchronizationEvent, FALSE);


    //
    // Initialize the vrp
    //
    RtlZeroMemory(&StatusBlock, sizeof(STATUS_BLOCK));

    vrp.IoControlCode = IoControlCode;
    vrp.StatusBlock = &StatusBlock;
    vrp.InputBuffer = InputBuffer;
    vrp.InputBufferLength = InputBufferLength;
    vrp.OutputBuffer = OutputBuffer;
    vrp.OutputBufferLength = OutputBufferLength;

    //
    // Call the start io routine directly
    //

    ACQUIRE_DEVICE_LOCK(VideoDeviceExtension);

    VideoDeviceExtension->HwStartIO(HwDeviceExtension, &vrp);

    RELEASE_DEVICE_LOCK(VideoDeviceExtension);

    if (StatusBlock.Status != NO_ERROR) {
        pVideoPortMapToNtStatus(&StatusBlock);
    }

    status = StatusBlock.Status;

    //
    // If the call is synchronous, the IO is always completed
    // and the Status is the same as the Iosb.Status.
    //

    if (status == STATUS_PENDING) {

        KeWaitForSingleObject(&event, UserRequest, KernelMode, TRUE, NULL);
    }

    *BytesReturned = StatusBlock.Information;


    return status;
}



VOID
pVideoPortMapToNtStatus(
    IN PSTATUS_BLOCK StatusBlock
    )

/*++

Routine Description:

    This function maps a Win32 error code to an NT error code, making sure
    the inverse translation will map back to the original status code.

Arguments:

    StatusBlock - Pointer to the status block

Return Value:

    None.

--*/

{
    PNTSTATUS status = &StatusBlock->Status;

    switch (*status) {

    case ERROR_INVALID_FUNCTION:
        *status = STATUS_NOT_IMPLEMENTED;
        break;

    case ERROR_NOT_ENOUGH_MEMORY:
        *status = STATUS_INSUFFICIENT_RESOURCES;
        break;

    case ERROR_INVALID_PARAMETER:
        *status = STATUS_INVALID_PARAMETER;
        break;

    case ERROR_INSUFFICIENT_BUFFER:
        *status = STATUS_BUFFER_TOO_SMALL;

        //
        // Make sure we zero out the information block if we get an
        // insufficient buffer.
        //

        StatusBlock->Information = 0;
        break;

    case ERROR_MORE_DATA:
        *status = STATUS_BUFFER_OVERFLOW;
        break;

    case ERROR_DEV_NOT_EXIST:
        *status = STATUS_DEVICE_DOES_NOT_EXIST;
        break;

    case ERROR_IO_PENDING:
        ASSERT(FALSE);
        // Fall through.

    case NO_ERROR:
        *status = STATUS_SUCCESS;
        break;

    default:

        ASSERT(FALSE);

        *status = STATUS_UNSUCCESSFUL;

        break;

    }

    return;

} 



NTSTATUS
NvAllocRoot(
    ULONG hClass,
    ULONG *phClient
    )
{
    NVOS01_PARAMETERS parms;
    PVOID pParms;

    ULONG BytesReturned;
    NTSTATUS status;

    parms.hClass = hClass;
    pParms = &parms;


    IOCTL_CMD((ULONG)IOCTL_NV01_ALLOC_ROOT,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        (ULONG)sizeof(NVOS01_PARAMETERS));

    *phClient = (ULONG)parms.hObjectNew;

    status = (parms.status == 0 ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL);

CleanupAndExit:

    return status;

}


NTSTATUS
NvAllocDevice(
    ULONG hClient,
    ULONG hDevice,
    ULONG hClass,
    PUCHAR szName
    )
{
    NVOS06_PARAMETERS parms;
    PVOID pParms;

    ULONG BytesReturned;
    NTSTATUS status;

    parms.hObjectParent     = (NvV32)hClient;
    parms.hObjectNew        = (NvV32)hDevice;
    parms.hClass            = (NvV32)hClass;

#ifdef NV20MINIX
    parms.szName            = (NvP64)szName;
#else
    parms.szName            = (NvV32)szName;
#endif
    
    pParms = &parms;

    IOCTL_CMD((ULONG)IOCTL_NV01_ALLOC_DEVICE,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS06_PARAMETERS));

    status = (parms.status == 0 ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL);

CleanupAndExit:

    return status;
}


NTSTATUS
NvAllocContextDma(
    ULONG hClient,
    ULONG hDma,
    ULONG hClass,
    ULONG flags,
    PVOID base,
    ULONG limit
    )
{
    NVOS03_PARAMETERS parms;
    PVOID pParms;

    ULONG BytesReturned;
    NTSTATUS status;

    parms.hObjectParent     = (NvV32)hClient;
    parms.hObjectNew        = (NvV32)hDma;
    parms.hClass            = (NvV32)hClass;
    parms.flags             = (NvV32)flags;
#ifdef NV20MINIX
    parms.pBase             = (NvP64)base;
    parms.limit             = (NvU64)limit;
#else    
    parms.pBase.selector    = 0x00000000;
    parms.pBase.offset      = (NvU32)base;
    parms.limit.high        = 0x00000000;
    parms.limit.low         = (NvU32)limit;
#endif

    pParms = &parms;

    IOCTL_CMD((ULONG)IOCTL_NV01_ALLOC_CONTEXT_DMA,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS03_PARAMETERS));

    status = (parms.status == 0 ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL);

CleanupAndExit:

    return status;
}


NTSTATUS
NvAllocObject(
    ULONG hClient,
    ULONG hChannel,
    ULONG hObject,
    ULONG hClass
    )
{
    NVOS05_PARAMETERS parms;
    PVOID pParms;

    ULONG BytesReturned;
    NTSTATUS status;


    // set input parameters
    parms.hRoot             = (NvV32)hClient;
    parms.hObjectParent     = (NvV32)hChannel;
    parms.hObjectNew        = (NvV32)hObject;
    parms.hClass            = (NvV32)hClass;
    pParms = &parms;

    IOCTL_CMD((ULONG)IOCTL_NV01_ALLOC_OBJECT,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS05_PARAMETERS));

    status = (parms.status == 0 ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL);

CleanupAndExit:

    return status;
}


NTSTATUS
NvAllocEvent(
    ULONG hClient,
    ULONG hParent,
    ULONG hObject,
    ULONG hClass,
    ULONG index,
    ULONG hEvent
    )
{
    NVOS10_PARAMETERS parms;
    PVOID pParms;

    ULONG BytesReturned;
    NTSTATUS status;

    // set input parameters
    parms.hRoot             = (NvV32)hClient;
    parms.hObjectParent     = (NvV32)hParent;
    parms.hObjectNew        = (NvV32)hObject;
    parms.hClass            = (NvV32)hClass;
    parms.index             = (NvV32)index;

#ifdef NV20MINIX
    parms.hEvent            = (NvU64)hEvent;
#else
    parms.hEvent.low        = (NvV32)hEvent;
    parms.hEvent.high       = (NvV32)0;
#endif

    pParms = &parms;

    IOCTL_CMD((ULONG)IOCTL_NV01_ALLOC_EVENT,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS10_PARAMETERS));

    status = (parms.status == 0 ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL);

CleanupAndExit:

    return status;
}


NTSTATUS
NvFree(
    ULONG hClient,
    ULONG hParent,
    ULONG hObject
    )
{
    NVOS00_PARAMETERS parms;
    PVOID pParms;

    ULONG BytesReturned;
    NTSTATUS status;

    parms.hRoot             = (NvV32)hClient;
    parms.hObjectParent     = (NvV32)hParent;
    parms.hObjectOld        = (NvV32)hObject;
    pParms = &parms;

    IOCTL_CMD((ULONG)IOCTL_NV01_FREE,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS00_PARAMETERS));

    status = (parms.status == 0 ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL);

CleanupAndExit:

    return status;
}


NTSTATUS
NvAllocMemory(
    ULONG hClient,
    ULONG hParent,
    ULONG hMemory,
    ULONG hClass,
    ULONG flags,
    PVOID *ppAddress,
    ULONG *pLimit
    )
{
    NVOS02_PARAMETERS parms;
    PVOID pParms;

    ULONG BytesReturned;
    NTSTATUS status;


    // set input parameters
    parms.hRoot             = (NvV32)hClient;
    parms.hObjectParent     = (NvV32)hParent;
    parms.hObjectNew        = (NvV32)hMemory;
    parms.hClass            = (NvV32)hClass;
    parms.flags             = (NvV32)flags;

#ifdef NV20MINIX
    parms.pMemory           = (NvP64)*ppAddress;
    parms.pLimit            = (NvU64)*pLimit;
#else
    parms.pMemory.selector  = 0x00000000;
    parms.pMemory.offset    = (NvU32)*ppAddress;
    parms.pLimit.high       = 0x00000000;
    parms.pLimit.low        = (NvU32)*pLimit;
#endif

    pParms = &parms;

    IOCTL_CMD((ULONG)IOCTL_NV01_ALLOC_MEMORY,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS02_PARAMETERS));

#ifdef NV20MINIX
    *ppAddress = (PVOID)((ULONG)parms.pMemory);
    *pLimit = (ULONG)parms.pLimit;
#else
    *ppAddress = (PVOID)parms.pMemory.offset;
    *pLimit = (ULONG)parms.pLimit.low;
#endif

    status = (parms.status == 0 ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL);

CleanupAndExit:

    return status;
}



ULONG 
NvAllocChannelDma(
    ULONG hClient,
    ULONG hDevice,
    ULONG hChannel,
    ULONG hClass,
    ULONG hErrorCtx,
    ULONG hDataCtx,
    ULONG offset,
    PVOID *ppChannel
    )
{
    NVOS07_PARAMETERS parms;
    PVOID pParms;
    
    ULONG BytesReturned;
    NTSTATUS status;

    parms.hRoot             = (NvV32)hClient;
    parms.hObjectParent     = (NvV32)hDevice;
    parms.hObjectNew        = (NvV32)hChannel;
    parms.hClass            = (NvV32)hClass;
    parms.hObjectError      = (NvV32)hErrorCtx;
    parms.hObjectBuffer     = (NvV32)hDataCtx;
    parms.offset            = (NvU32)offset;
    pParms = &parms;

    IOCTL_CMD((ULONG)IOCTL_NV03_ALLOC_CHANNEL_DMA,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS07_PARAMETERS));

#ifdef NV20MINIX
    *ppChannel = (PVOID)((ULONG)parms.pControl);
#else
    *ppChannel = (PVOID)parms.pControl.offset;
#endif

    status = (parms.status == 0 ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL);

CleanupAndExit:

    return status;
}





LONG
MxAllocContextDma(
    IN ULONG Dma,
    IN ULONG Class,
    IN ULONG Flags,
    IN PVOID Base,
    IN ULONG Limit
    )
{
    return NvAllocContextDma(MxDriverExtension->Client, Dma, Class, Flags, Base, Limit);
}





NTSTATUS
MxCreateObjects(
    VOID
    )
{

    NTSTATUS status;

    //
    // Create objects
    //

#if defined(NV20MINIX) && defined(KELVIN)

    CHECKSTATUS(NvAllocObject(MxDriverExtension->Client,
                           NV_DD_DEV_DMA,
                           D3D_KELVIN_PRIMITIVE,
                           NV20_KELVIN_PRIMITIVE));

#else

    CHECKSTATUS(NvAllocObject(MxDriverExtension->Client, 
                           NV_DD_DEV_DMA, 
                           D3D_CELSIUS_PRIMITIVE, 
                           NV10_CELSIUS_PRIMITIVE));

    CHECKSTATUS(NvAllocObject(MxDriverExtension->Client, 
                           NV_DD_DEV_DMA,
                           D3D_RENDER_SOLID_RECTANGLE, 
                           NV4_RENDER_SOLID_RECTANGLE));

#endif

    CHECKSTATUS(NvAllocObject(MxDriverExtension->Client, 
                           NV_DD_DEV_DMA,
                           NV_DD_SURFACES_2D, 
                           NV10_CONTEXT_SURFACES_2D));

    CHECKSTATUS(NvAllocObject(MxDriverExtension->Client, 
                           NV_DD_DEV_DMA,
                           NV_DD_VIDEO_LUT_CURSOR_DAC, 
                           NV05_VIDEO_LUT_CURSOR_DAC));

CleanupAndExit:

    return status;
}



NTSTATUS
MxAllocContextDmas(
    IN PMINIX_MODE_INFO ModeInfo,
    IN PVOID VidMemAddress,
    IN ULONG VidMemLength
    )
{

    NTSTATUS status;

    CHECKSTATUS(NvAllocContextDma(MxDriverExtension->Client, 
                                  DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM,
                                  NV01_CONTEXT_DMA, 
                                  NVOS03_FLAGS_ACCESS_READ_WRITE, 
                                  VidMemAddress, 
                                  VidMemLength));


    CHECKSTATUS(NvAllocContextDma(MxDriverExtension->Client, 
                                  NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY,
                                  NV01_CONTEXT_DMA, 
                                  (ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR |
                                     ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                                  VidMemAddress, 
                                  VidMemLength));


    CHECKSTATUS(NvAllocContextDma(MxDriverExtension->Client, 
                               NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY,
                               NV01_CONTEXT_DMA, 
                               (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                                  ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                               VidMemAddress, 
                               VidMemLength));

    
    CHECKSTATUS(NvAllocContextDma(MxDriverExtension->Client, 
                                  NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY,
                                  NV01_CONTEXT_DMA, 
                                  (ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY |
                                      ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                                  VidMemAddress, 
                                  VidMemLength));

    //
    // Create DMA contexts for notifications
    //
    
    CHECKSTATUS(NvAllocContextDma(MxDriverExtension->Client, 
                                  NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
                                  NV01_CONTEXT_DMA, 
                                  ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY,
                                  (PVOID)ModeInfo->NotificationBase,
                                  (2 * sizeof(NvNotification) - 1)) );


    //
    // Create DMA context for host memory
    //
    CHECKSTATUS(NvAllocContextDma(MxDriverExtension->Client, 
                                  D3D_CONTEXT_DMA_HOST_MEMORY, 
                                  NV01_CONTEXT_DMA,
                                  (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY | 
                                      ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED),
                                  (PVOID)ModeInfo->HostMemoryBase, 
                                  ModeInfo->HostMemoryLimit));


    //
    // Allocate context DMA for the push buffer
    //

#if defined(NV20MINIX) && defined(KELVIN)

    CHECKSTATUS(NvAllocContextDma(MxDriverExtension->Client, 
                                  NV_DD_DMA_PUSHER_CONTEXT_DMA_FROM_MEMORY, 
                                  NV01_CONTEXT_DMA,
                                  (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY | 
                                      ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED),
                                  (PVOID)ModeInfo->HostMemoryBase, 
                                  ModeInfo->HostMemoryLimit));

#else !KELVIN

    CHECKSTATUS(NvAllocContextDma(MxDriverExtension->Client, 
                                  NV_DD_DMA_PUSHER_CONTEXT_DMA_FROM_MEMORY, 
                                  NV01_CONTEXT_DMA,
                                  (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY | 
                                      ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED),
                                  (PVOID)ModeInfo->PushBase, 
                                  ModeInfo->PushLimit));

#endif !KELVIN

CleanupAndExit:

    return status;
}




LONG
MxSetVideoMode(
    IN OUT PMINIX_MODE_INFO ModeInfo
    )
{
    ULONG BytesReturned;
    NTSTATUS status;

    ULONG Device;
    ULONG DeviceReference;

    VIDEO_PUBLIC_ACCESS_RANGES VideoAccessRange[16];
    
    VIDEO_MEMORY VideoMemory;
    VIDEO_MEMORY_INFORMATION VideoMemoryInfo;

    union
    {
        ULONG osName;
        CHAR devName[128];
    } NameBuffer;


    ULONG Mode;
    ULONG ModeIndex;
    PVIDEOMODEDEF ModeDef;

    BOOLEAN HwVidOvl;

    VIDEO_MODE_INFORMATION VideoModeInfo;
    TV_CURSOR_ADJUST_INFO CursorAdjustInfo;

    PVOID VidMemAddress;
    PVOID Temp;
    ULONG VidMemLength;
    ULONG VidMemHandle;

    BOOLEAN FullInit;


    Device = DD_DEVICE_OBJECT_HANDLE;
    VidMemAddress = 0;

    //
    // Determine if we need to do full initialization
    //
    FullInit = (BOOLEAN)(ModeInfo->PushBase != 0);



    if (FullInit) {
        
        // 
        // Enable P6 write combining when mapping the video memory
        //
        IOCTL_CMD(IOCTL_VIDEO_ENABLE_WRITE_COMBINING,
            NULL,
            0,
            NULL,
            0);
    
        // 
        // Get the device reference from the miniport to the device class
        //
        IOCTL_CMD(IOCTL_VIDEO_GET_DEVICE_REF,
            NULL,
            0,
            &DeviceReference,
            sizeof(ULONG));
    
    
        //
        // Get virtual addresses for the memory ranges
        //
        IOCTL_CMD(IOCTL_VIDEO_QUERY_PUBLIC_ACCESS_RANGES,
            NULL,                     
            0,
            VideoAccessRange,         
            sizeof(VideoAccessRange));


        //
        // Get the linear memory address range for the video memory
        //
        VideoMemory.RequestedVirtualAddress = NULL;
    
        IOCTL_CMD(IOCTL_VIDEO_MAP_VIDEO_MEMORY,
            &VideoMemory,      
            sizeof(VIDEO_MEMORY),
            &VideoMemoryInfo,  
            sizeof(VideoMemoryInfo));

        VidMemAddress = VideoMemoryInfo.FrameBufferBase;

        // 
        // Register the client 
        //
        CHECKSTATUS(NvAllocRoot(NV01_ROOT, &(MxDriverExtension->Client)));
    
        //
        // Allocate the device
        //
        NameBuffer.osName = DeviceReference;
    
        CHECKSTATUS(NvAllocDevice(MxDriverExtension->Client, Device, NV03_DEVICE_XX, NameBuffer.devName));
    }

    //
    // Set the video mode
    //
    status = STATUS_INVALID_PARAMETER; 

    ModeDef = &g_VideoModeDef[0];
    for (ModeIndex = 0; ModeIndex < (sizeof(g_VideoModeDef) / sizeof(g_VideoModeDef[0])); 
         ModeIndex++) {
        
        if (ModeInfo->Width == ModeDef->Width && 
            ModeInfo->Height == ModeDef->Height &&
            ModeInfo->BitDepth == ModeDef->Depth) {
            
            Mode = ModeDef->Mode;
            status = STATUS_SUCCESS;
            break;
        }

        ModeDef++;
    }

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;            
    }


    IOCTL_CMD(IOCTL_VIDEO_SET_CURRENT_MODE,
        &Mode,  
        sizeof(ULONG),
        &HwVidOvl,
        sizeof(BOOLEAN));


    //
    // Get video mode information
    //
    IOCTL_CMD(IOCTL_VIDEO_QUERY_CURRENT_MODE,
        NULL,
        0,
        &VideoModeInfo,
        sizeof(VideoModeInfo));


    //
    // Get cursor adjust information
    //
    IOCTL_CMD(IOCTL_VIDEO_GET_TV_CURSOR_ADJUST_INFO,
        NULL,
        0,
        &CursorAdjustInfo,
        sizeof(CursorAdjustInfo));


    if (FullInit) {
        
        //
        // Get the size for video memory from RM
        //
        Temp = (PVOID)1;  // NO_MAPPING
        VidMemHandle = DD_FRAME_BUFFER_OBJECT_HANDLE;
        CHECKSTATUS(NvAllocMemory(MxDriverExtension->Client, Device, VidMemHandle, NV01_MEMORY_LOCAL_USER, 0,
                                  &Temp, &VidMemLength));
    
        
        //
        // Return the video memory address
        //
        ModeInfo->VidMemAddress = VidMemAddress;
    
        //
        // Return the base address of the hardware.
        //
        ModeInfo->NvBase = VideoAccessRange[0].VirtualAddress;

        // BUGBUG: testcode: clear memory
        memset((PVOID)((unsigned char*)VidMemAddress), 0xFF, VidMemLength);
        // end test code

    
        //
        // Allocate DMA contexts to video memory. 
        //
        
        CHECKSTATUS(MxAllocContextDmas(ModeInfo, VidMemAddress, VidMemLength));

        //
        // Allocate channel DMA for the push buffer
        //

#if defined(NV20MINIX) && defined(KELVIN)

        CHECKSTATUS(NvAllocChannelDma(MxDriverExtension->Client,
                                      Device,
                                      NV_DD_DEV_DMA,
                                      NV20_CHANNEL_DMA,
                                      0,
                                      NV_DD_DMA_PUSHER_CONTEXT_DMA_FROM_MEMORY,
                                      0,
                                      (PVOID*)&(ModeInfo->DevFlatDma)));
#else
    
        CHECKSTATUS(NvAllocChannelDma(MxDriverExtension->Client, 
                                      Device, 
                                      NV_DD_DEV_DMA,
                                      NV10_CHANNEL_DMA, 
                                      0, 
                                      NV_DD_DMA_PUSHER_CONTEXT_DMA_FROM_MEMORY, 
                                      0,
                                      (PVOID*)&(ModeInfo->DevFlatDma)));
#endif

        //
        // Create objects
        //
    
        CHECKSTATUS(MxCreateObjects());
    }

 
    status = STATUS_SUCCESS;

CleanupAndExit:

    return (LONG)status;
}


