//=============================================================================
// File: Input.cpp
//
// Desc: This is the input module for the XDemos project
// Created: 07/23/2001 by Michael Lyons (mlyons@microsoft.com)
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//=============================================================================
#include "XDemos.h"

#define STICK_THRESHHOLD	15000
//=============================================================================
DWORD					g_dwDeviceState	= 0;
HANDLE					g_hInput[12]	= {0};
static XINPUT_STATE		xis_old[12] = {0};
static bool				xis_received[12] = {0};

//=============================================================================
//=============================================================================
void HandleDeviceChanges(XPP_DEVICE_TYPE *pxdt, DWORD dwInsert, DWORD dwRemove)
{

	return;
}

//=============================================================================
//=============================================================================
HANDLE GetController(bool bFirst, int *pnum=NULL)
{
	static int i=0;

	if (bFirst)
		i=0;

	for ( ; i<12 ; i++)
	{
		if (g_dwDeviceState & (1<<i))
		{
			if (pnum)
			{
				*pnum = i;
			}
			return g_hInput[i++];
		}
	}

	return NULL;
}

//=============================================================================
//=============================================================================
void SaveState(int dw1, int dw2, int dw3)
{
	HANDLE hFile = CreateFile(g_pszSettingsFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwSize;

		WriteFile(hFile, &dw1, sizeof(dw1), &dwSize, NULL);
		WriteFile(hFile, &dw2, sizeof(dw2), &dwSize, NULL);
		WriteFile(hFile, &dw3, sizeof(dw3), &dwSize, NULL);

		CloseHandle(hFile);
		return;
	}

	return;
}

//=============================================================================
//=============================================================================
void GetState(int &dw1, int &dw2, int &dw3)
{
	HANDLE hFile = CreateFile(g_pszSettingsFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwSize;

		ReadFile(hFile, &dw1, sizeof(dw1), &dwSize, NULL);
		ReadFile(hFile, &dw2, sizeof(dw2), &dwSize, NULL);
		ReadFile(hFile, &dw3, sizeof(dw3), &dwSize, NULL);

		CloseHandle(hFile);
		return;
	}

	dw1=0;
	dw2=0;
}

//=============================================================================
//=============================================================================
void HandleInput(void)
{
	DWORD				dwInsert;
	DWORD				dwRemove;
	XINPUT_STATE		xis;

	if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsert, &dwRemove))
	{
		HandleDeviceChanges(XDEVICE_TYPE_GAMEPAD, dwInsert, dwRemove);

		g_dwDeviceState &= ~dwRemove;
		g_dwDeviceState |= dwInsert;

		for( DWORD i=0; i < XGetPortCount(); i++ )
		{
			// Handle removed devices.
			if( dwRemove & (1<<i) )
			{
				if (g_hInput[i])
				{
					XInputClose( g_hInput[i] );
					g_hInput[i] = NULL;
				}
			}

			if( dwInsert & (1<<i) )
			{
				if (!g_hInput[i])
					g_hInput[i] = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL );

				int err;
				if (g_hInput[i] == NULL)
				{
					err=GetLastError();
				}
				assert(g_hInput[i]);
			}

		}

	}


	HANDLE hPad;

	int n;
	hPad=GetController(true, &n);
	bool bButton=false;

	while (hPad)
	{

		XInputGetState(hPad, &xis);

		// now, make the analog thumb sticks either all or nothing
		if (xis.Gamepad.sThumbLY > STICK_THRESHHOLD)
			xis.Gamepad.sThumbLY = STICK_THRESHHOLD;
		else if (xis.Gamepad.sThumbLY < -STICK_THRESHHOLD)
			xis.Gamepad.sThumbLY = -STICK_THRESHHOLD;
		else
			xis.Gamepad.sThumbLY = 0;

		if (xis.Gamepad.sThumbRY > STICK_THRESHHOLD)
			xis.Gamepad.sThumbRY = STICK_THRESHHOLD;
		else if (xis.Gamepad.sThumbRY < -STICK_THRESHHOLD)
			xis.Gamepad.sThumbRY = -STICK_THRESHHOLD;
		else
			xis.Gamepad.sThumbRY = 0;

		// make sure we've received input from this controller before,
		// if not, don't process the input yet!
		if (xis_received[n])
		{
			// only process messages if they're different
			if (
				((xis.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A]) && (!xis_old[n].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A]))
				||
				((xis.Gamepad.wButtons & XINPUT_GAMEPAD_START) && !((xis_old[n].Gamepad.wButtons & XINPUT_GAMEPAD_START)))
				)
			{
				Input();
				ButtonA();
			}

			if ((xis.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B]) && (!xis_old[n].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B]))
			{
				Input();
				ButtonB();
			}

			if (xis.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
			{
				bButton = true;
				Input();
				ButtonUp();
			}

			if (xis.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			{
				bButton = true;
				Input();
				ButtonDn();
			}

			if (xis.Gamepad.sThumbLY == STICK_THRESHHOLD)
			{
				bButton = true;
				Input();
				ButtonUp();
			}

			if (xis.Gamepad.sThumbLY == -STICK_THRESHHOLD)
			{
				bButton = true;
				Input();
				ButtonDn();
			}

			if (xis.Gamepad.sThumbRY == STICK_THRESHHOLD)
			{
				bButton = true;
				Input();
				ButtonUp();
			}

			if (xis.Gamepad.sThumbRY == -STICK_THRESHHOLD)
			{
				bButton = true;
				Input();
				ButtonDn();
			}

		}
		else
		{
			xis_received[n] = true;
		}

		xis_old[n] = xis;

		hPad=GetController(false, &n);
	}

	if (!bButton)
		ButtonMiddle();

}

