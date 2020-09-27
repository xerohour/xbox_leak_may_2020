/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    xapidrv.h

Abstract:

    XAPI methods exposed for use of drivers.

--*/

#ifndef _XAPIDRV_H_
#define _XAPIDRV_H_

#ifdef __cplusplus
extern "C" {
#endif

//
// Define API decoration for direct importing of DLL references.
//

#if !defined(_XAPI_)
#define XAPIDRVAPI DECLSPEC_IMPORT
#else
#define XAPIDRVAPI
#endif

//---------------------------------------------------------------------------------------------------------------
//  XAPI Driver APIS for reporting USB devices
//---------------------------------------------------------------------------------------------------------------
XAPIDRVAPI
VOID XdReportDeviceInsertionRemoval(
    PXPP_DEVICE_TYPE XppDeviceType,
    ULONG PortBit,
    BOOLEAN fInserted
    );

#define XDEVICE_ILLEGAL_PORT 32

#ifdef __cplusplus
}
#endif

#endif //_XAPIDRV_H_
