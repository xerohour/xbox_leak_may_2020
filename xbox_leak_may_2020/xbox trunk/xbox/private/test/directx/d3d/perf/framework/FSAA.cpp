/*--
Copyright (c) 1999 - 2000 Microsoft Corporation - Xbox SDK

Module Name:

    Main.cpp

Abstract:

    Illustrates effects of various FSAA modes

Revision History:

    Derived from a DX8 sample.
--*/

//#include "SDKCommon.h"
#include <xtl.h>
#include <xdbg.h>
#include <xtestlib.h>
#include <xlog.h>
#include <macros.h>
#include <xgraphics.h>
#include <TCHAR.h>
#include <WCHAR.h>
#include <stdio.h>

#include "Fsaa.h"

INT AddModulus(INT Value, INT Increment, INT Max);

CFsaa::CFsaa()
{
	m_pD3D = NULL;
	m_pDevice = NULL;                  // our device
	m_pVB = NULL;                   // our vertex buffer
	
	m_wLoops = GetPrivateProfileIntA( "FSAA", "Loops", 1, FSAA_INI_PATH );;
	m_wLoop = 0;
	m_dwStartLoopTime = 0;
	m_dwFrames;
	ReadIni();

	m_dwFVF = D3DFVF_XYZRHW|D3DFVF_DIFFUSE;
	
	m_hDevice = 0;      // handle of the gamepad
	m_bPause = FALSE;
	m_bUseReset = TRUE;  // Use Reset API instead of CreateDevice/Release
	m_bUpdate = FALSE;

	StateDescription AntialiasTypes[] =
	{
		D3DMULTISAMPLE_NONE,                                    L"NONE", 
			D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR,            L"2_SAMPLES_MULTISAMPLE_LINEAR",
			D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX,          L"2_SAMPELS_MULTISAMPLE_QUINCUNX",
			D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR, L"2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR",
			D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR,   L"2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR",
			D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR,            L"4_SAMPLES_MULTISAMPLE_LINEAR",
			D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN,          L"4_SAMPLES_MULTISAMPLE_GAUSSIAN",
			D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR,            L"4_SAMPLES_SUPERSAMPLE_LINEAR",
			D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN,          L"4_SAMPLES_SUPERSAMPLE_GAUSSIAN",
			D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN,          L"9_SAMPLES_MULTISAMPLE_GAUSSIAN",
			D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN,          L"9_SAMPLES_SUPERSAMPLE_GAUSSIAN",
	};
	
	StateDescription AntialiasFormats[] =
	{
		D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5,   L"PREFILTER_FORMAT_X1R5G5B5",
			D3DMULTISAMPLE_PREFILTER_FORMAT_R5G6B5,     L"PREFILTER_FORMAT_R5G6B5",
			D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8,   L"PREFILTER_FORMAT_X8R8G8B8",
			D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8,   L"PREFILTER_FORMAT_A8R8G8B8",
	};

	memcpy( &m_AntialiasTypes, AntialiasTypes, sizeof(m_AntialiasTypes) );
	memcpy( &m_AntialiasFormats, AntialiasFormats, sizeof(m_AntialiasFormats) );

//	m_AntialiasTypesCount = sizeof(m_AntialiasTypes) / sizeof(m_AntialiasTypes[0]);
//	m_AntialiasFormatsCount = sizeof(m_AntialiasFormats) / sizeof(m_AntialiasFormats[0]);
}

//------------------------------------------------------------------------------
HRESULT CFsaa::InitGraphics()
{
	HRESULT hr = S_OK;

    // Create D3D 8.
    if(m_pD3D == NULL)
    {
        m_pD3D = Direct3DCreate8(D3D_SDK_VERSION);
        if ( NULL == m_pD3D )
		{
            return E_FAIL;
		}
    }

    // Set the screen mode.
    D3DPRESENT_PARAMETERS d3dpp;
    InitPresentationParameters(&d3dpp);

    // Create the device.
    EXECUTE( m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
                             D3DDEVTYPE_HAL,
                             0,
                             D3DCREATE_HARDWARE_VERTEXPROCESSING,
                             &d3dpp,
                             &m_pDevice) )
    if ( FAILED ( hr ) )
	{
		return hr;
	}

    m_pDevice->SetFlickerFilter(m_iFlicker);

	if ( !InitGammaRamp() )
	{
		return E_FAIL;
	}

	m_dwStartLoopTime = timeGetTime();
	m_dwFrames = 0;

    return hr;
}

