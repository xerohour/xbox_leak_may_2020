/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    xinput.h

Abstract:

    Input API for XBOX
    
Environment:

    XBOX API Layer

Notes:

Revision History:

    03-01-00 created by Mitchell Dernis (mitchd)

--*/

#ifndef __XINPUT_API__
#define __XINPUT_API__
#include <xusbenum.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
//  Device Types and Subtypes.
//
//      TYPES designate major types of input devices.  Titles designed
//      to work with one device type will not in general be able to utilize
//      another unless the title explicitly supports the other device type.
//
//      SUBTYPES designate a variation on a device type.  Titles designed
//      for one subtype will work with any subtype though perhaps not as well.
//      In general, titles should follow guidelines for their genre and they
//      will work well with at least the standard gamepad, and a genre targeted
//      device.
//      
//----------------------------------------------------------------------------
//  Subtypes for Game Controllers
//
#define XINPUT_DEVSUBTYPE_GC_GAMEPAD    0
#define XINPUT_DEVSUBTYPE_GC_JOYSTICK   1
#define XINPUT_DEVSUBTYPE_GC_WHEEL      2


#include <PSHPACK1.H>

//-------------------------------------------------------------------------------------------------------
//   Reports:  Each report represents the packet that is transmitted to or from a device.
//             There are input reports, and output reports.
//
//             All XID reports begin with a one byte ReportId followed by a one byte size.
//             Additionally, reports may be input or output.  This is a second byte which is
//             not in the report itself.
//
//             XInput defines three stuctures which are the union of multiple reports:
//             XINPUT_REPORT, XOUTPUT_REPORT, and XINPUT_CAPABILITIES_REPORT.
//
//             XINPUT_REPORT is used with XInputGetDeviceState to retrieve the state of a device, it is
//             the union of the input reports for all the device types.
//
//             XOUTPUT_REPORT is used with XInputSendDeviceReport.  It has a header and then then
//             a union of all the output reports for all the device types.
//
//             XINPUT_CAPABILITIES_REPORT is used with XInputQueryCapabilities.  It is a subtype followed
//             followed by a union of all the input and output reports.
//             
//-------------------------------------------------------------------------------------------------------


//**        
//**    Definition of all the device specific reports
//**

//  Standard Report ID - for use with XInputSendDeviceReport and XInputQueryCapabilities.
//
#define XINPUT_GAME_REPORT_ID      0x0100
#define XOUTPUT_RUMBLE_REPORT_ID   0x0200

#define XINPUT_CHAT_REPORT_ID      0x0100
#define XOUTPUT_CHAT_REPORT_ID     0x0200

//
//  Standard Input for Game Controllers
//
typedef struct _XINPUT_GAME_REPORT
{
    WORD    fDpadUp:1;
    WORD    fDpadDown:1;
    WORD    fDpadLeft:1;
    WORD    fDpadRight:1;
    WORD    fStartButton:1;
    WORD    fBackBackButton:1;
    WORD    fLeftThumbButton:1;
    WORD    fRightThumbButton:1;
    WORD    bmButtonsUnused:8;
    BYTE    rgbAnalogButtons[8];
    WORD    wThumbLX;
    WORD    wThumbLY;
    WORD    wThumbRX;
    WORD    wThumbRY;
} XINPUT_GAME_REPORT, *PXINPUT_GAME_REPORT;
        
//
//  Standard Rumble Motor Output for Game Controllers
//
typedef struct _XOUTPUT_RUMBLE_REPORT
{
   WORD   wLeftMotorSpeed;
   WORD   wRightMotorSpeed;
} XOUTPUT_RUMBLE_REPORT, *PXOUTPUT_RUMBLE_REPORT;

//
//  Standard Chat Controller Input Report (TBD)
//
typedef struct _XINPUT_CHAT_REPORT
{
   WORD   TBD;
} XINPUT_CHAT_REPORT, *PXINPUT_CHAT_REPORT;


//
//  Standard Chat Controller Output Report (TBD)
//
typedef struct _XOUTPUT_CHAT_REPORT
{
   WORD   TBD;
} XOUTPUT_CHAT_REPORT, *PXOUTPUT_CHAT_REPORT;

//**        
//**  Definition of the unions of reports used with
//**  XInputGetDeviceState, XInputSendDeviceReport,
//**  and XInputQueryCapabilities
//**

