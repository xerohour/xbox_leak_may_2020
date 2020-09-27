/*++

Copyright (c) Microsoft Corporation

Module Name:

    XDCS2.CPP 

Abstract:

    CHANGED THE API COMPLETELY FROM THE ORIGINAL XDCS

    This source file implements the major entry points to the Xbox DVD Code Server
    driver (XDCS).  XDCS is a USB interface based device that contains the DVD playback
    code library.  The code resides in ROM on the device.
   

    The entire implementation is in this file.

    Client API:

      XDCSGetInformation     - Gets information about the code on the device
      XDCSDownloadCode       - Downloads the code synchronously
      XDCSDownloadCodeAsync  - Downloads the code asynchronously

    Entries required by the USB core stack:

      XDCS_Init
      XDCS_AddDevice
      XDCS_RemoveDevice

Environment:

    Xbox USB class driver.

Revision History:
    
    03-30-01 : Mitchell Dernis(mitchd) - created

Implementation and Use Overview:
    
    Totally different API from the original XDCS
   
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
#include <xboxp.h>

// Useful Debug definitions
#define MODULE_POOL_TAG  'SCDX'
#include <debug.h>
DEFINE_USB_DEBUG_FUNCTIONS("XDCS");

// Xbox USB Class driver header
#include <usb.h>

// XDCS specific header



//*****************************************************************************
// XDCS constants and structures from the XDCS specification
//*****************************************************************************
#define XDCS_REQUEST_GET_ROM_FILE_INFO  1
#define XDCS_REQUEST_GET_ROM_FILE_BLOCK 2
#define XDCS_BLOCK_LENGTH               1024       //1k blocks
#define XDCS_BLOCK_SHIFT                10         //bit shift to divide by 1k
#define XDCS_CONTROL_TD_QUOTA     (2*(3+1024/64))  //2 1k transfers with 64 byte fifo
#define XDCS_MAX_PORTS            4                //The maximum number of ports supported
#define XDCS_REQUEST_TIMEOUT        (-10000*200)   //200 ms timeout
#define XDCS_MAX_TRIES                  4

//*****************************************************************************
//  Declare XDCS types and class.
//*****************************************************************************
DECLARE_XPP_TYPE(XDEVICE_TYPE_DVD_CODE_SERVER)
USB_DEVICE_TYPE_TABLE_BEGIN(XDCS_)
USB_DEVICE_TYPE_TABLE_ENTRY(XDEVICE_TYPE_DVD_CODE_SERVER)
USB_DEVICE_TYPE_TABLE_END()
USB_CLASS_DRIVER_DECLARATION(XDCS_, XBOX_DEVICE_CLASS_XDCS, 0, 0)
#pragma data_seg(".XPP$ClassXdcs")
USB_CLASS_DECLARATION_POINTER(XDCS_)
#pragma data_seg(".XPP$Data")

//*****************************************************************************
// Declare Local Structures
//*****************************************************************************

typedef struct _XDCS_DEVICE
{
    UCHAR                       bFlags;
    UCHAR                       bInterfaceNumber;
    USHORT                      usInstance;         //Update whenever a device is removed
    IUsbDevice                  *Device;
    XDCS_DVD_CODE_INFORMATION   CodeInformation;
} XDCS_DEVICE, *PXDCS_DEVICE;

#define XDCS_DEVICE_FLAG_CONNECTED    0x01
#define XDCS_DEVICE_FLAG_INUSE        0x02
#define XDCS_DEVICE_FLAG_ENUMERATING  0x04

#define MAKE_DWORD_INSTANCE(usInstance, dwPort) (((usInstance) << 16)|(dwPort))
#define PORT_FROM_DWORD_INSTANCE(dwInstance) (0x0000FFFF&(dwInstance))
#define USHORT_INSTANCE_FROM_DWORD_INSTANCE(dwInstance) ((USHORT)(((dwInstance)&0xFFFF0000)>>16))

typedef struct _XDCS_GLOBALS
{
    // Single set of resources for data transfer
    ULONG                        ulGlobalFlags;
    ULONG                        Urb1LastLength;  //*** DO NOT REORDER
    URB                          Urb1;            //*** SEE 
    ULONG                        Urb2LastLength;  //*** #define XDCS_URB_LAST_LEN
    URB                          Urb2;            //*** BELOW
    URB                          UrbEnum;
    ULONG                        BytesSubmitted;
    ULONG                        RetryCount;
    USBD_STATUS                  UsbdStatus;                        
    KTIMER                       WatchDogTimer;
    KDPC                         WatchDogTimerDpc;
    PXDCS_ASYNC_DOWNLOAD_REQUEST CurrentDownloadRequest;
    // Track attached devices
    XDCS_DEVICE   Devices[XDCS_MAX_PORTS];
} XDCS_GLOBALS, *PXDCS_GLOBALS;

#define XDCS_GLOBAL_FLAG_TRANSFER_IN_PROGRESS       0x00000001
#define XDCS_GLOBAL_FLAG_URB1_OUTSTANDING           0x00000002
#define XDCS_GLOBAL_FLAG_URB2_OUTSTANDING           0x00000004
#define XDCS_GLOBAL_FLAG_ENUM_URB_OUTSTANDING       0x00000008
#define XDCS_GLOBAL_FLAG_DOWNLOAD_URBS_OUTSTANDING  (XDCS_GLOBAL_FLAG_URB1_OUTSTANDING|\
                                                     XDCS_GLOBAL_FLAG_URB2_OUTSTANDING)
#define XDCS_GLOBAL_FLAG_URBS_OUTSTANDING           (XDCS_GLOBAL_FLAG_URB1_OUTSTANDING|\
                                                     XDCS_GLOBAL_FLAG_ENUM_URB_OUTSTANDING|\
                                                     XDCS_GLOBAL_FLAG_URB2_OUTSTANDING)
#define XDCS_GLOBAL_FLAG_WATCHDOG_RUNNING           0x00000010

//
//  Takes advantage that Urb1LastLength is stored
//  right before Urb1 and Urb2LastLength is stored
//  right before Urb2.
//
#define XDCS_URB_LAST_LEN(Urb) (*((PULONG)(Urb)-1))

//****************************************
// Internally useful macros
//****************************************
#define SET_FLAG(variable, flag)  ((variable) |= (flag))
#define CLEAR_FLAG(variable, flag)  ((variable) &= ~(flag))
#define TEST_FLAG(variable, flag)  ((variable)&(flag))
#define TEST_ANY_FLAG(variable, flag)  ((variable)&(flag))
#define TEST_ALL_FLAGS(variable, flag)  ((flag)==((variable)&(flag)))

//*****************************************************************************
// Really cool inline template function for walking buffers.
//*****************************************************************************
template <class T>
inline T *AdvancePtr(T *pT, ULONG cb)
{
    return (T *)(((ULONG)pT) + cb);
}


//*****************************************************************************
// Declare Globals
//*****************************************************************************
USB_RESOURCE_REQUIREMENTS XDCS_gResourceRequirements = 
    {USB_CONNECTOR_TYPE_DIRECT, 1, 2, 1, 0, 0, XDCS_CONTROL_TD_QUOTA, 0, 0, 0};
XDCS_GLOBALS XDCS_Globals = {0};

//*****************************************************************************
// Forward Declaration of Local Functions
//*****************************************************************************
#if DBG
#define LOCALFUNC
#else
//Compiler may be able to optimize static functions better.
#define LOCALFUNC static
#endif 
LOCALFUNC VOID XDCS_AddComplete(PURB Urb, PXDCS_DEVICE XdcsDevice);
LOCALFUNC VOID XDCS_DownloadStateMachine(PURB Urb, PXDCS_DEVICE XdcsDevice);
LOCALFUNC BOOL XDCS_BuildAndSubmitDownloadRequest(PURB Urb,PXDCS_DEVICE XdcsDevice);
LOCALFUNC VOID XDCS_CompleteDownload(PURB Urb, PXDCS_DEVICE XdcsDevice);
LOCALFUNC VOID XDCS_WatchDogTimeout (PKDPC Dpc,PVOID,PVOID,PVOID);
LOCALFUNC VOID XDCS_StartExtendWatchdog();
LOCALFUNC VOID XDCS_StopWatchdog();    

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
        Initialize our globals, register our driver resource requirements.
--*/
{
    USB_DBG_ENTRY_PRINT(("Entering XDCS_Init"));

    //Init the timer and timer dpc
    KeInitializeDpc(&XDCS_Globals.WatchDogTimerDpc, XDCS_WatchDogTimeout, NULL);
    KeInitializeTimer(&XDCS_Globals.WatchDogTimer);

    // register resources
    pUsbInit->RegisterResources(&XDCS_gResourceRequirements);

    USB_DBG_EXIT_PRINT(("Exiting XDCS_Init"));
    return;
}
#pragma code_seg(".XPPCODE")