//------------------------------------------------------------------------------
VOID CFsaa::InitPresentationParameters(D3DPRESENT_PARAMETERS *ppp)
{
    ZeroMemory(ppp, sizeof(*ppp));

    ppp->BackBufferWidth           = 640;
    ppp->BackBufferHeight          = 480;
    ppp->BackBufferFormat          = D3DFMT_X8R8G8B8;
    ppp->BackBufferCount           = 2;
    ppp->Windowed                  = false;   // Must be false for Xbox.
    ppp->EnableAutoDepthStencil    = true;
    ppp->AutoDepthStencilFormat    = D3DFMT_D24S8;
    ppp->SwapEffect                = D3DSWAPEFFECT_DISCARD;
    ppp->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    if (m_iType != 0)
        ppp->MultiSampleType       = m_AntialiasTypes[m_iType].MultisampleFormat
                                    | m_AntialiasFormats[m_iFormat].MultisampleFormat;

/*    DbgPrint("Flicker: %li  FSAA type: %s | %s\n", 
             m_iFlicker,
             m_AntialiasTypes[m_iType].Text,
             m_AntialiasFormats[m_iFormat].Text);*/
}

//------------------------------------------------------------------------------
BOOL CFsaa::InitGammaRamp()
{
    D3DGAMMARAMP ramp;
    DWORD i;

    for (i = 0; i <= 255; i++)
    {
        FLOAT f = i / 255.0f;

        // sRGB is a gamma of 2.2, but we'll approximate with 2:

        f = (FLOAT) sqrt(f);

        // Normalize and round:

        BYTE b = (BYTE) (f * 255.0f + 0.5f);

        ramp.red[i] = b;
        ramp.green[i] = b;
        ramp.blue[i] = b;
    }

    m_pDevice->SetGammaRamp(0, &ramp);

	// only need to do this once as we are no longer taking input
	m_pDevice->SetFlickerFilter(m_iFlicker);

    return true;
}

//------------------------------------------------------------------------------
HRESULT CFsaa::DestroyGraphics()
{
	HRESULT hr = S_OK;

	DumpStats();

    RELEASE( m_pDevice )

    RELEASE( m_pD3D )

	return hr;
}

//------------------------------------------------------------------------------
HRESULT CFsaa::InitResources()
{
	HRESULT hr = S_OK;

	// Create the vertex buffer.
    void* pVerts;

    UINT v = 0;
    for(UINT i=0; i<NUMPRIMS; i++)
    {
        Verts[v].x = 320.0f;
        Verts[v].y = 240.0f;
        Verts[v].z = 0.5f;
        Verts[v].w = 1.0f;
        Verts[v].color = 0xffffffff;
        v++;

        Verts[v].x = (FLOAT)(320. + 320.*(cos(i*2.0*DR)));
        Verts[v].y = (FLOAT)(240. + 320.*(sin(i*2.0*DR)));
        Verts[v].z = 0.5f;
        Verts[v].w = 1.0f;
        Verts[v].color = 0xffffffff;
        v++;

        Verts[v].x = (FLOAT)(320. + 320.*(cos((i*2.0+1.0)*DR)));
        Verts[v].y = (FLOAT)(240. + 320.*(sin((i*2.0+1.0)*DR)));
        Verts[v].z = 0.5f;
        Verts[v].w = 1.0f;
        Verts[v].color = 0xffffffff;
        v++;
    }
    EXECUTE( m_pDevice->CreateVertexBuffer( sizeof(Verts), D3DUSAGE_WRITEONLY, m_dwFVF, D3DPOOL_MANAGED, &m_pVB ) )
    if ( FAILED ( hr ) )
	{
		return hr;
	}

    m_pVB->Lock( 0, sizeof(Verts), (BYTE **)(&pVerts), 0 );
    memcpy( (void*)pVerts, (void*)Verts, sizeof(Verts) );
    m_pVB->Unlock();

	return hr;
}

