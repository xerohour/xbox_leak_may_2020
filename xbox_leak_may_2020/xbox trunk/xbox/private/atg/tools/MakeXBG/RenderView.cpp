//-----------------------------------------------------------------------------
// File: RenderView.cpp
//
// Desc: MFC view class for render the geoemtry model using D3D.
//
// Hist: 03.01.00 - New for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "MakeXBG.h"




//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
extern LPDIRECT3D8      g_pD3D;
extern D3DDISPLAYMODE   g_d3ddm;




//-----------------------------------------------------------------------------
// Name: class CModelRenderView
// Desc: A view class to render the geometry model loaded from a file.
//-----------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CModelRenderView, CView)

BEGIN_MESSAGE_MAP(CModelRenderView, CView)
    //{{AFX_MSG_MAP(CModelRenderView)
    ON_WM_MOUSEACTIVATE()
    ON_WM_DESTROY()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_CREATE()
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CModelRenderView::CModelRenderView()
{
    m_pd3dDevice = NULL;
}

CModelRenderView::~CModelRenderView()
{
}




//-----------------------------------------------------------------------------
// Name: OnDestroy()
// Desc: Destroys the model view
//-----------------------------------------------------------------------------
void CModelRenderView::OnDestroy() 
{
    CView::OnDestroy();

    if( m_pd3dDevice )
        m_pd3dDevice->Release();
}




//-----------------------------------------------------------------------------
// Name: OnInitialUpdate()
// Desc: Creates objects for the view (i.e. the d3d rendering device and loads
//       the object's geometry).
//-----------------------------------------------------------------------------
void CModelRenderView::OnInitialUpdate()
{
    // Now that we have a file to load, create a d3ddevice and use to it load
    // the geometry file (and also, of course, to render the geometry).
    CXBGDocument* pDoc = GetDocument();
    pDoc->m_pD3DFile        = NULL;
    pDoc->m_pSelectedFrame  = NULL;
    pDoc->m_lSelectedSubset = NULL;

    // Set up the structure used to create the D3DDevice.
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.hDeviceWindow          = m_hWnd;
    d3dpp.Windowed               = TRUE;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.BackBufferFormat       = g_d3ddm.Format;
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

    // Create the Direct3D device.
    if( FAILED( g_pD3D->CreateDevice( 0L, D3DDEVTYPE_HAL, m_hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &m_pd3dDevice ) ) )
    {
        MessageBox( _T("Could not create a D3D rendering device!\n\n")
                    _T("This app requires a D3D hardware rendering device\n")
                    _T("in order to run. The D3D device is used to display\n")
                    _T("loaded geometry.\n"),
                    _T("MakeXBG Error"), MB_ICONERROR|MB_OK );
        GetParentFrame()->PostMessage( WM_CLOSE );
        return;
    }

    // Load the mesh
    TCHAR* strInputFilename = pDoc->m_strFilename.GetBuffer(MAX_PATH);
    pDoc->m_pD3DFile = new CD3DFile();

    if( FAILED( pDoc->m_pD3DFile->Create( m_pd3dDevice, strInputFilename, 
                                          pDoc->m_bCollapseMesh ) ) )
    {
        MessageBox( _T("Could not load file!\n\n")
                    _T("Please note that .x files with more than\n")
                    _T("one mesh per frame cannot be loaded by\n")
                    _T("this app."), _T("File Error"), MB_ICONERROR|MB_OK );
        GetParentFrame()->PostMessage( WM_CLOSE );
        return;
    }

    // Set ArcBall info
    RECT rc;
    GetClientRect( &rc );
    D3DXQuaternionIdentity( &m_qDown );
    D3DXQuaternionIdentity( &m_qNow );
    D3DXMatrixIdentity( &m_matWorld );
    D3DXMatrixIdentity( &m_matRotation );
    D3DXMatrixIdentity( &m_matTranslation );
    m_bDrag = FALSE;

    // Setup the light
    D3DLIGHT8 light;
    light.Type         = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r    = light.Diffuse.g  = light.Diffuse.b  = 1.0f; light.Diffuse.a  = 1.0f;
    light.Specular.r   = light.Specular.g = light.Specular.b = 0.0f; light.Specular.a = 1.0f;
    light.Ambient.r    = light.Ambient.g  = light.Ambient.b  = 0.3f; light.Ambient.a  = 1.0f;
    light.Position     = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &D3DXVECTOR3( 0.3f, -1.0f, 1.0f ) );
    light.Attenuation0 = light.Attenuation1 = light.Attenuation2 = 0.0f;
    light.Range        = sqrtf(FLT_MAX);
    m_pd3dDevice->SetLight(0, &light );
    m_pd3dDevice->LightEnable(0, TRUE );

    // Setup render state
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,     TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

    // Setup transforms
    D3DXMATRIX matView, matProj;
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0, 0,-3.0f*pDoc->m_pD3DFile->m_fRadius ),
                                  &D3DXVECTOR3( 0, 0, 0 ), &D3DXVECTOR3( 0, 1, 0 ) );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 640.0f/480.0f, 
                                pDoc->m_pD3DFile->m_fRadius/64.0f, pDoc->m_pD3DFile->m_fRadius*200.0f );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,  &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION,  &matProj );

    // Update the treeview
    ((CSplitterFrame*)GetParentFrame())->m_pTreeView->OnUpdate( this, 0, NULL );
}




