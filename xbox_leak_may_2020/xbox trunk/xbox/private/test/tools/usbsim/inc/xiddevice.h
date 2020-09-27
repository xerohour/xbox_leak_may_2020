/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    XIDDevice.h

Abstract:

    USB Device child class that handles XID enumeration and reporting

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/

#ifndef _XIDDEVICE_H_
#define _XIDDEVICE_H_

#pragma pack(push, 1) // align structure members on a byte boundry
struct XIDInputReport
    {
    unsigned __int8  reportID;
    unsigned __int8  size;
    unsigned __int16 buttons;
    unsigned __int8  analogButtons[8];
    __int16 thumbLX;
    __int16 thumbLY;
    __int16 thumbRX;
    __int16 thumbRY;
    };
extern const XIDInputReport defaultXIDReport;
#pragma pack(pop)



/*****************************************************************************

Class Description:

    XIDDevice

    USB Device child class that handles XID enumeration and reporting

Notes:
    

*****************************************************************************/
class XIDDevice : public USBDevice
    {
    public:
        unsigned enumerateState[10];

    protected:
        unsigned __int8 *xidPacket;
        unsigned __int8 *inCapabilities;
        unsigned __int8 *outCapabilities;
        unsigned __int8 *xidDescriptor;
        unsigned xidPacketLen;
        unsigned inCapabilitiesLen;
        unsigned outCapabilitiesLen;
        unsigned xidDescriptorLen;

    public:
        XIDDevice();
        ~XIDDevice();

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
        DWORD SetInputReport(XIDInputReport *xid);
        DWORD SetInputReport(unsigned __int8 reportID, unsigned __int8 size, unsigned __int16 buttons, unsigned __int8 *analogButtons, unsigned __int16 thumbLX, unsigned __int16 thumbLY, unsigned __int16 thumbRX, unsigned __int16 thumbRY);

    public:
        friend class CDuke; // give the COM object access to us
    };

#endif // _XIDDEVICE_H_
