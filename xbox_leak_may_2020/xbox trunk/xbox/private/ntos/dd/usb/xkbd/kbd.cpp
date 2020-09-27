/*++

Copyright (c) 2000 Microsoft Corporation


Module Name:

    kbd.cpp

Abstract:

    High-level Xbox keyboard support


Environment:

    Designed for XBOX.

Notes:

    
    
Revision History:

    12-20-00 created by Mitchell Dernis (mitchd)

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

//Nothing in here matters, if we don't have keyboard support enabled.
#include <kbd.h>

//
//  Setup the debug information for this file (see ..\inc\debug.h)
//
#define MODULE_POOL_TAG          '_DBK'
#include <debug.h>
DEFINE_USB_DEBUG_FUNCTIONS("KBD");

//
//  Pull in public usb headers
//
#include <usb.h>
//
//  Pull in xid headers
//
#include "xid.h"
#include <xboxverp.h>

#pragma data_seg(".XBLD$V")
#if DBG
USHORT XKbdBuildNumberD[8] = { 'KX', 'DB', 'D', 0,
    VER_PRODUCTVERSION | 0x8000
#else
USHORT XKbdBuildNumber[8] = { 'KX', 'DB', 0, 0,
    VER_PRODUCTVERSION
#endif
};
#pragma data_seg(".XPP$Data")


//--------------------------------------------------------------------
//  Local Structure Definitions
//--------------------------------------------------------------------
typedef struct _XID_KEYBOARD_INSTANCE
{
    HANDLE                  hDevice;
    XINPUT_KEYBOARD         LastPacket;
    XINPUT_KEYBOARD_LEDS    KeyboardLeds;
    XINPUT_DEBUG_KEYSTROKE  *KeyQueue;
    ULONG                   QueueReadPos;
    ULONG                   QueueWritePos;
    XINPUT_DEBUG_KEYSTROKE  RepeatKeystroke;
    DWORD                   LastRepeatTick;
} XID_KEYBOARD_INSTANCE, *PXID_KEYBOARD_INSTANCE;

typedef struct _XID_KEYBOARD_STATE
{
#ifdef  SINGLE_KEYBOARD_ONLY
    XID_KEYBOARD_INSTANCE       Keyboards[1];
#else
    XID_KEYBOARD_INSTANCE       Keyboards[5];
#endif
    XINPUT_DEBUG_KEYQUEUE_PARAMETERS  QueueParameters;
} XID_KEYBOARD_STATE, *PXID_KEYBOARD_STATE;

XID_KEYBOARD_STATE XID_KeyboardState = {0};

//--------------------------------------------------------------------
// Keyboard Services Service Table for XID
//--------------------------------------------------------------------
void XID_KeyboardOpen(HANDLE hDevice);
void XID_KeyboardClose(HANDLE hDevice);
void XID_KeyboardRemove(HANDLE hDevice);
void XID_KeyboardNewData(HANDLE hDevice, XINPUT_KEYBOARD *pPacket);

XID_KEYBOARD_SERVICES XID_KeyboardServices =
{
    XID_KeyboardOpen,
    XID_KeyboardClose,
    XID_KeyboardRemove,
    XID_KeyboardNewData
};

//--------------------------------------------------------------------
// Local Utility Functions
//--------------------------------------------------------------------
BOOL XID_KeyboardInitQueue(int iInstance, DWORD dwQueueLength);
void XID_KeyboardReset(int iInstance);
void XID_KeyboardUpdate(int iInstance, XINPUT_KEYBOARD *pPacket);
XINPUT_DEBUG_KEYSTROKE *XID_KeyboardQueueGetWritePos(int iQueueIndex);
XINPUT_DEBUG_KEYSTROKE *XID_KeyboardQueueIncrementWritePos(int iQueueIndex);
VOID XID_KeyboardQueueHidKeystroke(UCHAR HidUsage, UCHAR Flags, PXINPUT_DEBUG_KEYSTROKE pKeystroke);
/*--------------------------------------------------------------------
 *  HID to VK_ convertion table.
 *----------------------------------------------
 *  HID_USAGE_INDEX_KEYBOARD_NOEVENT to 
 *  HID_USAGE_INDEX_KEYBOARD_UNDEFINED are not
 *  real keys, but so don't need convertion.
 *----------------------------------------------
 *  HID_USAGE_INDEX_KEYBOARD_aA to
 *  HID_USAGE_INDEX_KEYBOARD_zZ are
 *  not converted by lookup but rather by
 *  by adding (VK_A-HID_USAGE_INDEX_KEYBOARD_aA)
 *  to the HID usage.
 *
 *  NOTE: localization, may require a couple of
 *  substitutions in this range.
 *----------------------------------------------
 *  HID_USAGE_INDEX_KEYBOARD_ONE to 
 *  HID_USAGE_INDEX_KEYBOARD_NINE are
 *  not converted by lookeup but rather by
 *  adding (VK_1-HID_USAGE_INDEX_KEYBOARD_ONE)
 *  to the HID usage.
 *  (0 is last in HID, first in ASCII, so
 *  is just part of the lookup table)
 *----------------------------------------------
 **/

