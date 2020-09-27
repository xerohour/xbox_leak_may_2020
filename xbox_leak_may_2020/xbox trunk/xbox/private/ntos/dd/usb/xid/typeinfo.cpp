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


//----------------------------------------------
//  Game Controller Reports
//----------------------------------------------
#define XID_DEFAULT_MAX_GAMEPAD        4
XINPUT_GAMEPAD       GamepadDefaults ={0, //All the digital buttons (including DPAD) up.
                                       0,0,0,0,0,0,0,0,   //All the analog buttons up
                                       0,0,       //The Left Thumbstick centered
                                       0,0        //The Right Thumbstick centered
                                       };
XID_REPORT_TYPE_INFO GameInputReportInfoList[1] = {sizeof(XINPUT_GAMEPAD),(PVOID)&GamepadDefaults};
XINPUT_RUMBLE        RumbleDefaults = {0};  //The motors are off by default
XID_REPORT_TYPE_INFO GameOutputReportInfoList[1] = {sizeof(XINPUT_RUMBLE),(PVOID)&RumbleDefaults};
XINPUT_POLLING_PARAMETERS GameDefaultPolling = {TRUE,FALSE,0,8,0,0}; //AutoPoll On, control-OUT,
                                                                     //8 ms Input Poll Interval,
                                                                     

//----------------------------------------------
//  Keyboard Reports
//----------------------------------------------
#define XID_DEFAULT_MAX_KEYBOARD          1
XINPUT_KEYBOARD      KeyboardDefaults ={0, //All modified keys up
                                       0, //Reserved byte is zero
                                       0,0,0,0,0,0 //No keys down
                                       };
XID_REPORT_TYPE_INFO KeyboardReportInfoList[1] = {sizeof(XINPUT_KEYBOARD),(PVOID)&KeyboardDefaults};
XINPUT_KEYBOARD_LEDS KeyboardLEDDefaults = {0};  //All light off by default
XID_REPORT_TYPE_INFO KeyboardLEDReportInfoList[1] = {sizeof(XINPUT_KEYBOARD_LEDS),(PVOID)&KeyboardLEDDefaults};
XINPUT_POLLING_PARAMETERS KeyboardDefaultPolling = {TRUE,FALSE,0,16,0,0}; //AutoPoll On, SET_REPORT for out,
                                                                          //16 ms Input Poll Interval,

//----------------------------------------------
//  IR Remote Reports
//----------------------------------------------
#define XID_DEFAULT_MAX_IRREMOTE          1
XINPUT_IR_REMOTE     IrRemoteDefaults = {0,0};
XID_REPORT_TYPE_INFO IrRemoteReportInfoList[1] = {sizeof(XINPUT_IR_REMOTE),(PVOID)&IrRemoteDefaults};
XINPUT_POLLING_PARAMETERS IrRemoteDefaultPolling = {TRUE,FALSE,0,16,0,0};  //AutoPoll On, Interrupt-OUT,
                                                                           //16 ms Input Poll Interval

//----------------------------------------------
// Type information list
//----------------------------------------------
XID_TYPE_INFORMATION XID_TypeInformationList[XID_DEVTYPE_COUNT] = {
    // Game Controllers
    {XID_DEFAULT_MAX_GAMEPAD, XID_DEVSUBTYPE_MAX_GAME, XID_INPUT_REPORT_ID_MAX_GAME, XID_OUTPUT_REPORT_ID_MAX_GAME,
    GameInputReportInfoList, GameOutputReportInfoList, &GameDefaultPolling, XID_ProcessGamepadData, 0}
    //Keyboard
    ,{XID_DEFAULT_MAX_KEYBOARD, XID_DEVSUBTYPE_MAX_KEYBOARD, XID_INPUT_REPORT_ID_MAX_KEYBOARD, XID_OUTPUT_REPORT_ID_MAX_KEYBOARD,
    KeyboardReportInfoList, KeyboardLEDReportInfoList, &KeyboardDefaultPolling, XID_ProcessNewKeyboardData,
    XID_BSF_NO_CAPABILITIES|XID_BSF_NO_OUTPUT_HEADER}
    // IR Remote
    ,{XID_DEFAULT_MAX_IRREMOTE, XID_DEVSUBTYPE_MAX_IRREMOTE, XID_INPUT_REPORT_ID_MAX_IRREMOTE, XID_OUTPUT_REPORT_ID_MAX_IRREMOTE,
    IrRemoteReportInfoList, NULL, &IrRemoteDefaultPolling, XID_ProcessIRRemoteData, 0}
    };