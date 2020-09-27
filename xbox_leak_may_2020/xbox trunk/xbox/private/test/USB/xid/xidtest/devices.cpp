#include <xtl.h>
#include "draw.h"
#include "devices.h"


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

CGameController::CGameController(DWORD dwPortNumber) : 
    m_dwPortNumber(dwPortNumber),
    m_fOpened(FALSE),
    m_dwPacketNumber(0),
    m_hDevice(0),
    m_iLabelColorOpened(LABEL_WHITE),
    m_iLabelColorClosed(DISCONNECTED_BLUE),
    m_iStateColor(CONNECTED_YELLOW)
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
    m_Feedback.Header.dwStatus = ERROR_SUCCESS;
    m_Feedback.Rumble.wLeftMotorSpeed = 0;
    m_Feedback.Rumble.wRightMotorSpeed = 0;
}

CGameController::~CGameController()
{
    Close();
}
void CGameController::Open(XINPUT_POLLING_PARAMETERS *pPollingParameters)
{
    DWORD dwError;
    if(!m_fOpened)
    {
        m_Feedback.Header.dwStatus = ERROR_SUCCESS;
        m_hDevice =  XInputOpen(XDEVICE_TYPE_GAMEPAD, m_dwPortNumber, XDEVICE_NO_SLOT, pPollingParameters);
        if(m_hDevice)
        {
            m_fOpened = TRUE;
        } else
        {   
            DebugPrint("Opened port %d Failed: %d\n", m_dwPortNumber, GetLastError());
        }
    }
}
void CGameController::Close()
{
    DWORD dwError;
    if(m_fOpened)
    {
        while(ERROR_IO_PENDING == m_Feedback.Header.dwStatus);
        XInputClose(m_hDevice);
        m_fOpened = FALSE;
        m_hDevice = NULL;
    }
}

void CGameController::PollDevice()
{
    if(!m_fOpened) return;
    XInputPoll(m_hDevice);
}

