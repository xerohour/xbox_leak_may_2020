/*++

Copyright (c) 2002  Microsoft Corporation

Module Name:

    usbsamp.h

Abstract:

    This module contains the public interface for a sample USB class driver.

--*/

#ifndef _USBSAMP_
#define _USBSAMP_

#ifdef __cplusplus
extern "C" {
#endif

extern XPP_DEVICE_TYPE XDEVICE_TYPE_SAMPLE_TABLE;
#define XDEVICE_TYPE_SAMPLE (&XDEVICE_TYPE_SAMPLE_TABLE)

DWORD
WINAPI
SampleOpenDevice(
    DWORD dwPort
    );

DWORD
WINAPI
SampleReadButtons(
    DWORD dwPort,
    LPBYTE lpbButtons
    );

VOID
WINAPI
SampleCloseDevice(
    DWORD dwPort
    );

#ifdef __cplusplus
}
#endif

#endif  // USBSAMP