//-----------------------------------------------------------------------------
// Name: OnDraw()
// Desc: Draws the geometry in the model view window
//-----------------------------------------------------------------------------
void CModelRenderView::OnDraw( CDC* pDC )
{
    CXBGDocument*     pDoc  = GetDocument();
    CD3DFile*         pMesh = pDoc->m_pD3DFile;

    // Draw scene
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
                         0xff0000ff, 1.0f, 0L );
    m_pd3dDevice->BeginScene();

    // Setup viewing postion from ArcBall
    D3DXMATRIX matRotationInverse;
    D3DXMatrixTranslation( &m_matWorld, -pMesh->m_vCenter.x,
                                        -pMesh->m_vCenter.y,
                                        -pMesh->m_vCenter.z );
    D3DXMatrixInverse( &matRotationInverse, NULL, &m_matRotation );
    D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &matRotationInverse );
    D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &m_matTranslation );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

    // Set states
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,          0xff404040 );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

    // Render opaque subsets of the visible frame
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    pMesh->Render( m_pd3dDevice, D3DFILE_RENDERVISIBLEFRAMES|D3DFILE_RENDEROPAQUESUBSETS );

    // Render alpha subsets of the visible frame
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    pMesh->Render( m_pd3dDevice, D3DFILE_RENDERVISIBLEFRAMES|D3DFILE_RENDERALPHASUBSETS );

    // Transparently draw subsets for all other frames
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0x30303030 );
    pMesh->Render( m_pd3dDevice, D3DFILE_RENDERALLFRAMES|D3DFILE_RENDERALLSUBSETS );

    m_pd3dDevice->EndScene();
    m_pd3dDevice->Present( NULL,  NULL,  NULL,  NULL );
}




//-----------------------------------------------------------------------------
// Name: ScreenToVector()
// Desc: Helper function to turn a screen coordinate into a vector. Used by the
//       code below that controls object rotations with the mouse.
//-----------------------------------------------------------------------------
D3DXVECTOR3 CModelRenderView::ScreenToVector( int sx, int sy )
{
    RECT rc;
    GetClientRect( &rc );
    FLOAT fViewportWidth  = ( rc.right - rc.left ) / 2.0f;
    FLOAT fViewportHeight = ( rc.bottom - rc.top ) / 2.0f;

    // Scale to screen
    FLOAT x   =  (sx - fViewportWidth)  / fViewportWidth;
    FLOAT y   = -(sy - fViewportHeight) / fViewportHeight;
    FLOAT z   = 0.0f;
    FLOAT mag = x*x + y*y;

    if( mag > 1.0f )
    {
        FLOAT scale = 1.0f/sqrtf(mag);
        x *= scale;
        y *= scale;
    }
    else
        z = sqrtf( 1.0f - mag );

    // Return vector
    return D3DXVECTOR3( x, y, z );
}




//-----------------------------------------------------------------------------
// Name: OnLButtonDown()
// Desc: Handle mouse buttons, used to control the object rotation.
//-----------------------------------------------------------------------------
void CModelRenderView::OnLButtonDown( UINT nFlags, CPoint point ) 
{
    // Start arcball drag mode
    m_bDrag = TRUE;
    m_vDown = ScreenToVector( point.x, point.y );

    CView::OnLButtonDown(nFlags, point);
}




//-----------------------------------------------------------------------------
// Name: OnLButtonUp()
// Desc: Handle mouse buttons, used to control the object rotation.
//-----------------------------------------------------------------------------
void CModelRenderView::OnLButtonUp( UINT nFlags, CPoint point ) 
{
    // End arcball drag mode
    m_bDrag = FALSE;
    m_qDown = m_qNow;

    CView::OnLButtonUp(nFlags, point);
}