UCHAR HidToVK_Table[] =
{
/*HID_USAGE_INDEX_KEYBOARD_ZERO*/           '0',
/*HID_USAGE_INDEX_KEYBOARD_RETURN*/         VK_RETURN,
/*HID_USAGE_INDEX_KEYBOARD_ESCAPE*/         VK_ESCAPE,
/*HID_USAGE_INDEX_KEYBOARD_BACKSPACE*/      VK_BACK,
/*HID_USAGE_INDEX_KEYBOARD_TAB*/            VK_TAB,
/*HID_USAGE_INDEX_KEYBOARD_SPACEBAR*/       VK_SPACE,
/*HID_USAGE_INDEX_KEYBOARD_MINUS*/          VK_OEM_MINUS,
/*HID_USAGE_INDEX_KEYBOARD_EQUALS*/         VK_OEM_PLUS,
/*HID_USAGE_INDEX_KEYBOARD_OPEN_BRACE*/     VK_OEM_4,
/*HID_USAGE_INDEX_KEYBOARD_CLOSE_BRACE*/    VK_OEM_6,
/*HID_USAGE_INDEX_KEYBOARD_BACKSLASH*/      VK_OEM_5,
/*HID_USAGE_INDEX_KEYBOARD_NON_US_TILDE*/   VK_OEM_3,
/*HID_USAGE_INDEX_KEYBOARD_COLON*/          VK_OEM_1,
/*HID_USAGE_INDEX_KEYBOARD_QUOTE*/          VK_OEM_7,
/*HID_USAGE_INDEX_KEYBOARD_TILDE*/          VK_OEM_3,
/*HID_USAGE_INDEX_KEYBOARD_COMMA*/          VK_OEM_COMMA,
/*HID_USAGE_INDEX_KEYBOARD_PERIOD*/         VK_OEM_PERIOD,
/*HID_USAGE_INDEX_KEYBOARD_QUESTION*/       VK_OEM_2,
/*HID_USAGE_INDEX_KEYBOARD_CAPS_LOCK*/      VK_CAPITAL,
/*HID_USAGE_INDEX_KEYBOARD_F1*/             VK_F1,
/*HID_USAGE_INDEX_KEYBOARD_F2*/             VK_F2,
/*HID_USAGE_INDEX_KEYBOARD_F3*/             VK_F3,
/*HID_USAGE_INDEX_KEYBOARD_F4*/             VK_F4,
/*HID_USAGE_INDEX_KEYBOARD_F5*/             VK_F5,
/*HID_USAGE_INDEX_KEYBOARD_F6*/             VK_F6,
/*HID_USAGE_INDEX_KEYBOARD_F7*/             VK_F7,
/*HID_USAGE_INDEX_KEYBOARD_F8*/             VK_F8,
/*HID_USAGE_INDEX_KEYBOARD_F9*/             VK_F9,
/*HID_USAGE_INDEX_KEYBOARD_F10*/            VK_F10,
/*HID_USAGE_INDEX_KEYBOARD_F11*/            VK_F11,
/*HID_USAGE_INDEX_KEYBOARD_F12*/            VK_F12,
/*HID_USAGE_INDEX_KEYBOARD_PRINT_SCREEN*/   VK_PRINT,
/*HID_USAGE_INDEX_KEYBOARD_SCROLL_LOCK*/    VK_SCROLL,
/*HID_USAGE_INDEX_KEYBOARD_PAUSE*/          VK_PAUSE,
/*HID_USAGE_INDEX_KEYBOARD_INSERT*/         VK_INSERT,
/*HID_USAGE_INDEX_KEYBOARD_HOME*/           VK_HOME,
/*HID_USAGE_INDEX_KEYBOARD_PAGE_UP*/        VK_PRIOR,
/*HID_USAGE_INDEX_KEYBOARD_DELETE*/         VK_DELETE,
/*HID_USAGE_INDEX_KEYBOARD_END*/            VK_END,
/*HID_USAGE_INDEX_KEYBOARD_PAGE_DOWN*/      VK_NEXT,
/*HID_USAGE_INDEX_KEYBOARD_RIGHT_ARROW*/    VK_RIGHT,
/*HID_USAGE_INDEX_KEYBOARD_LEFT_ARROW*/     VK_LEFT,
/*HID_USAGE_INDEX_KEYBOARD_DOWN_ARROW*/     VK_DOWN,
/*HID_USAGE_INDEX_KEYBOARD_UP_ARROW*/       VK_UP,
/*HID_USAGE_INDEX_KEYPAD_NUM_LOCK*/         VK_NUMLOCK,
/*HID_USAGE_INDEX_KEYPAD_BACKSLASH*/        VK_DIVIDE,
/*HID_USAGE_INDEX_KEYPAD_ASTERICK*/         VK_MULTIPLY,
/*HID_USAGE_INDEX_KEYPAD_MINUS*/            VK_SUBTRACT,
/*HID_USAGE_INDEX_KEYPAD_PLUS*/             VK_ADD,
/*HID_USAGE_INDEX_KEYPAD_ENTER*/            VK_SEPARATOR,
/*HID_USAGE_INDEX_KEYPAD_ONE*/              VK_NUMPAD1,
/*HID_USAGE_INDEX_KEYPAD_TWO*/              VK_NUMPAD2,
/*HID_USAGE_INDEX_KEYPAD_THREE*/            VK_NUMPAD3,
/*HID_USAGE_INDEX_KEYPAD_FOUR*/             VK_NUMPAD4,
/*HID_USAGE_INDEX_KEYPAD_FIVE*/             VK_NUMPAD5,
/*HID_USAGE_INDEX_KEYPAD_SIX*/              VK_NUMPAD6,
/*HID_USAGE_INDEX_KEYPAD_SEVEN*/            VK_NUMPAD7,
/*HID_USAGE_INDEX_KEYPAD_EIGHT*/            VK_NUMPAD8,
/*HID_USAGE_INDEX_KEYPAD_NINE*/             VK_NUMPAD9,
/*HID_USAGE_INDEX_KEYPAD_ZERO*/             VK_NUMPAD0,
/*HID_USAGE_INDEX_KEYPAD_DECIMAL*/          VK_DECIMAL,
/*HID_USAGE_INDEX_KEYBOARD_NON_US_BACKSLASH*/ VK_OEM_5,
/*HID_USAGE_INDEX_KEYBOARD_APPLICATION*/    VK_APPS,
/*HID_USAGE_INDEX_KEYBOARD_POWER*/          VK_SLEEP,
/*HID_USAGE_INDEX_KEYPAD_EQUALS*/           VK_OEM_NEC_EQUAL,    
/*HID_USAGE_INDEX_KEYBOARD_F13*/            VK_F13,
/*HID_USAGE_INDEX_KEYBOARD_F14*/            VK_F14,     
/*HID_USAGE_INDEX_KEYBOARD_F15*/            VK_F15,    
/*HID_USAGE_INDEX_KEYBOARD_F16*/            VK_F16,    
/*HID_USAGE_INDEX_KEYBOARD_F17*/            VK_F17,    
/*HID_USAGE_INDEX_KEYBOARD_F18*/            VK_F18,    
/*HID_USAGE_INDEX_KEYBOARD_F19*/            VK_F19,    
/*HID_USAGE_INDEX_KEYBOARD_F20*/            VK_F20,    
/*HID_USAGE_INDEX_KEYBOARD_F21*/            VK_F21,    
/*HID_USAGE_INDEX_KEYBOARD_F22*/            VK_F22,    
/*HID_USAGE_INDEX_KEYBOARD_F23*/            VK_F23,    
/*HID_USAGE_INDEX_KEYBOARD_F24*/            VK_F24,    
/*HID_USAGE_INDEX_KEYBOARD_EXECUTE*/        VK_EXECUTE,
/*HID_USAGE_INDEX_KEYBOARD_HELP*/           VK_HELP,
/*HID_USAGE_INDEX_KEYBOARD_MENU*/           VK_MENU,
/*HID_USAGE_INDEX_KEYBOARD_SELECT*/         VK_SELECT,
/*HID_USAGE_INDEX_KEYBOARD_STOP*/           VK_BROWSER_STOP,
/*HID_USAGE_INDEX_KEYBOARD_AGAIN*/          0,
/*HID_USAGE_INDEX_KEYBOARD_UNDO*/           0,
/*HID_USAGE_INDEX_KEYBOARD_CUT*/            0,
/*HID_USAGE_INDEX_KEYBOARD_COPY*/           0,
/*HID_USAGE_INDEX_KEYBOARD_PASTE*/          0,
/*HID_USAGE_INDEX_KEYBOARD_FIND*/           VK_BROWSER_SEARCH,
/*HID_USAGE_INDEX_KEYBOARD_MUTE*/           VK_VOLUME_MUTE,
/*HID_USAGE_INDEX_KEYBOARD_VOLUME_UP*/      VK_VOLUME_UP,
/*HID_USAGE_INDEX_KEYBOARD_VOLUME_DOWN*/    VK_VOLUME_DOWN,
/*HID_USAGE_INDEX_KEYBOARD_LOCKING_CAPS*/   0,
/*HID_USAGE_INDEX_KEYBOARD_LOCKING_NUM*/    0,
/*HID_USAGE_INDEX_KEYBOARD_LOCKING_SCROLL*/ 0,
/*HID_USAGE_INDEX_KEYPAD_COMMA*/            VK_DECIMAL, //Brazillian keyboards have comma instead of period
/*HID_USAGE_INDEX_KEYPAD_EQUALS_AS400*/     VK_OEM_NEC_EQUAL,
/*HID_USAGE_INDEX_KEYBOARD_INTERNATIONAL1*/ 0,
/*HID_USAGE_INDEX_KEYBOARD_INTERNALIONAL2*/ 0,
/*HID_USAGE_INDEX_KEYBOARD_INTERNATIONAL3*/ 0,
/*HID_USAGE_INDEX_KEYBOARD_INTERNATIONAL4*/ 0,
/*HID_USAGE_INDEX_KEYBOARD_INTERNATIONAL5*/ 0,
/*HID_USAGE_INDEX_KEYBOARD_INTERNATIONAL6*/ 0,
/*HID_USAGE_INDEX_KEYBOARD_INTERNATIONAL7*/ 0,
/*HID_USAGE_INDEX_KEYBOARD_INTERNATIONAL8*/ 0,
/*HID_USAGE_INDEX_KEYBOARD_INTERNATIONAL9*/ 0,
/*HID_USAGE_INDEX_KEYBOARD_LANG1*/          0,
/*HID_USAGE_INDEX_KEYBOARD_LANG2*/          VK_HANJA,
/*HID_USAGE_INDEX_KEYBOARD_LANG3*/          VK_KANA,
/*HID_USAGE_INDEX_KEYBOARD_LANG4*/          0,
/*HID_USAGE_INDEX_KEYBOARD_LANG5*/          0,
/*HID_USAGE_INDEX_KEYBOARD_LANG6*/          0,
/*HID_USAGE_INDEX_KEYBOARD_LANG7*/          0,
/*HID_USAGE_INDEX_KEYBOARD_LANG8*/          0,
/*HID_USAGE_INDEX_KEYBOARD_LANG9*/          0
};

