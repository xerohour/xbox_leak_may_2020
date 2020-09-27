/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    ohci.h

Abstract:

    OpenHCI Version 1.0a defined registers and data structures
    Some comments included.  See OpenHCI specification for more details.
    
Environment:

    Designed for XBOX.

Notes:

Revision History:

    01-13-00 created by Mitchell Dernis (mitchd)

--*/
#ifndef __OCHI_H__
#define __OCHI_H__

//
//  Program interface code for PCI configuration space.
//  Couldn't find it in OpenHCI or UniversalHCI specification.
//  I figure it out by looking at the Win2k usb.inf file.
//
#define OHCI_PCI_PROGRAM_INTERFACE 0x10

//
//  Endpoint Control
//
typedef union _OHCI_ENDPOINT_CONTROL
{
    ULONG ul;
    struct {
        ULONG   FunctionAddress:7;      //Address of function (or hub) containing endpoint
        ULONG   EndpointAddress:4;      //Address of endpoint within function (or hub)
        ULONG   Direction:2;            //00b - 11b = read from TD, 01b = OUT, 10b = IN
        ULONG   Speed:1;                //0 = full speed, 1 = low speed
        ULONG   Skip:1;                 //When set, HC skips this Endpoint in the schedule
        ULONG   Format:1;               //0 = Control, Bulk or Interrupt, 1=Isochronous
        ULONG   MaximumPacketSize:11;   //Maximum bytes in a single packet
        ULONG   FutureSoftwareUse:5;    //Preserved by HC, available for driver use.
    };
} OHCI_ENDPOINT_CONTROL, *POHCI_ENDPOINT_CONTROL;

//
//  Structure OHCI_ENDPOINT_DESCRIPTOR
//  This structure is used to represent an endpoint in the USB schedule on OHCI controllers.
//  (DO NOT CONFUSE WITH USB_ENDPOINT_DESCRIPTOR DEFINED IN USB SPECIFICATION)
//
//  Comments:
//
//  Must be 4 DWORDs (16 bytes) long.
//
//  When TailP = HeadP the list contains no TD that the HC can process.
//
//
typedef struct _OHCI_ENDPOINT_DESCRIPTOR
{
    //First DWORD
    OHCI_ENDPOINT_CONTROL Control;  //see OHCI_ENDPOINT_CONTROL

    //Second DWORD
    ULONG   TailP;                  //Points to last TD for Endpoint.  Pointer should be
                                    //paragraph(16 bytes) aligned.  4 lsb are ignored.
    
    //Third DWORD
    ULONG   HeadPHaltCarry;         //Point to first TD for Endpoint. Pointer should be
                                    //paragraph(16 bytes) aligned.  2 lsb are used though
                                    //for Halt and toggleCarry fields.  Bits 3, 4 must be zero.
                                    //Therefore, use access macros.  WRITE_HEADP/READ_HEADP,
                                    //SET_HALTED/CLEAR_HALTED/GET_HALTED, and SET_TOGGLECARRY/
                                    //CLEAR_TOGGLECARRY/GET_TOGGLECARRY defined below.

    //Fourth DWORD
    ULONG   NextED;                 //Points to next endpoint.  Pointer should be
                                    //paragraph(16 bytes) aligned.  4 lsb are ignored.

} OHCI_ENDPOINT_DESCRIPTOR, *POHCI_ENDPOINT_DESCRIPTOR;

//
//  Macros for access to HeadPHaltCarry element of OHCI_ENDPOINT_DESCRIPTOR
//
#define WRITE_HEADP(_endpoint_descriptor_pointer_, _HeadP_)\
            (_endpoint_descriptor_pointer_)->HeadPHaltCarry = (_HeadP_ & 0xFFFFFFF0) | \
            ((_endpoint_descriptor_pointer_)->HeadPHaltCarry & 0x0000000F);
#define READ_HEADP(_endpoint_descriptor_pointer_) ((_endpoint_descriptor_pointer_)->HeadPHaltCarry & 0xFFFFFFF0)

