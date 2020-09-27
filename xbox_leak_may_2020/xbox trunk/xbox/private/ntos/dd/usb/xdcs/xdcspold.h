#ifndef __XDCSP_H__
#define __XDCSP_H__
/*++

Copyright (c) Microsoft Corporation

Module Name:

    XDCSP.H

Abstract:

    Private Header file XDCS device driver.
   
Environment:

    Xbox

Revision History:
    
    02-23-01 : Mitchell Dernis(mitchd) - created

--*/

#include <xdcs.h>

//***************************************
// XDCS constants and structures from the
// XDCS specification
//***************************************
#define USB_DEVICE_CLASS_XDCS 0x59

#define XDCS_REQUEST_GET_ROM_FILE_INFO  1
#define XDCS_REQUEST_GET_ROM_FILE_BLOCK 2
#define XDCS_BLOCK_LENGTH               1024 //1k blocks

#include <pshpack1.h>
typedef struct _XDCS_ROM_INFORMATION
{
    USHORT bcdCodeVersion;
    ULONG  dwCodeLength;
} XDCS_ROM_INFORMATION, *PXDCS_ROM_INFORMATION;
#include <poppack.h>

//****************************************
// Internally useful constants
//****************************************
#define XDCS_NUM_BLOCKS_TO_VERIFY 3           //Number of blocks to verify against
                                              //against a dongle
#define XDCS_CONTROL_TD_QUOTA     (3+1024/64) //1k transfer with 64 byte fifo
#define XDCS_MAX_PORTS            4           //The maximum number of ports supported
#define XDCS_FLAG_CONNECTED       0x01        //set if the device is conneceted
#define XDCS_FLAG_IN_USE          0x02        //set if the device is currently being
                                              //accessed by the 
#define XDCS_MAX_TRIES            3           //number of times we try an XDCS before
                                              //reporting it as not responding

#define XDCS_DVD_LIB_NAME         "t:\\testdvdlib.cab"
//****************************************
// Definitions for the buffers
//****************************************
#define XDCS_BUFFER_COUNT   3
#define XDCS_BUFFER_START   0
#define XDCS_BUFFER_FILE    1
#define XDCS_BUFFER_DEVICE  2
#define XDCS_BUFFER_DONE    3
#define DEFAULT_URB XDCS_Globals.Buffers[0].Urb

//****************************************
// Internally useful macros
//****************************************
#define SET_FLAG(variable, flag)  ((variable) |= (flag))
#define CLEAR_FLAG(variable, flag)  ((variable) &= ~(flag))
#define TEST_FLAG(variable, flag)  ((variable)&(flag))
#define TEST_ANY_FLAG(variable, flag)  ((variable)&(flag))
#define TEST_ALL_FLAGS(variable, flag)  ((flag)==((variable)&(flag)))

//****************************************
// Internal data structures
//****************************************

/*++
    XDCS_DEVICE - Represents the state of
    one of the device ports and the device
    plugged into if, any.
--*/
typedef struct _XDCS_DEVICE
{
    UCHAR       bFlags;
    UCHAR       bInterfaceNumber;
    USHORT      Pad;
    IUsbDevice *Device;
} XDCS_DEVICE, *PXDCS_DEVICE;

/*++
    XDCS_DOWNLOAD_BUFFER - During download of the code
    this structure is ping-ponged between reading the code
    from the device and writing it to file.

    There are XDCS_BUFFER_COUNT of these in the globals structure
    so that multiple requests can be outstanding at a time.

    Members of some of the instance of this structure are borrowed
    in a few places, just to be careful about using memory.
--*/
typedef struct _XDCS_DOWNLOAD_BUFFER
{
    DWORD  dwStage;
    union
    {
        URB        Urb;
        OVERLAPPED Overlapped;
    };
    CHAR  buffer[XDCS_BLOCK_LENGTH]; //For download
    
} XDCS_DOWNLOAD_BUFFER, *PXDCS_DOWNLOAD_BUFFER;

/*++
    XDCS_GLOBALS - this structure binds all the globals into
    a single structure.
--*/
typedef struct _XDCS_GLOBALS
{
    //LibrayInfo is the portion that the client can read at will
    XDCS_DVD_LIBRARY_INFORMATION LibraryInfo;

    //Additional information about the library, which is useful to 
    //cache, but the client doesn't need to see it
    DWORD                        dwLastBlockLength;
    DWORD                        dwBlockCount;

    //Keep track of devices, and which one (if any has been used
    //to verify the library)
    PXDCS_DEVICE                 CurrentDevice;
    XDCS_DEVICE                  Devices[XDCS_MAX_PORTS];

    //Event to waken worker thread
    KEVENT                       ThreadEvent;

    //Utility memory for doing the download
    HANDLE                       BufferEvents[XDCS_BUFFER_COUNT];
    XDCS_DOWNLOAD_BUFFER         Buffers[XDCS_BUFFER_COUNT];
} XDCS_GLOBALS, *PXDCS_GLOBALS;

#endif //__XDCSP_H__