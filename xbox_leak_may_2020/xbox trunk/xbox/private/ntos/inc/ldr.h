/*++

Copyright (c) 1989-2002  Microsoft Corporation

Module Name:

    ldr.h

Abstract:

    This module contains the public data structures and procedure
    prototypes for the XBE loader.

--*/

#ifndef _LDR_
#define _LDR_

#ifdef __cplusplus
extern "C" {
#endif

NTSTATUS
XeLoadImage(
    IN PCOSTR ImageFileName,
    IN BOOLEAN LoadingDashboard,
    IN ULONG SettingsError
    );

VOID
XeLoadDashboardImage(
    VOID
    );

VOID
XeLoadDashboardImageWithReason(
    IN ULONG Reason,
    IN ULONG Parameter1
    );

VOID
XeLoadTitleImage(
    IN ULONG SettingsError
    );

VOID
XeLoadImageAfterTrayEjectBoot(
    VOID
    );

NTKERNELAPI
NTSTATUS
XeLoadSection(
    IN struct _XBEIMAGE_SECTION *Section
    );

NTKERNELAPI
NTSTATUS
XeUnloadSection(
    IN struct _XBEIMAGE_SECTION *Section
    );

//
// Macro to access the image header of an XBE image.
//

#define XeImageHeader() ((struct _XBEIMAGE_HEADER*)XBEIMAGE_STANDARD_BASE_ADDRESS)

//
// Macro to retrieve the size of the utility drive cluster size from the image
// header.
//

#define XeUtilityDriveClusterSize() (16384 << \
    ((XeImageHeader()->InitFlags & XINIT_UTILITY_DRIVE_CLUSTER_SIZE_MASK) >> \
    XINIT_UTILITY_DRIVE_CLUSTER_SIZE_SHIFT))

//
// Global that contains the NT object manager path of the current XBE image.
//

#if !defined(_NTSYSTEM_)
extern POBJECT_STRING XeImageFileName;
extern PUCHAR XePublicKeyData;
#else
extern OBJECT_STRING XeImageFileName;
extern UCHAR XePublicKeyData[];
extern ULONG XeCdRomMediaTitleID;
#endif

#ifdef __cplusplus
}
#endif

#endif // LDR