//
//  Structure used with XInputGetDeviceState
//
typedef struct _XINPUT_REPORT
{
    DWORD dwPacketNumber;
    union
    {
        XINPUT_GAME_REPORT GameReport;
        XINPUT_CHAT_REPORT ChatReport;
    };
} XINPUT_REPORT, *PXINPUT_REPORT;

//
//  Structures used with XInputSendDeviceReport
//
#define XOUTPUT_SIZE_OF_INTERNAL_HEADER 90
typedef struct _XOUTPUT_HEADER
{
    DWORD           dwStatus;
    HANDLE OPTIONAL hEvent;
    BYTE            Reserved[XOUTPUT_SIZE_OF_INTERNAL_HEADER];
} XOUTPUT_HEADER, *PXOUTPUT_HEADER;

typedef struct _XOUTPUT_REPORT
{
    XOUTPUT_HEADER  Header;
    union
    {
        XOUTPUT_RUMBLE_REPORT RumbleReport;
        XOUTPUT_CHAT_REPORT   ChatReport;
    };
} XOUTPUT_REPORT, *PXOUTPUT_REPORT;

//
//  Structures used with XInputQueryCapabilities
//
typedef struct _XINPUT_CAPABILTIES_REPORT
{
    BYTE    SubType;
    WORD    Reserved;
    union  
    {
        XINPUT_GAME_REPORT    GameInputReport;
        XINPUT_CHAT_REPORT    ChatInputReport;
        XOUTPUT_RUMBLE_REPORT RumbleOutputReport;
        XOUTPUT_CHAT_REPORT   ChatOutputReport;
    };
} XINPUT_CAPABILTIES_REPORT, *PXINPUT_CAPABILTIES_REPORT;

//------------------------------------------------------------------------
//  Polling Parameters
//------------------------------------------------------------------------
typedef struct _XINPUT_POLLING_PARAMETERS
{
    BYTE       fAutoPoll:1;
    BYTE       fInterruptOut:1;
    BYTE       ReservedMBZ1:6;
    BYTE       bInputInterval;  
    BYTE       bOutputInterval;
    BYTE       ReservedMBZ2;
} XINPUT_POLLING_PARAMETERS, *PXINPUT_POLLING_PARAMETERS;

