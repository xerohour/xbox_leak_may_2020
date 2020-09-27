/*++

Copyright (c) Microsoft Corporation

Module Name:

    XDCS.CPP

Abstract:

    This source file implements the major entry points to the Xbox DVD Code Server
    driver (XDCS).  XDCS is a USB interface based device that contains the DVD playback
    code library.  The code resides in ROM on the device.
    
    Basically, this driver maintains the current version of the library, including: downloading
    the library and saving it to a file, verifying the presence of the dongle, and providing the
    validated library file and version info to the client.  This driver is currently private and
    the only intended client is the XDash application.


    The entire implementation is in this file.

    Client API:

      XGetDvdLibraryInfoPointer

    Entries required by the USB core stack:

      XDCS_Init
      XDCS_AddDevice
      XDCS_RemoveDevice

Environment:

    Xbox USB class driver.

Revision History:
    
    02-22-01 : Mitchell Dernis(mitchd) - created

Implementation and Use Overview:

    1) The driver must be linked explicitly as it is not a publically support device.

    2) XDCS devices are not enumerable by clients.  There is no public device type name.  
    
    3) The first XDCS device to be enumerated (i.e. XDCS_AddDevice is called) is the device used.

    4) During XInitDevices, a configuration file is read from the harddisk (or created if not present)
       with information about the DVD library currently on the harddisk, including: version number, and
       the filename, and length.  This information is read into a global data structure.  Other fields
       indicate whether or not the library has been verified against the device, and the status of
       enumeration, download and verification, etc.

    5) Calling XGetDvdPlaybackLibraryInfo returns a pointer (to be consider read only by the client)
       to the global library information structure.

    6) When ever the persistant fields of the library info change, the file is updated.

    7) The library status goes the through the following stages:
        a) XDCS_STATUS_INIT        - stays in the state for up to 2 seconds after XInitDevices.
        b) XDCS_STATUS_NO_DONGLE   - there is no dongle present.  The user should insert one.
        c) XDCS_STATUS_VERIFYING   - dongle found and the library on the harddisk is being verified against
                                     the code on the device.
        d) XDCS_STATUS_DOWNLOADING - dongle found and is being downloaded (either there is no code on the
                                     harddisk or the version is different).
        e) XDCS_STATUS_ACCEPTED   - dongle found and the code has been verified against the code on the dongle
                                     or it has been newly downloaded.
        f) XDCS_STATUS_REJECTED    - dongle found, BUT the code did not match the code on the harddisk despite
                                     being the same version, and the code could not be successfully downloaded.

    8) There are two progress fields:  percent complete and time remaining.  The percent complete is increments of 
        1%.  The timing remaing is a guess in milliseconds.  Initially the guess is based on the original dongle,
        but it dynamically updates taking into account how much has already been transfered, and how long it has taken.

    9) There is a status change event.  It is has an automatic reset property.  The event is set every time the status
       field is changed.  It is optional.

    10) The filename and code version are used internally at all times, the client should only consider these fields valid
        when the status is XDCS_STATUS_ACCEPTED.

--*/

//*****************************************************************************
// Headers
//*****************************************************************************

// Xbox Kernel Service Headers
#define _XAPI_
extern "C" {
#include <ntos.h>
}
#include <ntrtl.h>
#include <nturtl.h>

// Xbox System Software Headers
#include <xtl.h>

// Useful Debug definitions
#define MODULE_POOL_TAG  'SCDX'
#include <debug.h>
DEFINE_USB_DEBUG_FUNCTIONS("XDCS");

// Xbox USB Class driver header
#include <usb.h>

// XDCS specific header
#include "xdcsp.h"


//*****************************************************************************
// For debugging the download code
//*****************************************************************************
#if DBG
    struct XDCS_STAGE_LOG
    {
        DWORD  dwStage;
        DWORD  dwBufferIndex;
    };
    
    DWORD XDCS_StageLogEntries;
    XDCS_STAGE_LOG XDCS_StageLog[100];
#define XDCS_INIT_STAGE_LOG() (XDCS_StageLogEntries = 0)
#define XDCS_LOG(dwBufferIndex)\
    {\
    XDCS_StageLog[XDCS_StageLogEntries].dwStage = XDCS_Globals.Buffers[dwBufferIndex].dwStage;\
    XDCS_StageLog[XDCS_StageLogEntries].dwBufferIndex = dwBufferIndex;\
    XDCS_StageLogEntries++;\
    }
#else 
    #define XDCS_INIT_STAGE_LOG()
    #define XDCS_LOG(dwBufferIndex)
#endif