//-----------------------------------------------------------------------------
// Name: OnMouseMove()
// Desc: Handle mouse move messages. Used here when the mouse left button is
//       down, to change the object rotation matrix.
//-----------------------------------------------------------------------------
void CModelRenderView::OnMouseMove( UINT nFlags, CPoint point ) 
{
    if( m_bDrag && m_pd3dDevice )
    {
        // Update arcball
        D3DXVECTOR3 vPart;
        D3DXVECTOR3 vCur = ScreenToVector( point.x, point.y );
        D3DXVec3Cross( &vPart, &m_vDown, &vCur );
        m_qNow = m_qDown * D3DXQUATERNION( vPart.x, vPart.y, vPart.z,
                                           D3DXVec3Dot( &m_vDown, &vCur ) );

        D3DXQUATERNION qConj;
        D3DXQuaternionConjugate( &qConj, &m_qNow );

        D3DXMATRIX matRotationDelta;
        D3DXMatrixRotationQuaternion( &matRotationDelta, &qConj );
        D3DXMatrixTranspose( &matRotationDelta, &matRotationDelta );
        D3DXMatrixMultiply( &m_matRotation, &m_matRotation, &matRotationDelta );

        D3DXQuaternionIdentity( &m_qDown );
        D3DXQuaternionIdentity( &m_qNow );
        m_vDown = ScreenToVector( point.x, point.y );
        m_bDrag = TRUE;

        // Render the scene
        OnDraw( NULL );
    }
    
    CView::OnMouseMove(nFlags, point);
}




//-----------------------------------------------------------------------------
// Name: OnSize()
// Desc: Handle resizing of the model view window, which will trigger a reset
//       for the d3ddevice, and all of it's state.
//-----------------------------------------------------------------------------
void CModelRenderView::OnSize( UINT nType, int cx, int cy ) 
{
    CView::OnSize( nType, cx, cy );
    
    if( m_pd3dDevice )
    {
        CXBGDocument*     pDoc  = GetDocument();
        CD3DFile*         pMesh = pDoc->m_pD3DFile;

        // Set up the structure used to create the D3DDevice.
        D3DPRESENT_PARAMETERS d3dpp; 
        ZeroMemory( &d3dpp, sizeof(d3dpp) );
        d3dpp.hDeviceWindow          = m_hWnd;
        d3dpp.Windowed               = TRUE;
        d3dpp.EnableAutoDepthStencil = TRUE;
        d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
        d3dpp.BackBufferFormat       = g_d3ddm.Format;
        d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

        // Resize the Direct3D device.
        if( FAILED( m_pd3dDevice->Reset( &d3dpp ) ) )
            return;

        // Setup the light
        D3DLIGHT8 light;
        light.Type         = D3DLIGHT_DIRECTIONAL;
        light.Diffuse.r    = light.Diffuse.g  = light.Diffuse.b  = 1.0f; light.Diffuse.a  = 1.0f;
        light.Specular.r   = light.Specular.g = light.Specular.b = 0.0f; light.Specular.a = 1.0f;
        light.Ambient.r    = light.Ambient.g  = light.Ambient.b  = 0.3f; light.Ambient.a  = 1.0f;
        light.Position     = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
        D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &D3DXVECTOR3( 0.3f, -1.0f, 1.0f ) );
        light.Attenuation0 = light.Attenuation1 = light.Attenuation2 = 0.0f;
        light.Range        = sqrtf(FLT_MAX);
        m_pd3dDevice->SetLight(0, &light );
        m_pd3dDevice->LightEnable(0, TRUE );

        // Setup render state
        m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,     TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

        // Setup transforms
        D3DXMATRIX matView, matProj;
        D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0, 0,-3.0f*pDoc->m_pD3DFile->m_fRadius ),
                                      &D3DXVECTOR3( 0, 0, 0 ), &D3DXVECTOR3( 0, 1, 0 ) );
        D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 640.0f/480.0f, 
                                    pDoc->m_pD3DFile->m_fRadius/64.0f, pDoc->m_pD3DFile->m_fRadius*200.0f );
        m_pd3dDevice->SetTransform( D3DTS_VIEW,  &matView );
        m_pd3dDevice->SetTransform( D3DTS_PROJECTION,  &matProj );
    }
}



