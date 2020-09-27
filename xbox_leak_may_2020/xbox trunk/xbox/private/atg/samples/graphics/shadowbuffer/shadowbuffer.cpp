//-----------------------------------------------------------------------------
// File: ShadowBuffer.cpp
//
// Desc: Illustrates how to do shadow buffering on the XBox.
//
// Hist: 06.06.01 - Adding start/stop capability
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include "resource.h"
#include "xpath.h"
#include "xmenu.h"
#include "xobj.h"




//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "Floor.bmp",  resource_Floor_OFFSET },
    { "Wings.bmp",  resource_Wings_OFFSET },
    { "BiHull.bmp", resource_BiHull_OFFSET },
    { NULL, 0 },
};

extern CXBPackedResource* g_pModelResource;




//-----------------------------------------------------------------------------
// Shadowbuffer types
//-----------------------------------------------------------------------------
#define SHADOWBUFFERTYPE_D16    0
#define SHADOWBUFFERTYPE_D24S8  1
#define SHADOWBUFFERTYPE_F16    2
#define SHADOWBUFFERTYPE_F24S8  3

// Z ranges for all buffer types
float g_fShadowBufferZRange[4] = { D3DZ_MAX_D16, D3DZ_MAX_D24S8, 
                                   D3DZ_MAX_F16, (float)D3DZ_MAX_F24S8 };

// Descriptions
WCHAR g_fShadowBufferDesc[4][8] = { L"D16", L"D24S8", L"F16", L"F24S8" };

// Shadow buffer width and height.
const int SHADOWBUFFERWIDTH  = 512;
const int SHADOWBUFFERHEIGHT = 512;



//-----------------------------------------------------------------------------
// Projection frustrum
//-----------------------------------------------------------------------------
struct LINEVERTEX
{
    FLOAT x, y, z;
    DWORD color;
};


D3DXVECTOR4 g_vHomogenousFrustum[8] =
{
    D3DXVECTOR4( 1.0f, 1.0f, 0.0f, 1.0f ),
    D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ),

    D3DXVECTOR4(-1.0f, 1.0f, 0.0f, 1.0f ),
    D3DXVECTOR4(-1.0f, 1.0f, 1.0f, 1.0f ),

    D3DXVECTOR4(-1.0f,-1.0f, 0.0f, 1.0f ),
    D3DXVECTOR4(-1.0f,-1.0f, 1.0f, 1.0f ),

    D3DXVECTOR4( 1.0f,-1.0f, 0.0f, 1.0f ),
    D3DXVECTOR4( 1.0f,-1.0f, 1.0f, 1.0f ),
};


LINEVERTEX g_vFrustumLines[8] =
{
    { 1.0f, 1.0f, 0.0f, 0xffffffff },
    { 1.0f, 1.0f, 1.0f, 0xffffffff },

    {-1.0f, 1.0f, 0.0f, 0xffffffff },
    {-1.0f, 1.0f, 1.0f, 0xffffffff },

    {-1.0f,-1.0f, 0.0f, 0xffffffff },
    {-1.0f,-1.0f, 1.0f, 0xffffffff },

    { 1.0f,-1.0f, 0.0f, 0xffffffff },
    { 1.0f,-1.0f, 1.0f, 0xffffffff },
};



//-----------------------------------------------------------------------------
// Help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Move light" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Move camera" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_2, L"Adjust\nZ offset/slope" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_1, L"Toggle rotation" },
    { XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_1, L"Toggle frustum" },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_2, L"Change\nZ-buffer format" },
    { XBHELP_LEFT_BUTTON,  XBHELP_PLACEMENT_1, L"Zoom Out" },
    { XBHELP_RIGHT_BUTTON, XBHELP_PLACEMENT_1, L"Zoom In" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
};

