//-----------------------------------------------------------------------------
// File: BeginEnd.cpp
//
// Desc: [TODO: Describe source file here]
//
// Hist: 8.27.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
};

#define NUM_HELP_CALLOUTS 1

typedef struct {
    D3DXVECTOR3 p;
    D3DCOLOR c;
} CUSTOMVERTEX;
#define FVF_CUSTOMVERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE )


typedef enum {
    LOCK_VB,
    NOLOCK_VB,
    DRAW_UP,
    BEGINEND,
    BEGINPUSH,

    MODE_MAX
} MODE;

WCHAR * g_szLabels[] = {
    L"Lock + Update VertexBuffer",
    L"Update VertexBuffer (No lock)",
    L"DrawPrimitiveUP",
    L"Begin/End + SetVertexData",
    L"BeginPush/EndPush",
};

#define AVERAGE_OVER 500

//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    CXBoxSample();

    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();

    void UpdateVertexBuffer();
    void UpdateNoLock();
    void UpdateUserPrim();
    void SpewVertices();
    void BeginPushEndPush();

    // Font and help
    CXBFont     m_Font;
    CXBHelp     m_Help;

    BOOL        m_bDrawHelp;
    DWORD       m_dwVertices;
    MODE        m_Mode;

    LPDIRECT3DVERTEXBUFFER8 m_pVB;
    CUSTOMVERTEX * m_pUP;
    FLOAT       m_fMSecsPerTick;
    FLOAT       m_fTime;

    DWORD       m_dwCount;
    FLOAT       m_fTimeSum;
    FLOAT       m_fAverage;

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
    m_bDrawHelp = FALSE;
    m_dwCount = 0;
    m_fTimeSum = 0.0f;
    m_fAverage = 0.0f;
    IDirect3D8::SetPushBufferSize( 1024 * 1024 * 10, 1024 * 1024 * 1 );
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

    m_Mode = LOCK_VB;
    m_dwVertices = 10;

    m_pd3dDevice->CreateVertexBuffer( m_dwVertices * sizeof( CUSTOMVERTEX ), 0, 0, 0, &m_pVB );
    m_pUP = new CUSTOMVERTEX[ m_dwVertices ];

    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    D3DXMATRIX matView;  
    D3DXVECTOR3 vPos = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vLookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vPos, &vLookAt, &vUp );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4.0f, 4.0f / 3.0f, 1.0f, 1000.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    LARGE_INTEGER liFreq;
    QueryPerformanceFrequency( &liFreq );
    m_fMSecsPerTick = 1000.0f / liFreq.QuadPart;

    return S_OK;
}


void
CXBoxSample::UpdateVertexBuffer()
{
    CUSTOMVERTEX * pVerts;
    LARGE_INTEGER liBefore, liAfter;

    QueryPerformanceCounter( &liBefore );

    m_pVB->Lock( 0, 0, (BYTE **)&pVerts, 0 );
    for( DWORD i = 0; i < m_dwVertices; i++ )
    {
        pVerts[i].p.x = (float)cos( D3DX_PI * 2.0f * i / m_dwVertices );
        pVerts[i].p.y = (float)sin( D3DX_PI * 2.0f * i / m_dwVertices );
        pVerts[i].p.z = 1.0f;

        pVerts[i].c = 0xFFFF0000;
    }

    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof( CUSTOMVERTEX ) );
    m_pd3dDevice->DrawVertices( D3DPT_TRIANGLEFAN, 0, m_dwVertices );

    QueryPerformanceCounter( &liAfter );
    m_fTime = m_fMSecsPerTick * ( liAfter.QuadPart - liBefore.QuadPart );
}


void
CXBoxSample::UpdateNoLock()
{
    CUSTOMVERTEX * pVerts;
    LARGE_INTEGER liBefore, liAfter;

    m_pVB->Lock( 0, 0, (BYTE **)&pVerts, 0 );

    QueryPerformanceCounter( &liBefore );

    for( DWORD i = 0; i < m_dwVertices; i++ )
    {
        pVerts[i].p.x = (float)cos( D3DX_PI * 2.0f * i / m_dwVertices );
        pVerts[i].p.y = (float)sin( D3DX_PI * 2.0f * i / m_dwVertices );
        pVerts[i].p.z = 1.0f;

        pVerts[i].c = 0xFFFFFF00;
    }

    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof( CUSTOMVERTEX ) );
    m_pd3dDevice->DrawVertices( D3DPT_TRIANGLEFAN, 0, m_dwVertices );

    QueryPerformanceCounter( &liAfter );
    m_fTime = m_fMSecsPerTick * ( liAfter.QuadPart - liBefore.QuadPart );
}

void
CXBoxSample::UpdateUserPrim()
{
    LARGE_INTEGER liBefore, liAfter;

    QueryPerformanceCounter( &liBefore );

    for( DWORD i = 0; i < m_dwVertices; i++ )
    {
        m_pUP[i].p.x = (float)cos( D3DX_PI * 2.0f * i / m_dwVertices );
        m_pUP[i].p.y = (float)sin( D3DX_PI * 2.0f * i / m_dwVertices );
        m_pUP[i].p.z = 1.0f;

        m_pUP[i].c = 0xFF00FF00;
    }
    
    m_pd3dDevice->DrawVerticesUP( D3DPT_TRIANGLEFAN, m_dwVertices, m_pUP, sizeof( CUSTOMVERTEX ) );

    QueryPerformanceCounter( &liAfter );
    m_fTime = m_fMSecsPerTick * ( liAfter.QuadPart - liBefore.QuadPart );
}


