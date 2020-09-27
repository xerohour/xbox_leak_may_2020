//-----------------------------------------------------------------------------
// File: Controls.cpp
//
// Desc: Implementation of CController and CControl Classes
//
// Hist: Oct 01 2001 - Created
//
//
// Author:  Dennis Krueger <a-denkru>
//
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "controls.h"
#include "testcontrols.h"

#ifdef __cplusplus
extern "C" {
#endif
int DebugPrint(char* format, ...);
#ifdef __cplusplus
}
#endif




WORD DPadButtonID[4] = {
	XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_LEFT,
	XINPUT_GAMEPAD_DPAD_RIGHT
};



// CDPad methods
CDPad::CDPad(CController * pController) : CControl(pController)
{
		WORD wMask = 1;
		for(int i = 0; i < 4; i++)
		{
			if(pController->m_InputCaps.In.Gamepad.wButtons & wMask)
			{
				m_pButton[i] = new CDigitalButton(pController,DPadButtonID[i]);
			}
			wMask <<= 1;
		}

}

CDPad::~CDPad()
{
	for(int i = 0; i<4; i++)
	{
		if(m_pButton[i]) delete m_pButton[i];
	}
}

BOOL
CDPad::GetUp()
{
	if(m_pButton[0]
		return m_pButton[0]->GetState();
	else
		return FALSE;
}

BOOL
CDPad::GetDown()
{
	if(m_pButton[1]
		return m_pButton[1]->GetState();
	else
		return FALSE;
}

BOOL
CDPad::GetLeft()
{
	if(m_pButton[2]
		return m_pButton[2]->GetState();
	else
		return FALSE;
}

BOOL
CDPad::GetRight()
{
	if(m_pButton[3]
		return m_pButton[3]->GetState();
	else
		return FALSE;
}

//CDigitalButton methods
CDigitalButton::CDigitalButton(CController * pController, WORD wType)
		:CControl(pController)
{
	m_wType = wType;
	m_pController = pController;
}

BOOL
CDigitalButton::GetState()
{
	return m_pController->m_InputState.Gamepad.wButtons & m_wType ? TRUE : FALSE;
}




// CAnalogButton methods
CAnalogButton::CAnalogButton(CController * pController, int index) : CControl(pController)
{
			m_Resolution = pController->m_InputCaps.In.Gamepad.bAnalogButtons[index];
			m_Type = index;

}

BYTE
CAnalogButton::GetState()
{

	return m_pController->m_InputState.Gamepad.bAnalogButtons[m_Type];
}


// CThumbstick methods
CThumbstick::CThumbstick(CController * pController,WORD wThumbType) : CControl(pController)
{
	m_wThumbType = wThumbType;
	if(XINPUT_GAMEPAD_LEFT_THUMB == wThumbType)
	{
		m_Axis[0].SetResolution(pController->m_InputCaps.In.Gamepad.sThumbLX);
		m_Axis[1].SetResolution(pController->m_InputCaps.In.Gamepad.sThumbLY);
	} else
	{
		m_Axis[0].SetResolution(pController->m_InputCaps.In.Gamepad.sThumbRX);
		m_Axis[1].SetResolution(pController->m_InputCaps.In.Gamepad.sThumbRY);
	}

	if(pController->m_InputCaps.In.Gamepad.wButtons & wThumbType)
	{
		m_pButton = new CDigitalButton(pController, wThumbType);
	} else
	{
		m_pButton = NULL;
	}
}



CThumbstick::~CThumbstick()
{
	if(m_pButton) delete m_pButton;
}

SHORT
CThumbstick::GetX()
{
	SHORT sState;
	if(XINPUT_GAMEPAD_LEFT_THUMB == m_wThumbType)
	{
		sState = m_pController->m_InputState.Gamepad.sThumbLX;
	} else
	{
		sState =  m_pController->m_InputState.Gamepad.sThumbRX;
	}
	return sState;
}

SHORT
CThumbstick::GetY()
{
	SHORT sState;
	if(XINPUT_GAMEPAD_LEFT_THUMB == m_wThumbType)
	{
		sState = m_pController->m_InputState.Gamepad.sThumbLY;
	} else
	{
		sState =  m_pController->m_InputState.Gamepad.sThumbRY;
	}
	return sState;
}

BOOL
CThumbstick::GetButtonState()
{
	return m_pController->m_InputState.Gamepad.wButtons & m_wThumbType ? TRUE : FALSE;
}


// Controller Class Methods
CController::CController()
{
	m_dwPacketNumber = 0;
	// set handles and pointers to NULL
	m_Handle = NULL;
	m_pDPad = NULL;
	for(int i = 0; i < 8; i++)
	{
		m_pAnalogButton[i] = NULL;
	}
	m_pThumbStick[0] = m_pThumbStick[1] = NULL;
	m_pStartButton = NULL;
	m_pBackButton = NULL;

}

CController::~CController()
{
	if(m_pDPad) delete m_pDPad;
	for(int i = 0; i<8; i++)
	{
		if(m_pAnalogButton[i]) delete m_pAnalogButton[i];
	}
	if(m_pThumbStick[0]) delete m_pThumbStick[0];
	if(m_pThumbStick[1]) delete m_pThumbStick[1];
	if(m_pStartButton) delete m_pStartButton;
	if(m_pBackButton) delete m_pBackButton;
	if(m_Handle) XInputClose(m_Handle);
}


DWORD
CController::Init(DWORD dwPort)
{
	DWORD	dwResult;
	WORD	wMask;

	// first open a handle to this controller
	m_Handle = XInputOpen(XDEVICE_TYPE_GAMEPAD,dwPort,XDEVICE_NO_SLOT,NULL);
	if(NULL == m_Handle)
		return GetLastError(); // failed

	// have open controller, populate controls
	m_dwPort = dwPort; // stash active port for this controller
	dwResult = XInputGetCapabilities(m_Handle, &m_InputCaps);
	if(ERROR_SUCCESS != dwResult)
	{
		XInputClose(m_Handle);
		m_Handle = NULL;
		return dwResult;
	}
	// got caps, now set existence flags for supported controls
	
	// DPAD
	if(m_InputCaps.In.Gamepad.wButtons & 0x000f) // test for any portion of dpad
	{
		m_pDPad = new CDPad(this);
	}
	
	// Start Button
	if(m_InputCaps.In.Gamepad.wButtons & XINPUT_GAMEPAD_START)
	{
		m_pStartButton = new CDigitalButton(this, XINPUT_GAMEPAD_START);
	}

	// Back Button
	if(m_InputCaps.In.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
	{
		m_pBackButton = new CDigitalButton(this,XINPUT_GAMEPAD_BACK);
	}

	// All Analog buttons

	for(int i = 0; i < 8 ; i++)
	{
		if(m_InputCaps.In.Gamepad.bAnalogButtons[i])
		{
			m_pAnalogButton[i] = new CAnalogButton(this,i);
		}
	}

	// Left Thumbstick
	if(m_InputCaps.In.Gamepad.sThumbLX || 
			m_InputCaps.In.Gamepad.sThumbLY ||
			(m_InputCaps.In.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB))
	{
		m_pThumbStick[0] = new CThumbstick(this,XINPUT_GAMEPAD_LEFT_THUMB);
	}

	// Right Thumbstick
	if(m_InputCaps.In.Gamepad.sThumbRX ||
				m_InputCaps.In.Gamepad.sThumbRY ||
				(m_InputCaps.In.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB))
	{
		m_pThumbStick[1] = new CThumbstick(this,XINPUT_GAMEPAD_RIGHT_THUMB);
	}

	return S_OK;
}

void CController::Refresh()
{
	DWORD dwResult;

	dwResult = XInputGetState(m_Handle,&m_InputState);
	if(ERROR_SUCCESS != dwResult)
		return;

	if(m_InputState.dwPacketNumber == m_dwPacketNumber)
		return; // nothing new
}