#define SET_HALTED(_endpoint_descriptor_pointer_) ((_endpoint_descriptor_pointer_)->HeadPHaltCarry |= 0x00000001)
#define CLEAR_HALTED(_endpoint_descriptor_pointer_) ((_endpoint_descriptor_pointer_)->HeadPHaltCarry &= ~0x00000001)
#define GET_HALTED(_endpoint_descriptor_pointer_) \
            (((_endpoint_descriptor_pointer_)->HeadPHaltCarry & 0x00000001) ? TRUE : FALSE)


#define SET_TOGGLECARRY(_endpoint_descriptor_pointer_) ((_endpoint_descriptor_pointer_)->HeadPHaltCarry |= 0x00000002)
#define CLEAR_TOGGLECARRY(_endpoint_descriptor_pointer_) ((_endpoint_descriptor_pointer_)->HeadPHaltCarry &= ~0x00000002)
#define GET_TOGGLECARRY(_endpoint_descriptor_pointer_) \
            (((_endpoint_descriptor_pointer_)->HeadPHaltCarry & 0x00000002) ? TRUE : FALSE)


//
//  Structure OHCI_TRANSFER_DESCRIPTOR
//  This structure is used to represent bulk, interrupt and control transfers
//  descriptors in the USB schedule on OHCI controllers.
//
//  Must be 4 DWORDs (16 bytes) long.
//
typedef struct _OHCI_TRANSFER_DESCRIPTOR *POHCI_TRANSFER_DESCRIPTOR;
typedef struct _OHCI_TRANSFER_DESCRIPTOR
{
    //First DWORD
    ULONG FutureSoftwareUse1:16;    //Preserved by HC, available for driver use.
    ULONG Format:1;                 //Preserved by HC, XBOX OHCD driver defined analog to format set in ED
    ULONG FutureSoftwareUse2:1;     //Preserved by HC, available for driver use.
    ULONG BufferRounding:1;         //If clear, than short packets generate an error.
    ULONG Direction_PID:2;          //00b = SETUP, 01b = OUT, 10b = IN, 11b = Reserved
    ULONG DelayInterrupt:3;         //Time (in frames) before interrupt must generated after
                                    //this is TD is completed.  111b = No Interrupt
    ULONG DataToggle:2;             //Used for DATA0\DATA1 toggling.  MSb = 0 indicates
                                    //that toggleCarry in ED should be used.  MSb = 1 indicates
                                    //that LSb should be used.
    ULONG ErrorCount:2;             //Error count on attempts to process TD.  After 3, TD is
                                    //retired as a failure with the ConditionCode set. Endpoint
                                    //is halted.
    ULONG ConditionCode:4;          //Status of TD

    //Second DWORD
    ULONG CurrentBufferPointer;     //Pointer to current location in data buffer which receives
                                    //or sends data.

    //Third DWORD
    //  When a TD is handed over to the HC NextTD has a correct physical address.
    //  However, after dequeueing a TD from the DoneQueue, we use NextTDVirtual
    //  so that the driver may easily traverse the list.
    //
    union
    {
        ULONG                       NextTD;         //Pointer to next Transfer Desciptor.  Must be paragraph
        POHCI_TRANSFER_DESCRIPTOR   NextTDVirtual;  //aligned, i.e. bits 0-3 must be 0. We also
    };

    //Fourth DWORD
    ULONG BufferEnd;                //BufferEnd - used for 2 page scatter\gather on long packets.
                                    //see OHCI specification.
} OHCI_TRANSFER_DESCRIPTOR;


//
//  Macros for OHCI_TRANSFER_DESCRIPTOR.Direction_PID
//
#define OHCI_TD_DIRECTION_PID_SETUP         0
#define OHCI_TD_DIRECTION_PID_OUT           1
#define OHCI_TD_DIRECTION_PID_IN            2


//
// Macros for OHCI_TRANSFER_DESCRIPTOR.DelayInterrupt;
//
#define OHCI_TD_DELAY_INTERRUPT_0_MS        0   // Interrupt at end of frame TD is completed
#define OHCI_TD_DELAY_INTERRUPT_1_MS        1   // Interrupt within 1 frame of TD compeletion
#define OHCI_TD_DELAY_INTERRUPT_2_MS        2   // Interrupt within 2 frames of TD compeletion
#define OHCI_TD_DELAY_INTERRUPT_3_MS        3   // Interrupt within 3 frame of TD compeletion
#define OHCI_TD_DELAY_INTERRUPT_4_MS        4   // Interrupt within 3 frame of TD compeletion
#define OHCI_TD_DELAY_INTERRUPT_5_MS        5   // Interrupt within 3 frame of TD compeletion
#define OHCI_TD_DELAY_INTERRUPT_6_MS        6   // Interrupt within 3 frame of TD compeletion
#define OHCI_TD_DELAY_INTERRUPT_NONE        7   // Do not Interrupt upon completion of TD


