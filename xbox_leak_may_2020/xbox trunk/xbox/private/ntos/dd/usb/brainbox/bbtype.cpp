/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    bbtype.cpp

Abstract:
    
    Pluggable table entry for the Brain Box controller device.
    
    
Environment:

    Designed for XBOX.

Notes:

Revision History:

    12-03-00 created by Mitchell Dernis (mitchd)

--*/
extern "C" {
#include <ntos.h>
}
#include <ntrtl.h>
#include <nturtl.h>
#include <xtl.h>
#include <xboxp.h>
#include <usb.h>
#include "xid.h"
#include "brainbox.h"

// Forward declare
void FASTCALL fProcessBrainBoxData(PXID_OPEN_DEVICE OpenDevice);

//Brain Box 
#define  XID_DEVTYPE_BRAINBOX              0x80
#define  XID_INPUT_REPORT_ID_MAX_BRAINBOX  0
#define  XID_OUTPUT_REPORT_ID_MAX_BRAINBOX 0
#define  XID_DEFAULT_MAX_BRAINBOX          2

DECLARE_XPP_TYPE(XDEVICE_TYPE_BRAINBOX)
XINPUT_BRAINBOX_DATA BrainboxDefaults = {0};  //Everything zero.
XID_REPORT_TYPE_INFO BrainboxInputReportInfoList[1] = {sizeof(XINPUT_BRAINBOX_DATA),(PVOID)&BrainboxDefaults};
WORD bmLEDDefaults[3] = {0};  //All lights are off
XID_REPORT_TYPE_INFO BrainboxOutputReportInfoList[1] = {6,(PVOID)&bmLEDDefaults};
XINPUT_POLLING_PARAMETERS BrainboxDefaultPolling = {TRUE,FALSE,0,8,0,0}; //AutoPoll On, control-OUT

XID_TYPE_INFORMATION  BrainBoxTypeInfo = 
 {XID_DEVTYPE_BRAINBOX, XID_DEFAULT_MAX_BRAINBOX, XID_INPUT_REPORT_ID_MAX_BRAINBOX, XID_OUTPUT_REPORT_ID_MAX_BRAINBOX,
  XDEVICE_TYPE_BRAINBOX, BrainboxInputReportInfoList, BrainboxOutputReportInfoList, &BrainboxDefaultPolling, fProcessBrainBoxData, 0};
     
#pragma data_seg(".XID$BrainBox")
extern "C" ULONG_PTR BrainBoxTypeInfoENTRY = (ULONG_PTR)&BrainBoxTypeInfo;
#pragma data_seg(".XPP$Data")

void
FASTCALL
fProcessBrainBoxData(
    PXID_OPEN_DEVICE OpenDevice
    )
/*++
 Routine Description:
   Processing of brain box data.  Basically, it copies the data
   from the buffer that is ping-ponged to the hardware, to the one
   that is always available for copying from XInputGetState.

   Then it checks to see if anything changes and hits XAutoPowerDownResetTimer
--*/
{
    XINPUT_BRAINBOX_DATA *pBrainBox = (XINPUT_BRAINBOX_DATA *)OpenDevice->Report;
    if(OpenDevice->Urb.CommonTransfer.TransferBufferLength >= XID_REPORT_HEADER)
    {
        RtlCopyMemory(
          (PVOID)pBrainBox,
          (PVOID)(OpenDevice->ReportForUrb+XID_REPORT_HEADER),
          OpenDevice->Urb.ControlTransfer.TransferBufferLength-XID_REPORT_HEADER
          );

        //
        //  XAutoPowerDownResetTimer if any digital buttons are pressed.
        //  
        int i;
        for(i=0; i<3; i++)
        {
            if(pBrainBox->bmButtons[i])
            {
                XAutoPowerDownResetTimer();
                return;
            }
        }
        //
        //  ??? Should we check any other controls to avoid auto power down. ???
        //
    }
}