EXTERNUSB VOID XDCS_AddDevice(IN IUsbDevice *Device)
/*++

    Routine Description:
    
      1) Records that the device is present and relevant info that
         it may need later.
      2) Gets the ROM info

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
    ASSERT(!TEST_FLAG(xdcsDevice->bFlags,XDCS_DEVICE_FLAG_CONNECTED));
    xdcsDevice->bInterfaceNumber = Device->GetInterfaceNumber();
    Device->SetExtension((PVOID)xdcsDevice);
    xdcsDevice->Device = Device;
    SET_FLAG(xdcsDevice->bFlags, XDCS_DEVICE_FLAG_ENUMERATING);

    //  Build and submit the XDCS_REQUEST_GET_ROM_FILE_INFO request
    USB_BUILD_CONTROL_TRANSFER(
        &XDCS_Globals.UrbEnum.ControlTransfer,
        NULL,
        (PVOID)&xdcsDevice->CodeInformation,         //right into the user's buffer
        sizeof(XDCS_DVD_CODE_INFORMATION),
        USB_TRANSFER_DIRECTION_IN,
        (PURB_COMPLETE_PROC)XDCS_AddComplete,
        (PVOID)xdcsDevice,
        FALSE,
        USB_DEVICE_TO_HOST|USB_VENDOR_COMMAND|USB_COMMAND_TO_INTERFACE,
        XDCS_REQUEST_GET_ROM_FILE_INFO,
        0,
        xdcsDevice->bInterfaceNumber,
        sizeof(XDCS_DVD_CODE_INFORMATION)
        );

    SET_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_ENUM_URB_OUTSTANDING);
    XDCS_StartExtendWatchdog();

    //Don't worry about errors the completion routine will handle.
    xdcsDevice->Device->SubmitRequest(&XDCS_Globals.UrbEnum);
    
    USB_DBG_EXIT_PRINT(("ExitingXDCS_AddDevice"));
    return;
}

LOCALFUNC VOID XDCS_AddComplete(PURB Urb, PXDCS_DEVICE XdcsDevice)
{

    CLEAR_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_ENUM_URB_OUTSTANDING);
    CLEAR_FLAG(XdcsDevice->bFlags, XDCS_DEVICE_FLAG_ENUMERATING);
    XDCS_StopWatchdog();

    if(USBD_SUCCESS(Urb->Header.Status))
    {
        //Mark the device as connected
        SET_FLAG(XdcsDevice->bFlags,XDCS_DEVICE_FLAG_CONNECTED);
        //Complete the Add.
        XdcsDevice->Device->SetClassSpecificType(0);
        XdcsDevice->Device->AddComplete(USBD_STATUS_SUCCESS);
    } else
    {
        XdcsDevice->Device->AddComplete(Urb->Header.Status);
    }
}


EXTERNUSB VOID
XDCS_RemoveDevice (
    IN IUsbDevice *Device
    )
/*++
    Routine Description:
       
      1) Records the removal.
      2) If the device is not in use, it and completes the remove.

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
    CLEAR_FLAG(xdcsDevice->bFlags,XDCS_DEVICE_FLAG_CONNECTED);
    
    //Increment the instance counter - so when it is
    //plugged back in it is different.
    xdcsDevice->usInstance++;

    //If the device is not in use, complete the removal.
    //Otherwise, who ever is using it will complete the removal
    //after they clean up.
    if(!TEST_FLAG(xdcsDevice->bFlags,XDCS_DEVICE_FLAG_INUSE))
    {
       xdcsDevice->Device = NULL;
       Device->SetExtension(NULL);
       Device->RemoveComplete();
    }

    USB_DBG_EXIT_PRINT(("exiting XDCS_RemoveDevice"));
    return;
}

DWORD
WINAPI
XDCSGetInformation(
    IN  DWORD dwPort,
    OUT PDWORD pdwDeviceInstance,
    OUT PXDCS_DVD_CODE_INFORMATION pDvdCodeInformation
    )
{

    PXDCS_DEVICE xdcsDevice;
    KIRQL oldIrql;
    DWORD dwError = ERROR_SUCCESS;

    // Validate parameters
    RIP_ON_NOT_TRUE("XDCSGetInformation", (dwPort < XDCS_MAX_PORTS));
    RIP_ON_NOT_TRUE("XDCSGetInformation", (NULL != pdwDeviceInstance));
    RIP_ON_NOT_TRUE("XDCSGetInformation", (NULL != pDvdCodeInformation));
    
    //Get the requested device.
    xdcsDevice = &XDCS_Globals.Devices[dwPort];
    
    //Synchronize access
    oldIrql = KeRaiseIrqlToDpcLevel();

    // Make sure the device is connected
    if(!TEST_FLAG(xdcsDevice->bFlags, XDCS_DEVICE_FLAG_CONNECTED))
    {
        dwError = ERROR_DEVICE_NOT_CONNECTED;
    }
    else
    {
        RtlCopyMemory(pDvdCodeInformation, &xdcsDevice->CodeInformation, sizeof(XDCS_DVD_CODE_INFORMATION));
        *pdwDeviceInstance = MAKE_DWORD_INSTANCE(xdcsDevice->usInstance, dwPort);
    }
    
    //Done synchronizing
    KeLowerIrql(oldIrql);

    return dwError;

}

DWORD
WINAPI
XDCSDownloadCode(
    DWORD   dwDeviceInstance,
    PVOID   pvBuffer,
    ULONG   ulOffset,
    ULONG   ulLength,
    PULONG  pulBytesRead
    )
{
    XDCS_ASYNC_DOWNLOAD_REQUEST asyncRequest;

    //
    //  Need an event to signal when done
    //
    asyncRequest.hCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if(NULL == asyncRequest.hCompleteEvent)
    {
        return GetLastError();
    }

    //
    //  Fill out rest of async request
    //
    asyncRequest.dwDeviceInstance = dwDeviceInstance;
    asyncRequest.pvBuffer = pvBuffer;
    asyncRequest.ulOffset = ulOffset;
    asyncRequest.ulLength = ulLength;
    
    //
    //  Submit async request
    //
    XDCSDownloadCodeAsync(&asyncRequest);
    
    //
    //  Wait for the request to complete
    //
    WaitForSingleObject(asyncRequest.hCompleteEvent, INFINITE);

    //
    //  Copy byte read
    //
    *pulBytesRead = asyncRequest.ulBytesRead;

    //
    //  Return the status
    //
    return asyncRequest.ulStatus;
}

__inline BOOL SleepIfBusy(DWORD dwError)
{
    if(ERROR_BUSY == dwError)
    {
        Sleep(10);
        return TRUE;
    }
    return FALSE;
}

DWORD
WINAPI
XDCSDownloadCodeAsync(
    IN OUT PXDCS_ASYNC_DOWNLOAD_REQUEST pXDCSDownloadRequest
    )
{
    DWORD dwPort = PORT_FROM_DWORD_INSTANCE(pXDCSDownloadRequest->dwDeviceInstance);
    PXDCS_DEVICE xdcsDevice = &XDCS_Globals.Devices[dwPort];
    DWORD dwError;
    KIRQL oldIrql;
    
    
    //
    //  Loop until we managed to get capture the 
    //  transfer resources, poor man's mutex, but
    //  there should not be any contention for this.
    do
    {
        dwError = ERROR_SUCCESS;
        
        //Synchronize access
        oldIrql = KeRaiseIrqlToDpcLevel();
    
        //
        //  Check instance to make sure that the caller is getting
        //  what it thinks it is getting.  As long as the client
        //  got the instance from us, this is sufficient to check that
        //  the device is actually connected, unless the device has been
        //  added and removed 65536 times since they called us last.
        //
        if( xdcsDevice->usInstance != USHORT_INSTANCE_FROM_DWORD_INSTANCE(pXDCSDownloadRequest->dwDeviceInstance))
        {
            dwError = ERROR_DEVICE_NOT_CONNECTED;
        } else if(TEST_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_TRANSFER_IN_PROGRESS))
        {
            dwError = ERROR_BUSY;
#if DBG        
        } else if(pXDCSDownloadRequest->ulOffset%XDCS_BLOCK_LENGTH)
        {
            KeLowerIrql(oldIrql);
            RIP("XDCSDownloadCode\\XDCSDownloadCodeAsync, Offset must be 1 kbyte aligned.");
            oldIrql = KeRaiseIrqlToDpcLevel();
        } else if(
            (pXDCSDownloadRequest->ulOffset+pXDCSDownloadRequest->ulLength) > 
            xdcsDevice->CodeInformation.dwCodeLength
          )
        {
            KeLowerIrql(oldIrql);
            RIP("XDCSDownloadCode\\XDCSDownloadCodeAsync, Transfer request is beyond data length");
            oldIrql = KeRaiseIrqlToDpcLevel();
#endif
        } else
        {
            //
            //  Claim the transfer resources
            //
            SET_FLAG(xdcsDevice->bFlags, XDCS_DEVICE_FLAG_INUSE);
            SET_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_TRANSFER_IN_PROGRESS);
            XDCS_Globals.CurrentDownloadRequest = pXDCSDownloadRequest;
            
            //
            //  Open the default endpoint
            //
            USB_BUILD_OPEN_DEFAULT_ENDPOINT(&XDCS_Globals.Urb1.OpenEndpoint);
            xdcsDevice->Device->SubmitRequest(&XDCS_Globals.Urb1);
        
            //
            //  Start Download
            //
            XDCS_DownloadStateMachine(NULL, xdcsDevice);
            dwError = ERROR_IO_PENDING;
        }
        KeLowerIrql(oldIrql);

    }while (SleepIfBusy(dwError));
    
    return dwError;
}

LOCALFUNC
VOID
XDCS_DownloadStateMachine(
    PURB Urb,
    PXDCS_DEVICE XdcsDevice
    )
{
    //
    //  Check if this is the first call.
    //
    if(NULL==Urb)
    {
        XDCS_Globals.UsbdStatus = USBD_STATUS_SUCCESS;
        XDCS_Globals.BytesSubmitted = 0;
        XDCS_Globals.CurrentDownloadRequest->ulStatus = ERROR_IO_PENDING;
        XDCS_Globals.CurrentDownloadRequest->ulBytesRead = 0;
        XDCS_Globals.RetryCount = 0;
        SET_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_DOWNLOAD_URBS_OUTSTANDING);
        XDCS_BuildAndSubmitDownloadRequest(&XDCS_Globals.Urb1, XdcsDevice);
        XDCS_BuildAndSubmitDownloadRequest(&XDCS_Globals.Urb2, XdcsDevice);
    } else
    //
    //  This is an URB completing
    //
    {
        BOOL fDone = FALSE;
        if(USBD_SUCCESS(Urb->Header.Status))
        {
            XDCS_Globals.RetryCount = 0;
            XDCS_Globals.CurrentDownloadRequest->ulBytesRead += Urb->ControlTransfer.TransferBufferLength;
            //
            // Resubmit the URB if there is more to transfer
            //
            fDone = XDCS_BuildAndSubmitDownloadRequest(Urb, XdcsDevice);
        }else if( (XDCS_Globals.RetryCount++ < XDCS_MAX_TRIES) && TEST_FLAG(XdcsDevice->bFlags, XDCS_DEVICE_FLAG_CONNECTED) )
        //
        //  Check the retry count and make sure the device is still connected.
        //  If we have retries left, just resubmit, but remember to reset
        //  the length, this is an IN\OUT parameter.
        //
        {
          XDCS_StartExtendWatchdog(); //Pet watchdog
          Urb->CommonTransfer.TransferBufferLength = XDCS_URB_LAST_LEN(Urb);
          XdcsDevice->Device->SubmitRequest(Urb);
        } else
        //
        //  Failed and the device is not connected or the retry count is exceeded.
        //
        {
            //
            //  Record the error if it is the first
            //
            if(USBD_SUCCESS(XDCS_Globals.UsbdStatus))
            {
              XDCS_Globals.UsbdStatus = Urb->Header.Status;
            }
            //
            //  If the twin URB is outstanding, cancel it.  If it is not oustanding
            //  mark that we are done.
            //
            if( &XDCS_Globals.Urb1 == Urb)
            {
                CLEAR_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_URB1_OUTSTANDING);
                if(TEST_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_URB2_OUTSTANDING))
                {
                    XdcsDevice->Device->CancelRequest(&XDCS_Globals.Urb2);
                } else
                {
                    fDone = TRUE;
                }
            } else
            {
                CLEAR_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_URB2_OUTSTANDING);
                if(TEST_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_URB1_OUTSTANDING))
                {
                    XdcsDevice->Device->CancelRequest(&XDCS_Globals.Urb1);
                } else
                {
                    fDone = TRUE;
                }
            }
        }
        //
        //  If the transfer is done, close down the endpoint.
        //
        if(fDone)
        {
            XDCS_StopWatchdog();
            USB_BUILD_CLOSE_DEFAULT_ENDPOINT(
                &XDCS_Globals.Urb1.CloseEndpoint,
                (PURB_COMPLETE_PROC)XDCS_CompleteDownload,
                (PVOID)XdcsDevice
                );
            XdcsDevice->Device->SubmitRequest(&XDCS_Globals.Urb1);
        }
    }
}

LOCALFUNC
BOOL
XDCS_BuildAndSubmitDownloadRequest(
    PURB Urb,
    PXDCS_DEVICE XdcsDevice
    )
{
    //
    //  If there are more byte to submit, do it.
    //
    if(XDCS_Globals.BytesSubmitted < XDCS_Globals.CurrentDownloadRequest->ulLength)
    {
        //Calculate the transfer length for this command
        ULONG ulTransferSize = XDCS_Globals.CurrentDownloadRequest->ulLength - XDCS_Globals.BytesSubmitted;
        if(ulTransferSize > XDCS_BLOCK_LENGTH) ulTransferSize = XDCS_BLOCK_LENGTH;
        
        //
        //  Store the length in case we need to retry
        //
        XDCS_URB_LAST_LEN(Urb) = ulTransferSize;

        USBD_STATUS usbdStatus;
        USB_BUILD_CONTROL_TRANSFER(
                &Urb->ControlTransfer,
                NULL,
                AdvancePtr(XDCS_Globals.CurrentDownloadRequest->pvBuffer, XDCS_Globals.BytesSubmitted),
                ulTransferSize,
                USB_TRANSFER_DIRECTION_IN,
                (PURB_COMPLETE_PROC)XDCS_DownloadStateMachine,
                (PVOID)XdcsDevice,
                FALSE,
                USB_DEVICE_TO_HOST|USB_VENDOR_COMMAND|USB_COMMAND_TO_INTERFACE,
                XDCS_REQUEST_GET_ROM_FILE_BLOCK,
                (WORD)((XDCS_Globals.CurrentDownloadRequest->ulOffset + XDCS_Globals.BytesSubmitted)>>XDCS_BLOCK_SHIFT),
                (WORD)XdcsDevice->bInterfaceNumber,
                (WORD)ulTransferSize
                );
        XDCS_Globals.BytesSubmitted += ulTransferSize;
        XDCS_StartExtendWatchdog();
        usbdStatus = XdcsDevice->Device->SubmitRequest(Urb);
        return FALSE;
    }

    //
    //  If there are no more requests to submit then clear the appropriate
    //  XDCS_GLOBAL_FLAG_URB?_OUTSTANDING flag.
    //
    if( &XDCS_Globals.Urb1 == Urb)
    {
        CLEAR_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_URB1_OUTSTANDING);
    } else
    {
        CLEAR_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_URB2_OUTSTANDING);
    }

    return 
      TEST_ANY_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_DOWNLOAD_URBS_OUTSTANDING) ?
      FALSE : TRUE;

}

LOCALFUNC
VOID 
XDCS_CompleteDownload(
    PURB Urb,
    PXDCS_DEVICE XdcsDevice
    )
{
    //
    //  Set the status and signal the hEvent.
    //
    XDCS_Globals.CurrentDownloadRequest->ulStatus = IUsbDevice::Win32FromUsbdStatus(XDCS_Globals.UsbdStatus);
    if(XDCS_Globals.CurrentDownloadRequest->hCompleteEvent)
        SetEvent(XDCS_Globals.CurrentDownloadRequest->hCompleteEvent);

    //
    //  Release the shared resources
    //
    XDCS_Globals.CurrentDownloadRequest = NULL;
    CLEAR_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_TRANSFER_IN_PROGRESS);

    //
    //  Clear the in use flag
    //
    CLEAR_FLAG(XdcsDevice->bFlags, XDCS_DEVICE_FLAG_INUSE);
    //
    //  Check if the device was removed while we were using it.  If it was
    //  need to complete the remove.
    //
    if(!TEST_FLAG(XdcsDevice->bFlags, XDCS_DEVICE_FLAG_CONNECTED))
    {
       XdcsDevice->Device->SetExtension(NULL);
       XdcsDevice->Device->RemoveComplete();
       XdcsDevice->Device = NULL;
    }
}


LOCALFUNC
VOID
XDCS_WatchDogTimeout (
    IN PKDPC Dpc,
    IN PVOID Context,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
/*++
    Routine Description:
        
        This is the DPC routine for the XDCS watchdog timer.  It protects
        against hung requests, NAK forever.
        
        Whenever it fires it cancels all outstanding URBs.

--*/
{

    USB_DBG_WARN_PRINT(("XDCS timed out."));

    CLEAR_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_WATCHDOG_RUNNING);
    
    PXDCS_DEVICE xdcsDevice = NULL;
    if(XDCS_Globals.CurrentDownloadRequest)
    {
        xdcsDevice = &XDCS_Globals.Devices[
                        PORT_FROM_DWORD_INSTANCE(XDCS_Globals.CurrentDownloadRequest->dwDeviceInstance)
                        ];

        if(TEST_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_URB1_OUTSTANDING))
        {
            xdcsDevice->Device->CancelRequest(&XDCS_Globals.Urb1);
        }
        if(TEST_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_URB2_OUTSTANDING))
        {
            xdcsDevice->Device->CancelRequest(&XDCS_Globals.Urb2);
        }
    }

    if(TEST_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_ENUM_URB_OUTSTANDING))
    {
        for(DWORD dwPort=0; dwPort<XDCS_MAX_PORTS; dwPort++)
        {
            if( TEST_FLAG(XDCS_Globals.Devices[dwPort].bFlags, XDCS_DEVICE_FLAG_ENUMERATING) )
            {
                XDCS_Globals.Devices[dwPort].Device->CancelRequest(&XDCS_Globals.UrbEnum);
                break;
            }
        }
    }
        
}

LOCALFUNC 
VOID 
XDCS_StartExtendWatchdog()
{
    LARGE_INTEGER timeout;
    timeout.QuadPart = XDCS_REQUEST_TIMEOUT;
    SET_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_WATCHDOG_RUNNING);
    KeSetTimer(&XDCS_Globals.WatchDogTimer, timeout, &XDCS_Globals.WatchDogTimerDpc);
}

LOCALFUNC VOID XDCS_StopWatchdog()
{
    if(!TEST_ANY_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_URBS_OUTSTANDING))
    {
        KeCancelTimer(&XDCS_Globals.WatchDogTimer);
        CLEAR_FLAG(XDCS_Globals.ulGlobalFlags, XDCS_GLOBAL_FLAG_WATCHDOG_RUNNING);
    }
}
