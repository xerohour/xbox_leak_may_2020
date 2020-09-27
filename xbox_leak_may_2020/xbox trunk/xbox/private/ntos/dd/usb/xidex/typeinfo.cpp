/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    input.c

Abstract:
    
    Tables with information concerning the currently supported
    set of XID devices.
    
    
Environment:

    Designed for XBOX.

Notes:

Revision History:

    08-01-00 created by Mitchell Dernis (mitchd)

--*/
//
//  Pull in OS headers
//
#define _XAPI_
extern "C" {
#include <ntos.h>
}
#include <ntrtl.h>
#include <nturtl.h>
#include <xtl.h>
#include <xboxp.h>

//
//  Setup the debug information for this file (see ..\inc\debug.h)
//
#define MODULE_POOL_TAG          '_DIH'
#include <debug.h>
DEFINE_USB_DEBUG_FUNCTIONS("INPUT");

//
//  Pull in public usb headers
//
#include <usb.h>

//
//  Pull in xid headers
//
#include "xid.h"

//This will create a table of subtypes that we can find to walk
#pragma data_seg(".XID$A")
extern "C" DWORD XID_BeginTypeDescriptionTable = 0;
#pragma data_seg(".XID$ZZZ")
extern "C" DWORD XID_EndTypeDescriptionTable = 0;
#pragma data_seg(".XPP$Data")
#pragma code_seg(".XPPCODE")

//  Tells the linker to merge the USB sections together
#pragma comment(linker, "/merge:.XID=.XPP")

//----------------------------------------------
// Functions to walk the type tables.
//----------------------------------------------

PXID_TYPE_INFORMATION FASTCALL GetTypeInformation(UCHAR XidType, UCHAR *TypeIndex)
{
    *TypeIndex = 0;
    UCHAR ucTypeIndex = 0;
    PXID_TYPE_INFORMATION *ppTypeInformation;
    ppTypeInformation = (PXID_TYPE_INFORMATION *)((&XID_BeginTypeDescriptionTable)+1);
    while( (ULONG_PTR)ppTypeInformation < (ULONG_PTR)&XID_EndTypeDescriptionTable )
    {
        if(*ppTypeInformation)
        {
            if((*ppTypeInformation)->ucType == XidType)
            {
                *TypeIndex = ucTypeIndex;
                return *ppTypeInformation;
            }
            ucTypeIndex++;
        }
        ppTypeInformation++;
    }
    return NULL;
}

PXID_TYPE_INFORMATION FASTCALL GetTypeInformation(PXPP_DEVICE_TYPE XppType)
{
    PXID_TYPE_INFORMATION *ppTypeInformation;
    ppTypeInformation = (PXID_TYPE_INFORMATION *)((&XID_BeginTypeDescriptionTable)+1);
    while( (ULONG_PTR)ppTypeInformation < (ULONG_PTR)&XID_EndTypeDescriptionTable )
    {
        if(*ppTypeInformation && ((*ppTypeInformation)->XppType == XppType))
        {
            return *ppTypeInformation;
        }
        ppTypeInformation++;
    }
    return NULL;
}

/****
*****   Type Table Entries for the standard types.
****/

//----------------------------------------------
//  Game Controller Reports
//----------------------------------------------
//#define  XID_DEVTYPE_GAMECONTROLLER      1 - defined in xid.h
#define  XID_DEVSUBTYPE_MAX_GAME           1
#define  XID_INPUT_REPORT_ID_MAX_GAME      0
#define  XID_OUTPUT_REPORT_ID_MAX_GAME     0
#define XID_DEFAULT_MAX_GAMEPAD        4

DECLARE_XPP_TYPE(XDEVICE_TYPE_GAMEPAD)
XINPUT_GAMEPAD       GamepadDefaults ={0, //All the digital buttons (including DPAD) up.
                                       0,0,0,0,0,0,0,0,   //All the analog buttons up
                                       0,0,       //The Left Thumbstick centered
                                       0,0        //The Right Thumbstick centered
                                       };

XID_REPORT_TYPE_INFO GameInputReportInfoList[1] = {sizeof(XINPUT_GAMEPAD),(PVOID)&GamepadDefaults};
XINPUT_RUMBLE        RumbleDefaults = {0};  //The motors are off by default
XID_REPORT_TYPE_INFO GameOutputReportInfoList[1] = {sizeof(XINPUT_RUMBLE),(PVOID)&RumbleDefaults};
XINPUT_POLLING_PARAMETERS GameDefaultPolling = {TRUE,FALSE,0,8,0,0}; //AutoPoll On, control-OUT,

