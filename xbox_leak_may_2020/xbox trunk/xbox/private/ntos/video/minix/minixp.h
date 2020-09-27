/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    minixp.h

Abstract:

    This module contains the private data structures and procedure prototypes
    for the minix port driver.

--*/

#ifndef _MINIXP_
#define _MINIXP_

// Define ourselves as an NV20 to make life easy when including Nvidia
// header files:

#define NVARCH 0x20


#include <ntos.h>
#include <videoprt.h>
#include <pci.h>
#include <ntddvdeo.h>
#include <video.h>
#include <dderror.h>
#include <v86emul.h>   // needed by nv.h
#include <nv.h>
#include <minix.h>


BOOLEAN AgpUmaEnable();


typedef struct _MINIX_DRIVER_EXTENSION {

    VIDEO_DEVICE_EXTENSION VideoExtension;
    HW_DEVICE_EXTENSION HwExtension;

    ULONG Client;


} MINIX_DRIVER_EXTENSION, *PMINIX_DRIVER_EXTENSION;



LONG
MxCreatePort(
    VOID
    );


 
#endif  // _MINIXP_