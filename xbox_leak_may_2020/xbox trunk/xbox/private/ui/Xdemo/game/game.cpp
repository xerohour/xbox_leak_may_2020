//=============================================================================
// File: XDemos.cpp
//
// Desc: This is the main module for the XDemos project
// Created: 07/02/2001 by Michael Lyons (mlyons@microsoft.com)
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//=============================================================================
#include <xtl.h>
#include <assert.h>
#include "XDemos\Image\XImage.h"


//=============================================================================
//=============================================================================
#define WIDTH	640
#define HEIGHT	480

//=============================================================================
// Global variables
//=============================================================================
LPDIRECT3D8				g_pD3D			= NULL;
LPDIRECT3DDEVICE8		g_pd3dDevice	= NULL;
LPDIRECT3DTEXTURE8		g_pTexture		= NULL;
HANDLE					g_hInput[12]	= {0};
XImage					g_Background;
DWORD					g_dwDeviceState	= 0;

//=============================================================================
//=============================================================================
HRESULT InitD3D()
{
    // Create the D3D object, which is used to create the D3DDevice.
    if( NULL == ( g_pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );

    // Set fullscreen 640x480x32 mode
    d3dpp.BackBufferWidth        = WIDTH;
    d3dpp.BackBufferHeight       = HEIGHT;
    d3dpp.BackBufferFormat       = D3DFMT_A8R8G8B8;

    // Create one backbuffer and a zbuffer
    d3dpp.BackBufferCount        = 1;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    // Set up how the backbuffer is "presented" to the frontbuffer each frame
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

    // Create the Direct3D device. Hardware vertex processing is specified
    // since all vertex processing takes place on Xbox hardware.
    if( FAILED( g_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
        return E_FAIL;

    // After creating the device, initial state would normally be set

    return S_OK;
}



//=============================================================================
//=============================================================================
HRESULT InitBackground(void)
{
	DWORD dwLaunchType;
	LAUNCH_DATA ld;

	// were we launched by XDemos?
	if (XGetLaunchInfo(&dwLaunchType, &ld) == ERROR_SUCCESS)
	{
		if ((dwLaunchType == LDT_TITLE) && (!strcmp((char *)ld.Data, "XDEMOS")))
			g_Background.Load(g_pd3dDevice, "D:\\game2.png");
		else
			g_Background.Load(g_pd3dDevice, "D:\\game.png");
	}
	else
		g_Background.Load(g_pd3dDevice, "D:\\game.png");


	return S_OK;
}

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
void HandleInput(void)
{
	DWORD				dwInsert;
	DWORD				dwRemove;
	XINPUT_STATE		xis;
	static XINPUT_STATE	xis_old[12] = {0};

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
				XInputClose( g_hInput[i] );
				g_hInput[i] = NULL;
			}

			if( dwInsert & (1<<i) ) 
			{
				g_hInput[i] = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL );
			}

		}

	}


	HANDLE hPad;

	int n;
	hPad=GetController(true, &n);

	while (hPad)
	{

		XInputGetState(hPad, &xis);

		// only process messages if they're different

		if ((xis.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y]) && (!xis_old[n].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y]))
		{
			// launch XDemos here
			LAUNCH_DATA ld;
			XLaunchNewImage("d:\\XDemos\\XDemos.xbe", &ld);
		}

		if ((xis.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A]) && (!xis_old[n].Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A]))
		{
			// launch XDemos here
			LAUNCH_DATA ld;
			XLaunchNewImage("d:\\XDemos\\XDemos.xbe", &ld);
		}

		xis_old[n] = xis;

		hPad=GetController(false, &n);

	}
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

	g_hInput[0]		= XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL);
	g_hInput[1]		= XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT1, XDEVICE_NO_SLOT, NULL);
	g_hInput[2]		= XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT2, XDEVICE_NO_SLOT, NULL);
	g_hInput[3]		= XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT3, XDEVICE_NO_SLOT, NULL);

	return S_OK;
}


//=============================================================================
//=============================================================================
VOID Render()
{
	// Cycle through some colors to clear the screen (just to show some output)
	static FLOAT r = 0.0f; if( (r+=1.3f) > 255.0f ) r = 0.0f;
	static FLOAT g = 0.0f; if( (g+=1.7f) > 255.0f ) g = 0.0f;
	static FLOAT b = 0.0f; if( (b+=1.5f) > 255.0f ) b = 0.0f;

	// Clear the backbuffer to a changing color
	//g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( (int)r, (int)g, (int)b ), 1.0f, 0L );
	g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L );

	// Begin the scene
	g_pd3dDevice->BeginScene();

	g_Background.Blt();

    // End the scene
    g_pd3dDevice->EndScene();
}


//=============================================================================
//=============================================================================
void __cdecl main()
{
    if (FAILED(InitD3D()))
        return;

	if (FAILED(InitBackground()))
		return;

	if (FAILED(InitInput()))
		return;

    while (true)
    {
		HandleInput();
        Render();
        g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    }
}




