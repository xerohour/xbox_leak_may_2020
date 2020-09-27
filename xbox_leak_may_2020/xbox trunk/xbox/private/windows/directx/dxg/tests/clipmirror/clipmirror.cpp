//-----------------------------------------------------------------------------
// File: ClipMirror.cpp
//
// Desc: This sample shows how to use clip planes to implement a planar mirror.
//       The scene is reflected in a mirror and rendered in a 2nd pass. The
//       corners of the mirrors, together with the camera eye point, are used
//       to define a custom set of clip planes so that the reflected geometry
//       appears only within the mirror's boundaries.
//
//       Note: This code uses the D3D Framework helper library.
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT

#ifdef XBOX
#include <xtl.h>
#else
#include <windows.h>
#include <D3DX8.h>
#endif
#include <math.h>
#include "..\Common\D3DApp.h"
#include "..\Common\D3DFile.h"
#include "..\Common\D3DUtil.h"
#include "..\Common\DXUtil.h"




//-----------------------------------------------------------------------------
// Name: struct MIRRORVERTEX
// Desc: Custom mirror vertex type
//-----------------------------------------------------------------------------
struct MIRRORVERTEX
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    DWORD       color;
};

#define D3DFVF_MIRRORVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE)




//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    CD3DMesh*    m_pTeapot;     // The teapot object
    D3DXMATRIX   m_matTeapot;   // Teapot's local matrix

    LPDIRECT3DVERTEXBUFFER8 m_pMirrorVB;

    D3DXVECTOR3  m_vEyePt;      // Vectors defining the camera
    D3DXVECTOR3  m_vLookatPt;
    D3DXVECTOR3  m_vUpVec;

    void RenderMirror();
    void RenderScene();

protected:
    void OneTimeSceneInit();
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

#ifdef PCBUILD

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    d3dApp.Create( hInst );

    d3dApp.Run();

    return 0;
}
#else

void __cdecl main()
{
    CMyD3DApplication d3dApp;

    d3dApp.Create( NULL );

    d3dApp.Run();
}

#endif