UCHAR HidModifierToVK_Table[] =
{
/*HID_USAGE_INDEX_KEYBOARD_LCTRL*/  VK_LCONTROL,
/*HID_USAGE_INDEX_KEYBOARD_LSHFT*/  VK_LSHIFT,
/*HID_USAGE_INDEX_KEYBOARD_LALT*/   VK_LMENU,
/*HID_USAGE_INDEX_KEYBOARD_LGUI*/   VK_LWIN,
/*HID_USAGE_INDEX_KEYBOARD_RCTRL*/  VK_RCONTROL,
/*HID_USAGE_INDEX_KEYBOARD_RSHFT*/  VK_RSHIFT,
/*HID_USAGE_INDEX_KEYBOARD_RALT*/   VK_RMENU,
/*HID_USAGE_INDEX_KEYBOARD_RGUI*/   VK_RWIN
};

UCHAR HidSymToAscii_Table[] =
{
/*HID_USAGE_INDEX_KEYBOARD_ZERO*/           '0',
/*HID_USAGE_INDEX_KEYBOARD_RETURN*/         '\n',
/*HID_USAGE_INDEX_KEYBOARD_ESCAPE*/         27,
/*HID_USAGE_INDEX_KEYBOARD_BACKSPACE*/      '\b',
/*HID_USAGE_INDEX_KEYBOARD_TAB*/            '\t',
/*HID_USAGE_INDEX_KEYBOARD_SPACEBAR*/       ' ',
/*HID_USAGE_INDEX_KEYBOARD_MINUS*/          '-',
/*HID_USAGE_INDEX_KEYBOARD_EQUALS*/         '=',
/*HID_USAGE_INDEX_KEYBOARD_OPEN_BRACE*/     '[',
/*HID_USAGE_INDEX_KEYBOARD_CLOSE_BRACE*/    ']',
/*HID_USAGE_INDEX_KEYBOARD_BACKSLASH*/      '\\',
/*HID_USAGE_INDEX_KEYBOARD_NON_US_TILDE*/   '~',
/*HID_USAGE_INDEX_KEYBOARD_COLON*/          ';',
/*HID_USAGE_INDEX_KEYBOARD_QUOTE*/          '\'',
/*HID_USAGE_INDEX_KEYBOARD_TILDE*/          '`',
/*HID_USAGE_INDEX_KEYBOARD_COMMA*/          ',',
/*HID_USAGE_INDEX_KEYBOARD_PERIOD*/         '.',
/*HID_USAGE_INDEX_KEYBOARD_QUESTION*/       '/'
};