#define NUM_HELP_CALLOUTS (sizeof(g_HelpCallouts)/sizeof(g_HelpCallouts[0]))




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont     m_Font;
    CXBHelp     m_Help;
    BOOL        m_bDrawHelp;
    
    CXBPackedResource  m_xprResource;

    D3DXMATRIX  m_matProj;
    D3DXMATRIX  m_matShadowProj;
    D3DXVECTOR3 m_vCameraPos;
    D3DXVECTOR3 m_vCameraRot;
    D3DXMATRIX  m_matView;

    D3DXVECTOR3 m_vLightPos;
    D3DXMATRIX  m_matTexture;                    // Texture projection matrix

    CXObject    m_FloorObj;
    CXObject    m_Obj;
    CXObject    m_LightObj;

    float       m_fObjRadius;

    LPDIRECT3DSURFACE8 m_pShadowBufferTarget;
    LPDIRECT3DTEXTURE8 m_pShadowBufferDepth;    // Shadow buffer depth texture

    LPDIRECT3DSURFACE8 m_pRenderTarget;
    LPDIRECT3DSURFACE8 m_pZBuffer;              // Back buffer depth surface

    D3DSurface  m_FakeTarget;

    DWORD       m_dwShadowBufVS;                // Shadow buffer vertex shader
    DWORD       m_dwShadowBufPS;                // Shadow buffer pixel shader

    DWORD       m_dwShadowBufferType;           // Shadowbuffer type
    float       m_fZOffset;                     // Shadowbuffer z offset
    float       m_fZSlopeScale;                 // Shadowbuffer z slope scale

    BOOL        m_bRotateFlag;                  // Object rotate flag
    BOOL        m_bDrawFrustum;                 // Draw frustum flag

public:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();
    HRESULT InitPixelShader();

    HRESULT DisplaySBObject(CXObject *obj);

    CXBoxSample();
};




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
void __cdecl main()
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
    m_vCameraPos = D3DXVECTOR3(0.0f, 20.0f, -30.0f);
    m_vCameraRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

    m_fZOffset = 4.0f;
    m_fZSlopeScale = 2.0f;
    m_bRotateFlag = true;
    m_bDrawFrustum = true;
    m_bDrawHelp = false;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    D3DXVECTOR3 v;

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create our vertex shader
    DWORD vdecl[] =
    {
        D3DVSD_STREAM(0),
        D3DVSD_REG(0, D3DVSDT_FLOAT3), // v0 = XYZ
        D3DVSD_REG(1, D3DVSDT_FLOAT3), // v1 = normals
        D3DVSD_REG(2, D3DVSDT_FLOAT2), // v2 = TEX1
        D3DVSD_END()
    };

    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "shaders\\vshader.xvu", vdecl, &m_dwShadowBufVS ) ) )
        return E_FAIL;

    if( FAILED( XBUtil_CreatePixelShader( m_pd3dDevice, "shaders\\shadwbuf.xpu", &m_dwShadowBufPS ) ) )
        return E_FAIL;

    // Set projection transform
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 640.0f/480.0f, 1.0f, 1000.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Set the resource globally so the CXModel can access textures
    g_pModelResource = &m_xprResource;

    // Set base path to point to our data. All the _FNA commands use this
    XPath_SetBasePath( _T("d:\\media\\") );

    // Floor
    m_FloorObj.m_Model = new CXModel;
    m_FloorObj.m_Model->GridXZ(16, 16, 80.0f, 80.0f, FVF_XYZNORMTEX1, "Floor.bmp", 10.0f, 10.0f);

    // Object
    CXBMesh *xbm = new CXBMesh;
    xbm->Create(g_pd3dDevice, "Models\\Airplane.xbg", &m_xprResource);
    m_Obj.SetXBMesh(xbm);
    m_Obj.SetPosition(0.0f, 4.0f, 0.0f);
    m_Obj.SetRotation(-1.5708f, 0.0f, 0.0f);
    m_fObjRadius = m_Obj.GetRadius();

    // Light
    m_LightObj.m_Model = new CXModel;
    m_LightObj.m_Model->Cylinder( 0.2f, 0xc0ffffff, 0.5f, 0x00ffffff, 0.6f, 16, FVF_XYZDIFF, NULL, 1.0f );
    m_LightObj.SetPosition( 10.0f, 10.0f, 0.0f );

    // Create shadow buffer
    m_pd3dDevice->CreateTexture( SHADOWBUFFERWIDTH, SHADOWBUFFERHEIGHT, 1, 0, 
                                 D3DFMT_LIN_D16, 0, &m_pShadowBufferDepth );
    m_dwShadowBufferType = SHADOWBUFFERTYPE_D16;

    // Setup dummy color buffer (bad things will happen if you write to it).
    ZeroMemory( &m_FakeTarget, sizeof(m_FakeTarget) );
    XGSetSurfaceHeader( SHADOWBUFFERWIDTH, SHADOWBUFFERHEIGHT, D3DFMT_LIN_R5G6B5,
                        &m_FakeTarget, 0, 0 );

    m_pShadowBufferTarget = &m_FakeTarget;

    // Get original color and z-buffer.
    m_pd3dDevice->GetDepthStencilSurface( &m_pZBuffer );
    m_pd3dDevice->GetRenderTarget( &m_pRenderTarget );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX  mat;

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    // Toggle frustum
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] )
        m_bDrawFrustum = !m_bDrawFrustum;

    // Toggle object rotation
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
        m_bRotateFlag = !m_bRotateFlag;

    // Rotate the object
    if( m_bRotateFlag )
        m_Obj.m_vRotation.y += 1.57f*m_fElapsedAppTime;
    m_Obj.m_vRotation.x = 0.0f;

    // Check for buffer change
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE] )
    {
        // Release existing depth buffer
        m_pShadowBufferDepth->Release();

        switch( m_dwShadowBufferType )
        {
            case SHADOWBUFFERTYPE_D16:
                m_pd3dDevice->CreateTexture( SHADOWBUFFERWIDTH, SHADOWBUFFERHEIGHT, 1, 0, 
                                             D3DFMT_LIN_D24S8, 0, &m_pShadowBufferDepth );
                m_dwShadowBufferType = SHADOWBUFFERTYPE_D24S8;
                break;
            case SHADOWBUFFERTYPE_D24S8:
                m_pd3dDevice->CreateTexture( SHADOWBUFFERWIDTH, SHADOWBUFFERHEIGHT, 1, 0, 
                                             D3DFMT_LIN_F16, 0, &m_pShadowBufferDepth );
                m_dwShadowBufferType = SHADOWBUFFERTYPE_F16;
                break;
            case SHADOWBUFFERTYPE_F16:
                m_pd3dDevice->CreateTexture( SHADOWBUFFERWIDTH, SHADOWBUFFERHEIGHT, 1, 0, 
                                             D3DFMT_LIN_F24S8, 0, &m_pShadowBufferDepth );
                m_dwShadowBufferType = SHADOWBUFFERTYPE_F24S8;
                break;
            case SHADOWBUFFERTYPE_F24S8:
                m_pd3dDevice->CreateTexture( SHADOWBUFFERWIDTH, SHADOWBUFFERHEIGHT, 1, 0, 
                                             D3DFMT_LIN_D16, 0, &m_pShadowBufferDepth );
                m_dwShadowBufferType = SHADOWBUFFERTYPE_D16;
                break;
        }
    }

    // Adjust z offset
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
        m_fZOffset += 0.5f;
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
        m_fZOffset -= 0.5f;

    // Adjust z offset slope scale
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )
        m_fZSlopeScale += 0.1f;
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
        m_fZSlopeScale -= 0.1f;

    // Adjust light position
    m_LightObj.m_vPosition.x += m_DefaultGamepad.fX1*m_fElapsedTime*8.0f;
    m_LightObj.m_vPosition.z += m_DefaultGamepad.fY1*m_fElapsedTime*8.0f;

    // Adjust camera position
    D3DXVECTOR3 vAt(0.0f, 4.0f, 0.0f);
    D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);

    // Rotate camera around z axis.
    D3DXMATRIX matRotate;
    D3DXMatrixRotationAxis( &matRotate, &vUp, m_DefaultGamepad.fX2*m_fElapsedTime );
    D3DXVec3TransformCoord( &m_vCameraPos, &m_vCameraPos, &matRotate );

    // Rotate camera points around side axis.
    D3DXVECTOR3 vView = (m_vCameraPos - vAt);
    D3DXVec3Normalize(&vView, &vView);

    // Place limits so we dont go over the top or under the bottom.
    FLOAT dot = D3DXVec3Dot(&vView, &vUp);
    if( (dot > -0.99f || m_DefaultGamepad.fY2 < 0.0f) && (dot < 0.99f || m_DefaultGamepad.fY2 > 0.0f) )
    {
        D3DXVECTOR3 axis;
        D3DXVec3Cross( &axis, &vUp, &vView );
        D3DXMatrixRotationAxis( &matRotate, &axis, m_DefaultGamepad.fY2*m_fElapsedTime );
        D3DXVec3TransformCoord( &m_vCameraPos, &m_vCameraPos, &matRotate );
    }

    // In/out based on triggers.
    float fIn = (m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] / 255.0f);
    float fOut = (m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] / 255.0f);

    if( fIn > 0.1f )
        m_vCameraPos -= vView * 30.0f * fIn * m_fElapsedTime;

    if( fOut > 0.1f )
        m_vCameraPos += vView * 30.0f * fOut * m_fElapsedTime;

    D3DXMatrixLookAtLH( &m_matView, &m_vCameraPos, &vAt, &vUp );

    // Light orientation (looks at object)
    D3DXMatrixLookAtLH( &m_LightObj.m_matOrientation, &m_LightObj.m_vPosition, &m_Obj.m_vPosition, &up );
    D3DXMatrixInverse( &m_LightObj.m_matOrientation, NULL, &m_LightObj.m_matOrientation );

    // Generate the texture transform matrix
    //
    // Note: if you are using multiple lights & shadow buffers, this needs
    // to be calculated for each light.
    //
    // We are starting with worldspace coordinates, so we need to
    // transform from worldspace to lightspace.
    //
    // We go from 3D worldspace to 3D lightspace by multiplying by the
    // inverse of the light matrix.
    // You will probably notice that we are doing two inverses in a row (see previous
    // line of code). I did this to clearly illustrate the steps in generating the
    // texture matrix. The previous line would not always be available.
    D3DXMatrixInverse( &m_matTexture, NULL, &m_LightObj.m_matOrientation );

    // Find a projection that will fit all the objects we want to shadow into the
    // view frustum of the light.
    float fRadius = m_fObjRadius;
    float fDist   = D3DXVec3Length( &(m_Obj.m_vPosition - m_LightObj.m_vPosition) );
    float fNear   = fDist - fRadius;
    float fFar    = fNear + 100.0f;
    float fAngle  = 2.0f * asinf(fRadius / fDist);
    
    D3DXMatrixPerspectiveFovLH( &m_matShadowProj, fAngle, 1.0f, fNear, fFar );

    // Combine the light orientation matrix with the shaowbuffer projection 
    // matrix.  This projects our light space position onto the shadowbuffer 
    // the same way a projection matrix projects a cameraspace coordinate onto 
    // the screen.
    D3DXMatrixMultiply( &m_matTexture, &m_matTexture, &m_matShadowProj );

    // Finally, we scale and offset by SHADOWBUFFERWIDTH/2, SHADOWBUFFERHEIGHT/2
    // to move from [-1,+1] space to [0:0, SHADOWBUFFERWIDTH:SHADOWBUFFERHEIGHT]
    // texture space we also need to scale z by the zbuffer range.  An additional
    // half texel offset is necessary because of the differences between texture
    // addressing and pixel addressing.
    D3DXMatrixIdentity( &mat );

    // Scale
    mat._11 = SHADOWBUFFERWIDTH * 0.5f;
    mat._22 = -SHADOWBUFFERHEIGHT * 0.5f;
    mat._33 = g_fShadowBufferZRange[m_dwShadowBufferType];

    // Offset
    mat._41 = SHADOWBUFFERWIDTH*0.5f + 0.5f;
    mat._42 = SHADOWBUFFERHEIGHT*0.5f + 0.5f;

    D3DXMatrixMultiply( &m_matTexture, &m_matTexture, &mat );

    // m_TextureMat now holds the appropriate transformation matrix
    // for shadowmapping on the XBox GPU.

    // Calculate the frustum lines
    D3DXMATRIX matInvTexProj;
    D3DXMatrixInverse( &matInvTexProj, NULL, &m_matShadowProj );

    for( int i = 0; i < 8; i++ )
    {
        D3DXVECTOR4 vT = g_vHomogenousFrustum[i];

        D3DXVec4Transform( &vT, &vT, &matInvTexProj );
        D3DXVec4Transform( &vT, &vT, &m_LightObj.m_matOrientation );

        g_vFrustumLines[i].x = vT.x / vT.w;
        g_vFrustumLines[i].y = vT.y / vT.w;
        g_vFrustumLines[i].z = vT.z / vT.w;
    }

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
    D3DXMATRIX m;
    LPDIRECT3DSURFACE8 pSurface;

    // Restore state that text clobbers
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

    // Render the scene into the shadow buffer from the viewpoint of the light

    // Set shadowbuffer as render target z buffer & clear it
    m_pShadowBufferDepth->GetSurfaceLevel( 0, &pSurface );

    m_pd3dDevice->SetRenderTarget( m_pShadowBufferTarget, pSurface );

    D3DVIEWPORT8 viewport = { 0, 0, SHADOWBUFFERWIDTH, SHADOWBUFFERHEIGHT, 0.0f, 1.0f };
    m_pd3dDevice->SetViewport( &viewport );

    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0 ); 

    D3DXMatrixInverse( &m, NULL, &m_LightObj.m_matOrientation );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matShadowProj );
    
    // Disable color writes
    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0 );

    // Disable culling so all triangles cause shadows
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Turn on z-offset.
    m_pd3dDevice->SetRenderState( D3DRS_SOLIDOFFSETENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_POLYGONOFFSETZOFFSET, FtoDW(m_fZOffset) );
    m_pd3dDevice->SetRenderState( D3DRS_POLYGONOFFSETZSLOPESCALE, FtoDW(m_fZSlopeScale) );

    // Render our scene into the shadowbuffer
    m_FloorObj.Render(0);
    m_Obj.Render(0);

    // Restore important state
    m_pd3dDevice->SetRenderTarget( m_pRenderTarget, m_pZBuffer );
    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL );
    m_pd3dDevice->SetRenderState( D3DRS_SOLIDOFFSETENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

    pSurface->Release();

    // Now render the scene from the point of view of the camera
    // with shadow compare functionality enabled

    // Clear the main view
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0xff400000, 1.0f, 0L );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    // Set shadowbuffer state
    m_pd3dDevice->SetRenderState( D3DRS_SHADOWFUNC, D3DCMP_GREATER );

    // Set shadowbuffer texture
    m_pd3dDevice->SetTexture( 1, m_pShadowBufferDepth );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_BORDERCOLOR, 0xffffffff );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

    // Set the shadowbuffer pixel shader
    m_pd3dDevice->SetPixelShader( m_dwShadowBufPS );

    // Render the objects in the scene 
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

    DisplaySBObject( &m_FloorObj );
    DisplaySBObject( &m_Obj );

    // Reset shadowbuffer state
    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetTexture( 1, NULL );
    m_pd3dDevice->SetRenderState(D3DRS_SHADOWFUNC, D3DCMP_NEVER );

    // Draw the light object
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    m_LightObj.Render( OBJ_NOMCALCS );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

    // Draw light frustrum
    if( m_bDrawFrustum )
    {
        m_pd3dDevice->SetTexture( 1, NULL );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

        D3DXMATRIX matIdentity;
        D3DXMatrixIdentity( &matIdentity );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
        m_pd3dDevice->SetVertexShader( D3DFVF_XYZ|D3DFVF_DIFFUSE );
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 4, g_vFrustumLines, 
                                       sizeof(g_vFrustumLines[0]) );

        m_pd3dDevice->DrawVerticesUP( D3DPT_LINELOOP, 4, g_vFrustumLines, 
                                      sizeof(g_vFrustumLines[0])*2 );

        m_pd3dDevice->DrawVerticesUP( D3DPT_LINELOOP, 4, g_vFrustumLines+1, 
                                      sizeof(g_vFrustumLines[0])*2 );
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
    {
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    }
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"ShadowBuffer" );
        m_Font.DrawText( 640-64, 50, 0xffffff00, m_strFrameRate, XBFONT_RIGHT );

        // Show buffer description
        WCHAR strBuffer[80];
        swprintf( strBuffer, L"Type: %s, ZOffset: %.01f, ZOffset Slope Scale: %.01f", 
                             g_fShadowBufferDesc[m_dwShadowBufferType], 
                             m_fZOffset, m_fZSlopeScale );
        m_Font.DrawText( 64, 70, 0xffffffff, strBuffer );

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DisplaySBObject
// Desc: Displays a shadowbuffered object. 
//       The two matrices set up here are the World*View*Projection matrix
//       that transforms the objects points on to the screen, and the
//       World*Texture matrix that transforms the objects points into
//       shadowbuffer space.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DisplaySBObject( CXObject* pObject )
{
    D3DXMATRIX m, wvpmat, wtmat;

    pObject->CrunchMatrix();                    // Get object orientation matrix

    // WVP matrix
    D3DXMatrixMultiply( &m, &pObject->m_matOrientation, &m_matView );
    D3DXMatrixMultiply( &wvpmat, &m, &m_matProj );
    D3DXMatrixTranspose( &wvpmat, &wvpmat );
    m_pd3dDevice->SetVertexShaderConstant( 0, &wvpmat, 4 );

    // WT matrix
    D3DXMatrixMultiply( &wtmat, &pObject->m_matOrientation, &m_matTexture );
    D3DXMatrixTranspose( &wtmat, &wtmat);
    m_pd3dDevice->SetVertexShaderConstant( 4, &wtmat, 4 );

    // Light position
    D3DXVECTOR4 v4LocalLightPos;
    D3DXMatrixInverse( &m, NULL, &pObject->m_matOrientation );
    D3DXVec3Transform( &v4LocalLightPos, &m_LightObj.m_vPosition, &m );
    m_pd3dDevice->SetVertexShaderConstant( 8, &v4LocalLightPos, 1 );

    // Ambient color
    float fAmbient[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    m_pd3dDevice->SetPixelShaderConstant( 0, fAmbient, 1 );

    // Send the object
    if( pObject->m_Model )
    {
        pObject->m_Model->SetVertexShader( m_dwShadowBufVS );         // Set shadow buffer vshader
        pObject->m_Model->Render();
        pObject->m_Model->SetVertexShader( 0 );                       // Restore to fvf vshader
    }
    else    // Object has an XBG model
    {
        g_pd3dDevice->SetVertexShader( m_dwShadowBufVS );
        pObject->m_pXBMesh->Render( m_pd3dDevice, XBMESH_NOFVF );
    }       

    return S_OK;
}
