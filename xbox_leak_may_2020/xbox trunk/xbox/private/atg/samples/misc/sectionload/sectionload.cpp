//-----------------------------------------------------------------------------
// File: SectionLoad.cpp
//
// Desc: Illustrates loading and unloading of sections.
//
// Note: This was a somewhat difficult sample to write. Especially because
//       samples are by definition very small bits of code meant only to 
//       illustrate one particular part of the system.
//
//       Section loading and unloading is something that will be used
//       primarily in large projects where memory restrictions prevent you
//       from having all code and data loaded simultaneously. Furthermore,
//       flagging a section as NOPRELOAD only works if that section cannot
//       be squeezed into a preloaded section. Thus, when I tried to write
//       a couple of small routines that were loaded and unloaded, they were
//       squeezed in to the end of a preloaded section and the code would
//       always execute (as they were always in memory).
//
//       As a result, this sample shows that section loading and unloading
//       work properly by doing the following: If the data section is 
//       loaded, write access to the data area will work just fine and
//       the sample shows the time of the last successful access.
//       If the data section is not loaded, accessing the data area will 
//       generate an exception.
//
//       This is not the best behaviour for a sample, I know. But it was
//       the only way to verify that unloaded sections did not work. Note
//       that you should only run this in DEBUG mode so you can see what
//       is going on!
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>

#include "resource.h"

//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "Wings.bmp", resource_Wings_OFFSET },
    { "BiHull.bmp", resource_BiHull_OFFSET },
    { NULL, 0 },
};

//-----------------------------------------------------------------------------
// Help screen definitions
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] =
{
    { XBHELP_BACK_BUTTON, XBHELP_PLACEMENT_1, L"Display\nhelp" },
    { XBHELP_A_BUTTON, XBHELP_PLACEMENT_1, L"Toggle DataSeg\nLoaded" },
    { XBHELP_B_BUTTON, XBHELP_PLACEMENT_1, L"Attempt DataSeg\nAccess" },
};
#define NUM_HELP_CALLOUTS 3


//-----------------------------------------------------------------------------
// put some data in another section
// note that uninitialize sections will always be flagged as preload
//-----------------------------------------------------------------------------
#define BUFSIZE (32*1024)
#pragma data_seg("dataseg1")
BYTE g_Seg1Data[BUFSIZE] = {0};
BYTE g_Seg1Data1[BUFSIZE] = {0};
BYTE g_Seg1Data2[BUFSIZE] = {0};
#pragma data_seg()


//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont m_Font;
    CXBHelp m_Help;
    BOOL m_bDrawHelp;

    CXBPackedResource  m_xprResource;
	CXBMesh *m_pPlaneMesh;

	BOOL m_bDataSegLoaded;
	float m_fDataSegAccessTime;

public:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();

    CXBoxSample();
};


//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}

//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
	// initialize our stuff
    m_bDrawHelp  = FALSE;

	m_bDataSegLoaded = FALSE;
	m_fDataSegAccessTime = 0.0f;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    D3DXMATRIX matProj, matView;

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Initialize the help system
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Set projection transform
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 640.0f/480.0f, 0.1f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	// Set view position
    D3DXMatrixTranslation( &matView, 0.0f, 0.0f, 60.0f);
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

	// initialize the airplanes
	m_pPlaneMesh = new CXBMesh;
	m_pPlaneMesh ->Create(g_pd3dDevice, "models\\airplane.xbg", &m_xprResource );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
	D3DXMATRIX matWorld, m;
	static float fYRot = 0.0f;
	float x, z;
	VOID *loc;
	DWORD i, d;

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons&XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;
	if(m_bDrawHelp)
		return S_OK;

    // Toggle dataseg load
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A])
	{
		// if not loaded, do the load
		if(!m_bDataSegLoaded)
		{
			loc = XLoadSection("dataseg1");
			if(loc!=NULL)
				m_bDataSegLoaded = TRUE;
		}
		else // otherwise, free it up
		{
			XFreeSection("dataseg1");
			m_bDataSegLoaded = FALSE;
		}
	}

	// attempt dataseg read
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B])
	{
		for(i=0; i<BUFSIZE; i++)
		{
			d = g_Seg1Data[i];
			d = g_Seg1Data1[i];
			d = g_Seg1Data2[i];
		}

		m_fDataSegAccessTime = m_fTime;
	}


	// move plane
	fYRot += 1.57f*m_fElapsedTime;
	D3DXMatrixRotationY(&matWorld, fYRot);
	x = 20.0f * (float)cos(fYRot);
	z = -20.0f * (float)sin(fYRot);
	D3DXMatrixTranslation(&m, x, 0.0f, z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m);
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         0x00400000, 1.0f, 0L );

    // Restore state that text clobbers
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	m_pPlaneMesh->Render(0);

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
		m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"SectionLoad" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.DrawText( 64, 70, 0xffffff00, L"Press A to toggle DataSeg loaded" );
        m_Font.DrawText( 64, 90, 0xffffff00, L"Press B to access DataSeg" );

		if(m_bDataSegLoaded)
	        m_Font.DrawText( 64, 110, 0xff00ff00, L"DataSeg Loaded" );
		else
	        m_Font.DrawText( 64, 110, 0xffff0000, L"DataSeg NOT Loaded" );

		if(m_fDataSegAccessTime!=0.0f)
		{
			WCHAR s[80];
			swprintf(s, L"Last successful DataSeg access time: %3.2f, %3.2f", m_fDataSegAccessTime, m_fTime);
			m_Font.DrawText( 64, 130, 0xffffff00, s);
		}

		m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