//*****************************************************************************
//  Declare XDCS types and class.
//*****************************************************************************
USB_DEVICE_TYPE_TABLE_BEGIN(XDCS_)
USB_DEVICE_TYPE_TABLE_ENTRY(NULL)
USB_DEVICE_TYPE_TABLE_END()
USB_CLASS_DRIVER_DECLARATION(XDCS_, USB_DEVICE_CLASS_XDCS, 0, 0)
#pragma data_seg(".XPP$ClassXdcs")
USB_CLASS_DECLARATION_POINTER(XDCS_)
#pragma data_seg(".XPP$Data")

//*****************************************************************************
// Declare Globals
//*****************************************************************************
USB_RESOURCE_REQUIREMENTS XDCS_gResourceRequirements = 
    {USB_CONNECTOR_TYPE_DIRECT, 1, 2, 1, 0, 0, XDCS_CONTROL_TD_QUOTA, 0, 0, 0};

XDCS_GLOBALS XDCS_Globals = {0};

// LOCALFUNC is a useful tag so that functions defined at file scope can be
// declared static in release builds (conserves global namespace, and perhaps
// allows better optimization), but non-static in debug for ease of debugging.
#if DBG
#define LOCALFUNC
#else 
#define LOCALFUNC static
#endif

//*****************************************************************************
// Declare Locally Used and Defined methods
//*****************************************************************************
__inline VOID XDCS_ChangeStatus(DWORD dwStatus);
LOCALFUNC VOID FASTCALL XDCS_fCompleteRemove(PXDCS_DEVICE XdcsDevice);
DWORD WINAPI XDCS_WorkThread(LPVOID);
LOCALFUNC BOOL FASTCALL XDCS_fVerifyFile();
LOCALFUNC BOOL FASTCALL XDCS_fDownloadFile();
VOID XDCS_SignalComplete(PURB, PVOID pEvent);
LOCALFUNC VOID FASTCALL XDCS_fCreateBufferEvents();
LOCALFUNC DWORD FASTCALL XDCS_fRandomBlock(DWORD BlockCount);


//*****************************************************************************
// Implementation of API
//*****************************************************************************
PXDCS_DVD_LIBRARY_INFORMATION XGetDvdLibraryInfoPointer()
{
    return &XDCS_Globals.LibraryInfo;
}

//*****************************************************************************
// Implementation of USB Required Entry Points:
//  XDCS_Init,
//  XDCS_AddDevice,
//  XDCS_RemoveDevice
//*****************************************************************************
#pragma code_seg(".XPPCINIT")

EXTERNUSB VOID XDCS_Init(IUsbInit *pUsbInit)
/*++
    Routine Description:

        Called by the USB core stack when XDCS is linked in to the application.
        Initialize our globals, register our driver resource requirements, set
        a timer guess when we first detect devices.
--*/
{
    USB_DBG_ENTRY_PRINT(("Entering XDCS_Init"));
    
    XDCS_Globals.LibraryInfo.dwStatus = XDCS_STATUS_INIT;
    XDCS_Globals.CurrentDevice = NULL;
    
    //Setup event handle
    XDCS_Globals.LibraryInfo.hStatusChangeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    ASSERT(XDCS_Globals.LibraryInfo.hStatusChangeEvent);

    //Initialize the event to signal our thread.
    KeInitializeEvent(&XDCS_Globals.ThreadEvent, SynchronizationEvent, FALSE);
    
    //Create buffer events
    XDCS_fCreateBufferEvents();

    //CreateThread, we need this to the File I/O work
#if DBG
    HANDLE thread = 
#endif
    CreateThread(NULL, 65536, XDCS_WorkThread, NULL, 0, NULL);
    ASSERT(thread);

    // register resources
    pUsbInit->RegisterResources(&XDCS_gResourceRequirements);

    USB_DBG_EXIT_PRINT(("Exiting XDCS_Init"));
    return;
}
#pragma code_seg(".XPPCODE")

