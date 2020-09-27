/*++

Copyright (c) 1995-1999 Microsoft Corporation

Module Name:

    syssetup.h

Abstract:

    Header file for internal-use routines exported from
    syssetup.dll.

    To use this file your code must #include setupapi.h first.

Author:

    Ted Miller (tedm) 15-Aug-1995

Revision History:

--*/


#ifndef _WINNT_SYSSETUP_
#define _WINNT_SYSSETUP_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

DWORD
SetupChangeLocale(
    IN HWND Window,
    IN LCID NewLocale
    );

DWORD
SetupChangeLocaleEx(
    IN HWND   Window,
    IN LCID   NewLocale,
    IN PCWSTR SourcePath,   OPTIONAL
    IN DWORD  Flags,
    IN PVOID  Reserved1,
    IN DWORD  Reserved2
    );

#define SP_INSTALL_FILES_QUIETLY  0x00000001

DWORD
SetupChangeFontSize(
    IN HWND   Window,
    IN PCWSTR SizeSpec
    );

//
//  ACL flags. These may be ORed.
//
//  SETUP_APPLYACL_PHASE1 indicates whether the Phase1 of ApplyAcls is to be
//  executed. If this flag is set then ApplyAcls() will set ACLs to the
//  Default hive only. If this flag is not set (Phase2), then ApplyAcls()
//  will set ACLs to system files and to all registry keys other than the
//  Default hive.
//
//  SETUP_APPLYACL_UPGRADE indicates whether ApplyAcls() was invoked on during
//  a clean install or upgrade.
//
#define SETUP_APPLYACL_PHASE1   0x00000001
#define SETUP_APPLYACL_UPGRADE  0x00000002

DWORD
ApplyAcls(
    IN HWND   OwnerWindow,
    IN PCWSTR PermissionsInfFileName,
    IN DWORD  Flags,
    IN PVOID  Reserved
    );

BOOL
SetupCreateOptionalComponentsPage(
    IN LPFNADDPROPSHEETPAGE AddPageCallback,
    IN LPARAM               Context
    );


//
// Define structure used by base and net setups to communicate
// with each other.
//
typedef struct _INTERNAL_SETUP_DATA {
    //
    // Structure validity test
    //
    DWORD dwSizeOf;

    //
    // Custom, typical, laptop, minimal
    //
    DWORD SetupMode;

    //
    // Workstation, pdc, bdc, standalone
    //
    DWORD ProductType;

    //
    // Upgrade, unattended, etc.
    //
    DWORD OperationFlags;

    //
    // Title net setup wizard is supposed to use.
    //
    PCWSTR WizardTitle;

    //
    // Installation source path.
    //
    PCWSTR SourcePath;

    //
    // If SETUPOPER_BATCH is set, this is the fully qualified
    // path of the unattend file.
    //
    PCWSTR UnattendFile;

    //
    // Installation source path to be used by legacy infs, etc.
    // This path has the platform-specific dir stuck on the end
    // because that's the way old-style infs and code expected it.
    //
    PCWSTR LegacySourcePath;

    //
    // The following generic data fields contain information that is
    // specific to the particular callout being made by Windows NT
    // Setup.
    //
    DWORD CallSpecificData1;
    DWORD CallSpecificData2;

} INTERNAL_SETUP_DATA, *PINTERNAL_SETUP_DATA;

typedef CONST INTERNAL_SETUP_DATA *PCINTERNAL_SETUP_DATA;

//
// Setup mode (custom, typical, laptop, etc)
// Do not change these values; the bit values are used with infs.
// Used for SetupMode in INTERNAL_SETUP_DATA structure.
//
#define SETUPMODE_MINIMAL   0
#define SETUPMODE_TYPICAL   1
#define SETUPMODE_LAPTOP    2
#define SETUPMODE_CUSTOM    3

//
// Operation flags. These may be or'ed together in some cases.
// Used for OperationFlags in INTERNAL_SETUP_DATA structure.
//
#define SETUPOPER_WIN31UPGRADE      0x00000001
#define SETUPOPER_WIN95UPGRADE      0x00000002
#define SETUPOPER_NTUPGRADE         0x00000004
#define SETUPOPER_BATCH             0x00000008
#define SETUPOPER_POSTSYSINSTALL    0x00000010
#define SETUPOPER_PREINSTALL        0x00000020

#define SETUPOPER_ALLPLATFORM_AVAIL 0x00008000

#define SETUPOPER_NETINSTALLED      0x00010000
#define SETUPOPER_INTERNETSERVER    0x00020000

//
// Product type flags.
// Used for ProductType in INTERNAL_SETUP_DATA structure.
//
// Note that the flags are carefully constructed such that
// if bit 0 is set, it's a DC.
//
#define PRODUCT_WORKSTATION         0
#define PRODUCT_SERVER_PRIMARY      1
#define PRODUCT_SERVER_SECONDARY    3
#define PRODUCT_SERVER_STANDALONE   2
#define ISDC(x) ((x) & 1)

//
// Maximum number of net setup wizard pages.
//
#define MAX_NETWIZ_PAGES            100

//
// API exported by net setup to give its wizard pages.
//
BOOL
NetSetupRequestWizardPages(
    OUT    HPROPSHEETPAGE      *Pages,
    IN OUT PUINT                PageCount,
    IN OUT PINTERNAL_SETUP_DATA SetupData
    );

#define NETSETUPPAGEREQUESTPROCNAME "NetSetupRequestWizardPages"

typedef
BOOL
(* NETSETUPPAGEREQUESTPROC) (
    OUT    HPROPSHEETPAGE      *Pages,
    OUT    PUINT                PageCount,
    IN OUT PINTERNAL_SETUP_DATA SetupData
    );

//
// API exported by net setup to allow post wizard software install
//
BOOL
NetSetupInstallSoftware(
    IN HWND Window,
    IN OUT PINTERNAL_SETUP_DATA SetupData
    );

#define NETSETUPINSTALLSOFTWAREPROCNAME "NetSetupInstallSoftware"

typedef
BOOL
(* NETSETUPINSTALLSOFTWAREPROC) (
    IN HWND Window,
    IN OUT PINTERNAL_SETUP_DATA SetupData
    );

//
// API exported by net setup to allow final setup operations (BDC replication)
//
BOOL
NetSetupFinishInstall(
    IN HWND Window,
    IN OUT PINTERNAL_SETUP_DATA SetupData
    );

#define NETSETUPFINISHINSTALLPROCNAME "NetSetupFinishInstall"

typedef
BOOL
(* NETSETUPFINISHINSTALLPROC) (
    IN HWND Window,
    IN OUT PINTERNAL_SETUP_DATA SetupData
    );

//
// API exported by printer setup to upgrade printer drivers
//
DWORD
NtPrintUpgradePrinters(
    IN HWND                  Window,
    IN PCINTERNAL_SETUP_DATA SetupData
    );

#define UPGRADEPRINTERSPROCNAME ((LPCSTR)1)

typedef
DWORD
(* UPGRADEPRINTERSPROC) (
    IN HWND                  Window,
    IN PCINTERNAL_SETUP_DATA SetupData
    );

#ifdef __cplusplus
}
#endif

#endif // def _WINNT_SYSSETUP_
