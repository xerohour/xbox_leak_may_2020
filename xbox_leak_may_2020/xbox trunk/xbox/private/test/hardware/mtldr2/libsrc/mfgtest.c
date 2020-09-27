/*++

Copyright (c) Microsoft Corporation

Module Name:

    mfgtest.c

Abstract:

    Test routines used by the Xbox Manufacturing Test code.

Author:

    Marc R. Whitten (marcw)  02-July-2001

Revision History:

--*/

#include <ntos.h>
#include <xtl.h>
#include <xboxp.h>
#include "xlaunch.h"
#include "xapip.h"
#include "dm.h"

#include "mfgtest.h"


static const OBJECT_STRING g_ZDosDevicePrefix =  CONSTANT_OBJECT_STRING(OTEXT("\\??\\Z:"));

extern PLAUNCH_DATA_PAGE *LaunchDataPage;

BOOL
WINAPI
MfgTestCopyImageToDrive (
    LPCSTR Name,
    PBYTE ImageBuffer,
    SIZE_T Size
    )
{

    //
    // Simple routine that dumps memory to the hard drive.
    //
    HANDLE h;
    DWORD bytesWritten;


    //
    // Mount the utility drive so that we have a place to store the XBE.
    //
    if (!XMountUtilityDrive(TRUE)) {
        XDBGWRN ("MfgTest", "Unable to mount utility drive.");
    }

    //
    // Create the new file and copy over the provided buffer.
    //
    h = CreateFile (Name, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    if (!WriteFile (h, ImageBuffer, Size, &bytesWritten, NULL)) {
        CloseHandle (h);
        return FALSE;
    }

    CloseHandle (h);

    return TRUE;
}

BOOL
MfgSaveLaunchPathAndReboot (
    PSTR Path
    )
/*
** This is based upon the XWriteTitleInfo* Routines in XAPI K32\LAUNCH.C. It is modified to work correctly in
** the slightly cheating mfg case.
*/

{

    PSTR launchPath;

    //
    // Make sure the LaunchDataPage is allocated and persisted.
    //
    if (NULL == *LaunchDataPage) {
        *LaunchDataPage = MmAllocateContiguousMemory(PAGE_SIZE);
    }

    if (NULL == *LaunchDataPage) {
        return FALSE;
    }

    MmPersistContiguousMemory(*LaunchDataPage, PAGE_SIZE, TRUE);
    RtlZeroMemory(*LaunchDataPage, PAGE_SIZE);


    launchPath = ((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.szLaunchPath;

    //
    // The Manufacturing test loader doesn't do anything with launch params.
    // Just zero this stuff out.
    //
    ((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.dwLaunchDataType = LDT_NONE;
    ((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.dwTitleId = 0;

    //
    // Persist the launch path to the new executable.
    //
    lstrcpynA (launchPath, Path, ARRAYSIZE(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.szLaunchPath) - 1);

    //
    // Reboot the machine.
    //
    #ifdef DEVKIT
        //
        // Notify the debugger that we're about to reboot and then reboot
        //
        DmTell(DMTELL_REBOOT, NULL);
    #endif

    HalReturnToFirmware(HalQuickRebootRoutine);
}



BOOL
WINAPI
MfgTestReboot (
    LPCSTR Name
    )
{

    OCHAR drivePath[MAX_PATH];
    OBJECT_ATTRIBUTES obja;
    NTSTATUS status;
    HANDLE linkHandle;
    OBJECT_STRING linkTarget;
    OCHAR linkBuffer[MAX_PATH * 2];

    //
    // Preserve mapping to cache partition, so we can reboot to the XBE we stuffed in there.
    //
    InitializeObjectAttributes (&obja, (POBJECT_STRING) &g_ZDosDevicePrefix, OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = NtOpenSymbolicLinkObject (&linkHandle, &obja);

    if (NT_SUCCESS(status)) {

        linkTarget.Buffer = linkBuffer;
        linkTarget.Length = 0;
        linkTarget.MaximumLength = sizeof (linkBuffer);

        status = NtQuerySymbolicLinkObject (linkHandle, &linkTarget, NULL);
        NtClose (linkHandle);
    }

    if (NT_SUCCESS(status)) {

        lstrcpynA (drivePath, linkBuffer, min(ARRAYSIZE(drivePath), (linkTarget.Length / sizeof(CHAR)) + 1));
        lstrcatA(drivePath,&Name[2]);
    }


    //
    // Write the necessary reboot information and reboot the machine.
    //
    return NT_SUCCESS(status) ? MfgSaveLaunchPathAndReboot (drivePath) : FALSE;
}