UCHAR HidSymToAsciiShift_Table[] =
{
/*HID_USAGE_INDEX_KEYBOARD_ONE*/            '!',
/*HID_USAGE_INDEX_KEYBOARD_TWO*/            '@',
/*HID_USAGE_INDEX_KEYBOARD_THREE*/          '#',
/*HID_USAGE_INDEX_KEYBOARD_FOUR*/           '$',
/*HID_USAGE_INDEX_KEYBOARD_FIVE*/           '%',
/*HID_USAGE_INDEX_KEYBOARD_SIX*/            '^',
/*HID_USAGE_INDEX_KEYBOARD_SEVEN*/          '&',
/*HID_USAGE_INDEX_KEYBOARD_EIGHT*/          '*',
/*HID_USAGE_INDEX_KEYBOARD_NINE*/           '(',
/*HID_USAGE_INDEX_KEYBOARD_ZERO*/           ')',
/*HID_USAGE_INDEX_KEYBOARD_RETURN*/         '\n',
/*HID_USAGE_INDEX_KEYBOARD_ESCAPE*/         27,
/*HID_USAGE_INDEX_KEYBOARD_BACKSPACE*/      '\b',
/*HID_USAGE_INDEX_KEYBOARD_TAB*/            '\t',
/*HID_USAGE_INDEX_KEYBOARD_SPACEBAR*/       ' ',
/*HID_USAGE_INDEX_KEYBOARD_MINUS*/          '_',
/*HID_USAGE_INDEX_KEYBOARD_EQUALS*/         '+',
/*HID_USAGE_INDEX_KEYBOARD_OPEN_BRACE*/     '{',
/*HID_USAGE_INDEX_KEYBOARD_CLOSE_BRACE*/    '}',
/*HID_USAGE_INDEX_KEYBOARD_BACKSLASH*/      '|',
/*HID_USAGE_INDEX_KEYBOARD_NON_US_TILDE*/   '~',
/*HID_USAGE_INDEX_KEYBOARD_COLON*/          ':',
/*HID_USAGE_INDEX_KEYBOARD_QUOTE*/          '\"',
/*HID_USAGE_INDEX_KEYBOARD_TILDE*/          '~',
/*HID_USAGE_INDEX_KEYBOARD_COMMA*/          '<',
/*HID_USAGE_INDEX_KEYBOARD_PERIOD*/         '>',
/*HID_USAGE_INDEX_KEYBOARD_QUESTION*/       '?'
};

UCHAR HidNumPadToAscii[] =
{
/*HID_USAGE_INDEX_KEYPAD_BACKSLASH*/        '/',
/*HID_USAGE_INDEX_KEYPAD_ASTERICK*/         '*',
/*HID_USAGE_INDEX_KEYPAD_MINUS*/            '-',
/*HID_USAGE_INDEX_KEYPAD_PLUS*/             '+',
/*HID_USAGE_INDEX_KEYPAD_ENTER*/            '\n',
/*HID_USAGE_INDEX_KEYPAD_ONE*/              0,
/*HID_USAGE_INDEX_KEYPAD_TWO*/              0,
/*HID_USAGE_INDEX_KEYPAD_THREE*/            0,
/*HID_USAGE_INDEX_KEYPAD_FOUR*/             0,
/*HID_USAGE_INDEX_KEYPAD_FIVE*/             0,
/*HID_USAGE_INDEX_KEYPAD_SIX*/              0,
/*HID_USAGE_INDEX_KEYPAD_SEVEN*/            0,
/*HID_USAGE_INDEX_KEYPAD_EIGHT*/            0,
/*HID_USAGE_INDEX_KEYPAD_NINE*/             0,
/*HID_USAGE_INDEX_KEYPAD_ZERO*/             0,
/*HID_USAGE_INDEX_KEYPAD_DECIMAL*/          127
};


UCHAR HidNumPadShiftToAscii[] =
{
/*HID_USAGE_INDEX_KEYPAD_BACKSLASH*/        '/',
/*HID_USAGE_INDEX_KEYPAD_ASTERICK*/         '*',
/*HID_USAGE_INDEX_KEYPAD_MINUS*/            '-',
/*HID_USAGE_INDEX_KEYPAD_PLUS*/             '+',
/*HID_USAGE_INDEX_KEYPAD_ENTER*/            '\n',
/*HID_USAGE_INDEX_KEYPAD_ONE*/              '1',
/*HID_USAGE_INDEX_KEYPAD_TWO*/              '2',
/*HID_USAGE_INDEX_KEYPAD_THREE*/            '3',
/*HID_USAGE_INDEX_KEYPAD_FOUR*/             '4',
/*HID_USAGE_INDEX_KEYPAD_FIVE*/             '5',
/*HID_USAGE_INDEX_KEYPAD_SIX*/              '6',
/*HID_USAGE_INDEX_KEYPAD_SEVEN*/            '7',
/*HID_USAGE_INDEX_KEYPAD_EIGHT*/            '8',
/*HID_USAGE_INDEX_KEYPAD_NINE*/             '9',
/*HID_USAGE_INDEX_KEYPAD_ZERO*/             '0',
/*HID_USAGE_INDEX_KEYPAD_DECIMAL*/          '.'
};

XBOXAPI
DWORD
WINAPI
XInputDebugInitKeyboardQueue(
    IN PXINPUT_DEBUG_KEYQUEUE_PARAMETERS pParameters OPTIONAL
    )
{
    BOOL fSuccess = TRUE;
    //
    // Store the parameters
    //
    if(pParameters)
    {
        RtlCopyMemory(&XID_KeyboardState.QueueParameters, pParameters, sizeof(XINPUT_DEBUG_KEYQUEUE_PARAMETERS));
    } else
    {
        XID_KeyboardState.QueueParameters.dwFlags = XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN |
                                                    XINPUT_DEBUG_KEYQUEUE_FLAG_KEYREPEAT;
        XID_KeyboardState.QueueParameters.dwQueueSize = 40;
        XID_KeyboardState.QueueParameters.dwRepeatDelay = 400;
        XID_KeyboardState.QueueParameters.dwRepeatInterval = 100;
    }

    //
    //  Allocate Queue(s)
    //
    DWORD dwQueueLength = XID_KeyboardState.QueueParameters.dwQueueSize * sizeof(XINPUT_DEBUG_KEYSTROKE);
#ifdef  SINGLE_KEYBOARD_ONLY
    fSuccess = XID_KeyboardInitQueue(0, dwQueueLength);
#else
    if(XID_KeyboardState.QueueParameters.dwFlags & XINPUT_DEBUG_KEYQUEUE_FLAG_ONE_QUEUE)
    {
        fSuccess = XID_KeyboardInitQueue(0, dwQueueLength);
    } else
    {
        int i;
        for(i=0; i<4;i++)
        {
            fSuccess = XID_KeyboardInitQueue(i,dwQueueLength);
            if(!fSuccess)
            {
                while(--i >= 0)
                {
                    ExFreePool(XID_KeyboardState.Keyboards[i].KeyQueue);
                    XID_KeyboardState.Keyboards[i].KeyQueue = NULL;
                }
                return ERROR_OUTOFMEMORY;
            }
        }
    }
#endif

    if(fSuccess)
    {
        //
        //  Setup the Hook
        //
        XID_pKeyboardServices = &XID_KeyboardServices;
        return ERROR_SUCCESS;
    }
    return ERROR_OUTOFMEMORY;
}