EXTERNUSB VOID XDCS_AddDevice(IN IUsbDevice *Device)
/*++

    Routine Description:
    
      Records that the device is present and relevant info that
      it may need later.  Then completes the add.

      If there is currently no device in use (i.e. backing a
      library that was downloaded from the device or checked
      against the device), then we download the code library or
      check a library already on the harddisk.

    Arguments:
        Device - Interface to USB core stack for device.

    Notes:
        AddDevice routines are always called at DPC level.
--*/
{
    PXDCS_DEVICE xdcsDevice;
    ULONG ulPort;

    USB_DBG_ENTRY_PRINT(("XDCS_AddDevice(Device=0x%0.8x)", Device));
    
    //Get and check the port. (It should only work direct connect).
    ulPort = Device->GetPort();
    if(ulPort >= XDCS_MAX_PORTS)
    {
        Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
        return;
    }
    
    xdcsDevice = &XDCS_Globals.Devices[ulPort];
    
    //This device should not already be marked as connected
    ASSERT(!TEST_FLAG(xdcsDevice->bFlags,XDCS_FLAG_CONNECTED));

    //Mark the device as connected
    SET_FLAG(xdcsDevice->bFlags,XDCS_FLAG_CONNECTED);
    xdcsDevice->bInterfaceNumber = Device->GetInterfaceNumber();
    Device->SetExtension((PVOID)xdcsDevice);
    xdcsDevice->Device = Device;
    
    //Complete the Add - this allows enumeration to continue, it
    //also has a couple of implications.
    // 1) The default endpoint is no longer opened when this returns
    // 2) A remove call can come for this device, as soon as we
    //    return from this routine.  (Not before, as we are at DPC
    //    the whole time.)
    Device->AddComplete(USBD_STATUS_SUCCESS);

    // 
    // If there is not currently a current device, make this
    // the current device.
    //
    if(!XDCS_Globals.CurrentDevice)
    {
        XDCS_Globals.CurrentDevice = xdcsDevice;
        KeSetEvent(&XDCS_Globals.ThreadEvent, IO_NO_INCREMENT, FALSE);
    }

    USB_DBG_EXIT_PRINT(("ExitingXDCS_AddDevice"));
    return;
}

EXTERNUSB VOID
XDCS_RemoveDevice (
    IN IUsbDevice *Device
    )
/*++
    Routine Description:
       
      Records the removal, and completes the remove.
      
      If the status of the DVD code is XDCS_STATUS_ACCEPTED and the device that was removed, is the device
      the code was verified against then the status is changed to XDCS_STATUS_NO_DEVICE.  If other devices
      are present, then verification may take place against another device.

    Arguments:
        Device - Interface to USB core stack for device that is to be
                 removed.
    Notes:
        RemoveDevice routines are always called at DPC level.
--*/
{
    USB_DBG_ENTRY_PRINT(("XDCS_RemoveDevice(Device=0x%0.8x)", Device));
    PXDCS_DEVICE xdcsDevice = (PXDCS_DEVICE) Device->GetExtension();

    //Mark the device as not connected.
    CLEAR_FLAG(xdcsDevice->bFlags,XDCS_FLAG_CONNECTED);

    //If the device is not in use (i.e. currently be accessed by our
    //worker thread, then we are done.  The worker thread will see
    //that the connected flag has been cleared, and will finish the removal.
    if(!TEST_FLAG(xdcsDevice->bFlags,XDCS_FLAG_IN_USE))
    {
       XDCS_fCompleteRemove(xdcsDevice);
    }

    USB_DBG_EXIT_PRINT(("exiting XDCS_RemoveDevice"));
    return;
}


//*****************************************************************************
// Implementation of Local Methods:
//  XDCS_ChangeStatus,
//*****************************************************************************

__inline VOID XDCS_ChangeStatus(DWORD dwStatus)
/*++
  Routine Description:
    Utility function that ensures that the status change event is signaled
    whenever the status is changed.
--*/
{
    XDCS_Globals.LibraryInfo.dwStatus = dwStatus;
    SetEvent(XDCS_Globals.LibraryInfo.hStatusChangeEvent);
}


LOCALFUNC
VOID
FASTCALL
XDCS_fCompleteRemove(
    PXDCS_DEVICE XdcsDevice
    )
/*++
    Routine Description:

      Calls IUsbDevice::RemoveComplete for the device.  All endpoints
      have to be closed (and there can be no outstanding traffic), so
      this routine is called either from XDCS_RemoveDevice (if the
      device was not being used when it was removed_, or it is called
      from the worker thread, when it is done trying to talk to the device.

      After calling remove complete, it checks to see if the removed device
      was the current device (i.e. the code library was downloaded or verified
      from this device).  If it was the current device, the status is changed
      to XDCS_STATUS_NO_DONGLE and a search is done for a new current device.

--*/
{
    //
    // We can complete the removal
    //
    XdcsDevice->Device->SetExtension(NULL);
    XdcsDevice->Device->RemoveComplete();
    XdcsDevice->Device = NULL;
    
    //
    // If the device just removed was the one backing
    // the DVD code library, we must change the status to not connected
    // then we can search for another and try and validate
    // the code library against it (or download a new one from another
    // device).
    //
    if(XDCS_Globals.CurrentDevice == XdcsDevice)
    {
        XDCS_Globals.CurrentDevice = NULL;
        XDCS_ChangeStatus(XDCS_STATUS_NO_DONGLE);
        //
        //Search for a connected XDCS, and reestablish a code library
        //
        for(ULONG index = 0; index < XDCS_MAX_PORTS; index++)
        {
            if(TEST_FLAG(XDCS_Globals.Devices[index].bFlags,XDCS_FLAG_CONNECTED))
            {
                //
                //  We found one, set it as the current device and signal
                //  the thread event to awaken our worker.
                //
                XDCS_Globals.CurrentDevice = &XDCS_Globals.Devices[index];
                KeSetEvent(&XDCS_Globals.ThreadEvent, IO_NO_INCREMENT, FALSE);
              
            }
        }
    }
}