//
// Macros for OHCI_TRANSFER_DESCRIPTOR.DataToggle
//
#define OHCI_TD_TOGGLE_FROM_ED          0   // Get the toggle from the ED 
#define OHCI_TD_TOGGLE_DATA_MASK        1   // Mask used for toggling the lsb.
#define OHCI_TD_TOGGLE_DATA0            2   // DATA0 and MSb is set, so it will not use
                                            // endpoint toggle carry.
#define OHCI_TD_TOGGLE_DATA1            3   // DATA1 and MSb is set, so it will not use
                                            // endpoint toggle carry.


//
//  Condition Code definitions for both general and isochronous transfers
//
#define OHCI_CC_NO_ERROR                0x0 //Completed without detected error
#define OHCI_CC_CRC                     0x1 //CRC error
#define OHCI_CC_BIT_STUFFING            0x2 //Bit-stuffing error
#define OHCI_CC_DATA_TOGGLE_MISMATCH    0x3 //DATA0\DATA1 mismatched
#define OHCI_CC_STALL                   0x4 //Stall 
#define OHCI_CC_DEVICE_NOT_RESPONDING   0x5 //Device screwed up handshake
#define OHCI_CC_PID_CHECK_FAILURE       0x6 //Send PID was not valid for endpoint
#define OHCI_CC_UNEXPECTED_PID          0x7 //Receive PID was not valid for endpoint
#define OHCI_CC_DATA_OVERRUN            0x8 //Endpoint returned more data than MaximumPacketSize
#define OHCI_CC_DATA_UNDERRUN           0x9 //Endpoint returned short packet (only if BufferRounding = 0)
#define OHCI_CC_RESERVED1               0xA //Reserved
#define OHCI_CC_RESERVED2               0xB //Reserved
#define OHCI_CC_BUFFER_OVERRUN          0xC //During an IN, HC received data faster than it could
                                            //write it with DMA.
#define OHCI_CC_BUFFER_UNDERRUN         0xD //During an OUT, HC could not fetch data fast enough
                                            //through DMA.
#define OHCI_CC_NOT_ACCESSED            0xE //Either of these may be set by driver before TD
#define OHCI_CC_NOT_ACCESSED2           0xF //is inserted in schedule


//
//  A little 'coincidence' in the way USBD_STATUS_XXXX codes are defined.
//  we can convert very easily.
//
#define OHCI_CC_TO_USBD_STATUS(_ConditionCode_)  ((ULONG)_ConditionCode_ | 0xC0000000)

//
//  Structure OHCI_ISOCHRONOUS_TRANSFER_DESCRIPTOR
//  This structure is used to represent isochronous transfers
//  in the USB schedule on OHCI controllers.
//
//  Must be 8 DWORDs (32 bytes) long.
//
typedef struct _OHCI_ISOCHRONOUS_TRANSFER_DESCRIPTOR
{
    //DWORD 0
    ULONG   StartingFrame:16;       //Describes which frame transfer should start in.
    ULONG   Format:1;               //Preserved by HC, XBOX driver defined analog to format set in ED
    ULONG   FutureSoftwareUse:4;    //Preserved by HC, available for driver use.
    ULONG   DelayInterrupt:3;       //Time (in frames) before interrupt must generated after
                                    //this is TD is completed.  111b = No Interrupt
    ULONG   FrameCount:3;           //One less than count of Data Packets (one data packet = one frame)
    ULONG   FutureSoftwareUse2:1;   //Preserved by HC, available for driver use.
    ULONG   ConditionCode:4;        //Status of TD
    
    //DWORD 1
    ULONG   BufferPage0;            //Pointer to current location in data buffer which receives
                                    //or sends data.
    //DWORD 2
    ULONG   NextTD;                 //Pointer to next Transfer Desciptor.  Must be paragraph
                                    //aligned, i.e. bits 0-3 must be 0.

    //DWORD 3
    ULONG   BufferEnd;              //BufferEnd - used for 2 page scatter\gather on long packets.
                                    //see OHCI specification.

    //DWORDs 4-7
    USHORT  Offset_PSW[8];          //On input each of these is a buffer offset for
                                    //primitive scatter gather processing for each packet.
                                    //When retired, they contain the size and status
                                    //of transfers.
                                    //There are eight of these, one for each possible frame
                                    //that data can be sent in.
} OHCI_ISOCHRONOUS_TRANSFER_DESCRIPTOR, *POHCI_ISOCHRONOUS_TRANSFER_DESCRIPTOR;