void
CXBoxSample::SpewVertices()
{
    LARGE_INTEGER liBefore, liAfter;

    QueryPerformanceCounter( &liBefore );

    m_pd3dDevice->Begin( D3DPT_TRIANGLEFAN );

    for( DWORD i = 0; i < m_dwVertices; i++ )
    {
        m_pd3dDevice->SetVertexDataColor( D3DVSDE_DIFFUSE, 0xFF0000FF );
        m_pd3dDevice->SetVertexData4f( D3DVSDE_POSITION, (float)cos( D3DX_PI * 2.0f * i / m_dwVertices ), (float)sin( D3DX_PI * 2.0f * i / m_dwVertices ), 1.0f, 1.0f );
    }

    m_pd3dDevice->End();

    QueryPerformanceCounter( &liAfter );
    m_fTime = m_fMSecsPerTick * ( liAfter.QuadPart - liBefore.QuadPart );
}



void
CXBoxSample::BeginPushEndPush()
{
    DWORD dwBytes = m_dwVertices * ( 3 * sizeof( FLOAT ) + 1 * sizeof( DWORD ) );
    DWORD* pPush;
    LARGE_INTEGER liBefore, liAfter;

    QueryPerformanceCounter( &liBefore );

    m_pd3dDevice->BeginPush( ( dwBytes + 3 ) / sizeof( DWORD ) + 5, &pPush );
    pPush[ 0 ] = D3DPUSH_ENCODE( D3DPUSH_SET_BEGIN_END, 1 );
    pPush[ 1 ] = D3DPT_TRIANGLEFAN;
    pPush[ 2 ] = D3DPUSH_ENCODE( D3DPUSH_NOINCREMENT_FLAG | D3DPUSH_INLINE_ARRAY, ( dwBytes + 3 ) / sizeof( DWORD ) );
    pPush += 3;

    for( DWORD i = 0; i < m_dwVertices; i++, pPush += 4 )
    {
        register FLOAT f;
        
        ((float *)pPush)[ 0 ] = cos( D3DX_PI * 2.0f * i / m_dwVertices );
        ((float *)pPush)[ 1 ] = sin( D3DX_PI * 2.0f * i / m_dwVertices );
        ((float *)pPush)[ 2 ] = 1.0f;
        pPush[ 3 ] = 0xFFFF00FF;
    }

    pPush[ 0 ] = D3DPUSH_ENCODE( D3DPUSH_SET_BEGIN_END, 1 );
    pPush[ 1 ] = 0;
    pPush += 2;
    m_pd3dDevice->EndPush( pPush );

    QueryPerformanceCounter( &liAfter );
    m_fTime = m_fMSecsPerTick * ( liAfter.QuadPart - liBefore.QuadPart );
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

    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
    {
        m_Mode = (MODE)(( m_Mode + 1 ) % MODE_MAX);
    }

    DWORD dwVerticesOld = m_dwVertices;

    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
    {
        m_dwVertices += 1;
    }
    else if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
    {
        if( m_dwVertices > 3 )
            m_dwVertices -= 1;
        else
            m_dwVertices = 3;
    }
    else if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
    {
        m_dwVertices += 100;
    }
    else if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )
    {
        if( m_dwVertices > 103 )
            m_dwVertices -= 100;
        else
            m_dwVertices = 3;
    }

    if( dwVerticesOld != m_dwVertices )
    {
        m_pVB->Release();
        m_pd3dDevice->CreateVertexBuffer( m_dwVertices * sizeof( CUSTOMVERTEX ), 0, 0, 0, &m_pVB );

        delete[] m_pUP;
        m_pUP = new CUSTOMVERTEX[ m_dwVertices ];
    }

    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
    {
        m_dwCount = 0;
        m_fTimeSum = 0.0f;
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    static bool bOtherStuff = TRUE;

    // Draw a gradient filled background
    if( bOtherStuff )
        RenderGradientBackground( 0xff404040, 0xff404080 );

    static bool bFirst = TRUE;

    if( bFirst || bOtherStuff )
    {
        bFirst = FALSE;

        m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
        m_pd3dDevice->SetVertexShader( FVF_CUSTOMVERTEX );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    }

    switch( m_Mode )
    {
    case LOCK_VB:
        UpdateVertexBuffer();
        break;
    case NOLOCK_VB:
        UpdateNoLock();
        break;
    case DRAW_UP:
        UpdateUserPrim();
        break;
    case BEGINEND:
        SpewVertices();
        break;
    case BEGINPUSH:
        BeginPushEndPush();
    }

    m_fTimeSum += m_fTime;
    m_dwCount++;
    

    if( bOtherStuff )
    {
        // Show title, frame rate, and help
        if( m_bDrawHelp )
            m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
        else
        {
            m_Font.Begin();
            m_Font.DrawText(  64, 50, 0xffffffff, L"BeginEnd" );
            m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        
            WCHAR sz[100];
            swprintf( sz, L"Mode: %s", g_szLabels[ m_Mode ] );
            m_Font.DrawText( 64, 80, 0xffffffff, sz );

            swprintf( sz, L"Vertices: %d", m_dwVertices );
            m_Font.DrawText( 64, 110, 0xffffffff, sz );

            swprintf( sz, L"Time: %0.4fms", m_fTime );
            m_Font.DrawText( 64, 340, 0xffffffff, sz );
            swprintf( sz, L"Avg: %0.4fms", m_fTimeSum / m_dwCount );
            m_Font.DrawText( 300, 340, 0xffffffff, sz );

            m_Font.End();
        }
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

