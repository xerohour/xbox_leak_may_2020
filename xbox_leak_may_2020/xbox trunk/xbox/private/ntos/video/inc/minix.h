/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    minix.h

Abstract:

    Contains declarations for minix    

--*/


typedef struct _MINIX_MODE_INFO
{
    LONG Width; 
    LONG Height; 
    LONG BitDepth;
    ULONG PushBase;
    ULONG PushLimit;
    ULONG NotificationBase;    
    ULONG NotificationLimit;
    ULONG HostMemoryBase;
    ULONG HostMemoryLimit;
    
    PVOID DevFlatDma;          // Filled by SetVideoMode 
    PVOID VidMemAddress;       // Filled by SetVideoMode
    PVOID NvBase;              // Filled by SetVideoMode

} MINIX_MODE_INFO, *PMINIX_MODE_INFO;




//
// Major function codes used for the to access functions in the dispatch table
// of the minix driver object. Since this is not a real driver, the dispatch 
// function is used to pass alternate functions
//

#define IRP_MJ_MINIX_CREATE                0x00    // PMINIX_CREATE
#define IRP_MJ_MINIX_SETVIDEOMODE          0x01    // PMINIX_SETVIDEOMODE
#define IRP_MJ_MINIX_ALLOCCONTEXTDMA       0x02    // PMINIX_ALLOCCONTEXTDMA


typedef
LONG
(*PMINIX_CREATE) (
    VOID
    );

typedef
LONG
(*PMINIX_SETVIDEOMODE) (
    IN OUT PMINIX_MODE_INFO ModeInfo
    );

typedef
LONG
(*PMINIX_ALLOCCONTEXTDMA)(
    IN ULONG Dma,
    IN ULONG Class,
    IN ULONG Flags,
    IN PVOID Base,
    IN ULONG Limit
        );



LONG
MxInitSystem(
    IN PVOID Argument1
    );

