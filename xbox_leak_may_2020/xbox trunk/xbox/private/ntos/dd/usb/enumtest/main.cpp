#include <xtl.h>
#include "draw.h"


// Hack! DbgPrint is much nicer than OutputDebugString so JonT exported it from xapi.lib
// (as DebugPrint) but the prototype isn't in the public headers...
extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}
//
// Colors
//
#define BACKDROP_BLUE 0x000080
#define LABEL_WHITE 0xffffff
#define DISCONNECTED_BLUE 0x000040
#define CONNECTED_YELLOW 0xffff00

//
//  Useful Macro
//
#define DECLARE_DO_TWICE int doTwice;
#define DO_TWICE doTwice=2; while(doTwice--)

//
//  Forward Declations
//
void InitDisplay(Draw *pDraw);
void InitDevices(PXPP_DEVICE_TYPE DeviceType, DWORD& dwState, BOOL fHasBottomSlots, Draw *pDraw, ULONG yPos);
void UpdateDevices(PXPP_DEVICE_TYPE DeviceType, DWORD& dwState, BOOL fHasBottomSlots, Draw *pDraw, ULONG yPos);
LPWSTR DeviceTypeToString(PXPP_DEVICE_TYPE DeviceType);
void DrawPortTopSlotsBitmap(Draw *pDraw, DWORD dwState, ULONG yPos);
void DrawPortBottomSlotsBitmap(Draw *pDraw, DWORD dwState, ULONG yPos);

//------------------------------------------------------------------------------
// TestMain

void __cdecl main()
{
    DebugPrint("ENUMTEST APPLICATION.\nBuilt on %s at %s\n", __DATE__, __TIME__);
    
    //Initialize core peripheral port support
    XDEVICE_PREALLOC_TYPE deviceTypes[] = 
    {
        {XDEVICE_TYPE_GAMEPAD,4},
        {XDEVICE_TYPE_MEMORY_UNIT,8},
        {XDEVICE_TYPE_VOICE_MICROPHONE,4},
        {XDEVICE_TYPE_VOICE_HEADPHONE,4}
    };
    XInitDevices(sizeof(deviceTypes)/sizeof(XDEVICE_PREALLOC_TYPE),deviceTypes);

    Draw draw;
    DWORD dwStateGamePads;
    DWORD dwStateMemoryUnits;
    DWORD dwStateChatButtons;
    DWORD dwStateChatMicrophone;
    DWORD dwStateChatHeadphone;
    InitDisplay(&draw);
    InitDevices(XDEVICE_TYPE_GAMEPAD, dwStateGamePads,  FALSE, &draw,      80);
    InitDevices(XDEVICE_TYPE_MEMORY_UNIT, dwStateMemoryUnits, TRUE, &draw,   100);
    InitDevices(XDEVICE_TYPE_VOICE_MICROPHONE, dwStateChatMicrophone, FALSE, &draw,   130);
    InitDevices(XDEVICE_TYPE_VOICE_HEADPHONE, dwStateChatHeadphone, FALSE, &draw,   150);
    while(1)
    {
        UpdateDevices(XDEVICE_TYPE_GAMEPAD, dwStateGamePads, FALSE, &draw,       80);
        UpdateDevices(XDEVICE_TYPE_MEMORY_UNIT, dwStateMemoryUnits, TRUE, &draw,   100);
        UpdateDevices(XDEVICE_TYPE_VOICE_MICROPHONE, dwStateChatMicrophone, FALSE, &draw,   130);
        UpdateDevices(XDEVICE_TYPE_VOICE_HEADPHONE, dwStateChatHeadphone, FALSE, &draw,   150);
    }
}

void InitDisplay(Draw *pDraw)
{
    DECLARE_DO_TWICE;
    DO_TWICE{
        //Draw the title screen
        pDraw->FillRect(0, 0, 640, 480, BACKDROP_BLUE); 
        pDraw->DrawText(L"USB Enumeration Test Application", 50,  50, LABEL_WHITE);
        pDraw->Present();
    }
}

