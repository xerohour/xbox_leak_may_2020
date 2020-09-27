#include <xtl.h>
#include "draw.h"
#include "devices.h"
//#include "hidkbd.h"


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

int PressureColor(UCHAR Pressure)
/*++
    Wild ass guess on how to make it go from blue to red on more pressure.
--*/
{
    UCHAR blue = Pressure << 2;
    if(blue > Pressure) blue -= Pressure;
    else blue = 0;
    UCHAR green = Pressure << 1;;
    if(green  > Pressure) green -= Pressure;
    else green = 0;
    UCHAR red = Pressure;
    int Color = 0;
    Color = D3DCOLOR_XRGB(red, green, blue);
    return Color;
}

CKeyboard::CKeyboard(DWORD dwPortNumber) : 
    m_dwPortNumber(dwPortNumber),
    m_fOpened(FALSE),
    m_dwPacketNumber(0),
    m_hDevice(0),
    m_iLabelColorOpened(LABEL_WHITE),
    m_iLabelColorClosed(DISCONNECTED_BLUE),
    m_iStateColor(CONNECTED_YELLOW),
    m_fScrollLock(FALSE),
    m_fNumLock(FALSE),
    m_fCapsLock(FALSE)
{
    switch(dwPortNumber)
    {
        case 0:
            m_iLeftEdge = 10;
            m_iTopEdge  = 10;
            break;
        case 1:
            m_iLeftEdge = 325;
            m_iTopEdge  = 10;
            break;
        case 2:
            m_iLeftEdge = 10;
            m_iTopEdge  = 255;
            break;
        case 3:
            m_iLeftEdge = 325;
            m_iTopEdge  = 255;
            break;
    }
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
        m_hDevice =  XInputOpen(XDEVICE_TYPE_KEYBOARD, m_dwPortNumber, XDEVICE_NO_SLOT, pPollingParameters);
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

void CKeyboard::Draw(CDraw& draw)
{
    int iLabelColor = m_fOpened ? m_iLabelColorOpened : m_iLabelColorClosed;
    WCHAR FormatBuffer[80];
    draw.FillRect(m_iLeftEdge, m_iTopEdge, 300, 200, BACKDROP_BLUE); 
    wsprintf(FormatBuffer, L"Port Number %d", m_dwPortNumber);
    draw.DrawText(FormatBuffer, m_iLeftEdge, m_iTopEdge, iLabelColor);
    draw.DrawText(L"Modifiers:", m_iLeftEdge+10, m_iTopEdge+10, iLabelColor);
    draw.DrawText(L"Buttons:",  m_iLeftEdge+10, m_iTopEdge+30, iLabelColor); 
    
    
    if(m_fOpened)
    {
        //
        //  Format modifiers.
        //
        wsprintf(FormatBuffer, L"0x%0.2x", (ULONG)m_kbdState.Keyboard.Modifiers);
        draw.DrawText(
            FormatBuffer,
            m_iLeftEdge+100,
            m_iTopEdge+10,
            m_iStateColor
            );
        //
        //  Show other six keys
        //
        for(int i=0; i<6; i++)
        {
            wsprintf(FormatBuffer, L"0x%0.2x", (ULONG)m_kbdState.Keyboard.Keys[i]);
            draw.DrawText(
                FormatBuffer,
                m_iLeftEdge+100+i*35,
                m_iTopEdge+30,
                m_iStateColor
                );
        }
    }
}

CKeyboards::CKeyboards(XINPUT_POLLING_PARAMETERS *pPollingParameters) :
    m_pPollingParameters(pPollingParameters)
{
    int i;
    for(i=0; i < 4; i++)
    {
        m_pKeyboards[i] = new CKeyboard(i);
        if(!m_pKeyboards[i]) DebugPrint("Failed to allocate a game controller!");
    }
    DWORD dwDevices;
    dwDevices = XGetDevices(XDEVICE_TYPE_KEYBOARD);
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
    XGetDeviceChanges(XDEVICE_TYPE_KEYBOARD, &dwInsertions, &dwRemovals);
    int i;
    for(i=0; i < 4; i++)
    {
        if(dwRemovals&(1 << i)) m_pKeyboards[i]->Close();
        if(dwInsertions&(1 << i)) m_pKeyboards[i]->Open(m_pPollingParameters);
    }
}
void CKeyboards::UpdateState()
{
   for(int i=0; i < 4; i++) m_pKeyboards[i]->UpdateState();
}
void CKeyboards::Draw(CDraw& draw)
{
    for(int i=0; i < 4; i++) m_pKeyboards[i]->Draw(draw);
}

void CKeyboards::PollDevices()
{
    for(int i=0; i < 4; i++) m_pKeyboards[i]->PollDevice();
}