XBOXAPI
DWORD
WINAPI
XInputDebugGetKeystroke(
#ifndef  SINGLE_KEYBOARD_ONLY
    IN HANDLE hDevice,
#endif // SINGLE_KEYBOARD_ONLY
    OUT PXINPUT_DEBUG_KEYSTROKE pKeystroke
    )
{

#ifdef  SINGLE_KEYBOARD_ONLY
    const int iQueueIndex=0;
#else
    int iQueueIndex = 0;
#endif
 
    DWORD dwError = ERROR_SUCCESS;
    DWORD dwReadPos;
    KIRQL oldIrql;
    DWORD dwTickCount = 0;

    oldIrql = KeRaiseIrqlToDpcLevel();

#ifndef  SINGLE_KEYBOARD_ONLY
    if(hDevice == NULL)
    {
        if(!(XID_KeyboardState.QueueParameters.dwFlags&XINPUT_DEBUG_KEYQUEUE_FLAG_ONE_QUEUE))
        {
            KeLowerIrql(oldIrql);
            RIP("XInputDebugGetKeystroke: hDevice may only be NULL, if XINPUT_DEBUG_KEYQUEUE_FLAG_ONE_QUEUE was set.");
            oldIrql = KeRaiseIrqlToDpcLevel();
        }
    } else
    {
        for(iQueueIndex=0; iQueueIndex<4; iQueueIndex++)
        {
            if(hDevice == XID_KeyboardState.Keyboards[iQueueIndex].hDevice)
            {
                break;
            }
        }
    }
    if(4==iQueueIndex)
    {
       KeLowerIrql(oldIrql);
       RIP("XInputDebugGetKeystroke: hDevice is not valid.");
       return ERROR_INVALID_HANDLE;
    }
#endif //SINGLE_KEYBOARD_ONLY    

    dwTickCount = GetTickCount();
    if(dwTickCount == 0) dwTickCount = 1;
    dwReadPos = XID_KeyboardState.Keyboards[iQueueIndex].QueueReadPos;
    if( dwReadPos == XID_KeyboardState.Keyboards[iQueueIndex].QueueWritePos)
    {
        //
        //  Handle Repeat Count
        //
        DWORD dwTickCountDiff = dwTickCount - XID_KeyboardState.Keyboards[iQueueIndex].LastRepeatTick;
        if(dwTickCountDiff > 2000) dwTickCountDiff = 0;
        if(
            (XID_KeyboardState.QueueParameters.dwFlags&XINPUT_DEBUG_KEYQUEUE_FLAG_KEYREPEAT) &&
            XID_KeyboardState.Keyboards[iQueueIndex].LastRepeatTick && 
            (dwTickCountDiff > XID_KeyboardState.QueueParameters.dwRepeatInterval)
        )
        {
            RtlCopyMemory(
                pKeystroke,
                &XID_KeyboardState.Keyboards[iQueueIndex].RepeatKeystroke,
                sizeof(XINPUT_DEBUG_KEYSTROKE)
                );
            XID_KeyboardState.Keyboards[iQueueIndex].LastRepeatTick = dwTickCount;
        } else
        {
            dwError = ERROR_HANDLE_EOF;
        }
        goto ExitXInputGetKeyStroke;
    }
    
    RtlCopyMemory(
        pKeystroke,
        &XID_KeyboardState.Keyboards[iQueueIndex].KeyQueue[dwReadPos],
        sizeof(XINPUT_DEBUG_KEYSTROKE)
        );

    if(
        (XID_KeyboardState.QueueParameters.dwFlags&XINPUT_DEBUG_KEYQUEUE_FLAG_KEYREPEAT)&&
        !(pKeystroke->Flags&XINPUT_DEBUG_KEYSTROKE_FLAG_KEYUP)
    )
    {
        RtlCopyMemory(
            &XID_KeyboardState.Keyboards[iQueueIndex].RepeatKeystroke,
            &XID_KeyboardState.Keyboards[iQueueIndex].KeyQueue[dwReadPos],
            sizeof(XINPUT_DEBUG_KEYSTROKE)
            );
        XID_KeyboardState.Keyboards[iQueueIndex].RepeatKeystroke.Flags |= XINPUT_DEBUG_KEYSTROKE_FLAG_REPEAT;
        dwTickCount += XID_KeyboardState.QueueParameters.dwRepeatDelay - XID_KeyboardState.QueueParameters.dwRepeatInterval;
        XID_KeyboardState.Keyboards[iQueueIndex].LastRepeatTick = dwTickCount;
    }

    XID_KeyboardState.Keyboards[iQueueIndex].QueueReadPos = 
        (dwReadPos + 1)%XID_KeyboardState.QueueParameters.dwQueueSize;

ExitXInputGetKeyStroke:
    KeLowerIrql(oldIrql);
    return dwError;
}

void XID_KeyboardOpen(HANDLE hDevice)
{
#ifdef  SINGLE_KEYBOARD_ONLY
    if(NULL == XID_KeyboardState.Keyboards[0].hDevice)
    {
      XID_KeyboardState.Keyboards[0].hDevice = hDevice;
      XID_KeyboardReset(0);
    }
#else  // !SINGLE_KEYBOARD_ONLY
    //
    //  Find an empty keyboard instance
    //
    int i;
    for(i=0; i<4; i++)
    {
        if(NULL == XID_KeyboardState.Keyboards[i].hDevice)
        {
            XID_KeyboardState.Keyboards[i].hDevice = hDevice;
            XID_KeyboardReset(i);
            break;
        }
    }
#endif //SINGLE_KEYBOARD_ONLY
}

void XID_KeyboardClose(HANDLE hDevice)
{
#ifdef  SINGLE_KEYBOARD_ONLY
    if(hDevice == XID_KeyboardState.Keyboards[0].hDevice)
    {
        XID_KeyboardState.Keyboards[0].hDevice = NULL;
    }
#else
    int i;
    for(i=0; i<4; i++)
    {
        if(hDevice == XID_KeyboardState.Keyboards[i].hDevice)
        {
            XID_KeyboardState.Keyboards[i].hDevice = NULL;
            XID_KeyboardReset(i);
            break;
        }
    }
#endif //SINGLE_KEYBOARD_ONLY
}

void XID_KeyboardRemove(HANDLE hDevice)
{
    XID_KeyboardClose(hDevice);
}

