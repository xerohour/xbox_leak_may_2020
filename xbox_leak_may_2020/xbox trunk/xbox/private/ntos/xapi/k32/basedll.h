/*++

Copyright (c) 1990-1999  Microsoft Corporation

Module Name:

    basedll.h

Abstract:

    This module contains private function prototypes
    and types for the 32-bit windows base APIs.

--*/

#ifndef _BASEP_
#define _BASEP_

#include "xapip.h"
#include "winbasep.h"
#include "xfiber.h"
#include <xcrypt.h>
#include <stdio.h>
#include <wchar.h>
#include "xapiver.h"

UCHAR
FASTCALL
RtlFindFirstSetRightMember(
    IN ULONG Set
    );

POBJECT_ATTRIBUTES
XapiFormatObjectAttributes(
    OUT POBJECT_ATTRIBUTES ObjectAttributes,
    OUT POBJECT_STRING ObjectName,
    IN PCOSTR lpName
    );

PLARGE_INTEGER
XapiFormatTimeOut(
    PLARGE_INTEGER TimeOut,
    DWORD Milliseconds
    );

ULONG
XapiSetLastNTError(
    NTSTATUS Status
    );

NTSTATUS
XapiNukeDirectoryFromHandle(
    HANDLE hDirectory,
    PFILE_DIRECTORY_INFORMATION DirectoryInfo
    );

NTSTATUS
XapiNukeDirectory(
    PCOSTR pszPath
    );

DWORD 
XapiNukeEmptySubdirs(
    IN PCOSTR  pszDrivePath,
    IN PCOSTR  pszPreserveDir OPTIONAL,
    IN BOOLEAN fNukeFiles
    );


#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))

extern const OBJECT_STRING XapiHardDisk;

typedef struct _FINDGAME_HANDLE
{
    DWORD dwSignature;
    HANDLE hFindFile;
    int cchRootDir;
    OCHAR szRootDir[MAX_PATH];
} FINDGAME_HANDLE, *PFINDGAME_HANDLE;

#define NUM_NICK_ENTRIES            50

typedef struct _X_NICK_DB_ENTRY
{
    DWORD    dwTitleID;
    WCHAR    szNickname[MAX_NICKNAME];
} X_NICK_DB_ENTRY, *PX_NICK_DB_ENTRY;

typedef struct _FINDNICKNAME_HANDLE
{
    DWORD           dwSignature;
    UINT            uCurIndex;
    BOOL            fThisTitleOnly;
    X_NICK_DB_ENTRY Nicknames[NUM_NICK_ENTRIES];
} FINDNICKNAME_HANDLE, *PFINDNICKNAME_HANDLE;

typedef struct _FINDCONTENT_HANDLE
{
    DWORD dwSignature;
    HANDLE hFindFile;
    DWORD dwFlagFilter;
    int cchRootDir;
    CHAR szRootDir[MAX_PATH];
} FINDCONTENT_HANDLE, *PFINDCONTENT_HANDLE;

typedef struct _XAPI_MU_INFO
{
    OCHAR   DriveWithAltDriveMapped;
    ULONG   MountedDrives;
} XAPI_MU_INFO, *PXAPI_MU_INFO;
extern XAPI_MU_INFO XapiMuInfo;

typedef struct _SNDTRK_ENUMSTATE {

    DWORD Signature;
    UINT Index;
    UINT MaxIndex;
    HANDLE DbHandle;

} SNDTRK_ENUMSTATE, *PSNDTRK_ENUMSTATE;


#define FH_SIG_SAVEGAME 0x53425645
#define FH_SIG_NICKNAME 0x4E49434B
#define FH_SIG_CONTENT 0x434F4E54
#define FH_SIG_SOUNDTRACK 0xBE4BEA00

#define HD_TDATA_DRIVE     OTEXT('T')
#define HD_UDATA_DRIVE     OTEXT('U')
#define HD_ALT_TDATA_DRIVE OTEXT('W')
#define HD_ALT_UDATA_DRIVE OTEXT('X')

#define MU_PORT_MIN     XDEVICE_PORT0
#define MU_PORT_MAX     (XDEVICE_PORT0 + XGetPortCount() - 1)

#define MU_SLOT_MIN     XDEVICE_TOP_SLOT
#define MU_SLOT_MAX     XDEVICE_BOTTOM_SLOT
#define MU_SLOTS        2
#define MU_SLOT_FACTOR  (XDEVICE_BOTTOM_SLOT - XDEVICE_TOP_SLOT)

#define MU_FIRST_DRIVE  OTEXT('F')
#define MU_LAST_DRIVE   (OTEXT('F') + (MU_PORT_MAX * MU_SLOTS) + (MU_SLOTS - 1))

#define MU_DRIVE_LETTER_FROM_PORT_SLOT(p, s) \
            ((OCHAR) (MU_FIRST_DRIVE + ((MU_SLOTS) * ((p) - MU_PORT_MIN)) + ((s / MU_SLOT_FACTOR))))

#define MU_PORT_FROM_DRIVE_LETTER(d) \
            (MU_PORT_MIN + (((d) - MU_FIRST_DRIVE) / MU_SLOTS))

#define MU_SLOT_FROM_DRIVE_LETTER(d) \
            ((MU_SLOT_MIN + (((d) - MU_FIRST_DRIVE) % MU_SLOTS)) * MU_SLOT_FACTOR)

#define MU_SET_MOUNTED(d) (XapiMuInfo.MountedDrives |= (1 << ((d) - MU_FIRST_DRIVE)))
#define MU_CLEAR_MOUNTED(d) (XapiMuInfo.MountedDrives &= ~(1 << ((d) - MU_FIRST_DRIVE)))
#define MU_IS_MOUNTED(d) (XapiMuInfo.MountedDrives&(1 << ((d) - MU_FIRST_DRIVE)))


#endif // _BASEP_

