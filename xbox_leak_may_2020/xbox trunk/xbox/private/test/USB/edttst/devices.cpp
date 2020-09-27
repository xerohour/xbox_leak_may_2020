#include <xtl.h>
#include "draw.h"
#include "devices.h"
#include <xkbd.h>


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

CKeyboard::CKeyboard(DWORD dwPortNumber, int iLeftEdge, int iTopEdge) : 
    m_dwPortNumber(dwPortNumber),
    m_fOpened(FALSE),
    m_dwPacketNumber(0),
    m_hDevice(0),
    m_iLabelColorOpened(LABEL_WHITE),
    m_iLabelColorClosed(DISCONNECTED_BLUE),
    m_iStateColor(CONNECTED_YELLOW),
    m_fScrollLock(FALSE),
    m_fNumLock(FALSE),
    m_fCapsLock(FALSE),
    m_iLeftEdge(iLeftEdge),
    m_iTopEdge(iTopEdge)
{

    /*
    m_Feedback.Header.dwStatus = ERROR_SUCCESS;
    m_Feedback.KeyboardLEDs.LedStates = 0;
    */
}

CKeyboard::~CKeyboard()
{
    Close();
}
void CKeyboard::Open(XINPUT_POLLING_PARAMETERS *pPollingParameters)
{
    DWORD dwError;
    if(!m_fOpened)
    {
        //m_Feedback.Header.dwStatus = ERROR_SUCCESS;
        m_hDevice =  XInputOpen(XDEVICE_TYPE_DEBUG_KEYBOARD, m_dwPortNumber, XDEVICE_NO_SLOT, pPollingParameters);
        if(m_hDevice)
        {
            m_fOpened = TRUE;
        } else
        {   
            DebugPrint("Opened port %d Failed: %d\n", m_dwPortNumber, GetLastError());
        }
    }
}
void CKeyboard::Close()
{
    DWORD dwError;
    if(m_fOpened)
    {
        //while(ERROR_IO_PENDING == m_Feedback.Header.dwStatus);
        XInputClose(m_hDevice);
        m_fOpened = FALSE;
        m_hDevice = NULL;
    }
}

#if 0
void CKeyboard::PollDevice()
{
    if(!m_fOpened) return;
    XInputPoll(m_hDevice);
}

void CKeyboard::UpdateState()
{
    if(!m_fOpened) return;
    DWORD dwError = XInputGetState(m_hDevice, &m_kbdState);
    /*
    if(ERROR_IO_PENDING != m_Feedback.Header.dwStatus)
    {
        if(
            ((m_kbdState.Modifiers.bAnalogButtons[6] << 8) != m_Feedback.Rumble.wLeftMotorSpeed) ||
            ((m_kbdState.Gamepad.bAnalogButtons[7] << 8) != m_Feedback.Rumble.wRightMotorSpeed)
        ){
            m_Feedback.Rumble.wLeftMotorSpeed = m_gpState.Gamepad.bAnalogButtons[6] << 8;
            m_Feedback.Rumble.wRightMotorSpeed = m_gpState.Gamepad.bAnalogButtons[7] << 8;
            XInputSetState(m_hDevice, (PXINPUT_FEEDBACK)&m_Feedback);
        }
    }
    */
    if(ERROR_SUCCESS != dwError)
    {
        if(dwError == ERROR_DEVICE_NOT_CONNECTED)
        {
            //
            //  Don't worry about closing the handle, since we
            //  poll for device removals and we will close
            //  the handle very soon anyway.
            //
            return;
        }
        DebugPrint("XInputGetDeviceState return error: %d\n", dwError);
        DebugPrint("This error is not expected in normal operation\n");
        DebugBreak();
    }
}
#endif

