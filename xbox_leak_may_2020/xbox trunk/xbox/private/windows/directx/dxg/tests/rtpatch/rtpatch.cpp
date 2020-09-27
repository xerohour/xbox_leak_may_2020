//-----------------------------------------------------------------------------
// File: RTPatch.cpp
//
// Desc: Example code showing how to use patches in D3D.
//
// Copyright (c) 1995-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include <tchar.h>
#include <math.h>
#include <stdio.h>

#ifdef XBOX
#include <xtl.h>
#else
#include <D3DX8.h>
#endif

#include "D3DApp.h"
#include "D3DFile.h"
#include "D3DUtil.h"
#include "DXUtil.h"

#include "teapot.h"

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
    D3DXVECTOR3 p; // The original position of the vertex
};

// Surface handles need to start with 1
const UINT SURFACEHANDLEBASE = 1;

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
#ifdef _FONT_
    CD3DFont*               m_pFont;          // Font for drawing text
#endif
    LPDIRECT3DVERTEXBUFFER8 m_pVB;            // Buffer to hold vertices
    DWORD                   m_hVShader;       // Handle to vertex shader
    CD3DArcBall             m_ArcBall;        // Mouse rotation utility

    UINT                    m_numSegments;    // Amount to tessellate
    BOOL                    m_bRetessellate;  // Need to retessellate patches
    UINT                    m_numPatches;
    BOOL                    m_bShowHelp;
    BOOL                    m_bWireframe;

    bool ConfirmDevice( D3DCAPS8*, DWORD, D3DFORMAT );

protected:
    void InitDeviceObjects();
    void RestoreDeviceObjects();
    void InvalidateDeviceObjects();
    void DeleteDeviceObjects();
    void Render();
    void FrameMove();
    void FinalCleanup();

public:
    CMyD3DApplication();
};




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------

#ifdef XBOX
void __cdecl main()
#else
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
#endif
{
    CMyD3DApplication d3dApp;

#ifdef XBOX
    d3dApp.Create( NULL );
#else
    d3dApp.Create( hInst );
#endif

    d3dApp.Run();

#ifndef XBOX
    return 0;
#endif
}



//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    m_bUseDepthBuffer   = TRUE;

#ifdef _FONT_
    m_pFont             = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
#endif
    m_pVB               = NULL;
    m_bRetessellate     = TRUE;
    m_numSegments       = 4;
    m_bShowHelp         = FALSE;
    m_bWireframe        = FALSE;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