void XID_KeyboardNewData(HANDLE hDevice, XINPUT_KEYBOARD *pPacket)
{
#ifdef  SINGLE_KEYBOARD_ONLY
    if(hDevice == XID_KeyboardState.Keyboards[0].hDevice)
    {
        XID_KeyboardUpdate(0, pPacket);
    }
#else
    int  i;
    for(i=0; i<4; i++)
    {
        if(hDevice == XID_KeyboardState.Keyboards[i].hDevice)
        {
            XID_KeyboardUpdate(i, pPacket);
            break;
        }
    }
#endif //SINGLE_KEYBOARD_ONLY
}

BOOL XID_KeyboardInitQueue(int iInstance, DWORD dwQueueLength)
{
    XID_KeyboardState.Keyboards[iInstance].KeyQueue = (PXINPUT_DEBUG_KEYSTROKE)ExAllocatePoolWithTag(
                                                                                    dwQueueLength,
                                                                                    'drbk'
                                                                                    );
    if(XID_KeyboardState.Keyboards[iInstance].KeyQueue)
    {
        XID_KeyboardState.Keyboards[iInstance].QueueReadPos = 0;
        XID_KeyboardState.Keyboards[iInstance].QueueWritePos = 0;
        XID_KeyboardState.Keyboards[iInstance].LastRepeatTick =0;
        return TRUE;
    } else
    {
        return FALSE;
    }
}

void XID_KeyboardReset(int iInstance)
{
    XID_KeyboardState.Keyboards[iInstance].KeyboardLeds.LedStates = 0;
    RtlZeroMemory(&XID_KeyboardState.Keyboards[iInstance].LastPacket,sizeof(XINPUT_KEYBOARD));
}