//------------------------------------------------------------------------------
HRESULT CFsaa::DestroyResources()
{
	HRESULT hr = S_OK;

	RELEASE( m_pVB );

	return hr;
}

//------------------------------------------------------------------------------
HRESULT CFsaa::ReadIni()
{
	HRESULT hr = S_OK;

	CHAR szSectionName[256];

	sprintf( szSectionName, "LOOP%d", m_wLoop );

	m_iType = GetPrivateProfileIntA( szSectionName, "Type", 0, FSAA_INI_PATH );
	m_iFormat = GetPrivateProfileIntA( szSectionName, "Format", 0, FSAA_INI_PATH );
	m_iFlicker = GetPrivateProfileIntA( szSectionName, "Flicker", 4, FSAA_INI_PATH );
	m_dwTime = GetPrivateProfileIntA( szSectionName, "Time", 60000, FSAA_INI_PATH );

	return hr;
}

//------------------------------------------------------------------------------
HRESULT CFsaa::ProcessInput()
{
	HRESULT hr = S_OK;

    DWORD dwInsertions, dwRemovals;
    //INT key = -1;
    
    //
    //  Check to see if a port 0 gamepad has come or gone.
    //
    if(XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals))
    {
        if(dwRemovals&XDEVICE_PORT0_MASK)
        {
            if(m_hDevice)
            {
                XInputClose(m_hDevice);
                m_hDevice = NULL;
            }
        }
        if(dwInsertions&XDEVICE_PORT0_MASK)
        {
            m_hDevice = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL);
        }
    }

#if 0
    // If we have a device, get its state.
    if (m_hDevice)
    {
        XINPUT_STATE InputState;

        XInputGetState(m_hDevice, &InputState);
        // check to see if we need to change modes
        for (UINT i = 0; i < 6; i++)
        {
            if ((InputState.Gamepad.bAnalogButtons[i] == 0) != 
                (m_PreviousState[i] == 0))
            {
                m_bKeyDown[i] = !m_bKeyDown[i];
                if (m_bKeyDown[i])
                    key = i;
            }
            m_PreviousState[i] = InputState.Gamepad.bAnalogButtons[i];
        }

		if (key == XINPUT_GAMEPAD_BLACK)
		{
			m_bPause = !m_bPause;
			m_bUpdate = TRUE;
		}
		else if (key == XINPUT_GAMEPAD_A)
		{
			m_iType = AddModulus(m_iType, 1, FSAA_NUM_AA_TYPES);
			m_bUpdate = FALSE;
		}
		else if (key == XINPUT_GAMEPAD_B)
		{
			m_iType = AddModulus(m_iType, -1, FSAA_NUM_AA_TYPES);
			m_bUpdate = FALSE;
		}
		else if (key == XINPUT_GAMEPAD_X)
		{
			m_iFlicker = AddModulus(m_iFlicker, 1, 5);
			m_bUpdate = FALSE;
		}
		else if (key == XINPUT_GAMEPAD_Y)
		{
			m_iFlicker = AddModulus(m_iFlicker, -1, 5);
			m_bUpdate = FALSE;
		}
    }
#endif 0

	return hr;
}