void InitDevices(PXPP_DEVICE_TYPE DeviceType, DWORD& dwState, BOOL fHasBottomSlots, Draw *pDraw, ULONG yPos)
{
    WCHAR FormatBuffer[80];
    DECLARE_DO_TWICE;
    if(fHasBottomSlots)
    {
        DO_TWICE{
            wsprintf(FormatBuffer, L"%ws's(Top):", DeviceTypeToString(DeviceType));
            pDraw->DrawText(FormatBuffer, 55, yPos, LABEL_WHITE);
            wsprintf(FormatBuffer, L"%ws's(Bottom):", DeviceTypeToString(DeviceType));
            pDraw->DrawText(FormatBuffer, 55, yPos+10, LABEL_WHITE);
            pDraw->Present();
        }
    } else
    {
        DO_TWICE
        {
            wsprintf(FormatBuffer, L"%ws's:", DeviceTypeToString(DeviceType));
            pDraw->DrawText(FormatBuffer, 55, yPos, LABEL_WHITE);
            pDraw->Present();
        }
    }
    dwState = XGetDevices(DeviceType);
    DebugPrint("%ws Initial State = 0x%0.8x\n", DeviceTypeToString(DeviceType), dwState);
    DrawPortTopSlotsBitmap(pDraw, dwState, yPos);
    if(fHasBottomSlots) DrawPortBottomSlotsBitmap(pDraw, dwState, yPos+10);
}

void UpdateDevices(PXPP_DEVICE_TYPE DeviceType, DWORD& dwState, BOOL fHasBottomSlots, Draw *pDraw, ULONG yPos)
{
    DWORD dwInsertions, dwRemovals;
    //
    //  Only update the text, if something changed.
    //  This is why you want to ask for insertions and removals (last parameter is false)
    //  rather than state, which returns TRUE if anything is attached.
    //
    if(XGetDeviceChanges(DeviceType, &dwInsertions, &dwRemovals))
    {
        dwState &= ~dwRemovals;  //Always process removals first
        dwState |= dwInsertions;
        DebugPrint("%ws Changed, Insert=0x%0.8x, Remove=0x%0.8x, State=0x%0.8x\n", DeviceTypeToString(DeviceType), dwInsertions, dwRemovals, dwState);
        DrawPortTopSlotsBitmap(pDraw, dwState, yPos);
        if(fHasBottomSlots) DrawPortBottomSlotsBitmap(pDraw, dwState, yPos+10);
    }
}

LPWSTR DeviceTypeToString(PXPP_DEVICE_TYPE DeviceType)
{
    if(XDEVICE_TYPE_GAMEPAD == DeviceType) return L"XDEVICE_TYPE_GAMEPAD";
    if(XDEVICE_TYPE_MEMORY_UNIT  == DeviceType) return L"XDEVICE_TYPE_MEMORY_UNIT";
    if(XDEVICE_TYPE_VOICE_MICROPHONE == DeviceType) return L"XDEVICE_TYPE_VOICE_MICROPHONE";
    if(XDEVICE_TYPE_VOICE_HEADPHONE  == DeviceType) return L"XDEVICE_TYPE_VOICE_HEADPHONE";
    return L"XDEVICE_TYPE_????";
}

void DrawPortTopSlotsBitmap(Draw *pDraw, DWORD dwState, ULONG yPos)
/*++
    Draws the state of the top slots of players one to four.
--*/
{
  DECLARE_DO_TWICE;
  DO_TWICE
  {
      ULONG xPos = 250;
      WCHAR PortString[2]=L"0";
      for(int index = 0; index < XGetPortCount(); index++)
      {
        (*PortString)++;        
        pDraw->DrawText(PortString, xPos, yPos, (dwState&(1<<index)) ? CONNECTED_YELLOW : DISCONNECTED_BLUE);
        xPos += 20;
       }
       pDraw->Present();
  }
}

void DrawPortBottomSlotsBitmap(Draw *pDraw, DWORD dwState, ULONG yPos)
/*++
    Draws the state of the bottom slots of players one to four
--*/
{
  DECLARE_DO_TWICE;
  DO_TWICE
  {
      ULONG xPos = 250;
      WCHAR PortString[2]=L"0";
      for(int index = 0; index < XGetPortCount(); index++)
      {
        (*PortString)++;        
        pDraw->DrawText(PortString, xPos, yPos, (dwState&(1<<(index+16))) ? CONNECTED_YELLOW : DISCONNECTED_BLUE);
        xPos += 20;
       }
       pDraw->Present();
  }
}
