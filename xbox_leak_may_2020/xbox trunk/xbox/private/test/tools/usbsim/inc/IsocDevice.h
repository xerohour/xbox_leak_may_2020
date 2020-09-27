/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    IsocDevice.h

Abstract:

    USB Device child class that handles Hawk enumeration and communication

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/

#ifndef _ISOCDEVICE_H_
#define _ISOCDEVICE_H_



/*****************************************************************************

Class Description:

    IsocDevice

    USB Device child class that handles Hawk enumeration and communication

Notes:
    

*****************************************************************************/
class IsocDevice : public USBDevice
    {
    public:
        unsigned enumerateState[10];

    protected:

    public:
        IsocDevice();
        ~IsocDevice();

    // virtual member functions
    public:
        DWORD EndpointConfig(unsigned config);
        DWORD Receive(void);
        const char* GetName(void);
        bool IsEnumerated(void);

    protected:
        DWORD HandleSETUP(void);
        DWORD HandleOUT(void);
        DWORD HandleIN(void);

    public:
        friend class CHawk; // give the COM object access to us
    };

#endif // _ISOCDEVICE_H_
