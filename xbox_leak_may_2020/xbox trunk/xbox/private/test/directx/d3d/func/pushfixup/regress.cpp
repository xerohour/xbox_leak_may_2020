/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       regress.cpp
 *  Content:    Main file for the D3D regression test
 *
 ***************************************************************************/
 
#include "precomp.hpp"

IDirect3DDevice8* g_pDev = NULL;
IDirect3D8* g_pD3D = NULL;

//------------------------------------------------------------------------------
bool InitD3D()
{
#if DBG
 //   D3D__Parser = TRUE;
#endif

    g_pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if (g_pD3D == NULL)
        return false;

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth           = 640;
    d3dpp.BackBufferHeight          = 480;
    d3dpp.BackBufferFormat          = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount           = 1;
    d3dpp.Windowed                  = false;   // Must be false for Xbox.
    d3dpp.EnableAutoDepthStencil    = true;
    d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;
    d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_RefreshRateInHz= 60;
    d3dpp.MultiSampleType           = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;

    if (g_pD3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        NULL,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &g_pDev) != S_OK)
    {
        return false;
    }

    g_pD3D->Release();

    return true;
}

bool DeinitD3D()
{

	g_pDev->Release();

    return true;
}

//------------------------------------------------------------------------------
HRESULT __cdecl PushFixup_Main()
{
	HANDLE hMutex;
	int i;

	hMutex = CreateMutex(NULL, FALSE, "XBox Direct3D Test");
    if (WaitForSingleObject(hMutex, INFINITE) != WAIT_OBJECT_0) {
         CloseHandle(hMutex);
         return E_FAIL;
    }

	DWORD StartTime = GetTickCount();

	do {
		for(i = 0; i < 16; i++) {
			InitD3D();
			TestPushBuffers();
			DeinitD3D();
		}
		Sleep(500);
	} while ((GetTickCount() - StartTime) < 1000 * 60 * 60 * 14); //repeat for 14 hours

    CloseHandle(hMutex);
	return S_OK;
}
