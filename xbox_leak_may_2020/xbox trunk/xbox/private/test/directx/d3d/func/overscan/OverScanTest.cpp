/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	OverScanTest.cpp

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 18-Dec-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	18-Dec-2001 jeffsul
		Initial Version

--*/

#include "OverScanTest.h"

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <macros.h>
#include <xgmath.h>
#include <medialoader.h>

#include <stdio.h>
#include <tchar.h>

COverScanTest::COverScanTest()
{
	m_pD3DDevice		=	NULL;
	m_hInputDevice		=	INVALID_HANDLE_VALUE;
	ZeroMemory( &m_PrevInputState, sizeof(XINPUT_STATE) );
	m_OverScanColor = D3DCOLOR_ARGB( 0, 0, 0, 0 );

}


COverScanTest::~COverScanTest()
{
}


HRESULT
COverScanTest::InitGraphics()
{
	HRESULT				hr			= S_OK;

	EXECUTE( InitD3DDevice( &m_pD3DDevice ) );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	DWORD				dwSuccess;
	DWORD				dwLaunchType;
	LAUNCH_DATA			LaunchData;
	dwSuccess = XGetLaunchInfo( &dwLaunchType, &LaunchData );
	if ( LDT_TITLE != dwLaunchType )
	{
		m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_STENCIL|D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, m_OverScanColor, 1.0f, 0L );
		m_pD3DDevice->Present( NULL, NULL, NULL, NULL );
		XLaunchNewImage( "d:\\overscan.xbe", &LaunchData );
	}

	return hr;
}


HRESULT
COverScanTest::DestroyGraphics()
{
	HRESULT				hr			= S_OK;

	RELEASE( m_pD3DDevice );

	return hr;
}


HRESULT
COverScanTest::InitResources()
{
	HRESULT				hr			= S_OK;

	return hr;
}


HRESULT
COverScanTest::DestroyResources()
{
	HRESULT				hr			= S_OK;

	return hr;
}


HRESULT
COverScanTest::ProcessInput()
{
	HRESULT				hr			= S_OK;

	XINPUT_STATE		XInputState;
	ZeroMemory(	&XInputState, sizeof(XINPUT_STATE) );

	hr = GetNextInputState( &m_hInputDevice, &XInputState );

	// a button pressed, change overscan color
	if ( XInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] > 15 && m_PrevInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] <= 15 )
	{
		D3DCOLOR	Color;
		SHORT		A, R, G, B;
		SHORT		A2, R2, G2, B2;
		Color = D3DDevice_GetOverscanColor();
		A = (SHORT)((Color & 0xff000000) >> 24);
		R = (SHORT)((Color & 0xff0000) >> 16);
		G = (SHORT)((Color & 0xff00) >> 8);
		B = (SHORT)(Color & 0xff);
		A2 = (SHORT)((m_OverScanColor & 0xff000000) >> 24);
		R2 = (SHORT)((m_OverScanColor & 0xff0000) >> 16);
		G2 = (SHORT)((m_OverScanColor & 0xff00) >> 8);
		B2 = (SHORT)( m_OverScanColor & 0xff);
		if ( Color != m_OverScanColor )
		{
			DbgPrint( "Wrong overscan color.  Expected %u, got %u.\n", m_OverScanColor, Color );
			DbgPrint( "A: %x:%x\tR: %x:%x\tG: %x:%x\tB: %x:%x\n", A, A2, R, R2, G, G2, B, B2 );
		}
		m_OverScanColor = D3DCOLOR_ARGB( rand()%256, rand()%256, rand()%256, rand()%256 );
		D3DDevice_SetOverscanColor( m_OverScanColor );
	}

	// b button pressed, reboot to same app
	if ( XInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] > 15 && m_PrevInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] <= 15 )
	{
		XLaunchNewImage( "d:\\overscan.xbe", NULL );
	}

	memcpy( &m_PrevInputState, &XInputState, sizeof(XINPUT_STATE) );

	return hr;
}


HRESULT
COverScanTest::Update()
{
	HRESULT				hr			= S_OK;

	return hr;
}


HRESULT
COverScanTest::Render()
{
	HRESULT				hr			= S_OK;

	return hr;
}