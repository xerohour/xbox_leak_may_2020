/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	linebug.cpp

Abstract:

	Demonstrates the line drawing bug

Author:

	Robert Heitkamp (robheit) 13-Apr-2001

Environment:

	Xbox only

Revision History:

	13-Apr-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <xtl.h>

//------------------------------------------------------------------------------
//	Vertex formats
//------------------------------------------------------------------------------
struct LINE_VERTEX
{
	float		x, y, z, rhw;
	D3DCOLOR	color;
};
#define FVF_LINE_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

//------------------------------------------------------------------------------
//	main
//------------------------------------------------------------------------------
void __cdecl 
main(void)
/*++

Routine Description:

	Main routine. Initializes, loops and draws

Arguments:

	None

Return Value:

	None

--*/
{
	D3DPRESENT_PARAMETERS	d3dpp;
	float					lw			= 1.0f;
	IDirect3D8*				d3d			= NULL;
	IDirect3DDevice8*		device		= NULL;
	LINE_VERTEX				verts[5]	= 
	{
		{  10.0f,  10.0f, 0.0f, 1.0f, 0x00ff0000 },
		{ 100.0f,  10.0f, 0.0f, 1.0f, 0x00ff0000 },
		{ 100.0f, 100.0f, 0.0f, 1.0f, 0x00ff0000 },
		{  10.0f, 100.0f, 0.0f, 1.0f, 0x00ff0000 },
		{  10.0f,  10.0f, 0.0f, 1.0f, 0x00ff0000 }
	};

	// Initialize
	d3d = Direct3DCreate8(D3D_SDK_VERSION);
	if(d3d == NULL)
		return;	

	// Setup the present parameters
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.BackBufferWidth					= 640;
	d3dpp.BackBufferHeight					= 480;
	d3dpp.BackBufferFormat					= D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount					= 1;
	d3dpp.WideScreen						= FALSE;
	d3dpp.MultiSampleType					= D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect						= D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow						= NULL;
	d3dpp.Windowed							= FALSE;
	d3dpp.EnableAutoDepthStencil			= TRUE;
	d3dpp.AutoDepthStencilFormat			= D3DFMT_D24S8;
	d3dpp.Flags								= 0;
	d3dpp.FullScreen_RefreshRateInHz		= D3DPRESENT_RATE_DEFAULT;
	d3dpp.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_ONE;

	// Create the device
	if(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, 
						 D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, 
						 &device) != D3D_OK)
	{
		d3d->Release();
		return;
	}

	// Setup a few device parameters
	device->SetRenderState(D3DRS_LINEWIDTH, *(DWORD*)&lw);

	while(1)
	{
		// Clear the viewport
		device->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);

		// Begin the scene
		device->BeginScene();

		// Set the vertex shader for the lines
		device->SetVertexShader(FVF_LINE_VERTEX);

		// Draw the lines
		device->DrawPrimitiveUP(D3DPT_LINELOOP, 4, verts, sizeof(LINE_VERTEX));

		// End the scene
		device->EndScene();

		// Update the screen
		device->Present(NULL, NULL, NULL, NULL);
	}
}
