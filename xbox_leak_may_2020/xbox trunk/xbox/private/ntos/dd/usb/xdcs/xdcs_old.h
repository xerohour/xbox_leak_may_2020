#ifndef __XDCS_H__
#define __XDCS_H__
/*++

Copyright (c) Microsoft Corporation

Module Name:

    XDCS.H

Abstract:

    Header file exposed to the private clients of the XDCS device.

    Client API:

      XGetDvdPlaybackLibraryInfo

Usage:

    Sometime after calling XInitDevices (you do not need to specify the XDCS device
    in the list of required devcies), call XGetDvdLibraryInfoPointer to get a pointer
    to the global XDCS_DVD_LIBRARY_INFORMATION.  This information is updated asynchronously
    by the driver.  Since DWORD read\writes are atomic, the client may poll dwStatus.
    
    When dwStatus is XDCS_STATUS_ACCEPTED the dwVersion and szName fields can be considered valid.

    Rather than polling, the client may wait on hStatusChangeEvent.  This event is set
    whenever the status changes.  This way other threads will run more efficiently while
    waiting for an accepted or rejected status.  It is an auto reset event.

    szName and dwVersion are only valid when dwStatus is XDCS_STATUS_ACCEPTED.  The file returned
    is the uncompressed version (there will be a compressed version stored somwhere too.)

    dwPercentComplete and dwTimeRemaining can be polled to display a progress bar, while the status
    is XDCS_STATUS_VERIFYING or XDCS_DOWNLOADING.

    The status will return to XDCS_STATUS_NO_DONGLE whenever the device is removed.  The filename
    may or may not change, if and when another device is inserted.  The filename will probably
    be a derivative of the library version.  I expect to probably maintain up to three different
    versions on the harddisk at a time.

    If there is more than one device present, it will always use the first one found, which is kind of
    random.

Library:
    
    You must explictly link XDCS.LIB to use this library.
    
Environment:

    Xbox

Revision History:
    
    02-22-01 : Mitchell Dernis(mitchd) - created

--*/

#define XDCS_STATUS_INIT         0
#define XDCS_STATUS_NO_DONGLE    1
#define XDCS_STATUS_VERIFYING    2
#define XDCS_STATUS_DOWNLOADING  3 // (including the decompress time)
#define XDCS_STATUS_ACCEPTED     4
#define XDCS_STATUS_REJECTED     5

typedef struct _XDCS_DVD_LIBRARY_INFORMATION
{
    DWORD   dwStatus;              //status of the DVD library
    DWORD   dwPercentComplete;     //percent complete if dwStatus is XDCS_STATUS_VERIFYING or XDCS_STATUS_DOWNLOADING
    DWORD   dwTimeRemaining;       //time remaining (ms) for verify or download (guess)
    HANDLE  hStatusChangeEvent;    //handle to event which is signaled whenever dwStatus is updated.
    DWORD   dwCodeLength;          //length of the code stored in ROM   
    WORD    bcdVersion;            //msb = major version, lsb = minor version each byte is bcd. 0x0110 = version 01.10
    CHAR    szName[MAX_PATH];      //filename of code library
} XDCS_DVD_LIBRARY_INFORMATION, *PXDCS_DVD_LIBRARY_INFORMATION;

PXDCS_DVD_LIBRARY_INFORMATION XGetDvdLibraryInfoPointer();


#endif //__XDCS_H__

