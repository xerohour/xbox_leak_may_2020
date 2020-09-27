//-----------------------------------------------------------------------------
// File: TestArea.cpp
//
// Desc: Sample Creates a Rectangle showing TCR Safe Area as a white rectangle
//       black background.  Will also display the same area as a black
//       rectangle on a white background if the A key is held down.
//
// Hist: 8.29.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>
#include <xfvf.h>

//-----------------------------------------------------------------------------
#define PIXELPERVIEWPORT	(1.f/320.f)
#define VIEWPORT15PERCENT	(640.f * 0.15f)
#define EDGE				(PIXELPERVIEWPORT * VIEWPORT15PERCENT * 0.5f)
#define TWOPIXEL			(PIXELPERVIEWPORT * 2.f)

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,  XBHELP_PLACEMENT_2, L"Display Black Rectangle" },
};

#define NUM_HELP_CALLOUTS 1


//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
	DWORD			m_dBackgroundColor;
	DWORD			m_dLineColor;
	FVFT_XYZRHWDIFF	*m_pVertices;
	WORD			*m_pIndices;

public:
    CXBoxSample();

    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();

    // Font and help
    CXBFont     m_Font;
    CXBHelp     m_Help;

    BOOL        m_bDrawHelp;
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
// Name: CXBoxSample (constructor)
// Desc: Constructor for CXBoxSample class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample() 
            :CXBApplication()
{
	m_dLineColor = 0xffffffff;
	m_dBackgroundColor = 0xff000000;

	m_pIndices = new WORD [16];
	for (DWORD i=0;i<16;i++)
		m_pIndices[i] = (WORD)i;

	m_pVertices = new FVFT_XYZRHWDIFF [16];

	//First Quad
	m_pVertices[0].v.x = -1.f + EDGE;
	m_pVertices[0].v.y = 1.f - EDGE;
	m_pVertices[0].v.z = 0.5f;
	m_pVertices[0].w = 0.5f;
	m_pVertices[0].diff = m_dLineColor;

	m_pVertices[1].v.x = 1.f - EDGE;
	m_pVertices[1].v.y = 1.f - EDGE;
	m_pVertices[1].v.z = 0.5f;
	m_pVertices[1].w = 0.5f;
	m_pVertices[1].diff = m_dLineColor;

	m_pVertices[2].v.x = 1.f - EDGE;
	m_pVertices[2].v.y = 1.f - EDGE - TWOPIXEL;
	m_pVertices[2].v.z = 0.5f;
	m_pVertices[2].w = 0.5f;
	m_pVertices[2].diff = m_dLineColor;

	m_pVertices[3].v.x = -1.f + EDGE;
	m_pVertices[3].v.y = 1.f - EDGE - TWOPIXEL;
	m_pVertices[3].v.z = 0.5f;
	m_pVertices[3].w = 0.5f;
	m_pVertices[3].diff = m_dLineColor;

	//Second Quad
	m_pVertices[4].v.x = 1.f - EDGE - TWOPIXEL;
	m_pVertices[4].v.y = 1.f - EDGE;
	m_pVertices[4].v.z = 0.5f;
	m_pVertices[4].w = 0.5f;
	m_pVertices[4].diff = m_dLineColor;

	m_pVertices[5].v.x = 1.f - EDGE;
	m_pVertices[5].v.y = 1.f - EDGE;
	m_pVertices[5].v.z = 0.5f;
	m_pVertices[5].w = 0.5f;
	m_pVertices[5].diff = m_dLineColor;

	m_pVertices[6].v.x = 1.f - EDGE;
	m_pVertices[6].v.y = -1.f + EDGE;
	m_pVertices[6].v.z = 0.5f;
	m_pVertices[6].w = 0.5f;
	m_pVertices[6].diff = m_dLineColor;

	m_pVertices[7].v.x = 1.f - EDGE - TWOPIXEL;
	m_pVertices[7].v.y = -1.f + EDGE;
	m_pVertices[7].v.z = 0.5f;
	m_pVertices[7].w = 0.5f;
	m_pVertices[7].diff = m_dLineColor;

	//Third Quad
	// Not sure why we need the extra pixel, but we do or it don't look right!
	m_pVertices[10].v.x = -1.f + EDGE;
	m_pVertices[10].v.y = -1.f + EDGE + TWOPIXEL + PIXELPERVIEWPORT;
	m_pVertices[10].v.z = 0.5f;
	m_pVertices[10].w = 0.5f;
	m_pVertices[10].diff = m_dLineColor;
				  
	m_pVertices[11].v.x = 1.f - EDGE;
	m_pVertices[11].v.y = -1.f + EDGE + TWOPIXEL + PIXELPERVIEWPORT; 
	m_pVertices[11].v.z = 0.5f;
	m_pVertices[11].w = 0.5f;
	m_pVertices[11].diff = m_dLineColor;

	m_pVertices[8].v.x = 1.f - EDGE;
	m_pVertices[8].v.y = -1.f + EDGE; 
	m_pVertices[8].v.z = 0.5f;
	m_pVertices[8].w = 0.5f;
	m_pVertices[8].diff = m_dLineColor;
				 
	m_pVertices[9].v.x = -1.f + EDGE;
	m_pVertices[9].v.y = -1.f + EDGE; 
	m_pVertices[9].v.z = 0.5f;
	m_pVertices[9].w = 0.5f;
	m_pVertices[9].diff = m_dLineColor;

	//Fourth Quad
	m_pVertices[12].v.x = -1.f + EDGE;
	m_pVertices[12].v.y = 1.f - EDGE;
	m_pVertices[12].v.z = 0.5f;
	m_pVertices[12].w = 0.5f;
	m_pVertices[12].diff = m_dLineColor;

	m_pVertices[13].v.x = -1.f + EDGE + TWOPIXEL;
	m_pVertices[13].v.y = 1.f - EDGE;
	m_pVertices[13].v.z = 0.5f;
	m_pVertices[13].w = 0.5f;
	m_pVertices[13].diff = m_dLineColor;

	m_pVertices[14].v.x = -1.f + EDGE + TWOPIXEL;
	m_pVertices[14].v.y = -1.f + EDGE;
	m_pVertices[14].v.z = 0.5f;
	m_pVertices[14].w = 0.5f;
	m_pVertices[14].diff = m_dLineColor;

	m_pVertices[15].v.x = -1.f + EDGE;
	m_pVertices[15].v.y = -1.f + EDGE;
	m_pVertices[15].v.z = 0.5f;
	m_pVertices[15].w = 0.5f;
	m_pVertices[15].diff = m_dLineColor;

    m_bDrawHelp = FALSE;
}



//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;    

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

	//Check if Button A is held down.  If so, change background clear to white
	// and lines to black.  Otherwise, draw background black, lines in white.
	if (m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_A] > XINPUT_GAMEPAD_MAX_CROSSTALK )
	{
		m_dBackgroundColor = 0xffffffff;
		m_dLineColor = 0xff000000;
	}
	else
	{
		m_dBackgroundColor = 0xff000000;
		m_dLineColor = 0xffffffff;
	}

	for (DWORD i=0;i<16;i++)
		m_pVertices[i].diff = m_dLineColor;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, m_dBackgroundColor, 1.0f, 0L );

    // Setup render state
	if (m_dBackgroundColor == 0xffffffff)
	{
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,     		TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,			0xff000000);
	}
	else
	{
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,     		FALSE );
	}

    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, 		FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      		FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, 		FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_ALL );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );

	//Render the Lines based on color setup by button press.
	m_pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_QUADLIST,0,16,4,m_pIndices,D3DFMT_INDEX16,m_pVertices,20);

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