DWORD 
WINAPI
XDCS_WorkThread(LPVOID)
/*++

  Routine Description:

    This routine is a worker thread for the XDCS driver.
    It sleeps until signalled, by XDCS_AddDevice (or in
    some circumstance as a result of XDCS_RemoveDevice).

    When woken up it looks XDCS_Globals.CurrentDevice and
    finds and verifies a Dvd playback code library against
    the ROM, or if necessary downloads the code
    XDCS_Globals.CurrentDevice.  When it is done, it goes
    back to sleep.

--*/
{
    
    XDCS_ROM_INFORMATION      romInfo;
    DWORD                     dwReason;
    LARGE_INTEGER             liWait;
    PLARGE_INTEGER            pInitialWait = &liWait;
    USBD_STATUS               usbdStatus;
    KIRQL                     oldIrql;
    BOOL                      fSuccess;
    ULONG                     ulRetry=0;

    USB_DBG_ENTRY_PRINT(("Entering XDCS_WorkThread"));
    //leave up to 2 seconds in XDCS_STATUS_INIT
    liWait.QuadPart = -10000*1000*2;

    // Thread loops forever and cannot be terminated
    while(1)
    {
        //
        // Wait until signaled to process XDCS device
        //
        dwReason = KeWaitForSingleObject(
                        &XDCS_Globals.ThreadEvent,
                        Executive,
                        KernelMode,
                        FALSE,
                        pInitialWait
                        );
        //
        //  The first time through, we may timeout
        //  at which time the library status must
        //  be changed from XDCS_STATUS_INIT to 
        //  XDCS_STATUS_NO_DONGLE
        //
        pInitialWait = NULL;
        if(WAIT_TIMEOUT==dwReason)
        {
          XDCS_ChangeStatus(XDCS_STATUS_NO_DONGLE);
          continue;
        }

        //
        //  Synchronize with XDCS_AddDevice and
        //  XDCS_RemoveDevice
        //
        oldIrql = KeRaiseIrqlToDpcLevel();

        //
        //  If the current device is still connected mark it in use, so that
        //  it doesn't get removed while we are mucking with it.
        //
        if(TEST_FLAG(XDCS_Globals.CurrentDevice->bFlags,XDCS_FLAG_CONNECTED))
        {
            SET_FLAG(XDCS_Globals.CurrentDevice->bFlags,XDCS_FLAG_IN_USE);
        } else
        {
            XDCS_fCompleteRemove(XDCS_Globals.CurrentDevice);
            KeLowerIrql(oldIrql);
            continue;
        }

        //
        //  Open the default endpoint
        //  Open is not allowed to fail, we don't even check
        //
        USB_BUILD_OPEN_DEFAULT_ENDPOINT(&DEFAULT_URB.OpenEndpoint);
        XDCS_Globals.CurrentDevice->Device->SubmitRequest(&DEFAULT_URB);
        
        //
        //  Build the XDCS_REQUEST_GET_ROM_FILE_INFO request
        //  (notice that this is synchronous)
        //
        USB_BUILD_CONTROL_TRANSFER(
            &DEFAULT_URB.ControlTransfer,
            NULL,
            (PVOID)&romInfo,
            sizeof(XDCS_ROM_INFORMATION),
            USB_TRANSFER_DIRECTION_IN,
            NULL,
            NULL,
            FALSE,
            USB_DEVICE_TO_HOST|USB_VENDOR_COMMAND|USB_COMMAND_TO_INTERFACE,
            XDCS_REQUEST_GET_ROM_FILE_INFO,
            0,
            XDCS_Globals.CurrentDevice->bInterfaceNumber,
            sizeof(XDCS_ROM_INFORMATION)
            );

        //
        //  Submit it (must be done at passive level), and the check for errors.
        //
        KeLowerIrql(oldIrql);
        usbdStatus = XDCS_Globals.CurrentDevice->Device->SubmitRequest(&DEFAULT_URB);
        if(USBD_SUCCESS(usbdStatus))
        {
            //
            //  Set the version and filename based on the rom version
            //
            XDCS_Globals.LibraryInfo.bcdVersion = romInfo.bcdCodeVersion;
            XDCS_Globals.LibraryInfo.dwCodeLength = romInfo.dwCodeLength;
            
            
            /*
             * Don't do this!  As per Chris's request we will use only one filename for all versions.
             * This way files don't accumulate when you start plugging in dongles of different versions.
             *
            
            wsprintfA(
                XDCS_Globals.LibraryInfo.szName,
                "y:\\DvdLib_v%x.%0.2x.cab", 
                (ULONG)((romInfo.bcdCodeVersion&0xff00)>>8),
                (ULONG)(romInfo.bcdCodeVersion&0x00ff)
                );
             *
             * If this does get put back, we have to make sure that there is some code, to 
             * manage this files and make sure that not more or two or three ever accumulate.
             *
             */
            RtlZeroMemory(&XDCS_Globals.LibraryInfo.szName, sizeof(XDCS_Globals.LibraryInfo.szName));
            wsprintfA(XDCS_Globals.LibraryInfo.szName, XDCS_DVD_LIB_NAME);

            //
            //  Calculate the blockCount, and the length of the last block.
            //
            XDCS_Globals.dwBlockCount = XDCS_Globals.LibraryInfo.dwCodeLength/XDCS_BLOCK_LENGTH;
            XDCS_Globals.dwLastBlockLength = XDCS_Globals.LibraryInfo.dwCodeLength%XDCS_BLOCK_LENGTH;
            if(XDCS_Globals.dwLastBlockLength)
            {
                XDCS_Globals.dwBlockCount++;
            } else
            {
                XDCS_Globals.dwLastBlockLength = XDCS_BLOCK_LENGTH;
            }

            //
            //  Try to the verify the file (whether or not the file
            //  is there).
            //
            fSuccess = XDCS_fVerifyFile();
            if(!fSuccess)
            {
                //
                //  Since we couldn't verify the file
                //  just try to download it.
                //
                fSuccess = XDCS_fDownloadFile();
            }
        } else
        {
            fSuccess = FALSE;
        }
        //
        //  Build and Submit the close URB
        //
        USB_BUILD_CLOSE_DEFAULT_ENDPOINT(
            &DEFAULT_URB.CloseEndpoint,
            NULL,
            NULL
            )
        XDCS_Globals.CurrentDevice->Device->SubmitRequest(&DEFAULT_URB);

        //
        // Synchronize with XDCS_AddDevice and XDCS_RemoveDevice
        // again.
        //
        oldIrql = KeRaiseIrqlToDpcLevel();

        //
        //  Turn off the in use flag
        //
        //
        CLEAR_FLAG(XDCS_Globals.CurrentDevice->bFlags,XDCS_FLAG_IN_USE);
        //Check to see if the device is still connected
        if(TEST_FLAG(XDCS_Globals.CurrentDevice->bFlags,XDCS_FLAG_CONNECTED))
        {
            //
            // The device is connected, but it failed, we need a remedy
            // the simplest thing is to try again.
            //
            if(!fSuccess)
            {
                //
                // If exceeded the try count, report the device as not responding.
                // When the device is removed, prior to being reset, an attempt
                // will be made to try another device if present.
                //
                if(++ulRetry > XDCS_MAX_TRIES)
                {
                    XDCS_Globals.CurrentDevice->Device->DeviceNotResponding();
                    ulRetry = 0;
                } else
                //
                //  The reset was not exceeded, so just set our thread event
                //  so we don't stop at the wait when it loops through again.
                //
                {
                    //Set the event, and going through the loop will 
                    KeSetEvent(&XDCS_Globals.ThreadEvent, IO_NO_INCREMENT, FALSE);
                }
            } else
            {
                //We succeeded, so just 0 out the retry count.
                ulRetry = 0;
            }
        } else
        {
            //
            // The device was removed, while we were try to process it.
            // Whether it succeeded or not remove complete, will handle it.
            //
            XDCS_fCompleteRemove(XDCS_Globals.CurrentDevice);
        }
        KeLowerIrql(oldIrql);
    }
    return 0;
}

