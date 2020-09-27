/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    UsbManager.h

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    07-20-2000  Created
    11-01-2000  Modified JNH -- Removed uneeded launcher and socket code

Notes:
    What's inside the HANDLE from XInputOpen (HANDLE == _XID_OPEN_DEVICE*):
        PXID_DEVICE_NODE    XidNode;
        PXID_OPEN_DEVICE    NextOpenDevice;
        LONG                OutstandingPoll;
        ULONG               PacketNumber;
        PVOID               InterruptInEndpointHandle;
        PVOID               InterruptOutEndpointHandle;
        LONG                OpenCount;
        UCHAR               Report[XID_MAXIMUM_REPORT_SIZE-2];
        UCHAR               ReportForUrb[XID_MAXIMUM_REPORT_SIZE];
        URB                 Urb;
        URB_CLOSE_ENDPOINT  CloseUrb;
        UCHAR               MdlBuffer[sizeof(MDL)+sizeof(PFN_NUMBER)*2];
        UCHAR               ClosePending:1;
        UCHAR               EndpointsOpen:1;
        UCHAR               EndpointsPendingClose:1;
        UCHAR               DeletePending:1;
        UCHAR               AutoPoll:1;
        UCHAR               Pad:3;

*/

#ifndef _USBMANAGER_H_
#define _USBMANAGER_H_

//#include <xbox.h>
#include <xtl.h>

extern "C" {ULONG DebugPrint(PCHAR Format, ...);}

#define ARRAYSIZE(a)        (sizeof(a) / sizeof(a[0]))
#define BREAK_INTO_DEBUGGER     _asm { int 3 }

#define NUM_XBOX_PORTS      4

#define NUM_DUKE_BUTTONS    14

// DPAD
/*
#define DPAD_TOP            0
#define DPAD_BOTTOM         1
#define DPAD_LEFT           2
#define DPAD_RIGHT          3
*/

// Buttons
enum BUTTONS
{
    BUTTON_A,
    BUTTON_B,
    BUTTON_C,
    BUTTON_D,
    BUTTON_E,
    BUTTON_F,
    BUTTON_7,
    BUTTON_8,
    BUTTON_9,
    BUTTON_10,
};

enum CONTROLS
{
    CONTROL_DPAD_TOP,
    CONTROL_DPAD_BOTTOM,
    CONTROL_DPAD_LEFT,
    CONTROL_DPAD_RIGHT,
	CONTROL_START,
	CONTROL_SELECT,
	CONTROL_TRIGGER_LEFT,
	CONTROL_TRIGGER_RIGHT
};

// Ports
enum PORTS
{
    PORT_ANY = -1,
    PORT_1 = 0,
    PORT_2,
    PORT_3,
    PORT_4
};

// Parameters
#define FALSE_ON_REPEAT     true
#define TRUE_ON_REPEAT      false

enum
{
    SLOT_CONTROLLER,
    SLOT_TOP,
    SLOT_BOTTOM,
    SLOT_MAX
};

class CControllerState
{
public:
    // Constructors and Destructors
    CControllerState()
    {
        for( unsigned int y = 0; y < NUM_XBOX_PORTS; ++y )
        {
            for( unsigned int x = 0; x < NUM_DUKE_BUTTONS; ++x )
            {
                bButtonPressed[y][x] = false;
            }

            for( x = 0; x < 8; ++x )
            {
                bControlPressed[y][x] = false;
            }
        }
    };
    
    ~CControllerState(){};

    // Public Properties
    bool bButtonPressed[NUM_XBOX_PORTS][NUM_DUKE_BUTTONS];
	bool bControlPressed[NUM_XBOX_PORTS][8];
private:
};

class USBDevice
{
public:
    PXPP_DEVICE_TYPE type; // XDEVICE type

public:
    USBDevice()
    {
        type = NULL;
    }
    
    ~USBDevice()
    {
        type = NULL;
    }

public:
    virtual void Insert( unsigned port, unsigned slot ) = 0;
    virtual void Remove( void ) = 0;
};

class DeviceDuke : public USBDevice
{
public:
    HANDLE duke;
    XINPUT_POLLING_PARAMETERS *pollingParameters;

public:
    DeviceDuke()
    {
        type = XDEVICE_TYPE_GAMEPAD;
        pollingParameters = NULL;
        duke = NULL;
    }