#define COLOR_LEFT_GREEN 0xff00ff00
#define COLOR_RIGHT_RED 0xffff0000
void CKeyboard::Draw(CDraw& draw)
{
    int iLabelColor = m_fOpened ? m_iLabelColorOpened : m_iLabelColorClosed;
    WCHAR FormatBuffer[80];
    draw.FillRect(m_iLeftEdge, m_iTopEdge, 600, 30, BACKDROP_BLUE); 
    wsprintf(FormatBuffer, L"P%d:", m_dwPortNumber);
    draw.DrawText(FormatBuffer, m_iLeftEdge, m_iTopEdge, iLabelColor);
/*    
    if(m_fOpened)
    {
        
        //
        //  Left and Right Control
        //
        iLabelColor = 0;
        if(HID_USAGE_MASK_KEYBOARD_LCTRL&m_kbdState.Keyboard.Modifiers)
        {
            iLabelColor |= COLOR_LEFT_GREEN;
        }
        if(HID_USAGE_MASK_KEYBOARD_RCTRL&m_kbdState.Keyboard.Modifiers)
        {
            iLabelColor |= COLOR_RIGHT_RED;
        }
        if(iLabelColor)
        {
            draw.DrawText(L"CTRL", m_iLeftEdge+50, m_iTopEdge, iLabelColor);
        }
        
        //
        //  Left and Right SHIFT
        //
        iLabelColor = 0;
        if(HID_USAGE_MASK_KEYBOARD_LSHFT&m_kbdState.Keyboard.Modifiers)
        {
            iLabelColor |= COLOR_LEFT_GREEN;
        }
        if(HID_USAGE_MASK_KEYBOARD_RSHFT&m_kbdState.Keyboard.Modifiers)
        {
            iLabelColor |= COLOR_RIGHT_RED;
        }
        if(iLabelColor)
        {
            draw.DrawText(L"SHIFT", m_iLeftEdge+100, m_iTopEdge, iLabelColor);
        }

        //
        //  Left and Right ALT
        //
        iLabelColor = 0;
        if(HID_USAGE_MASK_KEYBOARD_LALT&m_kbdState.Keyboard.Modifiers)
        {
            iLabelColor |= COLOR_LEFT_GREEN;
        }
        if(HID_USAGE_MASK_KEYBOARD_RALT&m_kbdState.Keyboard.Modifiers)
        {
            iLabelColor |= COLOR_RIGHT_RED;
        }
        if(iLabelColor)
        {
            draw.DrawText(L"ALT", m_iLeftEdge+150, m_iTopEdge, iLabelColor);
        }

        //
        //  Left and Right GUI
        //
        iLabelColor = 0;
        if(HID_USAGE_MASK_KEYBOARD_LGUI&m_kbdState.Keyboard.Modifiers)
        {
            iLabelColor |= COLOR_LEFT_GREEN;
        }
        if(HID_USAGE_MASK_KEYBOARD_RGUI&m_kbdState.Keyboard.Modifiers)
        {
            iLabelColor |= COLOR_RIGHT_RED;
        }
        if(iLabelColor)
        {
            draw.DrawText(L"GUI", m_iLeftEdge+200, m_iTopEdge, iLabelColor);
        }

        //
        //  Show other six keys
        //
        for(int i=0; i<6; i++)
        {
            wsprintf(FormatBuffer, L"0x%0.2x", (ULONG)m_kbdState.Keyboard.Keys[i]);
            draw.DrawText(
                FormatBuffer,
                m_iLeftEdge+250+i*35,
                m_iTopEdge,
                m_iStateColor
                );
        }
    }
*/
}

CKeyboards::CKeyboards(XINPUT_POLLING_PARAMETERS *pPollingParameters) :
    m_pPollingParameters(pPollingParameters)
{

     //
    //  Initialize High-Level Keyboard Function
    //
    XINPUT_DEBUG_KEYQUEUE_PARAMETERS QueueParameters;
    QueueParameters.dwFlags = XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN | XINPUT_DEBUG_KEYQUEUE_FLAG_KEYREPEAT;
    QueueParameters.dwQueueSize = 40;
    QueueParameters.dwRepeatDelay = 350;
    QueueParameters.dwRepeatInterval = 70;
    XInputDebugInitKeyboardQueue(&QueueParameters);
    //
    //  Create and initialize the keyboard instance objects
    //
    int i;
    for(i=0; i < 4; i++)
    {
        m_pKeyboards[i] = new CKeyboard(i, 10, 40+(i*30));
        if(!m_pKeyboards[i]) DebugPrint("Failed to allocate a keyboard!");
    }
    //
    //  Find and open keyboard instances
    //
    DWORD dwDevices;
    dwDevices = XGetDevices(XDEVICE_TYPE_DEBUG_KEYBOARD);
    for(i=0; i < 4; i++)
    {
        if(dwDevices&(1 << i)) m_pKeyboards[i]->Open(m_pPollingParameters);
    }
}

CKeyboards::~CKeyboards()
{
    int i;
    for(i=0; i < 4; i++)
    {
        delete m_pKeyboards[i];
    }
}
void CKeyboards::CheckForHotplugs()
{
    DWORD dwInsertions, dwRemovals;
    XGetDeviceChanges(XDEVICE_TYPE_DEBUG_KEYBOARD, &dwInsertions, &dwRemovals);
    int i;
    for(i=0; i < 4; i++)
    {
        if(dwRemovals&(1 << i)) m_pKeyboards[i]->Close();
        if(dwInsertions&(1 << i)) m_pKeyboards[i]->Open(m_pPollingParameters);
    }
}
/*
void CKeyboards::PollDevices()
{
    for(int i=0; i < 4; i++) m_pKeyboards[i]->PollDevice();
}
void CKeyboards::UpdateState()
{
   for(int i=0; i < 4; i++) m_pKeyboards[i]->UpdateState();
}
*/
void CKeyboards::Draw(CDraw& draw)
{
    for(int i=0; i < 4; i++) m_pKeyboards[i]->Draw(draw);
}