//------------------------------------------------------------------------------
HRESULT CFsaa::Update()
{
	HRESULT hr = S_OK;

#if 0
	D3DPRESENT_PARAMETERS pp;

	if ( m_bUpdate )
	{
		
		m_pDevice->PersistDisplay();
		
		if (m_bUseReset)
		{
			InitPresentationParameters(&pp);
			if (m_pDevice->Reset(&pp) != S_OK)
			{
				DbgPrint( "Could not Reset\n");
				_asm int 3;
			}
		}
		else
		{
			DestroyGraphics();
			if ( FAILED( InitGraphics() ) )
			{
				DbgPrint( "Could not initialize\n");
				_asm int 3;
			}
		}
		
		m_pDevice->SetFlickerFilter(m_iFlicker);
	}
#endif 0

	if ( m_dwCurrentTime-m_dwStartLoopTime > m_dwTime ) // run for time specified in ini file
	{
		m_wLoop++;
		if ( m_wLoop >= m_wLoops )
		{
			m_bQuit = TRUE;
		}
		else
		{
			EXECUTE( DestroyGraphics() );
			EXECUTE( ReadIni() );
			EXECUTE( InitGraphics() );
			if ( FAILED( hr ) )
			{
				return hr;
			}
		}
	}
	
	return hr;
}


//------------------------------------------------------------------------------
HRESULT CFsaa::Render()
{
	HRESULT hr = S_OK;

    DWORD minFilter;
    DWORD magFilter;

    // Exercise the synchornization code.
    TheVerts* pVerts;
    
    m_pVB->Lock( 0, sizeof(Verts), (BYTE **)(&pVerts), 0 );

    // Transform the verticies to make the triangle spin so we can verify that
    // this actually works over a period of time.
    //
    static DWORD Time = GetTickCount();
    static DWORD CurrentTime;
     
    // If paused, just use the same time as last time:
    if (!m_bPause)
        CurrentTime = GetTickCount();

    // 180 seconds per rotation
    float spin = 2 * 3.14159f * (float)(CurrentTime - Time) / 180000.0f;

    DWORD i;

    for (i = 0; i < NUMPRIMS*3; i++)
    {

        float x = Verts[i].x;
        float y = Verts[i].y;

        x = x - 320.0f;
        y = y - 240.0f;

        pVerts[i].x = x * (float)cos(spin) - y * (float)sin(spin);
        pVerts[i].y = x * (float)sin(spin) + y * (float)cos(spin);

        pVerts[i].x += 320.0f;
        pVerts[i].y += 240.0f;
    }

    m_pVB->Unlock();

    // Clear the frame buffer, Zbuffer.
    m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0X00000000, 1.0, 0);

    // Draw the vertex streamsetup in stream 0.
    m_pDevice->BeginScene();

    m_pDevice->SetStreamSource( 0, m_pVB, sizeof(Verts[0]) );
    m_pDevice->SetVertexShader( m_dwFVF );
    m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, NUMPRIMS );      // Draw a single triangle.

    m_pDevice->EndScene();

    // Flip the buffers.
    m_pDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    m_pDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    m_pDevice->SetShaderConstantMode(D3DSCM_192CONSTANTS);
    m_pDevice->Present(NULL, NULL, NULL, NULL);
    m_pDevice->SetShaderConstantMode(D3DSCM_96CONSTANTS);
    m_pDevice->GetTextureStageState(0, D3DTSS_MINFILTER, &minFilter);
    m_pDevice->GetTextureStageState(0, D3DTSS_MAGFILTER, &magFilter);

    if ((minFilter != D3DTEXF_POINT) || (magFilter != D3DTEXF_POINT))
        _asm int 3;

	m_dwFrames++;

	return hr;
}


//------------------------------------------------------------------------------
VOID CFsaa::DumpStats()
{
	WCHAR str[512];
	CHAR  szBuf[512];

	swprintf( str, L"\nType: %s\tFormat: %s\tFlicker: %d\n=>\tFPS: %f\n\n", m_AntialiasTypes[m_iType].Text, m_AntialiasFormats[m_iFormat].Text, m_iFlicker, (FLOAT)m_dwFrames*1000.0f/(FLOAT)(m_dwCurrentTime-m_dwStartLoopTime) );
	wcstombs( szBuf, str, 512 );
	DbgPrint( szBuf );
}


//-----------------------------------------------------------------------------
// Name: AddModulus()
//-----------------------------------------------------------------------------
INT AddModulus(INT Value, INT Increment, INT Max)
{
    Value += Increment;
    if (Value >= Max)
        Value -= Max;
    if (Value < 0)
        Value += Max;

    return Value;
}