//
//  Structure Represented Host Controller Communications Area(HCCA)
//
typedef struct _OHCI_HCCA
{
    ULONG   InterruptTable[32];     //Each entry points to list of Interrupt Endpoints.
                                    //These lists build the frame schedule for interrupts.
    USHORT  FrameNumber;            //Current Frame number (16 bits)
    USHORT  Pad1;                   //HC clears this when the FrameNumber is updated
    ULONG   DoneHead;               //Pointer to List of completed TDs
    UCHAR   Reserved[120];          //Scratch area for HC except for last 4 bytes which are
                                    //padding so that this is divisible by 32.
} OHCI_HCCA, *POHCI_HCCA;

//
//  Bit field for accessing the HcControl register
//
typedef union _HC_CONTROL_REGISTER
{
    ULONG   ul;
    struct
    {
        ULONG   ControlBulkServiceRatio:2;      //One less than the count of control to bulk packets services
        ULONG   PeriodicListEnable:1;           //If set, processing periodic lists is enabled
        ULONG   IsochronousEnable:1;            //If set, processing of Isochronous TD is enable if PeriodLists are.
        ULONG   ControlListEnable:1;            //If set, processing of Control Endpoints is enabled
        ULONG   BulkListEnable:1;               //If set, processing of Bulk Endpoints is enabled
        ULONG   HostControllerFunctionalState:2;//See HC_OPSTATE_USB_XXX definitions below
        ULONG   InterruptRouting:1;             //If set, interrupts are SMI routed, if clear normal bus interrupts
        ULONG   RemoteWakeupConnected:1;        //If set, HC supports remote wakeup.
        ULONG   RemoteWakeupEnabled:1;          //If set, remote wakeup is enabled.
        ULONG   Reserved:21;        
    };
} HC_CONTROL_REGISTER, *PHC_CONTROL_REGISTER;
#define HC_OPSTATE_USB_RESET        0
#define HC_OPSTATE_USB_RESUME       1
#define HC_OPSTATE_USB_OPERATIONAL  2
#define HC_OPSTATE_USB_SUSPEND      3
//
//  Bit fields are a bit inefficient.  When we are changing a 2 bit field
//  or even a bit at a time, it makes the code more readable.  However,
//  during the vary initial setup we want to go to the running state
//  while slamming a bunch of bits.  Here's what we slam.
//  
//  1) Control\Bulk Service Ratio start at 2:1
//
//  2) Periodic, Bulk, and Contorl lists are
//     enabled. Isochronous is disabled.
//
//  3) HostControllerFunctionState is UsbOperational
//  
//  4) Interrupt Routing is normal (bit clear)
//
//  5) Remote wakeup is disabled.
#define HC_CONTROL_REGISTER_START   0x000000B6

//
//  For quick reboot, we shutdown USB.  We slam it into the
//  following stopped state.
//
//  1) Control\Bulk Service Ratio 2:1
//
//  2) All list processing disabled.
//
//  3) HostControllerFunctionState is UsbReset
//
//  4) Interrupt Routing is normal (bit clear)
//
//  5) Remote wakeup is disabled.
#define HC_CONTROL_REGISTER_STOP  0x00000002

//
//  Bit field for accessing the HcCommandStatus register
//
typedef union _HC_COMMAND_STATUS
{
    ULONG   ul;
    struct {
        ULONG   HostControllerReset:1;          //Set to initiate software reset.
        ULONG   ControlListFilled:1;            //Set to indicate that a TD was added to a control ED
        ULONG   BulkListFilled:1;               //Set to indicate that a TD was added to a bulk ED
        ULONG   OwnerShipChangeRequest:1;       //Set to request control from SMM
        ULONG   Reserved:12;
        ULONG   SchedulingOverrunCount:2;       //Count of schedule overruns.
        ULONG   Reserved2:14;
    };
} HC_COMMAND_STATUS, *PHC_COMMAND_STATUS;