LOCALFUNC BOOL FASTCALL XDCS_fVerifyFile()
{
    
    HANDLE hFile;
    BOOL   fResult;
    //
    //  Trying opening the file
    //
    hFile = CreateFile(
        XDCS_Globals.LibraryInfo.szName,    
        GENERIC_READ,                       
        0,                                  
        NULL,                              
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
        );
    
    //
    //  If the file doesn't exist, then we fail
    //  (We sure hope it didn't fail for other reasons.)
    if(INVALID_HANDLE_VALUE == hFile)
    {
        return FALSE;
    }
    
    //
    //  Get the size of the file, and check it against
    //  the size we are expecting.
    //
    if(XDCS_Globals.LibraryInfo.dwCodeLength != GetFileSize(hFile, NULL))
    {
        CloseHandle(hFile);
        return FALSE;
    }

    //From here on, assume success, until we fail
    fResult = TRUE;

    //
    //  Switch the status to verifying
    //  
    XDCS_Globals.LibraryInfo.dwPercentComplete = 0;
    XDCS_ChangeStatus(XDCS_STATUS_VERIFYING);

    //
    //  Now go through and check a few random blocks.
    //
    DWORD dwBlockIndex;
    DWORD dwOffset;
    DWORD dwSize, dwBytesRead;

    int  loop;
    for(loop=0; loop<XDCS_NUM_BLOCKS_TO_VERIFY; loop++)
    {
        // Choose a random block
        dwBlockIndex = XDCS_fRandomBlock(XDCS_Globals.dwBlockCount);
        
        //Figure out the offset and size
        dwOffset = dwBlockIndex * XDCS_BLOCK_LENGTH;
        dwSize = (dwBlockIndex == XDCS_Globals.dwBlockCount-1) ?
                                XDCS_Globals.dwLastBlockLength : XDCS_BLOCK_LENGTH;

        //Zero the buffers, so there is no memory of what was read in the past
        RtlZeroMemory(XDCS_Globals.Buffers[0].buffer, dwSize);
        RtlZeroMemory(XDCS_Globals.Buffers[1].buffer, dwSize);

        // Read the file into the first buffer
        SetFilePointer(hFile, dwOffset, NULL, FILE_BEGIN);
        if(ReadFile(hFile, XDCS_Globals.Buffers[0].buffer, dwSize, &dwBytesRead, NULL))
        {
            USBD_STATUS usbdStatus;
            //Read from the ROM into the second buffer.
            USB_BUILD_CONTROL_TRANSFER(
                &DEFAULT_URB.ControlTransfer,
                NULL,
                (PVOID)XDCS_Globals.Buffers[1].buffer,
                dwSize,
                USB_TRANSFER_DIRECTION_IN,
                NULL, //synchronous
                NULL,
                FALSE,
                USB_DEVICE_TO_HOST|USB_VENDOR_COMMAND|USB_COMMAND_TO_INTERFACE,
                XDCS_REQUEST_GET_ROM_FILE_BLOCK,
                (WORD)dwBlockIndex,
                (WORD)XDCS_Globals.CurrentDevice->bInterfaceNumber,
                (WORD)dwSize
                );
            usbdStatus = XDCS_Globals.CurrentDevice->Device->SubmitRequest(&DEFAULT_URB);
            //
            //  If the device read failed, break the loop
            //
            if(USBD_ERROR(usbdStatus))
            {
                fResult = FALSE;
                break;
            }
            //
            //  If a byte compare fails, mark it and break the loop.
            //
            if(dwSize != RtlCompareMemory(XDCS_Globals.Buffers[0].buffer, XDCS_Globals.Buffers[1].buffer, dwSize))
            {
                fResult = FALSE;
                break;
            }
        } else
        //
        //  If the read file failed, just break the loop
        //
        {
            fResult = FALSE;
            break;
        }
        //
        //  Update the percent complete
        //
        XDCS_Globals.LibraryInfo.dwPercentComplete = ((loop+1)*100)/XDCS_NUM_BLOCKS_TO_VERIFY;
    }
    
    //
    //  If we made it this far, we at least managed to open
    //  the file,  so close it now, before we do anything.
    //
    CloseHandle(hFile);
    
    //
    //  If we made to here and fResult is TRUE, then the device is OK,
    //  otherwise it failed somehwere.
    //
    if(fResult)
    {
        XDCS_ChangeStatus(XDCS_STATUS_ACCEPTED);
    } else
    {
        XDCS_ChangeStatus(XDCS_STATUS_REJECTED);
    }

    return fResult;
}