//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    m_bUseDepthBuffer   = TRUE;
    m_pTeapot           = new CD3DMesh;
    m_pMirrorVB         = NULL;
}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
void CMyD3DApplication::OneTimeSceneInit()
{
    // Initialize the camera's orientation
    m_vEyePt    = D3DXVECTOR3( 0.0f, 2.0f, -6.5f );
    m_vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
    m_vUpVec    = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
void CMyD3DApplication::FrameMove()
{
    // Set the teapot's local matrix (rotating about the y-axis)
    D3DXMatrixRotationY( &m_matTeapot, m_fTime );

#if 0
    // When the window has focus, let the mouse adjust the camera view
    if( GetFocus() )
    {
        D3DXQUATERNION quat = D3DUtil_GetRotationFromCursor( m_hWnd );
        m_vEyePt.x = 5*quat.y;
        m_vEyePt.y = 5*quat.x;
        m_vEyePt.z = -sqrtf( 50.0f - 25*quat.x*quat.x - 25*quat.y*quat.y );

        D3DXMATRIX matView;
        D3DXMatrixLookAtLH( &matView, &m_vEyePt, &m_vLookatPt, &m_vUpVec );
        CheckHR(m_pDevice->SetTransform( D3DTS_VIEW, &matView ));
    }
#endif 0
}




//-----------------------------------------------------------------------------
// Name: RenderScene()
// Desc: Renders all objects in the scene.
//-----------------------------------------------------------------------------
void CMyD3DApplication::RenderScene()
{
    D3DXMATRIX matLocal, matWorldSaved;
    CheckHR(m_pDevice->GetTransform( D3DTS_WORLD, &matWorldSaved ));

    // Build the local matrix
    D3DXMatrixMultiply( &matLocal, &m_matTeapot, &matWorldSaved );
    CheckHR(m_pDevice->SetTransform( D3DTS_WORLD, &matLocal ));

    // Render the object
    m_pTeapot->Render( m_pDevice );

    // Restore the modified render states
    CheckHR(m_pDevice->SetTransform( D3DTS_WORLD, &matWorldSaved ));
}




//-----------------------------------------------------------------------------
// Name: RenderMirror()
// Desc: Renders the scene as reflected in a mirror. The corners of the mirror
//       define a plane, which is used to build the reflection matrix. The
//       scene is rendered with the cull-mode reversed, since all normals in
//       the scene are likewise reflected.
//-----------------------------------------------------------------------------
void CMyD3DApplication::RenderMirror()
{
    D3DXMATRIX matWorldSaved;
    D3DXMATRIX matReflectInMirror;
    D3DXPLANE  plane;

    // Save the world matrix so it can be restored
    CheckHR(m_pDevice->GetTransform( D3DTS_WORLD, &matWorldSaved ));

    // Get the four corners of the mirror. (This should be dynamic rather than
    // hardcoded.)
    D3DXVECTOR3 a(-1.5f, 1.5f, 3.0f );
    D3DXVECTOR3 b( 1.5f, 1.5f, 3.0f );
    D3DXVECTOR3 c( -1.5f,-1.5f, 3.0f );
    D3DXVECTOR3 d( 1.5f,-1.5f, 3.0f );

    // Construct the reflection matrix
    D3DXPlaneFromPoints( &plane, &a, &b, &c );
    D3DXMatrixReflect( &matReflectInMirror, &plane );
    CheckHR(m_pDevice->SetTransform( D3DTS_WORLD, &matReflectInMirror ));

    // Reverse the cull mode (since normals will be reflected)
    CheckHR(m_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW ));

    // Set the custom clip planes (so geometry is clipped by mirror edges).
    // This is the heart of this sample. The mirror has 4 edges, so there are
    // 4 clip planes, each defined by two mirror vertices and the eye point.
#if 0
    CheckHR(m_pDevice->SetClipPlane( 0, *D3DXPlaneFromPoints( &plane, &b, &a, &m_vEyePt ) ));
    CheckHR(m_pDevice->SetClipPlane( 1, *D3DXPlaneFromPoints( &plane, &d, &b, &m_vEyePt ) ));
    CheckHR(m_pDevice->SetClipPlane( 2, *D3DXPlaneFromPoints( &plane, &c, &d, &m_vEyePt ) ));
    CheckHR(m_pDevice->SetClipPlane( 3, *D3DXPlaneFromPoints( &plane, &a, &c, &m_vEyePt ) ));
#endif 0
    // !!! CheckHR(m_pDevice->SetRenderState( D3DRS_CLIPPLANEENABLE, 0x0f ));

    // Render the scene
    RenderScene();

    // Restore the modified render states
    CheckHR(m_pDevice->SetTransform( D3DTS_WORLD, &matWorldSaved ));
    // !!! CheckHR(m_pDevice->SetRenderState( D3DRS_CLIPPLANEENABLE, 0x00 ));
    CheckHR(m_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW ));

    // Finally, render the mirror itself (as an alpha-blended quad)
    CheckHR(m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE ));
    CheckHR(m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA ));
    CheckHR(m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ));

    CheckHR(m_pDevice->SetStreamSource( 0, m_pMirrorVB, sizeof(MIRRORVERTEX) ));
    CheckHR(m_pDevice->SetVertexShader( D3DFVF_MIRRORVERTEX ));
    CheckHR(m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 ));

    CheckHR(m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE ));
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
void CMyD3DApplication::Render()
{
    // Clear the viewport
    CheckHR(m_pDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                              0x000000ff, 1.0f, 0L ));
 
    // Begin the scene
    CheckHR(m_pDevice->BeginScene());
    
    // Render the scene
    RenderScene();

    // Render the scene in the mirror
    RenderMirror();

    // End the scene.
    CheckHR(m_pDevice->EndScene());
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
void CMyD3DApplication::InitDeviceObjects()
{
    // Set up the geometry objects
    m_pTeapot->Create( m_pDevice, "Teapot.x");

    // Create a square for rendering the mirror
    CheckHR(m_pDevice->CreateVertexBuffer( 4*sizeof(MIRRORVERTEX),
                                           D3DUSAGE_WRITEONLY,
                                           D3DFVF_MIRRORVERTEX,
                                           D3DPOOL_MANAGED, 
                                           &m_pMirrorVB ) );

    // Initialize the mirror's vertices
    MIRRORVERTEX* v;
    CheckHR(m_pMirrorVB->Lock( 0, 0, (BYTE**)&v, 0 ));
    v[0].p = D3DXVECTOR3(-1.5f, 1.5f, 3.0f );
    v[2].p = D3DXVECTOR3(-1.5f,-1.5f, 3.0f );
    v[1].p = D3DXVECTOR3( 1.5f, 1.5f, 3.0f );
    v[3].p = D3DXVECTOR3( 1.5f,-1.5f, 3.0f );
    v[0].n     = v[1].n     = v[2].n     = v[3].n     = D3DXVECTOR3(0.0f,0.0f,-1.0f);
    v[0].color = v[1].color = v[2].color = v[3].color = 0x80ffffff;
    CheckHR(m_pMirrorVB->Unlock());
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
void CMyD3DApplication::RestoreDeviceObjects()
{
    // Set up the geometry objects
    m_pTeapot->RestoreDeviceObjects( m_pDevice );

    // Set up the textures
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR ));

    // Set miscellaneous render states
    CheckHR(m_pDevice->SetRenderState( D3DRS_DITHERENABLE,   TRUE ));
    CheckHR(m_pDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE ));
    CheckHR(m_pDevice->SetRenderState( D3DRS_ZENABLE,        TRUE ));

    // Set up the matrices
    D3DXMATRIX matWorld, matView, matProj;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &m_vEyePt, &m_vLookatPt, &m_vUpVec );
    FLOAT fAspect = m_BackBufferDesc.Width / (FLOAT)m_BackBufferDesc.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );
    CheckHR(m_pDevice->SetTransform( D3DTS_WORLD,      &matWorld ));
    CheckHR(m_pDevice->SetTransform( D3DTS_VIEW,       &matView ));
    CheckHR(m_pDevice->SetTransform( D3DTS_PROJECTION, &matProj ));

    // Set up a light
    if( ( m_Caps.VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS ))
    {
        D3DLIGHT8 light;
        D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 0.2f, -1.0f, -0.2f );
        CheckHR(m_pDevice->SetLight( 0, &light ));
        CheckHR(m_pDevice->LightEnable( 0, TRUE ));
    }
    CheckHR(m_pDevice->SetRenderState( D3DRS_AMBIENT, 0xff555555 ));
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
void CMyD3DApplication::InvalidateDeviceObjects()
{
    m_pTeapot->InvalidateDeviceObjects();
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exitting, or the device is being changed,
//       this function deletes any device dependant objects.
//-----------------------------------------------------------------------------
void CMyD3DApplication::DeleteDeviceObjects()
{
    m_pTeapot->Destroy();

    SAFE_RELEASE( m_pMirrorVB );
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
void CMyD3DApplication::FinalCleanup()
{
    SAFE_DELETE( m_pTeapot );
}




