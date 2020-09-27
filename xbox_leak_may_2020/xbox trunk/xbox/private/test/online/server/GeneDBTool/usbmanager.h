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
#include "constants.h"

#define ARRAYSIZE(a)        (sizeof(a) / sizeof(a[0]))
#define BREAK_INTO_DEBUGGER     _asm { int 3 }

// Buttons
enum BUTTONS
{
    BUTTON_MIN = 0,
    BUTTON_A = 0,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
    BUTTON_BLACK,
    BUTTON_WHITE,
    BUTTON_LEFT_TRIGGER,
    BUTTON_RIGHT_TRIGGER,

    BUTTON_MAX
};

enum CONTROLS
{
    CONTROL_MIN = 0,
    CONTROL_DPAD_UP = 0,
    CONTROL_DPAD_DOWN,
    CONTROL_DPAD_LEFT,
    CONTROL_DPAD_RIGHT,
	CONTROL_START,
	CONTROL_BACK,
	CONTROL_LEFT_THUMB,
	CONTROL_RIGHT_THUMB,

    CONTROL_MAX
};

// Ports
enum PORTS
{
    PORT_ANY = -1,
    PORT_MIN = 0,
    PORT_1 = 0,
    PORT_2,
    PORT_3,
    PORT_4,

    PORT_MAX
};

// Joysticks
enum JOYSTICK
{
    JOYSTICK_ANY = -1,
    JOYSTICK_MIN = 0,
    JOYSTICK_LEFT = 0,
    JOYSTICK_RIGHT,

    JOYSTICK_MAX
};

// Parameters
#define FALSE_ON_REPEAT     true
#define TRUE_ON_REPEAT      false

enum
{
    SLOT_CONTROLLER = 0,
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
        for( unsigned int port = 0; port < NUM_XBOX_PORTS; ++port )
        {
            for( unsigned int button = 0; button < NUM_DUKE_BUTTONS; ++button )
            {
                nButtonPress[port][button] = 0;
            }

            for( unsigned int control = 0; control < NUM_DUKE_CONTROLS; ++control )
            {
                bControlPressed[port][control] = false;
            }

            for( unsigned int joystick = 0; joystick < JOYSTICK_MAX; ++joystick )
            {
                sThumbX[port][joystick] = 0;
                sThumbY[port][joystick] = 0;
            }

            dwControllerState[port] = 0;
        }
    };
    
    ~CControllerState(){};


   // Public Properties
   BYTE nButtonPress[NUM_XBOX_PORTS][NUM_DUKE_BUTTONS];
   bool bControlPressed[NUM_XBOX_PORTS][CONTROL_MAX];
   SHORT sThumbX[NUM_XBOX_PORTS][JOYSTICK_MAX];
   SHORT sThumbY[NUM_XBOX_PORTS][JOYSTICK_MAX];
   DWORD dwControllerState[NUM_XBOX_PORTS];private:
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
        duke = XInputOpen( XDEVICE_TYPE_GAMEPAD, port, 0, pollingParameters );
    }

    void Remove( void )
    {
        XInputClose( duke );
        duke = 0;
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
    // Calling this will update the controller state objects with the latest info
    void ProcessInput( void );

    // Calling this will check to see if controllers we added or removed
    void CheckForHotplugs( void );

    // This will allow you to determine if a button is 'depressed'.  If an address to a boolean
    // variable is provided, it will return whether or not it's the 'first' press of the button
    bool IsButtonPressed( int port, enum BUTTONS button, int nThreshold=0, bool* bFirstPress=NULL );
    
    // This will allow you to determine if a control is 'depressed'.  If an address to a boolean
    // variable is provided, it will return whether or not it's the 'first' press of the control
    bool IsControlPressed( int port, enum CONTROLS control, bool* bFirstPress=NULL );

    // This will allow you to get the value of the X or Y axis, adjusting for a "dead zone".
    // If a dead zone is passed in, the call will return '0' unless the value of the X or Y axis
    // is greater than the dead zone value.  If an address to a boolean variable is provided, it
    // will return whether or not it's the 'first' press of the joystick
    int  GetJoystickX( int port, enum JOYSTICK, int deadZone=0, bool* bFirstPress=NULL );
    int  GetJoystickY( int port, enum JOYSTICK, int deadZone=0, bool* bFirstPress=NULL );

    // Calling this will determine if there was movement for any item on a controller
    // Will return true if any button or thumbstick has changed since last poll
    bool MovementDetected( int port, int deadZone=0 );

private:
    CControllerState m_ControllerState;
    CControllerState m_PrevControllerState;
};

#endif // _USBMANAGER_H_