LOCALFUNC BOOL FASTCALL XDCS_fDownloadFile()
/*++
  Routine Description:

    Reads from the device and writes to disk.
--*/
{
    DWORD dwBufferIndex = 0;
    DWORD dwNextBlock = 0;
    DWORD dwWriteBytesCompleted = 0;
    DWORD dwReason;
    HANDLE hFile;
    PXDCS_DOWNLOAD_BUFFER pBuffer;
    BOOL  fDone = FALSE;
    BOOL  fSuccess = TRUE; //assume success, until failure
    DWORD dwOffset;
    DWORD dwSize;
    
    //  Open the file to write
    hFile = CreateFile( 
              XDCS_Globals.LibraryInfo.szName,
              GENERIC_WRITE,
              0,
              NULL,
              CREATE_ALWAYS,
              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN,
              NULL);
    if(INVALID_HANDLE_VALUE == hFile)
    {
        XDCS_ChangeStatus(XDCS_STATUS_REJECTED);
        return FALSE;
    }

    // Use SetFilePointer and SetEndOfFile to presize the file
    SetFilePointer(hFile, XDCS_Globals.LibraryInfo.dwCodeLength, NULL, FILE_BEGIN);
    SetEndOfFile(hFile);
    
    //
    //  Place every buffer in the start state, and signal
    //  all the events.
    //
    for(dwBufferIndex=0; dwBufferIndex < XDCS_BUFFER_COUNT; dwBufferIndex++)
    {
        XDCS_Globals.Buffers[dwBufferIndex].dwStage = XDCS_BUFFER_START;
        SetEvent(XDCS_Globals.BufferEvents[dwBufferIndex]);
    }

    XDCS_INIT_STAGE_LOG();

    //
    //  Switch the status to verifying
    //  
    XDCS_Globals.LibraryInfo.dwPercentComplete = 0;
    XDCS_ChangeStatus(XDCS_STATUS_DOWNLOADING);

    //
    //  Loop until done
    //
    while(!fDone)
    {
        //Wait for any of the buffer events to fire
        dwReason = WaitForMultipleObjects(
                        XDCS_BUFFER_COUNT,
                        XDCS_Globals.BufferEvents,
                        FALSE,
                        200);

        //Check for a timeout error condition
        if(WAIT_TIMEOUT == dwReason)
        {
            USB_DBG_ERROR_PRINT(("XDCS_fDownloadFile: Timeout, suggests a device hang."));
            fSuccess = FALSE;
            break; //break out of the while loop
        }

        //Otherwise it must be one of the events, set the dwBufferIndex
        //based on which event fired.
        dwBufferIndex = dwReason - WAIT_OBJECT_0;
        ASSERT(dwBufferIndex < XDCS_BUFFER_COUNT);
        pBuffer = &XDCS_Globals.Buffers[dwBufferIndex];
        XDCS_LOG(dwBufferIndex);

        // How we handle the event condition depends on which
        // stage the buffer was at, so switch on the stage
        switch(pBuffer->dwStage)
        {
            default:
                ASSERT(FALSE);
                fSuccess = FALSE; //assume something failed
                fDone = TRUE; //break out of the while loop
                break; //break out of the switch
            case XDCS_BUFFER_FILE:
            // We just finished writing to file
              {
                DWORD dwBytesTransferred;
                fSuccess = GetOverlappedResult(
                               hFile,
                               &pBuffer->Overlapped,
                               &dwBytesTransferred,
                               TRUE);
                if(!fSuccess)
                {
                    USB_DBG_ERROR_PRINT(("XDCS_fDownloadFile: GetOverlappedResult failed with %d.", GetLastError()));
                    fDone = TRUE; //leave the while loop
                    break; //leave the switch statement
                }
                
                //
                //  Update bytes written and progress
                //
                dwWriteBytesCompleted += dwBytesTransferred;
                XDCS_Globals.LibraryInfo.dwPercentComplete = 
                    (100*dwWriteBytesCompleted)/XDCS_Globals.LibraryInfo.dwCodeLength;
                //if done, then break out of the switch statement
                if(dwWriteBytesCompleted == XDCS_Globals.LibraryInfo.dwCodeLength)
                {
                    fDone = TRUE;
                    break;
                }
              }
              //fall through to next case by design
            case XDCS_BUFFER_START:
              //Have not started using the buffer yet(this case), or
              //a file write completed and the buffer is free agains
              //(the case above fell through)

              // if there are remaining blocks to
              // process, build the URB and submit it
              if(dwNextBlock < XDCS_Globals.dwBlockCount)
              {
                
                USB_BUILD_CONTROL_TRANSFER(
                    &pBuffer->Urb.ControlTransfer,
                    NULL,
                    pBuffer->buffer,
                    XDCS_BLOCK_LENGTH,
                    USB_TRANSFER_DIRECTION_IN,
                    XDCS_SignalComplete, 
                    (PVOID)XDCS_Globals.BufferEvents[dwBufferIndex],
                    FALSE,
                    USB_DEVICE_TO_HOST|USB_VENDOR_COMMAND|USB_COMMAND_TO_INTERFACE,
                    XDCS_REQUEST_GET_ROM_FILE_BLOCK,
                    (WORD)dwNextBlock,
                    (WORD)XDCS_Globals.CurrentDevice->bInterfaceNumber,
                    (WORD)XDCS_BLOCK_LENGTH
                    );
                dwNextBlock++;
                pBuffer->dwStage = XDCS_BUFFER_DEVICE;
                XDCS_Globals.CurrentDevice->Device->SubmitRequest(&pBuffer->Urb);
              } else
              {
                pBuffer->dwStage = XDCS_BUFFER_DONE;
              }
              break;
            case XDCS_BUFFER_DEVICE:
              //Just completed reading into a buffer, let's write it out
              {
                //Calculate the offset (pulling it out of the original setup packet.
                dwOffset = pBuffer->Urb.ControlTransfer.SetupPacket.wValue * XDCS_BLOCK_LENGTH;
                //Pull the size out of the number of bytes actually read
                dwSize = pBuffer->Urb.ControlTransfer.TransferBufferLength;
                
                //Check the status of the URB
                if(USBD_ERROR(pBuffer->Urb.Header.Status))
                {
                    USB_DBG_ERROR_PRINT(("XDCS_fDownloadFile: Device read failed, with USBD_STATUS 0x%0.8x.",
                            pBuffer->Urb.Header.Status));
                    fSuccess= FALSE;
                    fDone = TRUE;
                    break;
                }
                //Now we are going to write to the OVERLAPPED structure, which effectively
                //destroys the URB.
                pBuffer->Overlapped.Offset = dwOffset;
                pBuffer->Overlapped.OffsetHigh = 0;
                pBuffer->Overlapped.hEvent = XDCS_Globals.BufferEvents[dwBufferIndex];
                pBuffer->dwStage = XDCS_BUFFER_FILE;
                if(!WriteFile(hFile, pBuffer->buffer, dwSize, NULL, &pBuffer->Overlapped))
                {
                    USB_DBG_WARN_PRINT(("XDCS_fDownloadFile: WriteFile failed with error %d.", GetLastError()));
                    fDone = TRUE;
                }
              }
              break;
        };
    }
    // Close the file handle
    CloseHandle(hFile);


    if(fSuccess)
    {
        XDCS_ChangeStatus(XDCS_STATUS_ACCEPTED);
    } else
    {
        XDCS_ChangeStatus(XDCS_STATUS_REJECTED);
    }
    return fSuccess;
}