void CMyD3DApplication::FrameMove()
{
    // Set up viewing postion from ArcBall
    D3DXMATRIX matRotationInverse;
    D3DXMatrixInverse( &matRotationInverse, NULL, m_ArcBall.GetRotationMatrix() );
    m_pDevice->SetTransform( D3DTS_WORLD, &matRotationInverse );
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: 
//-----------------------------------------------------------------------------
void CMyD3DApplication::Render()
{
    // Clear the viewport
    m_pDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
                         0x000000ff, 1.0f, 0L );

    if( m_bWireframe )
        m_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
    else
        m_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

    // Begin the scene
    if( SUCCEEDED( m_pDevice->BeginScene() ) )
    {
        m_pDevice->SetVertexShader( m_hVShader );
        m_pDevice->SetStreamSource( 0, m_pVB, sizeof(CUSTOMVERTEX) );

        for( UINT i = 0; i < m_numPatches; i++ ) 
        {
            float numSegs[4];
            numSegs[0] = (FLOAT)m_numSegments;
            numSegs[1] = (FLOAT)m_numSegments;
            numSegs[2] = (FLOAT)m_numSegments;
            numSegs[3] = (FLOAT)m_numSegments;
            if ( m_bRetessellate ) 
            {
                // First time through, tesellate
                D3DRECTPATCH_INFO info;
                info.StartVertexOffsetWidth = 0;
                info.StartVertexOffsetHeight = i * 4;
                info.Width = 4;
                info.Height = 4;
                info.Stride = 4; // verticies to next row of verticies
                info.Basis = D3DBASIS_BEZIER;
                info.Order = D3DORDER_CUBIC;
                m_pDevice->DrawRectPatch(SURFACEHANDLEBASE + i, numSegs, &info);
            }
            else 
            {
                // After the first time, use the existing surface handles
                m_pDevice->DrawRectPatch(SURFACEHANDLEBASE + i, numSegs, 0);
            }
        }

        m_bRetessellate = FALSE;

#ifdef _FONT_      
        // Output statistics
        m_pFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
        m_pFont->DrawText( 2, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats );
        TCHAR szSegments[150];
        wsprintf(szSegments, TEXT("%d segments per patch"), m_numSegments);
        m_pFont->DrawText( 2, 40, D3DCOLOR_ARGB(255,255,255,255), szSegments );

        if( m_bShowHelp )
        {
            m_pFont->DrawText( 2, 60, D3DCOLOR_ARGB(255,255,255,255), 
                TEXT("Up/Down arrows: change number of segments") );
            m_pFont->DrawText( 2, 80, D3DCOLOR_ARGB(255,255,255,255), 
                TEXT("W: toggle wireframe") );
        }
        else
        {
            m_pFont->DrawText( 2, 60, D3DCOLOR_ARGB(255,255,255,255), 
                TEXT("Press F1 for Help") );
        }
#endif

        // End the scene.
        m_pDevice->EndScene();
    }
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: This creates all device-dependent managed objects, such as managed
//       textures and managed vertex buffers.
//-----------------------------------------------------------------------------
void CMyD3DApplication::InitDeviceObjects()
{
#ifdef _FONT_
    // Initialize the font's internal textures
    m_pFont->InitDeviceObjects( m_pDevice );
#endif

    // Create a vertex buffer and fill it with our vertices.
    UINT numVerts = sizeof(g_vTeapotData) / (3 * sizeof(float));
    m_numPatches = numVerts / (4 * 4);

    if( FAILED( m_pDevice->CreateVertexBuffer( numVerts*sizeof(CUSTOMVERTEX),
        D3DUSAGE_RTPATCHES, 0, D3DPOOL_MANAGED, &m_pVB ) ) )
    {
        return;
    }

    CUSTOMVERTEX* pVertices;
    if( FAILED( m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
        return;

    for( UINT i = 0; i < numVerts; i++ ) 
    {
        pVertices[i].p = g_vTeapotData[i];
    }
    m_pVB->Unlock();
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Restore device-memory objects and state after a device is created or
//       resized.
//-----------------------------------------------------------------------------
void CMyD3DApplication::RestoreDeviceObjects()
{
    HRESULT hr;

#ifdef _FONT_
    m_pFont->RestoreDeviceObjects();
#endif

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. 
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 3.0f,-400.0f ), 
                                  &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 
                                  &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    m_pDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set up world matrix
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    m_pDevice->SetTransform( D3DTS_WORLD, &matWorld );

    D3DXMATRIX matProj;
    FLOAT fAspect = ((FLOAT)m_BackBufferDesc.Width) / m_BackBufferDesc.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 800.0f );
    m_pDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    m_pDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
    m_pDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
    m_pDevice->SetRenderState( D3DRS_LIGHTING,     TRUE );
    m_pDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

    m_pDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );

    // Set up a material. The material here just has the diffuse and ambient
    // colors set to white.
    D3DMATERIAL8 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    m_pDevice->SetMaterial( &mtrl );

    // Set up a light
    D3DLIGHT8 light;
    light.Type         = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r    = light.Diffuse.g  = light.Diffuse.b  = 1.0f;
    light.Specular.r   = light.Specular.g = light.Specular.b = 0.0f;
    light.Ambient.r    = light.Ambient.g  = light.Ambient.b  = 0.3f;
    light.Position     = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &D3DXVECTOR3( 0.0f, 1.0f, 1.0f ) );
    light.Attenuation0 = light.Attenuation1 = light.Attenuation2 = 0.0f;
    light.Range        = sqrtf(FLT_MAX);
    m_pDevice->SetLight(0, &light );
    m_pDevice->LightEnable(0, TRUE );

    m_ArcBall.SetWindow( m_BackBufferDesc.Width, m_BackBufferDesc.Height, 1.0f );
    m_ArcBall.SetRadius( 1.0f );

    DWORD decl[] =
    {
       /* Vertex Data */
        D3DVSD_STREAM(0),
        D3DVSD_REG( D3DVSDE_POSITION,  D3DVSDT_FLOAT3),

        /* Data generation section implemented as a virtual stream */
        D3DVSD_STREAM_TESS(),

        /* Generate normal _using_ the position input and copy it to the normal register (output) */
        D3DVSD_TESSNORMAL( D3DVSDE_POSITION, D3DVSDE_NORMAL ),

        D3DVSD_END()
    };

    hr = m_pDevice->CreateVertexShader( decl, NULL, &m_hVShader, 0 );
    m_bRetessellate = TRUE;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Called when the device-dependent objects are about to be lost.
//-----------------------------------------------------------------------------
void CMyD3DApplication::InvalidateDeviceObjects()
{
#ifdef _FONT_
    m_pFont->InvalidateDeviceObjects();
#endif

    for( UINT i = 0; i < m_numPatches; i++ ) 
        m_pDevice->DeletePatch(SURFACEHANDLEBASE + i);

    m_pDevice->DeleteVertexShader(m_hVShader);
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
void CMyD3DApplication::DeleteDeviceObjects()
{
#ifdef _FONT_
    m_pFont->DeleteDeviceObjects();
#endif

    SAFE_RELEASE( m_pVB );
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
void CMyD3DApplication::FinalCleanup()
{
#ifdef _FONT_
    SAFE_DELETE( m_pFont );
#endif
}




//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device intialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
bool CMyD3DApplication::ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    if( pCaps->DevCaps & D3DDEVCAPS_RTPATCHES )
        return true;
    else
        return false;
}