//=============================================================================
//=============================================================================
HRESULT InitInput(void)
{
	XDEVICE_PREALLOC_TYPE xdpt[] = {
		{XDEVICE_TYPE_GAMEPAD, 4},
	};

	XInitDevices(sizeof(xdpt)/sizeof(xdpt[0]), xdpt);

	g_dwDeviceState = XGetDevices(XDEVICE_TYPE_GAMEPAD);

	for( DWORD i=0; i<4 ; i++ )
	{
		if( g_dwDeviceState & (1<<i) )
		{
			if (!g_hInput[i])
				g_hInput[i] = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL );
		}
	}

	return S_OK;
}



//=============================================================================
//=============================================================================
bool CheckForInput(void)
{
	DWORD				dwInsert;
	DWORD				dwRemove;
	XINPUT_STATE		xis;

	if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsert, &dwRemove))
	{
		HandleDeviceChanges(XDEVICE_TYPE_GAMEPAD, dwInsert, dwRemove);

		g_dwDeviceState &= ~dwRemove;
		g_dwDeviceState |= dwInsert;

		for( DWORD i=0; i < XGetPortCount(); i++ )
		{
			// Handle removed devices.
			if( dwRemove & (1<<i) )
			{
				if (g_hInput[i])
				{
					XInputClose( g_hInput[i] );
					g_hInput[i] = NULL;
				}
			}

			if( dwInsert & (1<<i) )
			{
				if (!g_hInput[i])
					g_hInput[i] = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL );

				int err;
				if (g_hInput[i] == NULL)
				{
					err=GetLastError();
				}
				assert(g_hInput[i]);
			}

		}

	}


	HANDLE hPad;

	int n;
	hPad=GetController(true, &n);

	while (hPad)
	{
		XInputGetState(hPad, &xis);

		// now, make the analog thumb sticks either all or nothing
		if (xis.Gamepad.sThumbLY > STICK_THRESHHOLD)
			xis.Gamepad.sThumbLY = STICK_THRESHHOLD;
		else if (xis.Gamepad.sThumbLY < -STICK_THRESHHOLD)
			xis.Gamepad.sThumbLY = -STICK_THRESHHOLD;
		else
			xis.Gamepad.sThumbLY = 0;

		if (xis.Gamepad.sThumbRY > STICK_THRESHHOLD)
			xis.Gamepad.sThumbRY = STICK_THRESHHOLD;
		else if (xis.Gamepad.sThumbRY < -STICK_THRESHHOLD)
			xis.Gamepad.sThumbRY = -STICK_THRESHHOLD;
		else
			xis.Gamepad.sThumbRY = 0;



		// only process messages if they're different

		if (
			((xis.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A]) && (!xis_old[n].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A]))
			||
			((xis.Gamepad.wButtons & XINPUT_GAMEPAD_START) && !((xis_old[n].Gamepad.wButtons & XINPUT_GAMEPAD_START)))
			)
		{
			xis_old[n] = xis;
			return true;
		}

		if ((xis.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B]) && (!xis_old[n].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B]))
		{
		}

		xis_old[n] = xis;

		hPad=GetController(false, &n);
	}

	return false;
}