//
//  Macros to use insted for ControlListFilled and BulkListFilled
//
#define HCCS_ControlListFilled  0x00000002
#define HCCS_BulkListFilled     0x00000004


//
//  Macros for HcInterruptEnable and HcInterruptDisable bit fields
//
//  The HCD can write to both HcInterruptEnable and HcInterruptDisable, however, only bit
//  setting is processed not bit clearing.  To enable an interrupt, set the corresponding bit
//  in HcInterruptEnable.  To disable an interrupt write to HcInterruptDisable.  Since the
//  bit layouts are identical, there are only one set of macros.
//
#define HCINT_SchedulingOverrun     0x00000001  //Enables\Disables interrupt on scheduling overrun
#define HCINT_WritebackDoneHead     0x00000002  //Enables\Disables interrupt on Writeback of HcDoneHead
#define HCINT_StartOfFrame          0x00000004  //Enables\Disables interrupt on Start of Frame
#define HCINT_ResumeDetect          0x00000008  //Enables\Disables interrupt on Resume detect
#define HCINT_UnrecoverableError    0x00000010  //Enables\Disables interrupt on Unrecoverable Error
#define HCINT_FrameNumberOverflow   0x00000020  //Enables\Disables interrupt on HcFmNumber changes
#define HCINT_RootHubStatusChange   0x00000040  //Enables\Disables interrupt on HcRhStatus or HcRhPortStatus[n] changes
//Bunch of reserved bits
#define HCINT_OwnershipChange       0x40000000  //Enables\Disables interrupt on Ownership change
#define HCINT_MasterInterruptEnable 0x80000000  //Master switch for enabling interrupts

//
//  Bit Field structure for HcFmInterval
//
typedef union _HC_FRAME_INTERVAL
{
    ULONG   ul;
    struct{
        ULONG   FrameInterval:14;               //adjustable interval in clocks between SOF, can be used
                                                //to sync to external clock source
        ULONG   Reserved:2;                     
        ULONG   FullSpeedMaximumPacketSize:15;  //Maximum number of bits in full speed packets to send
                                                //with scheduling over flow.  Calculated by HCD at initialization.
        ULONG   FrameIntervalToggle:1;          //Toggled by HCD when FrameInterval changes.
    };
} HC_FRAME_INTERVAL, *PHC_FRAME_INTERVAL;


//
//  The frame interval is set in terms of clocks.  However, there is overhead from SOF and other
//  minutia.  Additionally, there is bit stuffing which eats on average one out of 7 bits.
//  According to OHCI we need to calculate this.
//  210 is an estimate of the overhead.  I found this formula in existing Win2k code.
//
#define OHCI_DEFAULT_FRAME_INTERVAL 0x2EDF
#define OHCI_CLOCKS_TO_BITS(_Clocks_) ((((_Clocks_) - 210) * 6)/7)

//
//  bit field structure fpr HcRhDescriptorA
//
typedef union _HC_RH_DESCRIPTOR_A
{
    ULONG ul;
    struct
    {
        //First byte
        UCHAR   NumberDownstreamPorts;          //Count of downstream ports off of root hub.

        //Second byte
        UCHAR   PowerSwitchingMode:1;           //If NoPowerSwitch is set, this determines the mode.
        UCHAR   NoPowerSwitch:1;                //0 = ports are power switchable, 1 = ports powered if HC powered.
        UCHAR   DeviceType:1;                   //Should always be 0, because Root Hub is not a compound device.
        UCHAR   OverCurrentProtectionMode:1;    //If set overcurrent is reported on per port basis.fs
        UCHAR   NoOverCurrentProtection:1;      //If set overcurrent is not reported.
        UCHAR   ReservedBits:3;

        //Third byte
        UCHAR   Reserved;

        //Fourth byte
        UCHAR   PowerOnToPowerGoodTime;         //Time HCD waits after powering hub before accessing hub.
                                                //Reported in 2 ms units.
    };
} HC_RH_DESCRIPTOR_A, *PHC_RH_DESCRIPTOR_A;