void XID_KeyboardUpdate(int iInstance, XINPUT_KEYBOARD *pPacket)
{
   
    BYTE oldModifiers;
    BYTE newModifiers;
    XINPUT_KEYBOARD DownKeys;
    XINPUT_KEYBOARD UpKeys;
    int index, iNewKeyIndex, iOldKeyIndex;
    UCHAR keyStrokeFlags;
    //
    //  Assume key queue is per keyboard, until we know differently.
    //
    int iQueueIndex = iInstance;


    //**
    //**    Figure out which keys were pressed and released.
    //**    A good deal of the following code is for the
    //**    special case of a mixing all keyboards into a 
    //**    single queue.
    //**
    
    //
    //  Calculate the old and new state of the modifiers across all combined queues
    //
#ifndef  SINGLE_KEYBOARD_ONLY
    if(XID_KeyboardState.QueueParameters.dwFlags&XINPUT_DEBUG_KEYQUEUE_FLAG_ONE_QUEUE)
    {
        iQueueIndex = 0;    //Change the queue to 0 since this is one queue
        oldModifiers = 0;
        newModifiers = 0;
        for(index = 0; index < 4; index++)
        {
            if(XID_KeyboardState.Keyboards[index].hDevice)
            {
                oldModifiers |= XID_KeyboardState.Keyboards[index].LastPacket.Modifiers;
                if((iInstance != index))
                {
                    newModifiers |= XID_KeyboardState.Keyboards[index].LastPacket.Modifiers;
                } else
                {
                    newModifiers |= pPacket->Modifiers;
                }
            }
        }
    } else
#endif //SINGLE_KEYBOARD_ONLY
    //
    //  Queues are not combined so do just this keyboard
    //
    {
        oldModifiers = XID_KeyboardState.Keyboards[iInstance].LastPacket.Modifiers;
        newModifiers = pPacket->Modifiers;
    }
    //
    //  Calculate the modifiers pressed and released
    //
    UpKeys.Modifiers = oldModifiers & ~newModifiers;
    DownKeys.Modifiers = ~oldModifiers & newModifiers;
    //
    //  Shut off key repeat if modifiers changed
    //
    if(oldModifiers != newModifiers)
    {
        XID_KeyboardState.Keyboards[iQueueIndex].LastRepeatTick = 0;
    }
    
    //
    //  Calculate keys that went down (for this keyboard only)
    //
    for(iNewKeyIndex = 0; iNewKeyIndex<6; iNewKeyIndex++)
    {
        BOOL fAddKey = FALSE;
        UCHAR NewKey = pPacket->Keys[iNewKeyIndex];
        if(NewKey > HID_USAGE_INDEX_KEYBOARD_UNDEFINED)
        {
            fAddKey = TRUE;
            for(iOldKeyIndex = 0; iOldKeyIndex<6; iOldKeyIndex++)
            {
                if(NewKey == XID_KeyboardState.Keyboards[iInstance].LastPacket.Keys[iOldKeyIndex])
                {
                    fAddKey = FALSE;
                    break;
                }
            }
        }
        if(fAddKey)
        {
            XID_KeyboardState.Keyboards[iQueueIndex].LastRepeatTick = 0;
            DownKeys.Keys[iNewKeyIndex] = NewKey;
        } else
        {
            DownKeys.Keys[iNewKeyIndex] = 0;
        }
    }

    //
    //  Calculate keys that went up (for this keyboard only)
    //
    for(iOldKeyIndex = 0; iOldKeyIndex<6; iOldKeyIndex++)
    {
        UCHAR OldKey = XID_KeyboardState.Keyboards[iInstance].LastPacket.Keys[iOldKeyIndex];
        BOOL fAddKey = FALSE;
        if(OldKey > HID_USAGE_INDEX_KEYBOARD_UNDEFINED)
        {
            fAddKey = TRUE;
            for(iNewKeyIndex = 0; iNewKeyIndex<6; iNewKeyIndex++)
            {
                if(OldKey == pPacket->Keys[iNewKeyIndex])
                {
                    fAddKey = FALSE;
                    break;
                }
            }
        }
        if(fAddKey)
        {
            XID_KeyboardState.Keyboards[iQueueIndex].LastRepeatTick = 0;
            UpKeys.Keys[iOldKeyIndex] = OldKey;
        } else
        {
            UpKeys.Keys[iOldKeyIndex] = 0;
        }
    }

    //
    //  Now check up and down keys against other keyboards
    //  (if single queue).  Basically, a key cannot have gone up or down
    //  if it is down on another keyboard.
    //
#ifndef  SINGLE_KEYBOARD_ONLY
    if(XID_KeyboardState.QueueParameters.dwFlags&XINPUT_DEBUG_KEYQUEUE_FLAG_ONE_QUEUE)
    {
        for(index = 0; index < 4; index++)
        {
            if(XID_KeyboardState.Keyboards[index].hDevice && (iInstance != index))
            {   
                for(iNewKeyIndex = 0; iNewKeyIndex<6; iNewKeyIndex++)
                {
                    for(iOldKeyIndex = 0; iOldKeyIndex<6; iOldKeyIndex++)
                    {
                        UCHAR OldKey = XID_KeyboardState.Keyboards[index].LastPacket.Keys[iOldKeyIndex];
                        if(UpKeys.Keys[iNewKeyIndex]==OldKey)
                        {
                            UpKeys.Keys[iNewKeyIndex]=0;
                        }
                        if(DownKeys.Keys[iNewKeyIndex]==OldKey)
                        {
                            DownKeys.Keys[iNewKeyIndex]=0;
                        }
                    }
                }
            }
        }
    }
#endif //SINGLE_KEYBOARD_ONLY

    //
    //  Copy the packet into the last packet
    //
    RtlCopyMemory(&XID_KeyboardState.Keyboards[iInstance].LastPacket, pPacket, sizeof(XINPUT_KEYBOARD));

    //**
    //**  Calculate the new CapsLock, NumLock and ScrollLock states.
    //**    (The only left to do is the lock key checking.)
    
    BOOL fLedStateChange = FALSE;
    for(index =0; index < 6; index++)
    {
        switch(DownKeys.Keys[index])
        {
            case HID_USAGE_INDEX_KEYBOARD_SCROLL_LOCK:
                XID_KeyboardState.Keyboards[iQueueIndex].KeyboardLeds.LedStates ^= HID_KEYBOARDLED_MASK_SCROLL_LOCK;
                fLedStateChange = TRUE;
                break;
            case HID_USAGE_INDEX_KEYPAD_NUM_LOCK:
                XID_KeyboardState.Keyboards[iQueueIndex].KeyboardLeds.LedStates ^= HID_KEYBOARDLED_MASK_NUM_LOCK;
                fLedStateChange = TRUE;
                break;
            case HID_USAGE_INDEX_KEYBOARD_CAPS_LOCK:
                XID_KeyboardState.Keyboards[iQueueIndex].KeyboardLeds.LedStates ^= HID_KEYBOARDLED_MASK_CAPS_LOCK;
                fLedStateChange = TRUE;
                break;
        }
    }
    //
    //  Kick off LED update state machine.
    //
    if(fLedStateChange)
    {
        //TODO SOMETIME WAY IN FUTURE, NOT PLANNED ANYTIME BEFORE INITIAL XBOX LAUNCH.   
    }

    //**
    //**  Queue Events
    //**
    XINPUT_DEBUG_KEYSTROKE *pKeystrokeBuffer = XID_KeyboardQueueGetWritePos(iQueueIndex);
    // Set the Ctrl, Shift and Alt Flags
    keyStrokeFlags = (newModifiers | (newModifiers >> 4)) & 0x07;
    //Add in the Lock flags
    if(HID_KEYBOARDLED_MASK_SCROLL_LOCK&XID_KeyboardState.Keyboards[iQueueIndex].KeyboardLeds.LedStates) 
            keyStrokeFlags |= XINPUT_DEBUG_KEYSTROKE_FLAG_SCROLLLOCK;
    if(HID_KEYBOARDLED_MASK_NUM_LOCK&XID_KeyboardState.Keyboards[iQueueIndex].KeyboardLeds.LedStates) 
            keyStrokeFlags |= XINPUT_DEBUG_KEYSTROKE_FLAG_NUMLOCK;
    if(HID_KEYBOARDLED_MASK_CAPS_LOCK&XID_KeyboardState.Keyboards[iQueueIndex].KeyboardLeds.LedStates) 
            keyStrokeFlags |= XINPUT_DEBUG_KEYSTROKE_FLAG_CAPSLOCK;

    // Stuff Modifier events in queue
    if(!(XINPUT_DEBUG_KEYQUEUE_FLAG_ASCII_ONLY&XID_KeyboardState.QueueParameters.dwFlags))
    {
        int i,mask;
        BOOL fSet;
        for(i = 0, mask=1; i < 8; i++, mask<<=1)
        {
            fSet = FALSE;
            // UpKeys
            if(XINPUT_DEBUG_KEYQUEUE_FLAG_KEYUP&XID_KeyboardState.QueueParameters.dwFlags)
            {
                if(UpKeys.Modifiers&mask)
                {
                    pKeystrokeBuffer->Flags = XINPUT_DEBUG_KEYSTROKE_FLAG_KEYUP;
                    fSet = TRUE;
                }
            }
            // DownKeys
            if(XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN&XID_KeyboardState.QueueParameters.dwFlags)
            {
                if(DownKeys.Modifiers&mask)
                {
                    pKeystrokeBuffer->Flags = 0;
                    fSet = TRUE;
                }
            }
            if(fSet)
            {
                pKeystrokeBuffer->Flags |= keyStrokeFlags;
                pKeystrokeBuffer->VirtualKey = HidModifierToVK_Table[i];
                pKeystrokeBuffer->Ascii = 0;
                pKeystrokeBuffer = XID_KeyboardQueueIncrementWritePos(iQueueIndex);
            }
        }
    }

    //
    //  Place normal key up in queue
    //
    if(XINPUT_DEBUG_KEYQUEUE_FLAG_KEYUP&XID_KeyboardState.QueueParameters.dwFlags)
    {
        for(iNewKeyIndex=0; iNewKeyIndex<6; iNewKeyIndex++)
        {
            if(UpKeys.Keys[iNewKeyIndex] > HID_USAGE_INDEX_KEYBOARD_UNDEFINED)
            {
                XID_KeyboardQueueHidKeystroke(
                    UpKeys.Keys[iNewKeyIndex],
                    XINPUT_DEBUG_KEYSTROKE_FLAG_KEYUP|keyStrokeFlags,
                    pKeystrokeBuffer
                    );
                if( 
                    !(XINPUT_DEBUG_KEYQUEUE_FLAG_ASCII_ONLY&XID_KeyboardState.QueueParameters.dwFlags) ||
                    (pKeystrokeBuffer->Ascii)
                )
                {
                    pKeystrokeBuffer = XID_KeyboardQueueIncrementWritePos(iQueueIndex);
                }
            }
        }
    }
    //**
    //**  Place key down events in 
    //**
    if(XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN&XID_KeyboardState.QueueParameters.dwFlags)
    {
        for(iNewKeyIndex=0; iNewKeyIndex<6; iNewKeyIndex++)
        {
            if(DownKeys.Keys[iNewKeyIndex] > HID_USAGE_INDEX_KEYBOARD_UNDEFINED)
            {
                XID_KeyboardQueueHidKeystroke(
                    DownKeys.Keys[iNewKeyIndex],
                    keyStrokeFlags,
                    pKeystrokeBuffer
                    );
                if( 
                    !(XINPUT_DEBUG_KEYQUEUE_FLAG_ASCII_ONLY&XID_KeyboardState.QueueParameters.dwFlags) ||
                    (pKeystrokeBuffer->Ascii)
                )
                {
                    pKeystrokeBuffer = XID_KeyboardQueueIncrementWritePos(iQueueIndex);
                }
            }
        }
    }
}

