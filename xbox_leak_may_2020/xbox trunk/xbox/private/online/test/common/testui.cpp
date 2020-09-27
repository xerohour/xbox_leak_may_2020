/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Module implementing unit test UI components

Module Name:

    testui.c

--*/

#include <xtl.h>
#include "testui.h"

#define FILE_DATA_DEFAULT_FONT_FILENAME         L"t:\\tahoma.ttf"

//
// Globals for graphics
//
LPDIRECT3DDEVICE8	g_D3d = NULL; // Our rendering device

//
// Globals for text-mode screen
//
const DWORD ScreenWidth = 640;
const DWORD ScreenHeight = 480;
const DWORD XOriginOffset = 30;
const DWORD YOriginOffset = 30;
const DWORD	TextHeight = 16;
const DWORD LinesOfText = (ScreenHeight / TextHeight);

DWORD g_TextVerticalPosition = 0;

//
// API to initialize D3D8 graphics
//
HRESULT WINAPI InitializeGraphics()
{
	HRESULT					hr;
	LPDIRECT3D8				D3dCreate	 = NULL; // Used to create the D3DDevice
	D3DDISPLAYMODE			D3ddm;
	D3DPRESENT_PARAMETERS	D3dpp;

	// Create the D3D object.
		// Register the window class
	D3dCreate = Direct3DCreate8(D3D_SDK_VERSION);
	if (!D3dCreate)
		return(E_FAIL);

	// Get the current desktop display mode, so we can set up a back
	// buffer of the same format
	hr = D3dCreate->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &D3ddm);
	if (FAILED(hr))
		return(hr);

	// Set up the structure used to create the D3DDevice
	ZeroMemory(&D3dpp, sizeof(D3dpp));
	D3dpp.BackBufferWidth        = ScreenWidth;
	D3dpp.BackBufferHeight       = ScreenHeight;
	D3dpp.BackBufferFormat       = D3DFMT_A8R8G8B8;
	D3dpp.BackBufferCount        = 1;
	D3dpp.EnableAutoDepthStencil = TRUE;
	D3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	D3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

	// Create the D3DDevice
	hr = D3dCreate->CreateDevice(0, D3DDEVTYPE_HAL, NULL,
				D3DCREATE_HARDWARE_VERTEXPROCESSING,
				&D3dpp, &g_D3d);
	if (FAILED(hr))
		return(hr);				

	// Turn off culling, so we see the front and back of the triangle
	g_D3d->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Turn off D3D lighting, since we are providing our own vertex colors
	g_D3d->SetRenderState(D3DRS_LIGHTING, FALSE);

	return(S_OK);
}

XFONT				*xfont;
IDirect3DSurface8*	D3dBackBuffer;

//
// API to initialize Font support
//
HRESULT WINAPI InitializeFont(
			D3DCOLOR	TextColor,
			D3DCOLOR	BackgroundColor,
			int			iBkMode	// XFONT_OPAQUE or XFONTTRANSPARENT
			)
{
	HRESULT				hr;
	
	hr = XFONT_OpenTrueTypeFont(FILE_DATA_DEFAULT_FONT_FILENAME, 4096, &xfont);
	if (FAILED(hr))
		return(hr);

	if (!xfont)
		return(E_FAIL);

	hr = g_D3d->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &D3dBackBuffer); 
	if (FAILED(hr))
		return(hr);

	XFONT_SetTextColor(xfont, TextColor);
	XFONT_SetBkColor(xfont, BackgroundColor);
	XFONT_SetBkMode(xfont, iBkMode);

	return(S_OK);
}

//
// API to start a fresh rendering scene
//
VOID WINAPI BeginNewScene()
{
    D3DXMATRIX	ViewMatrix;
    D3DXMATRIX	ProjMatrix;

	g_D3d->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);

	g_D3d->BeginScene();

	// Set up our view matrix. A view matrix can be defined given an eye point,
	// a point to lookat, and a direction for which way is up. Here, we set the
	// eye five units back along the z-axis and up three units, look at the
	// origin, and define "up" to be in the y-direction.
	D3DXMatrixLookAtLH(&ViewMatrix,
				&D3DXVECTOR3( 4.0f, 4.0f,-10.0f ),
				&D3DXVECTOR3( 4.0f, 4.0f, 0.0f ),
				&D3DXVECTOR3( 0.0f, 1.0f, 0.0f ));
	g_D3d->SetTransform(D3DTS_VIEW, &ViewMatrix);

	// For the projection matrix, we set up a perspective transform (which
	// transforms geometry from 3D view space to 2D viewport space, with
	// a perspective divide making objects smaller in the distance). To build
	// a perpsective transform, we need the field of view (1/4 pi is common),
	// the aspect ratio, and the near and far clipping planes (which define at
	// what distances geometry should be no longer be rendered).
	D3DXMatrixPerspectiveFovLH(&ProjMatrix, 
				1.3f*D3DX_PI/4, 1.0f, 1.0f, 100.0f);
	g_D3d->SetTransform(D3DTS_PROJECTION, &ProjMatrix);

	// Emulate a CLS
	g_TextVerticalPosition = 0;
}

//
// API to render and display a scene
//
VOID WINAPI PresentScene()
{
	g_D3d->EndScene();

	g_D3d->Present(NULL, NULL, NULL, NULL);
}

//
// API to print a string to the screen as if the screen is in text mode
// Sorry, no scrolling
//
int WINAPI Xputs(WCHAR *wszString)
{
	WCHAR *pwcBegin = wszString; 
	
	while ((*wszString) && (g_TextVerticalPosition < LinesOfText))
	{
		if ((*wszString == L'\r') || (*wszString == L'\n'))
		{
			*wszString++ = L'\0';
			XFONT_TextOut(xfont, D3dBackBuffer, pwcBegin, -1, XOriginOffset, 
						YOriginOffset + (g_TextVerticalPosition * TextHeight));
			g_TextVerticalPosition++;

			while ((*wszString == L'\r') || (*wszString == L'\n'))
				*wszString++;
			pwcBegin = wszString;
		}
		else
			wszString++;
	}

	if ((g_TextVerticalPosition < LinesOfText) &&
		(!*wszString) && (wszString > pwcBegin))
	{
		// Print the last line
		XFONT_TextOut(xfont, D3dBackBuffer, pwcBegin, -1, XOriginOffset, 
					YOriginOffset + (g_TextVerticalPosition * TextHeight));
		g_TextVerticalPosition++;
	}

	return(1);
}