VOID XDCS_SignalComplete(PURB, PVOID pEvent)
{
    HANDLE hEvent = (HANDLE)pEvent;
    SetEvent(hEvent);
}

LOCALFUNC VOID FASTCALL XDCS_fCreateBufferEvents()
{
    DWORD dwBufferIndex;
    //
    //  Initialize the buffers into their starting state.
    //
    for(dwBufferIndex=0; dwBufferIndex < XDCS_BUFFER_COUNT; dwBufferIndex++)
    {
        XDCS_Globals.BufferEvents[dwBufferIndex] = CreateEvent(NULL, FALSE, FALSE, NULL);
        ASSERT(XDCS_Globals.BufferEvents);
    }
}

LOCALFUNC DWORD FASTCALL XDCS_fRandomBlock(DWORD BlockCount)
/*++
  Routine Description:
    This is a specialized random number generater.  It uses the system time
    as a seed to get a random block number.  It is used by XDCS_fVerifyFile
    to choose the blocks to compare.
--*/
{
    LARGE_INTEGER systemTime;
    static ULONG seed = 0;

    //Get a seed if we do not already have one
    if(0==seed)
    {
        //Start with the system time.  The units are 100's of nanoseconds
        //since some published date.  However, it is only updated on timer
        //ticks that occur about every millisecond.
        KeQuerySystemTime(&systemTime);

        //Divide by 10000 (converting to milliseconds, means every digit is
        //significant.  Casting to long effectively takes the date mod about
        //a month.
        seed = (ULONG)(systemTime.QuadPart/10000); 
    }
    
    //The linear equation below, was copied out of some other code, copied
    //out of a book, etc.  It purportely takes a pretty good random seed
    //and can turn it into a pretty good random sequence.
    seed = 1664525 * seed + 1013904223;
    
    //Taking the mod ought to be OK since the range of our number is 
    //very small compared to the whole range (about 1 millionth)
    return (seed%BlockCount);
}