XINPUT_DEBUG_KEYSTROKE *XID_KeyboardQueueGetWritePos(int iQueueIndex)
{
    return &XID_KeyboardState.Keyboards[iQueueIndex].KeyQueue[XID_KeyboardState.Keyboards[iQueueIndex].QueueWritePos];
}

XINPUT_DEBUG_KEYSTROKE *XID_KeyboardQueueIncrementWritePos(int iQueueIndex)
{
    //
    //  Get write position and increment it.
    //
    ULONG ulWritePos = XID_KeyboardState.Keyboards[iQueueIndex].QueueWritePos;
    ulWritePos = (ulWritePos+1)%XID_KeyboardState.QueueParameters.dwQueueSize;
    
    //
    //  Check for a full queue, and just don't increment.
    //
    if(XID_KeyboardState.Keyboards[iQueueIndex].QueueReadPos==ulWritePos)
    {
        ulWritePos = XID_KeyboardState.Keyboards[iQueueIndex].QueueWritePos;
    }else
    {
        XID_KeyboardState.Keyboards[iQueueIndex].QueueWritePos = ulWritePos;
    }
    
    //
    //  Return the next open keystroke position
    //
    return &XID_KeyboardState.Keyboards[iQueueIndex].KeyQueue[ulWritePos];    
}

VOID XID_KeyboardQueueHidKeystroke(UCHAR HidUsage, UCHAR Flags, PXINPUT_DEBUG_KEYSTROKE pKeystroke)
{
    UCHAR Shift = (XINPUT_DEBUG_KEYSTROKE_FLAG_SHIFT&Flags) ? 1 : 0;
    UCHAR CapsLock = (XINPUT_DEBUG_KEYSTROKE_FLAG_CAPSLOCK&Flags) ? 1 : 0;
    UCHAR NumLock = (XINPUT_DEBUG_KEYSTROKE_FLAG_NUMLOCK&Flags) ? 1 : 0;
    ASSERT(HidUsage > HID_USAGE_INDEX_KEYBOARD_UNDEFINED);
    pKeystroke->Flags = Flags;
    pKeystroke->Ascii = 0;
    //
    //  Check for A to Z range
    //  The VK_A and VK_Z are not defined, but are the same as 'A' to 'Z'.
    //  For ASCII we do the following:
    //      If alt is down the ASCII is 0 regardless of anything else.
    //      If ctrl is down and not shift, then it ^A to ^Z which is a
    //       contiguous ASCII range from 1 to 26.
    //      If CapsLock XOR Shift it is 'A' to 'Z', otherwise it is 'a' to 'z'.
    //       (Unfortunately there is no logical XOR operator)
    if( 
        (HidUsage >= HID_USAGE_INDEX_KEYBOARD_aA) &&
        (HidUsage <= HID_USAGE_INDEX_KEYBOARD_zZ)
    ){
        pKeystroke->VirtualKey = HidUsage + ('A' - HID_USAGE_INDEX_KEYBOARD_aA);
        //
        //  Figure out the ASCII
        //
         if(!(XINPUT_DEBUG_KEYSTROKE_FLAG_ALT & Flags))
        {
            if(XINPUT_DEBUG_KEYSTROKE_FLAG_CTRL & Flags)
            {
                if(!Shift)
                {
                    // Control
                    pKeystroke->Ascii = HidUsage - (HID_USAGE_INDEX_KEYBOARD_aA - 1);
                }
            }else
            {
                if(Shift^CapsLock)
                {
                    pKeystroke->Ascii = HidUsage + ('A' - HID_USAGE_INDEX_KEYBOARD_aA);
                } else
                {
                    pKeystroke->Ascii = HidUsage + ('a' - HID_USAGE_INDEX_KEYBOARD_aA);
                }
            }
        }
    }
    //
    //  check 1-9 range
    //
    else if(
        (HidUsage >= HID_USAGE_INDEX_KEYBOARD_ONE) &&
        (HidUsage <= HID_USAGE_INDEX_KEYBOARD_NINE)
    )
    {
        pKeystroke->VirtualKey = HidUsage + ('1' - HID_USAGE_INDEX_KEYBOARD_ONE);
        if(Shift)
        {
            pKeystroke->Ascii = HidSymToAsciiShift_Table[HidUsage-HID_USAGE_INDEX_KEYBOARD_ONE];
        } else
        {
            pKeystroke->Ascii = pKeystroke->VirtualKey;
        }
        
    } else
    //
    //  Use the lookup table to get the Virtual Key.
    //
    //  There are special ranges that require additional lookup
    //  for ASCII.  The numpad keys have wo tables, a (shifted xor numlock) table
    //  and two symbol lookup table, shifted only (caps lock doesn't effect it).
    //
    {
        pKeystroke->VirtualKey = HidToVK_Table[HidUsage-HID_USAGE_INDEX_KEYBOARD_ZERO];
        if(HidUsage <= HID_USAGE_INDEX_KEYBOARD_QUESTION)
        {
            if(Shift)
            {
                pKeystroke->Ascii = HidSymToAsciiShift_Table[HidUsage-HID_USAGE_INDEX_KEYBOARD_ONE];
            } else
            {
                pKeystroke->Ascii = HidSymToAscii_Table[HidUsage-HID_USAGE_INDEX_KEYBOARD_ZERO];
            }
        }  else if(
                (HidUsage >= HID_USAGE_INDEX_KEYPAD_BACKSLASH) &&
                (HidUsage <= HID_USAGE_INDEX_KEYPAD_DECIMAL)
            )
        {
            if(Shift^NumLock)
            {
                pKeystroke->Ascii = HidNumPadShiftToAscii[HidUsage-HID_USAGE_INDEX_KEYPAD_BACKSLASH];
            } else
            {
                pKeystroke->Ascii = HidNumPadToAscii[HidUsage-HID_USAGE_INDEX_KEYPAD_BACKSLASH];
            }
        }
    }
    return;
}
