/*

Copyright (c) 2001 Microsoft Corporation

Module Name:

    UsbManager.h

Abstract:


Author:

    Josh Poley (jpoley)
    Jim Helm (jimhelm)

Environment:

    Xbox

Revision History:
    07-20-2000  Created
    11-01-2000  Modified JNH -- Removed uneeded launcher and socket code
    06-01-2001  Modified JNH -- Added Remote Control support

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

class USBManager;

#ifndef _USBMANAGER_H_
#define _USBMANAGER_H_

#define ARRAYSIZE(a)            (sizeof(a) / sizeof(a[0]))
#define BREAK_INTO_DEBUGGER     _asm { int 3 }
#define USBM_APP_TITLE_NAME_A   "USBM"

#include "..\..\..\..\inc\xboxp.h"

// Buttons
// #define NUM_REMOTE_BUTTONS      27

static WORD g_aRemoteButtons[] =
{
    0xAD5,
    0xAE2,
    0xAEA,
    0xAE3,
    0xADD,
    0xAE0,
    0xAE6,
    0xADF,
    0xAE5,
    0xAC3,
    0xAF7,
    0xAD8,
    0xAA6,
    0xAA9,
    0xA0B,
    0xAA8,
    0xAA7,
    0xACF,
    0xACE,
    0xACD,
    0xACC,
    0xACB,
    0xACA,
    0xAC9,
    0xAC8,
    0xAC7,
    0xAC6
};

#define REMOTE_BUTTON_REPEAT_THRESHOLD      200 // milliseconds

enum REMOTE_BUTTONS
{
    REMOTE_BUTTON_MIN           = 0,
    REMOTE_BUTTON_POWER         = 0, // = 0xAD5,
    REMOTE_BUTTON_REVERSE,           // = 0xAE2,
    REMOTE_BUTTON_PLAY,              // = 0xAEA,
    REMOTE_BUTTON_FORWARD,           // = 0xAE3,
    REMOTE_BUTTON_SKIPBACK,          // = 0xADD,
    REMOTE_BUTTON_STOP,              // = 0xAE0,
    REMOTE_BUTTON_PAUSE,             // = 0xAE6,
    REMOTE_BUTTON_SKIPFORWARD,       // = 0xADF,
    REMOTE_BUTTON_GUIDE,             // = 0xAE5,
    REMOTE_BUTTON_INFO,              // = 0xAC3,
    REMOTE_BUTTON_MENU,              // = 0xAF7,
    REMOTE_BUTTON_REPLAY,            // = 0xAD8,
    REMOTE_BUTTON_UP,                // = 0xAA6,
    REMOTE_BUTTON_LEFT,              // = 0xAA9,
    REMOTE_BUTTON_OK,                // = 0xA0B,
    REMOTE_BUTTON_RIGHT,             // = 0xAA8,
    REMOTE_BUTTON_DOWN,              // = 0xAA7,
    REMOTE_BUTTON_0,                 // = 0xACF,
    REMOTE_BUTTON_1,                 // = 0xACE,
    REMOTE_BUTTON_2,                 // = 0xACD,
    REMOTE_BUTTON_3,                 // = 0xACC,
    REMOTE_BUTTON_4,                 // = 0xACB,
    REMOTE_BUTTON_5,                 // = 0xACA,
    REMOTE_BUTTON_6,                 // = 0xAC9,
    REMOTE_BUTTON_7,                 // = 0xAC8,
    REMOTE_BUTTON_8,                 // = 0xAC7,
    REMOTE_BUTTON_9,                 // = 0xAC6

    REMOTE_BUTTON_MAX
};

enum BUTTONS
{
    BUTTON_MIN = 0,
    BUTTON_A   = 0,
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
    CONTROL_MIN     = 0,
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
#define FALSE_ON_REPEAT     TRUE
#define TRUE_ON_REPEAT      FALSE

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
        for( unsigned int port = 0; port < XGetPortCount(); ++port )
        {
            for( unsigned int button = 0; button < BUTTON_MAX; ++button )
            {
                nButtonPress[port][button] = 0;
            }

            for( unsigned int control = 0; control < CONTROL_MAX; ++control )
            {
                bControlPressed[port][control] = FALSE;
            }

            for( unsigned int joystick = 0; joystick < JOYSTICK_MAX; ++joystick )
            {
                sThumbX[port][joystick] = 0;
                sThumbY[port][joystick] = 0;
            }

            for( unsigned int remotebutton = 0; remotebutton < REMOTE_BUTTON_MAX; ++ remotebutton )
            {
                m_bRemoteButtonPress[port][remotebutton] = FALSE;
            }

            dwControllerState[port] = 0;
        }
    };

    ~CControllerState(){};

    // Public Properties
    BOOL m_bRemoteButtonPress[XGetPortCount()][REMOTE_BUTTON_MAX];
    BYTE nButtonPress[XGetPortCount()][BUTTON_MAX];
    BOOL bControlPressed[XGetPortCount()][CONTROL_MAX];
    SHORT sThumbX[XGetPortCount()][JOYSTICK_MAX];
    SHORT sThumbY[XGetPortCount()][JOYSTICK_MAX];
    DWORD dwControllerState[XGetPortCount()];
private:
};

class USBDevice
{
public:
    PXPP_DEVICE_TYPE m_type; // XDEVICE type

public:
    USBDevice()
    {
        m_type = NULL;
    }

    ~USBDevice()
    {
        m_type = NULL;
    }

public:
    virtual void Insert( unsigned port, unsigned slot ) = 0;
    virtual void Remove( void ) = 0;
};


class DeviceRemote : public USBDevice
{
public:
    HANDLE m_hRemote;

public:
    DeviceRemote()
    {
        m_type = XDEVICE_TYPE_IR_REMOTE;
        m_hRemote = NULL;
    }

    DeviceRemote( unsigned port, unsigned slot )
    {
        m_type = XDEVICE_TYPE_IR_REMOTE;
        m_hRemote = NULL;

        Insert( port, slot );
    }

    ~DeviceRemote()
    {
        if( m_hRemote )
        {
            Remove();
        }
    }

public:
    void Insert( unsigned port, unsigned slot )
    {
        m_hRemote = XInputOpen( XDEVICE_TYPE_IR_REMOTE, port, XDEVICE_NO_SLOT, NULL );
        if( NULL == m_hRemote )
        {
            XDBGWRN( USBM_APP_TITLE_NAME_A, "DeviceRemote::Insert():Problem Opening Device!! Port - '%d', Error - '%d'", port, GetLastError() );
        }
    }

    void Remove( void )
    {
        XInputClose( m_hRemote );
        m_hRemote = NULL;
    }
};


class DeviceDuke : public USBDevice
{
public:
    HANDLE m_hDuke;
    XINPUT_POLLING_PARAMETERS* m_pPollingParameters;

public:
    DeviceDuke()
    {
        m_type = XDEVICE_TYPE_GAMEPAD;
        m_pPollingParameters = NULL;
        m_hDuke = NULL;
    }

    DeviceDuke( unsigned port, unsigned slot, XINPUT_POLLING_PARAMETERS *p )
    {
        m_type = XDEVICE_TYPE_GAMEPAD;
        m_pPollingParameters = p;
        m_hDuke = NULL;

        Insert( port, slot );
    }

    ~DeviceDuke()
    {
        if( m_hDuke )
        {
            Remove();
        }
    }

public:
    void Insert( unsigned port, unsigned slot )
    {
        m_hDuke = XInputOpen( XDEVICE_TYPE_GAMEPAD, port, 0, m_pPollingParameters );
        if( NULL == m_hDuke )
        {
            XDBGWRN( USBM_APP_TITLE_NAME_A, "DeviceDuke::Insert():Problem Opening Device!! Port - '%d', Error - '%d'", port, GetLastError() );
        }
    }

    void Remove( void )
    {
        XInputClose( m_hDuke );
        m_hDuke = 0;
    }
};

typedef struct _RUMBLE_INFO
{
    int nPort;
    WORD wForce;
    int nMilliseconds;
    USBManager* pUSBManager;
} RUMBLE_INFO, *PRUMBLE_INFO;

class USBManager
{
public:
    USBDevice *devices[XGetPortCount()][SLOT_MAX];
    DWORD packetNum[XGetPortCount()][SLOT_MAX];
    XINPUT_POLLING_PARAMETERS pollingParameters;
    XINPUT_STATE m_state;

public:
    USBManager();
    ~USBManager();

public:
    // Calling this will update the controller state objects with the latest info
    void ProcessInput( void );

    // Calling this will check to see if controllers we added or removed
    void CheckForHotplugs( void );

    // This will allow you to determine if a remote control button is 'depressed'.  If an address to a BOOLean
    // variable is provided, it will return whether or not it's the 'first' press of the button
    BOOL IsRemoteButtonPressed( int port, enum REMOTE_BUTTONS button, BOOL* bFirstPress=NULL );

    // This will allow you to determine if a button is 'depressed'.  If an address to a BOOLean
    // variable is provided, it will return whether or not it's the 'first' press of the button
    BOOL IsButtonPressed( int port, enum BUTTONS button, int nThreshold=0, BOOL* bFirstPress=NULL );

    // This will allow you to determine if a control is 'depressed'.  If an address to a BOOLean
    // variable is provided, it will return whether or not it's the 'first' press of the control
    BOOL IsControlPressed( int port, enum CONTROLS control, BOOL* bFirstPress=NULL );

    // This will allow you to get the value of the X or Y axis, adjusting for a "dead zone".
    // If a dead zone is passed in, the call will return '0' unless the value of the X or Y axis
    // is greater than the dead zone value.  If an address to a BOOLean variable is provided, it
    // will return whether or not it's the 'first' press of the joystick
    int  GetJoystickX( int port, enum JOYSTICK, int deadZone=0, BOOL* bFirstPress=NULL );
    int  GetJoystickY( int port, enum JOYSTICK, int deadZone=0, BOOL* bFirstPress=NULL );

    // Calling this will determine if there was movement for any item on a controller
    // Will return TRUE if any button or thumbstick has changed since last poll
    BOOL MovementDetected( int port, int deadZone=0 );

    // Will send a rumble command to the specified device
    void Rumble( int port, WORD wForce, int nMilliseconds );

private:
    CControllerState m_ControllerState;
    CControllerState m_PrevControllerState;

    HANDLE m_hThreads[4];
    RUMBLE_INFO m_RumbleInfo[4];

    static friend DWORD WINAPI RumbleThread( LPVOID lpParameter );
};

#endif // _USBMANAGER_H_