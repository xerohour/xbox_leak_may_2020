//
//	renderer.cpp
//
///////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2001, Pipeworks Software Inc.
//				All rights reserved
#include "precomp.h"
#include "renderer.h"
#include "xbs_app.h"

#ifdef STARTUPANIMATION
#include "mslogo.h"
extern "C" {
#include "av.h"
}
#endif // STARTUPANIMATION

#ifdef HARDWARE_CHECKSUM
extern "C" void __cdecl Ani_ChecksumCallback(DWORD dwContext);
#endif

IDirect3DDevice8 *gpd3dDev = NULL;

///////////////////////////////////////////////////////////////////////////////
void Renderer::Init()
{
	pD3D8 = NULL;
	pD3DDev8 = NULL;
}
///////////////////////////////////////////////////////////////////////////////
void Renderer::UnInit()
{
}
///////////////////////////////////////////////////////////////////////////////
bool Renderer::init(int width,int height)
{
	pD3D8 = Direct3DCreate8(D3D_SDK_VERSION);
	if(!pD3D8)
		return false;

	const int size_mul = 2;
	const int kick_mul = 2;
	pD3D8->SetPushBufferSize(size_mul * 512 * 1024, kick_mul * 32 * 1024);


	ZeroMemory( &d3dPresentParams, sizeof(d3dPresentParams) );
    d3dPresentParams.BackBufferWidth        = width;
    d3dPresentParams.BackBufferHeight       = height;
    d3dPresentParams.BackBufferFormat       = D3DFMT_A8R8G8B8;
    d3dPresentParams.BackBufferCount        = 1;
    d3dPresentParams.EnableAutoDepthStencil = TRUE;
    d3dPresentParams.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dPresentParams.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	d3dPresentParams.MultiSampleType        = D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR; 
//	d3dPresentParams.MultiSampleType        = D3DMULTISAMPLE_NONE; 

	// No VSync.
//	d3dPresentParams.FullScreen_PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

    if (gApp.bWideScreen)
    {
        d3dPresentParams.Flags |= D3DPRESENTFLAG_WIDESCREEN;
    }

#ifdef STARTUPANIMATION
    // Always do interlaced output on an HDTV.
    ULONG AvInfo;
    AvSendTVEncoderOption(NULL, AV_QUERY_AV_CAPABILITIES, 0, &AvInfo);

    if ((AvInfo & AV_PACK_MASK) == AV_PACK_HDTV)
    {
        d3dPresentParams.Flags |= D3DPRESENTFLAG_INTERLACED;
    }
#endif 
    
	if( pD3D8->CreateDevice(D3DADAPTER_DEFAULT,
		                    D3DDEVTYPE_HAL,
						    NULL,
						    D3DCREATE_HARDWARE_VERTEXPROCESSING,
						    &d3dPresentParams,
						    &pD3DDev8 ) != D3D_OK )
	{
#ifndef STARTUPANIMATION
		OutputDebugString("Could not initalize Xbox Video!\n");
#endif // STARTUPANIMATION
		return false;
	}

	for( int i=0; i<4; i++ )
	{
		pD3DDev8->SetTextureStageState( i, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		pD3DDev8->SetTextureStageState( i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		pD3DDev8->SetTextureStageState( i, D3DTSS_MIPFILTER, D3DTEXF_NONE );

		pD3DDev8->SetTextureStageState( i, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
		pD3DDev8->SetTextureStageState( i, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
		pD3DDev8->SetTextureStageState( i, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP );

		pD3DDev8->SetTextureStageState( i, D3DTSS_MAXANISOTROPY,4);
	}

	gpd3dDev = pD3DDev8;

#ifdef STARTUPANIMATION

    gpd3dDev->Clear(0,NULL,
                    D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET,
                    0x00000000,
                    1.f,
                    0 );

    gpd3dDev->Present(NULL,NULL,NULL,NULL);

    // This delay should allow the TV enough time to lock and ensure
    // that the picture doesn't roll on the first few frames of the
    // animation.

    for (i = 0; i < 4; i++)
    {
        gpd3dDev->BlockUntilVerticalBlank();
    }

#endif // STARTUPANIMATION

#ifdef HARDWARE_CHECKSUM
    gpd3dDev->SetVerticalBlankCallback(Ani_ChecksumCallback);
#endif

	return true;
}
///////////////////////////////////////////////////////////////////////////////
void Renderer::shutdown()
{
	if( pD3DDev8 )
	{
		pD3DDev8->Release();
		gpd3dDev = NULL;
	}

	if( pD3D8 )
	{
		pD3D8->Release();
	}
}