//
//  bit field structure fpr HcRhDescriptorB
//
typedef union _HC_RH_DESCRIPTOR_B
{
    ULONG ul;
    struct
    {
        USHORT  DeviceRemovable;                //Each bit is dedicated to a port.  If set indicates
                                                //that device can NOT be removed from root hub.
        USHORT  PortPowerControlMask;           //Each bit is dedicated to a port.  If set indicates
                                                //that device is only effected by per port power control.
                                                //Ignored if PowerSwitchingMode is cleared.
    };
} HC_RH_DESCRIPTOR_B, *PHC_RH_DESCRIPTOR_B;

//
//  macros and struct for HcRhStatus register
//
//  The HubStatus part represents the status if read.
//  Write a one to the corresponding bit in HubStatusChange.
//
//  However to complicate matter, the LocalPower bit can be written
//  to in HubStatus.  It has the effect of turning off power.  The corresponding
//  bit in HubStatusChange turns power on.  Guess what, reading LocalPower in the
//  HubStatusChange register always returns 0.
//
//  OverCurrentIndicator works as expected.  On read of HubStatus it returns the
//  state, it is not writeable. In HubStatusChange a read returns one if the state
//  change.   You can clear it with a write of a one to HubStatusChange.
//
//  Reading DeviceRemoteWakeup in HubStatus indicates whether a remote wakeup was signaled.
//  Writing a one to HubStatus enables remote wakeup.  Writing a one to HubStatusChange
//  disables remote wakeup.
//
#define HC_RH_STATUS_LocalPower                 0x0001
#define HC_RH_STATUS_OvercurrentIndicator       0x0002
#define HC_RH_STATUS_DeviceRemoteWakeupEnable   0x8000
typedef union _HC_RH_STATUS
{
    ULONG   ul;
    struct {
        USHORT  HubStatus;              //bit field indicating hub status
        USHORT  HubStatusChange;        //bit field for changing hub status
    };
} HC_RH_STATUS, *PHC_RH_STATUS;

//
//  Values for initializing the RootHub on Xbox
//

#define HC_RH_DESCRIPTOR_A_INIT_XBOX         0x00001200  //NoOverCurrentProtection|NoPowerSwitch
#define HC_RH_DESCRIPTOR_B_INIT_XBOX         0x00000000  
#define HC_RH_STATUS_INIT_XBOX               0x80000000  //Clears DeviceRemoteWakeupEnable

//
//  macros and struct for HcRhPortStatus,
//  !!!!Some of the bits do double duty (different on read and write)
//  so there may be more than one macro per bit!!!
//
#define HC_RH_PORT_STATUS_CurrentConnectStatus      0x0001  //Read PortStatus, 1 if device currently connected
#define HC_RH_PORT_STATUS_ClearPortEnable           0x0001  //Set PortStatus, disables  clears PortEnableStatus
                                                            //Read PortStatusChange, 1 indicates connect or disconnect
                                                            //Set PortStatusChange, clears this bit in PortStatusChange
#define HC_RH_PORT_STATUS_PortEnableStatus          0x0002  //Read PortStatus, 1 if port is enable
#define HC_RH_PORT_STATUS_SetPortEnable             0x0002  //Set PortStatus, enables device
                                                            //Read PortStatusChange, 1 indicates change in PortStatus bit
                                                            //Set PortStatusChange, clears this bit in PortStatusChange
#define HC_RH_PORT_STATUS_PortSuspendStatus         0x0004  //Read PortStatus, 1 if port is suspended
#define HC_RH_PORT_STATUS_SetPortSuspend            0x0004  //Set PortStatus suspends device
                                                            //Read PortStatusChange, 1 indicates change in PortStatus bit
                                                            //Set PortStatusChange, clears this bit in PortStatusChange
#define HC_RH_PORT_STATUS_PortOverCurrentIndicator  0x0008  //Read PortStatus, 1 if port is overcurrent detected
#define HC_RH_PORT_STATUS_ClearSuspendStatus        0x0008  //Set PortStatus resumes device
                                                            //Read PortStatusChange, 1 indicates change in PortStatus bit
                                                            //Set PortStatusChange, clears this bit in PortStatusChange