/*++

    INTERNAL REVIEW NOTE: AFTER DOING A CAREFUL ANALYSIS, THIS MAY PROVE TO BE WAY TOO
    MUCH CONTROL IF WE HAVE ONLY FOUR PORTS.  IT IS THE YEAR N CASE WITH AN EXTERNAL
    HUB WHERE GAME DEVELOPERS MAY REALLY NEED THIS.  NOTE THAT IS OPTIONAL, SO I AM
    RELUCTANT TO PULL IT.  I THINK IT PROVES WE ARE WILLING TO GO THE EXTRA MILE.  IT
    IS NOT COMPLICATION FOR THE SAKE OF COMPLEXITY, IT IS REAL FUNCTIONALITY.

   Callers of XInputOpen??? family of functions can optionally pass a pointer
   to the structure to exercise fine control over the polling parameters.  In
   cases with many audio chat devices, where bandwidth is tight, this may be
   useful.

    Members:
        fAutoPoll        -  If TRUE the system automatically keeps an input request
                            pending to the device.  The game can call XInputGetDeviceState
                            at any time and get the last known state of the device.
                    
                            If FALSE, the game must initiate every input request by
                            calling XInputPollDevice.   To be assurd that XInputGetDeviceState
                            has the latest data, there should be a time delay of
                            bInputInterval plus 2 ms between calling XInputPollDevice and
                            XInputGetDeviceState.

                            DEFAULT: TRUE, for all device types.

        fInterruptOut   -   If TRUE, opens the interrupt-OUT endpoint of the device.
                            IF FALSE, output is sent over the control endpoint.

                            Opening the out-endpoint takes memory and reserved USB bandwidth.
                            Output is guaranteed delivery (absent very rare bus errors) to the device
                            within bOutputInterval plus 1 millisecond.  The output request will complete
                            within in bOutputInterval plus 2 milliseconds.  The extra millisecond is
                            process the confirmation that there were no bus errors.

                            DEFAULT: TRUE for Game Controllers
                                     FALSE for Chat Controllers

        bInputInterval  -   Specifies in ms the input polling interval.  Valid values are are
                            1,2,4,8,16,32. Any other value will be rounded down to the nearest value.
                            (Zero is rounded to one though).

                            DEFAULT: 8 for Game Controllers
                                     16 for Chat Controllers

        bOutputInterval  -  Specifies in ms the output polling interval.  Valid values are are
                            1,2,4,8,16,32. Any other value will be rounded down to the nearest value.
                            (Zero is rounded to one though).

                            DEFAULT: 2 for Game Controllers
                                     Ignored for Chat Controllers, because fInterruptOut is FALSE.

        ReservedMBZ1     -  These parameters are unused at this time.  Set them to zero for minimum
        ReservedMBZ2        porting effort when compiling against a newer version of the OS.

      Comments:
        This level of fine control is overkill for many games that use just 1 to 4 game pads and memory units.
        It is provided for games that want to use multiple game pads with audio chat devices.  In these cases,
        it may be helpful to fine tune the USB bandwidth usage.  A bandwidth calculator should be provided with
        the XDK to aid in choosing these values.

        When adjusting these parameters, you are making tradeoffs between responsive of the device, latency on
        output, and the number of devices that can share the device.

        Turning fAutoPoll off may be useful to reduce the amount of data transfers while maintaining better
        responsiveness by synchronizing data polling with the video frame.  This way a game can initiate a
        poll once a video frame, but by carefully timing the call to XInputPollDevice, can be assurd of fresh data.
        Say that bInputInterval is set to 4 ms.  XInputPollDevice should be called 6 ms before XInputGetDeviceState.
        XInputGetDeviceState should return a state no more than 12 ms old, allowing that the device takes around 6 ms
        to debounce buttons presses, do A/D conversion for axes, and ready the data to be sent.  Devices may vary.

        Compare this with autopoll on, and an interval of 8 ms.  It potentially generates twice as must data, but
        reserves half the bandwidth.  In the worst case, the data is about 16 ms old, compared with 12 ms from the
        above.

        Setting fInterruptOut FALSE saves bandwidth, and memory at the expense of not guaranteeing latency.
        On average, performance will be excellent with fInterruptOut, perhaps even better than with fInterrupt
        set to TRUE.  In the worst case, say while writing so two or three memory units simulateneously* (using
        asynchronous file I/O), with audio chat going, and quickly updating the rumble motors on a couple of
        game pads.  In this case, the repsonse of the motors may be a bit unpredicatable (sometimes sluggish,
        good) if fInterruptOut is FALSE.  Unfortunately, this case may require you to set it to FALSE, to get
        all the devices on the bus at once.

  
        *In this case, the motors will get three times as much bandwidth as the memory units.
--*/
#include <POPPACK.H>

//------------------------------------------------------------------------
//  Open\Close Handle
//------------------------------------------------------------------------
//  Different open methods for each input device type.
//    XInputOpenGameController
//    XInputOpenChatController
DWORD XInputOpenDevice(DWORD,DWORD,DWORD,PXINPUT_POLLING_PARAMETERS,PDWORD);
//  One Close Method for everyone
//    XInputCloseDevice


__inline
DWORD
XInputOpenGameController(
    IN DWORD   dwPlayerNumber,
    IN PXINPUT_POLLING_PARAMETERS pPollingParameters OPTIONAL,
    OUT PDWORD pdwDeviceHandle
    )
/*++
    Description:  Opens a game controller for input.  This routine results in the allocation of memory.

    Parameters:
        dwPlayerNumber     - The one based player number to open (this corresponds to
                             the port number on the box).
        pPollingParameters - Optionally specifies fine control over polling behavior.
                             Pass NULL for default behavior).
        pdwDeviceHandle    - pointer to DWORD to receive handle.

    Return Values:
        ERROR_SUCCESS               - on success
        ERROR_NOT_ENOUGH_MEMORY     - Could not allocate enough memory to open device.
        ERROR_DEVICE_NOT_CONNECTED  - If the device is not connected.
        ERROR_SHARING_VIOLATION     - If the device is already open.
        ERROR_NO_SYSTEM_RESOURCES   - If there is insufficient bandwidth to open the device.
        ERROR_OUTOFMEMORY           - The USB stack failed a memory allocation.

        This list is pretty good, but additional error codes may be possible now, or added in the future.
--*/
{
    return XInputOpenDevice(
                XUSB_DEVICE_TYPE_GAME_CONTROLLER,
                XUSB_PORT_FRONT, 
                dwPlayerNumber,
                pPollingParameters,
                pdwDeviceHandle
                );
}


