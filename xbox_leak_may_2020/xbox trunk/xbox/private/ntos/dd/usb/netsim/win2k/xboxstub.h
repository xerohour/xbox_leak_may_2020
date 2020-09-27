
#pragma once

//
//  VOID
//  InitializeListHead(
//      PLIST_ENTRY ListHead
//      );
//

#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead))

//
//  BOOLEAN
//  IsListEmpty(
//      PLIST_ENTRY ListHead
//      );
//

#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))

//
//  PLIST_ENTRY
//  RemoveHeadList(
//      PLIST_ENTRY ListHead
//      );
//

#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {RemoveEntryList((ListHead)->Flink)}

//
//  VOID
//  RemoveEntryList(
//      PLIST_ENTRY Entry
//      );
//

#define RemoveEntryList(Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }

//
//  VOID
//  InsertTailList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

#define InsertTailList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
    }

//
// XInput types
//
#include <PSHPACK1.H>

typedef struct _XINPUT_GAMEPAD
{
    WORD    wButtons;
    BYTE    bAnalogButtons[8];
    SHORT   sThumbLX;
    SHORT   sThumbLY;
    SHORT   sThumbRX;
    SHORT   sThumbRY;
} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

#define XINPUT_GAMEPAD_DPAD_UP          0x00000001
#define XINPUT_GAMEPAD_DPAD_DOWN        0x00000002
#define XINPUT_GAMEPAD_DPAD_LEFT        0x00000004
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x00000008
#define XINPUT_GAMEPAD_START            0x00000010
#define XINPUT_GAMEPAD_BACK             0x00000020
#define XINPUT_GAMEPAD_LEFT_THUMB       0x00000040
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x00000080

#define XINPUT_GAMEPAD_A                0
#define XINPUT_GAMEPAD_B                1
#define XINPUT_GAMEPAD_X                2
#define XINPUT_GAMEPAD_Y                3
#define XINPUT_GAMEPAD_BLACK            4
#define XINPUT_GAMEPAD_WHITE            5
#define XINPUT_GAMEPAD_LEFT_TRIGGER     6
#define XINPUT_GAMEPAD_RIGHT_TRIGGER    7

typedef struct _XINPUT_RUMBLE
{
   WORD   wLeftMotorSpeed;
   WORD   wRightMotorSpeed;
} XINPUT_RUMBLE, *PXINPUT_RUMBLE;

typedef struct _XINPUT_STATE
{
    DWORD dwPacketNumber;
    union
    {
        XINPUT_GAMEPAD Gamepad;
    };
} XINPUT_STATE, *PXINPUT_STATE;

#define XINPUT_FEEDBACK_HEADER_INTERNAL_SIZE 58
typedef struct _XINPUT_FEEDBACK_HEADER
{
    DWORD           dwStatus;
    HANDLE OPTIONAL hEvent;
    BYTE            Reserved[XINPUT_FEEDBACK_HEADER_INTERNAL_SIZE];
} XINPUT_FEEDBACK_HEADER, *PXINPUT_FEEDBACK_HEADER;

typedef struct _XINPUT_FEEDBACK
{
    XINPUT_FEEDBACK_HEADER Header;
    union
    {
      XINPUT_RUMBLE              Rumble;
    };
} XINPUT_FEEDBACK, *PXINPUT_FEEDBACK;

typedef struct _XINPUT_CAPABILITIES
{
    BYTE    SubType;
    WORD    Reserved;
    union
    {
      XINPUT_GAMEPAD Gamepad;
    } In;
    union
    {
      XINPUT_RUMBLE Rumble;
    } Out;
} XINPUT_CAPABILITIES, *PXINPUT_CAPABILITIES;

#include <POPPACK.H>

#define XINPUT_DEVSUBTYPE_GC_GAMEPAD	 0x01
#define XINPUT_DEVSUBTYPE_GC_GAMEPAD_ALT 0x02
#define XINPUT_DEVSUBTYPE_GC_WHEEL		 0x10
#define XINPUT_DEVSUBTYPE_KBD_STANDARD   0

typedef struct _XINPUT_DEBUG_KEYSTROKE
{
    BYTE VirtualKey;
    CHAR Ascii;
    BYTE Flags;
} XINPUT_DEBUG_KEYSTROKE, *PXINPUT_DEBUG_KEYSTROKE;

#define XINPUT_DEBUG_KEYSTROKE_FLAG_CTRL       0x01
#define XINPUT_DEBUG_KEYSTROKE_FLAG_SHIFT      0x02
#define XINPUT_DEBUG_KEYSTROKE_FLAG_ALT        0x04
#define XINPUT_DEBUG_KEYSTROKE_FLAG_CAPSLOCK   0x08
#define XINPUT_DEBUG_KEYSTROKE_FLAG_NUMLOCK    0x10
#define XINPUT_DEBUG_KEYSTROKE_FLAG_SCROLLLOCK 0x20
#define XINPUT_DEBUG_KEYSTROKE_FLAG_KEYUP      0x40
#define XINPUT_DEBUG_KEYSTROKE_FLAG_REPEAT     0x80

typedef struct _XINPUT_DEBUG_KEYQUEUE_PARAMETERS
{
    DWORD dwFlags;
    DWORD dwQueueSize;
    DWORD dwRepeatDelay;
    DWORD dwRepeatInterval;
} XINPUT_DEBUG_KEYQUEUE_PARAMETERS, *PXINPUT_DEBUG_KEYQUEUE_PARAMETERS;

#define XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN      0x00000001
#define XINPUT_DEBUG_KEYQUEUE_FLAG_KEYREPEAT    0x00000002
#define XINPUT_DEBUG_KEYQUEUE_FLAG_KEYUP        0x00000004
#define XINPUT_DEBUG_KEYQUEUE_FLAG_ASCII_ONLY   0x00000010
#define XINPUT_DEBUG_KEYQUEUE_FLAG_ONE_QUEUE    0x00000020

