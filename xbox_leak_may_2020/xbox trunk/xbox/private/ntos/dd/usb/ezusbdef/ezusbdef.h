/*++

Copyright (c) 2002  Microsoft Corporation

Module Name:

    ezusbdef.h

Abstract:

    This module contains the public interface for a sample USB class driver.

--*/

#ifndef _EZUSBDEF_
#define _EZUSBDEF_

#ifdef __cplusplus
extern "C" {
#endif

extern XPP_DEVICE_TYPE XDEVICE_TYPE_EZUSBDEF_TABLE;
#define XDEVICE_TYPE_EZUSBDEF (&XDEVICE_TYPE_EZUSBDEF_TABLE)

VOID
WINAPI
EzusbGetDeviceChanges(
    LPDWORD lpdwInsertions,
    LPDWORD lpdwRemovals
    );

DWORD
WINAPI
EzusbFirmwareLoad(
    DWORD dwPort,
    WORD wStartingAddress,
    LPVOID lpvFirmware,
    WORD wNumberOfBytes,
    BOOL fUpload
    );

#ifdef __cplusplus
}
#endif

#endif  // EZUSBDEF