void CGameController::UpdateState()
{
    if(!m_fOpened) return;
    DWORD dwError = XInputGetState(m_hDevice, &m_gpState);
    
    if(ERROR_IO_PENDING != m_Feedback.Header.dwStatus)
    {
        if(
            ((m_gpState.Gamepad.bAnalogButtons[6] << 8) != m_Feedback.Rumble.wLeftMotorSpeed) ||
            ((m_gpState.Gamepad.bAnalogButtons[7] << 8) != m_Feedback.Rumble.wRightMotorSpeed)
        ){
            m_Feedback.Rumble.wLeftMotorSpeed = m_gpState.Gamepad.bAnalogButtons[6] << 8;
            m_Feedback.Rumble.wRightMotorSpeed = m_gpState.Gamepad.bAnalogButtons[7] << 8;
            XInputSetState(m_hDevice, (PXINPUT_FEEDBACK)&m_Feedback);
        }
    }
    
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

void CGameController::Draw(CDraw& draw)
{
    int iLabelColor = m_fOpened ? m_iLabelColorOpened : m_iLabelColorClosed;
    WCHAR FormatBuffer[80];
    draw.FillRect(m_iLeftEdge, m_iTopEdge, 300, 200, BACKDROP_BLUE); 
    wsprintf(FormatBuffer, L"Port Number %d", m_dwPortNumber);
    draw.DrawText(FormatBuffer, m_iLeftEdge, m_iTopEdge, iLabelColor);
    draw.DrawText(L"Start:", m_iLeftEdge+10, m_iTopEdge+10, iLabelColor);
    draw.DrawText(L"Back:", m_iLeftEdge+10, m_iTopEdge+20, iLabelColor);
    draw.DrawText(L"Analog  Buttons:", m_iLeftEdge+10, m_iTopEdge+30, iLabelColor);
    draw.DrawText(L"Left Thumbstick:", m_iLeftEdge+10, m_iTopEdge+50, iLabelColor);
    draw.DrawText(L"Right Thumbstick:", m_iLeftEdge+10, m_iTopEdge+60, iLabelColor);
    draw.DrawText(L"Direction Pad:", m_iLeftEdge+10, m_iTopEdge+70, iLabelColor);
    draw.DrawText(L"Packet Number:", m_iLeftEdge+10, m_iTopEdge+80, iLabelColor);
    if(m_fOpened)
    {
        
        draw.DrawText(
            GetButtonStateText(XINPUT_GAMEPAD_START),
            m_iLeftEdge+160,
            m_iTopEdge+10,
            m_iStateColor
            );
        draw.DrawText(
            GetButtonStateText(XINPUT_GAMEPAD_BACK),
            m_iLeftEdge+160,
            m_iTopEdge+20,
            m_iStateColor
            );

        for(int i=0; i<8; i++)
        {
            wsprintf(FormatBuffer, L"%3.0d", m_gpState.Gamepad.bAnalogButtons[i]);
            if(i < 4)
            {
                draw.DrawText(
                        FormatBuffer,
                        m_iLeftEdge+160+i*30,
                        m_iTopEdge+30,
                        PressureColor(m_gpState.Gamepad.bAnalogButtons[i])
                        );
            } else
            {
                draw.DrawText(
                        FormatBuffer,
                        m_iLeftEdge+160+(i-4)*30,
                        m_iTopEdge+40,
                        PressureColor(m_gpState.Gamepad.bAnalogButtons[i])
                        );
            }
        }
        wsprintf(
            FormatBuffer,
            L"(%d,%d, %s)",
            (LONG)m_gpState.Gamepad.sThumbLX,
            (LONG)m_gpState.Gamepad.sThumbLY,
            GetButtonStateText(XINPUT_GAMEPAD_LEFT_THUMB)
            );
        draw.DrawText(FormatBuffer, m_iLeftEdge+160, m_iTopEdge+50, m_iStateColor);
        wsprintf(
            FormatBuffer,
            L"(%d,%d, %s)",
            (LONG)m_gpState.Gamepad.sThumbRX,
            (LONG)m_gpState.Gamepad.sThumbRY,
            GetButtonStateText(XINPUT_GAMEPAD_RIGHT_THUMB)
            );
        draw.DrawText(FormatBuffer, m_iLeftEdge+160, m_iTopEdge+60, m_iStateColor);
        
        wsprintf(
            FormatBuffer,
            L"(%d)",
            m_gpState.dwPacketNumber
            );
        draw.DrawText(GetDPadDirectionText(), m_iLeftEdge+160, m_iTopEdge+70, m_iStateColor);
        draw.DrawText(FormatBuffer, m_iLeftEdge+160, m_iTopEdge+80, m_iStateColor);
    }
}

LPCWSTR CGameController::GetButtonStateText(DWORD dwButtonMask)
{
    
    if(dwButtonMask&m_gpState.Gamepad.wButtons)
    {
        return L"Down";
    }
    return L"Up";
}

LPCWSTR CGameController::GetDPadDirectionText()
{
    if(m_gpState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
    {
       if(m_gpState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
       {
            return L"Northwest";
       }
       if(m_gpState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
       {
            return L"Northeast";
       }
       return L"North";
    } 
    if(m_gpState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
    {
       if(m_gpState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
       {
            return L"Southwest";
       }
       if(m_gpState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
       {
            return L"Southeast";
       }
       return L"South";
    }
    if(m_gpState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
    {
        return L"West";
    }
    if(m_gpState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
    {
        return L"East";
    }
    return L"Centered";
}

CGameControllers::CGameControllers(XINPUT_POLLING_PARAMETERS *pPollingParameters) :
    m_pPollingParameters(pPollingParameters)
{
    int i;
    for(i=0; i < 4; i++)
    {
        m_pGameControllers[i] = new CGameController(i);
        if(!m_pGameControllers[i]) DebugPrint("Failed to allocate a game controller!");
    }
    DWORD dwDevices;
    dwDevices = XGetDevices(XDEVICE_TYPE_GAMEPAD);
    for(i=0; i < 4; i++)
    {
        if(dwDevices&(1 << i)) m_pGameControllers[i]->Open(m_pPollingParameters);
    }
}

CGameControllers::~CGameControllers()
{
    int i;
    for(i=0; i < 4; i++)
    {
        delete m_pGameControllers[i];
    }
}
void CGameControllers::CheckForHotplugs()
{
    DWORD dwInsertions, dwRemovals;
    XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals);
    int i;
    for(i=0; i < 4; i++)
    {
        if(dwRemovals&(1 << i)) m_pGameControllers[i]->Close();
        if(dwInsertions&(1 << i)) m_pGameControllers[i]->Open(m_pPollingParameters);
    }
}
void CGameControllers::UpdateState()
{
   for(int i=0; i < 4; i++) m_pGameControllers[i]->UpdateState();
}
void CGameControllers::Draw(CDraw& draw)
{
    for(int i=0; i < 4; i++) m_pGameControllers[i]->Draw(draw);
}

void CGameControllers::PollDevices()
{
    for(int i=0; i < 4; i++) m_pGameControllers[i]->PollDevice();
}