    DeviceDuke( unsigned port, unsigned slot, XINPUT_POLLING_PARAMETERS *p )
    {
        type = XDEVICE_TYPE_GAMEPAD;
        pollingParameters = p;
        duke = NULL;
        Insert( port, slot );
    }

    ~DeviceDuke()
    {
        if( duke )
        {
            Remove();
        }
    }

public:
    void Insert( unsigned port, unsigned slot )
    {
        /*DebugPrint("USBDevice: Insert duke\n");*/
        duke = XInputOpen( XDEVICE_TYPE_GAMEPAD, port, 0, pollingParameters );
    }

    void Remove( void )
    {
        /*DebugPrint("USBDevice: Remove duke\n");*/
        XInputClose( duke );
        duke = 0;
    }
};

class DeviceMU : public USBDevice
{
public:
    CHAR drive;

public:
    DeviceMU()
    {
        type = XDEVICE_TYPE_MEMORY_UNIT;
        drive = 0;
    }
    
    DeviceMU( unsigned port, unsigned slot )
    {
        type = XDEVICE_TYPE_MEMORY_UNIT;
        drive = 0;
        Insert(port, slot);
    }

    ~DeviceMU()
    {
        if( drive )
        {
            Remove();
        }
    }

public:
    void Insert( unsigned port, unsigned slot )
    {
        /*DebugPrint("USBDevice: Insert MU\n");*/
        XMountMU( port, slot==1?XDEVICE_TOP_SLOT:XDEVICE_BOTTOM_SLOT, &drive );
    }

    void Remove( void )
    {
        /*DebugPrint("USBDevice: Remove MU\n");*/
        XUnmountMU( XMUPortFromDriveLetter( drive ), XMUSlotFromDriveLetter( drive ) );
        drive = 0;
    }
};

class DeviceHawk : public USBDevice
{
public:
    XMediaObject *microphone;
    XMediaObject *headphone;

public:
    DeviceHawk()
    {
       // type = XDEVICE_TYPE_VOICE_MICROPHONE;
        microphone = headphone = NULL;
    }
    
    DeviceHawk( unsigned port, unsigned slot )
    {
     //   type = XDEVICE_TYPE_VOICE_MICROPHONE;
        microphone = headphone = NULL;
        Insert(port, slot);
    }

    ~DeviceHawk()
    {
        if( microphone || headphone )
        {
            Remove();
        }
    }

public:
    void Insert(unsigned port, unsigned slot)
    {
        /*DebugPrint("USBDevice: Insert HAWK\n");*/
    //    XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_MICROPHONE, port, slot==1?XDEVICE_TOP_SLOT:XDEVICE_BOTTOM_SLOT, NULL, &microphone);
     //   XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_HEADPHONE, port, slot==1?XDEVICE_TOP_SLOT:XDEVICE_BOTTOM_SLOT, NULL, &headphone);
    }

    void Remove(void)
    { 
        /*DebugPrint("USBDevice: Remove HAWK\n");*/
        if( microphone )
        {
            DebugPrint("microphone->Release()...");
            microphone->Release();
            DebugPrint("...microphone->Release()\n");
        } 

        if( headphone )
        {
            DebugPrint("headphone->Release()...");
            headphone->Release();  DebugPrint("...headphone->Release()\n");
        } 

        microphone = headphone = NULL; 
        /*DebugPrint("USBDevice: Remove HAWK\n");*/
    }
};

class USBManager
{
public:
    USBDevice *devices[XGetPortCount()][SLOT_MAX];
    DWORD packetNum[XGetPortCount()][SLOT_MAX];
    XINPUT_POLLING_PARAMETERS pollingParameters;

public:
    USBManager();
    ~USBManager();

public:
    void ProcessInput( void );
    void CheckForHotplugs( void );
    bool IsButtonPressed( int port, enum BUTTONS button, BOOL bFalseOnRepeat = false );
    bool IsControlPressed( int port, enum CONTROLS control, BOOL bFalseOnRepeat =false );

private:
    CControllerState g_ControllerState;
};

#endif // _USBMANAGER_H_