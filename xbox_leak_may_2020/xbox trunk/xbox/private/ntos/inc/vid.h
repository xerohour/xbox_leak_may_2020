/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    vid.h

Abstract:

    This module contains the public data structures and procedure
    prototypes for video APIs exported from NTOS


--*/

#ifndef _VID_H
#define _VID_H


#define VID_NVNOTIFICATION_SIZE               (16)
#define VID_NVNOTIFICATION_DMA_BUFFER_SIZE    (VID_NVNOTIFICATION_SIZE * 32)


//
// Structure to describe video mode information
//

typedef struct _VIDEO_MODE_DATA
{
    LONG  Width; 
    LONG  Height; 
    LONG  BitDepth;
    ULONG PushBase;
    ULONG PushLimit;
    ULONG NotificationBase; 
    ULONG NotificationLimit;
    ULONG HostMemoryBase;
    ULONG HostMemoryLimit;

    PVOID VidMemAddress;   // returned by VidSetVideoMode
    PVOID NvBase;          // returned by VidSetVideoMode

} VIDEO_MODE_DATA, *PVIDEO_MODE_DATA;



NTSTATUS
VidSetVideoMode(
    IN OUT PVIDEO_MODE_DATA ModeData,
    OUT PVOID* DevFlatDma
    );


#endif // _VID_H 