__inline
DWORD
XInputOpenChatController(
    IN DWORD   dwPlayerNumber,
    IN PXINPUT_POLLING_PARAMETERS pPollingParameters OPTIONAL,
    OUT PDWORD pdwDeviceHandle
    )
/*++
    Description:  Opens a chat controller for input.  This routine results in the allocation of memory.

    Parameters:
        dwPlayerNumber     - The player number to open (this corresponds to
                             the port number on the box).
        pPollingParameters - Optionally specifies fine control over polling behavior.
                             Pass NULL for default behavior).
        pdwDeviceHandle    - pointer to DWORD to receive handle.

    Return Values:
        ERROR_SUCCESS               - on success
        ERROR_NOT_ENOUGH_MEMORY     - Could not allocate enough memory to open device.
        ERROR_DEVICE_NOT_CONNECTED  - If the device is not connected.
        ERROR_SHARING_VIOLATION     - If the device is already open.
        ERROR_NO_SYSTEM_RESOURCES   - If there is insufficient bandwidth to open the device.
        ERROR_OUTOFMEMORY           - The USB stack failed a memory allocation.

        This list is pretty good, but additional error codes may be possible now, or added in the future.
    Comments:
        The slot is always hard-coded to XUSB_PORT_TOP_SLOT, because the currently available
        chat device only fit in the top slot.
--*/
{
    return XInputOpenDevice(
                XUSB_DEVICE_TYPE_CHAT_CONTROLLER,
                XUSB_PORT_TOP_SLOT, 
                dwPlayerNumber,
                pPollingParameters,
                pdwDeviceHandle
                );
}

DWORD
XInputOpenDevice(
    IN DWORD   dwDeviceType,
    IN DWORD   dwPort,
    IN DWORD   dwPlayerNumber,
    IN PXINPUT_POLLING_PARAMETERS  pPollingParameters OPTIONAL,
    OUT PDWORD pdwDeviceHandle
    );
/*++
    Description:  Opens an input device for use.  This routine results in the allocation of memory.

    Parameters:
        dwDeviceType       - One of the following (defined xusbenum.h):
                             XUSB_DEVICE_TYPE_GAME_CONTROLLER
                             XUSB_DEVICE_TYPE_CHAT_CONTROLLER
        dwPlayerNumber     - The player number to open (this corresponds to
                             the port number on the box.
        dwPort             - Valid values:
                             XUSB_PORT_FRONT      
                             XUSB_PORT_TOP_SLOT   
                             XUSB_PORT_BOTTOM_SLOT
                             XUSB_PORT_REAR       
        pPollingParameters - (Optional) Specifies fine control over polling behavior.  Pass NULL
                             to use defaults(see comment on XINPUT_POLLING_PARAMETERS).
                            
        pdwDeviceHandle    - pointer to DWORD to receive handle.

    Return Values:
        ERROR_SUCCESS               - on success
        ERROR_NOT_ENOUGH_MEMORY     - Could not allocate enough memory to open device.
        ERROR_DEVICE_NOT_CONNECTED  - If the device is not connected.
        ERROR_SHARING_VIOLATION     - If the device is already open.
        ERROR_NO_SYSTEM_RESOURCES   - If there is insufficient bandwidth to open the device.
        ERROR_OUTOFMEMORY           - The USB stack failed a memory allocation.

        This list is pretty good, but additional error codes may be possible now, or added in the future.
--*/

DWORD
XInputCloseDevice(
    IN DWORD dwDeviceHandle
    );

/*++
    Description: Close the handle for a device.

    Parameters:
        dwDeviceHandle          - Handle returned by XInputOpenDevice.

    Comments:
        XInputCloseDevice will cancel any outstanding input requests.
        Outstanding output is not cancelled, but the actual close operation
        is delayed until the output completes, this should never be more
        than a couple of milliseconds.  Even if there is no outstanding
        closing the device is inherently asynchronous and will usually
        take around 5-10 ms.

        To speed shutdown time, XInputCloseDevice does not block either for
        outstanding output or for the shutdown time itself.  It returns right
        away.  The memory required by the handle will not be freed until
        the close proceedure is complete.

        Since you may only open each device once during this close time, the device
        cannot be reopen until the close is really complete.  XInputOpenDevice will
        block and retry for many milliseconds before returning an ERROR_SHARING_VIOLATION.

    Return Values:
        ERROR_SUCCESS        - Success
        ERROR_INVALID_HANDLE - Handle was invalid
--*/    