#define HC_RH_PORT_STATUS_PortResetStatus           0x0010  //Read PortStatus, 1 if port reset signal is active
#define HC_RH_PORT_STATUS_SetPortReset              0x0010  //Set PortStatus initiates reset signal
                                                            //Read PortStatusChange, 1 indicates reset signal completed (10 ms)
                                                            //Set PortStatusChange, clears this bit in PortStatusChange
#define HC_RH_PORT_STATUS_PortPowerStatus           0x0100  //Read PortStatus, 1 if port power is on
#define HC_RH_PORT_STATUS_SetPortPower              0x0100  //Set PortStatus turns power on
                                                            //There is no PortStatusChange bit
#define HC_RH_PORT_STATUS_LowSpeedDeviceAttach      0x0200  //Read PortStatus, 1 connected device is low speed.
#define HC_RH_PORT_STATUS_ClearPortPower            0x0200  //Set PortStatus, shut port power off.
                                                            //There is no PortStatusChange bit

typedef union _HC_RH_PORT_STATUS
{
    ULONG   ul;
    struct
    {
        USHORT  PortStatus;             //bit field indicating\changing port status
        USHORT  PortStatusChange;       //bit field for indicating changes in port status
    };
} HC_RH_PORT_STATUS, *PHC_RH_PORT_STATUS;

                                                            
//
// Operational Registers
//
typedef struct _OHCI_OPERATIONAL_REGISTERS
{
    //Control and Status
    ULONG               HcRevision;             //LSB is BCD revision of OpenHCI
    HC_CONTROL_REGISTER HcControl;
    HC_COMMAND_STATUS   HcCommandStatus;
    ULONG               HcInterruptStatus;      //See HCINT_Xxxx macros above
    ULONG               HcInterruptEnable;      //See HCINT_Xxxx macros above
    ULONG               HcInterruptDisable;     //See HCINT_Xxxx macros above

    //Memory Pointers
    //  All are pointers to physical memory.  Excepting HcHCCA they are paragraph (16 bytes)
    //  aligned.  HcHCCA is on 256 byte boundary at minimum, some controllers require more
    //  stringent alignmnet.  Test by writing 0xFFFFFFFF and reading back to see how many
    //  of the least signficant bits are 0.
    ULONG   HcHCCA;                 //HCCA area
    ULONG   HcPeriodCurrentED;      //Current ED being processed in (interrupt and isoch lists)
    ULONG   HcControlHeadED;        //Head of ED list for control pipes
    ULONG   HcControlCurrentED;     //Current location ED list for control pipes
    ULONG   HcBulkHeadED;           //Head of ED list for bulk pipes
    ULONG   HcBulkCurrentED;        //Current location ED list for bulk pipes
    ULONG   HcDoneHead;             //Pointer list of completed TDs.  Head of list is last completed.

    //Frame Counter
    HC_FRAME_INTERVAL   HcFmInterval;   //HC_FRAME_INTERVAL for comments
    ULONG               HcFmRemaining;  //14 bit down counter of bits remaining in current frame
                                        //high bit is a toggle, to sync with FrameInterval
    ULONG               HcFmNumber;     //16-bit Frame Counter
    ULONG               HcPeriodicStart;//14-bit value calculated by HCD to reserve enough bandwidth
                                        //for the interrupt and isoch ED list processing.  It is the number
                                        //of bits into the frame before moving to periodic processing.
    ULONG               HcLSThreshold;  //12-bit value for minimum number of bits in frame for which to
                                        //attempt transfering an 8 byte low-speed packet.

    //Root Hub
    //  Registers which are involved with with the root hub emulation
    HC_RH_DESCRIPTOR_A  HcRhDescriptorA;    
    HC_RH_DESCRIPTOR_B  HcRhDescriptorB;
    HC_RH_STATUS        HcRhStatus;
    HC_RH_PORT_STATUS   HcRhPortStatus[1];  //Variable length really, depends on count of hubs
} OHCI_OPERATIONAL_REGISTERS, *POHCI_OPERATIONAL_REGISTERS;
//
//  This is not necessarily sizeof(OHCI_OPERATIONAL_REGISTERS).
//  Rather it is the address space that it occupies in memory.
//
#define OHCI_OPERATIONAL_REGISTER_SIZE 0x1000
#endif //__OCHI_H__
