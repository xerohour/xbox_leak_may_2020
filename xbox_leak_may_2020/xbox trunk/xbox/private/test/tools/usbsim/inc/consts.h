/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    consts.h

Abstract:

    Constant values.

    typedefs will be available in the COM object's type library

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/

#ifndef _USB_CONSTS_H_
#define _USB_CONSTS_H_

#ifndef __USBSIMULATORLib_LIBRARY_DEFINED__ // prevent multiple definitions within COM object

//
// Error Codes
//
typedef enum
    {
    USBSIM_ERROR_OK                =  0,  // A-OK
    USBSIM_ERROR_CONNECTED         =  1,  // Device is already connected
    USBSIM_ERROR_NOTCONNECTED      =  2,  // Device is not connected
    USBSIM_ERROR_CONNECT_FAILED    =  3,  // Use GetLastError to get more information
    USBSIM_ERROR_SOCKET_ERROR      =  4,  // Use GetLastError to get more information
    USBSIM_ERROR_INVALID_USB_PORT  =  5,  // Parameter is out of range or denotes a port currently not in use
    USBSIM_ERROR_INVALID_SIMULATOR =  6,  // Parameter is out of range or denotes a port currently not in use
    USBSIM_ERROR_INVALID_DEVICE    =  7,  // Pointer to a USBDevice is not valid
    USBSIM_ERROR_USER_ABORT        =  8,  // Abort specifed by the user
    USBSIM_ERROR_TIMEOUT           =  9,  // Timeout waiting for response from simulator
    USBSIM_ERROR_DATA_CORRUPTED    = 10,  // Got data from the simulator but it is likely corrupted
    USBSIM_ERROR_SEND_FAILURE      = 11,  // Use GetLastError to get more information
    } USBSimulatorErrorCodes;


//
// USB Endpoint Mode
//
typedef enum
    {
    ENDPOINT_DISABLE = 0,
    ENDPOINT_ENABLE,
    ENDPOINT_AUTOREPEAT
    } EndpointModes;


//
// Device Type IDs
//
typedef enum
    {
    DEVICE_UNKNOWN = -1,
    DEVICE_DUKE,
    DEVICE_MU,
    DEVICE_HAWK,
    DEVICE_DVDREMOTE,
    DEVICE_MAX_TYPES
    } DeviceTypes;


//
// Logging class
//
typedef enum 
    {
    RECORDING_FORMAT_TEXT_LOG,
    RECORDING_FORMAT_CHIEF_UTG,
    RECORDING_FORMAT_TEXT_XML
    } RecordingFormats;

//
// Port values used in USBDevice::Plug
//
typedef enum
    {
    USBSIM_HOST_PORT1 = 1,
    USBSIM_HOST_PORT2,
    USBSIM_HOST_PORT3,
    USBSIM_HOST_PORT4
    } USBSimulatorPorts;


//
// USB PID values (USB Specification section 8.3.1 table 8-1)
//
#define USB_PID_MDATA       0xF0
#define USB_PID_DATA2       0xE1
#define USB_PID_DATA1       0xD2
#define USB_PID_DATA0       0xC3
#define USB_PID_SETUP       0xB4
#define USB_PID_SOF         0xA5
#define USB_PID_IN          0x96
#define USB_PID_OUT         0x87
#define USB_PID_STALL       0x78
#define USB_PID_NYET        0x69
#define USB_PID_NAK         0x5A
#define USB_PID_ACK         0x4B
#define USB_PID_ERR         0x3C
#define USB_PID_PING        0x2D
#define USB_PID_SPLIT       0x1E
#define USB_PID_RESERVED    0x0F
#define USB_PID_PRE         USB_PID_ERR

#endif // __USBSIMULATORLib_LIBRARY_DEFINED__
#endif // _USB_SIMULATOR_H_