XID_TYPE_INFORMATION  GamepadTypeInfo = 
 {XID_DEVTYPE_GAMECONTROLLER, XID_DEFAULT_MAX_GAMEPAD, XID_INPUT_REPORT_ID_MAX_GAME, XID_OUTPUT_REPORT_ID_MAX_GAME,
 XDEVICE_TYPE_GAMEPAD, GameInputReportInfoList, GameOutputReportInfoList, &GameDefaultPolling, XID_ProcessGamepadData, 0};
     
#pragma data_seg(".XID$Gamepad")
extern "C" ULONG_PTR GamepadTypeInfoENTRY = (ULONG_PTR)&GamepadTypeInfo;
#pragma data_seg(".XPP$Data")

//----------------------------------------------
//  Keyboard Reports
//----------------------------------------------
//#define  XID_DEVTYPE_KEYBOARD            2 - defined in xid.h
#define  XID_DEVSUBTYPE_MAX_KEYBOARD       1
#define  XID_INPUT_REPORT_ID_MAX_KEYBOARD  0
#define  XID_OUTPUT_REPORT_ID_MAX_KEYBOARD 0
#define  XID_DEFAULT_MAX_KEYBOARD          1

DECLARE_XPP_TYPE(XDEVICE_TYPE_DEBUG_KEYBOARD)
XINPUT_KEYBOARD      KeyboardDefaults ={0, //All modified keys up
                                       0, //Reserved byte is zero
                                       0,0,0,0,0,0 //No keys down
                                       };

XID_REPORT_TYPE_INFO KeyboardReportInfoList[1] = {sizeof(XINPUT_KEYBOARD),(PVOID)&KeyboardDefaults};
XINPUT_KEYBOARD_LEDS KeyboardLEDDefaults = {0};  //All light off by default
XID_REPORT_TYPE_INFO KeyboardLEDReportInfoList[1] = {sizeof(XINPUT_KEYBOARD_LEDS),(PVOID)&KeyboardLEDDefaults};
XINPUT_POLLING_PARAMETERS KeyboardDefaultPolling = {TRUE,FALSE,0,16,0,0}; //AutoPoll On, SET_REPORT for out,

XID_TYPE_INFORMATION  KeyboardTypeInfo = 
 {XID_DEVTYPE_KEYBOARD, XID_DEFAULT_MAX_KEYBOARD, XID_INPUT_REPORT_ID_MAX_KEYBOARD, XID_OUTPUT_REPORT_ID_MAX_KEYBOARD,
 XDEVICE_TYPE_DEBUG_KEYBOARD, KeyboardReportInfoList, KeyboardLEDReportInfoList, &KeyboardDefaultPolling, XID_ProcessNewKeyboardData,
 XID_BSF_NO_CAPABILITIES|XID_BSF_NO_OUTPUT_HEADER};

#pragma data_seg(".XID$Keyboard")
extern "C" ULONG_PTR KeyboardTypeInfoENTRY = (ULONG_PTR)&KeyboardTypeInfo;
#pragma data_seg(".XPP$Data")

//----------------------------------------------
//  IR Remote Reports
//----------------------------------------------
#define  XID_DEVTYPE_IRREMOTE              3
#define  XID_DEVSUBTYPE_MAX_IRREMOTE       1
#define  XID_INPUT_REPORT_ID_MAX_IRREMOTE  0
#define  XID_OUTPUT_REPORT_ID_MAX_IRREMOTE 0
#define XID_DEFAULT_MAX_IRREMOTE           1

DECLARE_XPP_TYPE(XDEVICE_TYPE_IR_REMOTE)
XINPUT_IR_REMOTE     IrRemoteDefaults = {0,0};
XID_REPORT_TYPE_INFO IrRemoteReportInfoList[1] = {sizeof(XINPUT_IR_REMOTE),(PVOID)&IrRemoteDefaults};
XINPUT_POLLING_PARAMETERS IrRemoteDefaultPolling = {TRUE,FALSE,0,16,0,0};  //AutoPoll On, Interrupt-OUT, 16 ms Input Poll Interval

XID_TYPE_INFORMATION  IrRemoteTypeInfo =
 {XID_DEVTYPE_IRREMOTE, XID_DEFAULT_MAX_IRREMOTE, XID_INPUT_REPORT_ID_MAX_IRREMOTE, XID_OUTPUT_REPORT_ID_MAX_IRREMOTE,
 XDEVICE_TYPE_IR_REMOTE, IrRemoteReportInfoList, NULL, &IrRemoteDefaultPolling, XID_ProcessIRRemoteData, 0};

#pragma data_seg(".XID$IrRemote")
extern "C" ULONG_PTR IrRemoteTypeInfoENTRY = (ULONG_PTR)&IrRemoteTypeInfo;
#pragma data_seg(".XPP$Data")