/*++
    Description:  Given a DeviceInstance fills the DeviceInformation structure.

    Parameters:
        DeviceInstance - This is LPARAM from WM_USB_DEVICE_INSERTED or WM_USB_DEVICE_REMOVED
        DeviceInformation   - Caller provided buffer to receive information, dwSize must
                              be initialized.
    Return Values:
        ERROR_SUCCESS           - on success
        ERROR_INVALID_PARAMETER - if DeviceInstance could not be a device.

  Comments:
        This routine works whether the device is attached or not.  It is impossible
        to know whether such a device has ever been attached.  All we can do is
        range check and see if such a device could have been attached.  It is possible
        (likely even) to "randomly" generate a plausible device instance.

        However, the fConnected member will also accurately determine if the described
        device is attached.

        We don't check subtype, because the valid range is subject to change between
        versions.
--*/

//------------------------------------------------------------------------
//  Device Capabilities
//------------------------------------------------------------------------
DWORD
XInputQueryCapabilities(
        IN  DWORD                      dwDeviceHandle,
        IN  WORD                       wReportId,
        OUT PXINPUT_CAPABILTIES_REPORT pCapabilities
        );

/*++
    Description: Retrieves the capabilities of the device for a given report.  Under XID
                 the capabilities and the report format have the same structure.

    Parameters:
        dwDeviceHandle  - Handle of device returned by XInputOpenDevice
        wReportTypeAndID- ReportType and ID to retreive capabilities information for. (See XID Specification)
        pCapabilities   - Caller provided buffer to receive capabilities.

    Return Values:
        ERROR_SUCCESS               - Success
        ERROR_INVALID_HANDLE        - Handle was invalid
        ERROR_DEVICE_NOT_CONNECTED  - The device is no longer connected.
        ERROR_INVALID_PARAMETER     - wReportId was not valid for the report type.
        ERROR_IO_DEVICE             - If the wReportTypeAndId are not supported by the device,
                                      or if an error occurs in data transmission.
        

    Comments:
        This routine performs a data transfer on the USB bus.  It blocks the calling thread
        until the transfer completes.  This is typically a couple of milliseconds.

        pInputFormat->bSize should be filled out entry.  This value represents the maximum number
        of bytes to copy.  No range checking is done.  If the caller specifies 0, 0 bytes will be copied.
        If the pInputFormat->bSize exceeds the actual report size, only the report size will be copied.

--*/

//------------------------------------------------------------------------
//  Device Polling 
//------------------------------------------------------------------------
DWORD
XInputGetDeviceState(
        IN      DWORD           dwDeviceHandle,
        IN OUT  PXINPUT_REPORT  pInputReport,
        OUT     PBOOL           pfNewData OPTIONAL
        );
/*++
    Description:    Retrieves latest known state of device.  This does not
        invoke a request for data to the hardware.

    Parameters:
        dwDeviceHandle  - Handle of device returned by XInputOpenDevice
        pInputFormat    - Caller provided buffer to receive state.
        pfNewData       - (OPTIONAL) On output indicates whether the
                          data returned is new.

    Return Values:
        ERROR_SUCCESS               - Success
        ERROR_INVALID_HANDLE        - Handle was invalid 
        ERROR_DEVICE_NOT_CONNECTED  - The device is no longer connected.

    Comment:
        
        The driver always caches the last known state of the device.  Either the
        driver is automatically posting requests, or the caller is calling
        XInput_PollDevice to explcitely post requests for data to the device
        (See XINPUT_POLLING_PARAMETERS for details).  In either case, the
        last known state of the device is always cached.  This routine retrieves
        the state.

        If pfNewData is non-NULL, then *pfNewData is set to TRUE if device
        has reported a new state data since the last call to XInputGetDeviceState
        for the device.  In all cases, the data is copied.
--*/

DWORD
XInputSendDeviceReport(
        IN      DWORD           dwDeviceHandle,
        IN      WORD            wReportId,
        IN      PXOUTPUT_REPORT pOutputReport
        );
