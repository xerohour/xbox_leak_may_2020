/*****************************************************************************

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    setled.cpp

	Derived from \private\test\kernel\smc\smc.cpp

	Author:

Abstract:

    Defines functions needed to change LED colors on the Xbox.

*****************************************************************************/

#include "xbserver.h"

extern "C"
    {
	DECLSPEC_IMPORT
    LONG
    WINAPI
    HalReadSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN ReadWordValue,
        OUT ULONG *DataValue
        );

    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalWriteSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN WriteWordValue,
        IN ULONG DataValue
        );
    }

NTSTATUS WriteSMC(unsigned char addr, unsigned char value)
    {
    return HalWriteSMBusValue(0x20, addr, FALSE, value);
    }
