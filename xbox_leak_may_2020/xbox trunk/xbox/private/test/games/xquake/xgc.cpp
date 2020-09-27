#include <xtl.h>
#include "xgc.h"

CGameControllers::CGameControllers() : m_fIsSelected(FALSE), m_iControllerNumber(0)
{
    DWORD dwDevices = XGetDevices(XDEVICE_TYPE_GAMEPAD);
    for(int i =0; i < XGetPortCount(); i++)
    {
        if(dwDevices&(1<<i))
        {
            m_hGameControllerArray[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD,i,XDEVICE_NO_SLOT, NULL);
        } else
        {
            m_hGameControllerArray[i] = NULL;
        }
    }
}


CGameControllers::~CGameControllers()
{
    for(int i =0; i < XGetPortCount(); i++)
    {
        if(m_hGameControllerArray[i])
        {
            XInputClose(m_hGameControllerArray[i]);
            m_hGameControllerArray[i] = NULL;
        }
    }
}

void CGameControllers::SelectGameController()
{
    m_fIsSelected = FALSE;
}

BOOL CGameControllers::GetSelection(int *piControllerNumber)
{
    //
    //  Detect hot-plugs, finds new device and opens them.
    //  Closes expired ones.
    //

    DetectHotPlugs();

    //
    //  If there is no selection, try to get one.
    //
    if(!m_fIsSelected)
    {
        XINPUT_STATE InputState;
        for(int i =0; i < XGetPortCount(); i++)
        {
            if(m_hGameControllerArray[i])
            {
                if(ERROR_SUCCESS == XInputGetState(m_hGameControllerArray[i], &InputState))
                {
                    BOOL fAnalogButtonPressed = FALSE;
                    for(int j = 0; j < 8; j++)
                    {
                        if(InputState.Gamepad.bAnalogButtons[j] > 15)
                        {
                            fAnalogButtonPressed = TRUE;
                            break;
                        }
                    }
                    if(
                        fAnalogButtonPressed                ||
                        (InputState.Gamepad.wButtons)       ||
                        (InputState.Gamepad.sThumbLX > 15)  ||
                        (InputState.Gamepad.sThumbLX < -15) ||
                        (InputState.Gamepad.sThumbLY > 15)  ||
                        (InputState.Gamepad.sThumbLY < -15) ||
                        (InputState.Gamepad.sThumbRX > 15)  ||
                        (InputState.Gamepad.sThumbRX < -15) ||
                        (InputState.Gamepad.sThumbRY > 15)  ||
                        (InputState.Gamepad.sThumbRY < -15)
                    )
                    {
                        m_fIsSelected = TRUE;
                        m_iControllerNumber = i;
                        break;
                    }
                }
            }
        }
    }

    if(m_fIsSelected)
    {
        *piControllerNumber = m_iControllerNumber;
        return TRUE;
    } else
    {
        return FALSE;
    }
}

BOOL CGameControllers::GetState(XINPUT_STATE *pState)
{
    BOOL fRetVal = FALSE;
    
    DetectHotPlugs();

    if(!m_fIsSelected)
    {
        int iControllerNumber;
        GetSelection(&iControllerNumber);
    }
    if(m_fIsSelected)
    {
        if(ERROR_SUCCESS == XInputGetState(m_hGameControllerArray[m_iControllerNumber], pState))
        {
            fRetVal = TRUE;
        }
    }
    return fRetVal;
}

void CGameControllers::DetectHotPlugs()
{
    DWORD dwInsertions, dwRemovals;
    if(XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals))
    {
        for(int i =0; i < XGetPortCount(); i++)
        {
            if(dwRemovals&(1<<i))
            {
                if(i == m_iControllerNumber)
                {
                    m_fIsSelected = FALSE;
                }
                XInputClose(m_hGameControllerArray[i]);
                m_hGameControllerArray[i] = NULL;
            }

            if(dwInsertions&(1<<i))
            {
                m_hGameControllerArray[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD,i,XDEVICE_NO_SLOT, NULL);
            }

        }
    }
}