/*++
    Description:    Sends an output report to the device.  This is used for operations
                    such as turning on and off the motors of a rumble pack, or LEDs on a
                    device.

    Parameters:
        dwDeviceHandle      - Handle of device returned by XInputOpenDevice.
        wReportId           - ReportId in the OutputReport.
        pOutputReport       - Pointer to buffer containing the output report to send.
                
    Return Values:
        ERROR_IO_PENDING            - Output report has been successfully queued to be sent.
        ERROR_INVALID_HANDLE        - Invalid Device Handle.
        ERROR_DEVICE_NOT_CONNECTED  - The device is no longer connected.
        ERROR_NOT_SUPPORTED         - Device does not support output.
        
    Comments:
        
        ASYNCHRONICITY
            This request is asynchronous in nature.  The following notes are important:
    
            Upon successfully queueing the request, the call always returns ERROR_IO_PENDING.  dwStatus (in the XOUTPUT_HEADER structure)
            is also set to ERROR_IO_PENDING.  When the transfer completes the status is changed to reflect the completion status of the
            operation (ERROR_SUCCESS or some other error the transfer failed).  The caller MUST NOT WRITE to the buffer (or header)
            while dwStatus is set to ERROR_PENDING.  However, the caller may READ the buffer while the transfer is pending.  Fire and
            forget is *not* supported.  The output report may not be freed until the game has verified that the output has completed.
            
            At this time, there is no way to cancel pending output.  Even XInputCloseDevice does not internally complete until the
            output is complete.

            hEvent (in the XOUTPUT_HEADER structure) is an optional event that is signalled when the I/O completes (i.e. right after
            dwStatus is changed to reflect the completion status).  It must be set to either NULL or a valid event handle before calling
            XInputSendDeviceReport.  If it does not represent a valid event handle and is not NULL, it will be *silently* NULL'd out
            and the transfer will be performed without setting an event.

            As a general rule, output requests should take no longer than about 4 ms to complete.  To be safe ALWAYS check dwStatus.  One
            extermely unlikely failure mode is that the request hangs.  At this time, there is no protection against this.  If testing
            determines that this is a problem, we can add a watchdog timer.

            Output requests to the SAME device are guaranteed to be serialized.  There are NO guarantees of
            serialization between different devices.

            Output requests are NOT cancelled by XInputCloseDevice, and may continue after XInputCloseDevice returns.  See XInputCloseDevice
            for further details.
--*/


DWORD
XInputPollDevice(
    IN DWORD dwDeviceHandle
    );

/*++
    Description: 
        Initiates a poll request on the input pipe.
    
    Parameters:
        dwDeviceHandle      - Handle of device returned by XInputOpenDevice.

    Return Values:
        ERROR_SUCCESS               - on success
        ERROR_INVALID_HANDLE        - Invalid Device Handle.
        ERROR_DEVICE_NOT_CONNECTED  - The device is no longer connected.
    
    Comments:

      Initiates a poll request on the input pipe.  The state is later retrieved
      with XInputGetDeviceState.  The device must have been opened with the fAutoPoll parameter
      of XInputPollDevice set to FALSE.  About 4 ms after calling XInputPollDevice, XInputGetDeviceState
      is guaranteed to return the latest state of the device.
    
      Turning fAutoPoll off and calling XInputPollDevice can increase performance, but will reserve more USB bandwidth.
      This can be a problem if the title also supports USB audio chat devices.  When a device is opened with fAutoPoll
      set to TRUE, the interrupt-in endpoint is opened to be hardware polled at 8 ms intervals.  Whenever new
      data is transfered, the software immediately posts a new request.  As long as new data is available, this
      will result in an interrupt every 8 ms to process new input packets.  Assuming the game calls XInputGetDeviceState
      only once per video frame, half of these packets are thrown out. 

      With fAutoPoll set to FALSE at open, the hardware polling rate is set to 1 ms.  However, the software does not post a
      request until XInputPollDevice is called.  If the software calls InputPollDevice once per video frame, and there is always
      new data available, there will only be interrupts at the video frame rate (~16ms per frame).  No data is thrown out.
      The only caveat is that game needs to call XInputPollDevice about 4 ms before XInputGetDeviceState in order to be guaranteed
      fresh data.

      XInputPollDevice is a NOP and returns ERROR_SUCCESS if XInputOpenDevice was called with fAutoPoll set to true.
--*/


#ifdef __cplusplus
    }   //close extern C
#endif

#endif // __XINPUT_API__
