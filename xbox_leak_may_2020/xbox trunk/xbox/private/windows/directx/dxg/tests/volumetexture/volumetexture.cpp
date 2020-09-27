//-----------------------------------------------------------------------------
// File: VolumeTexture.cpp
//
// Desc: Example code showing how to do volume textures in D3D.
//
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT

#ifdef XBOX
#include <xtl.h>
#include <xgraphics.h>
#else
#include <windows.h>
#include <D3DX8.h>
#endif
#include <stdio.h>
#include <math.h>
#include "D3DApp.h"
#include "D3DFile.h"
#include "D3DUtil.h"
#include "DXUtil.h"


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
struct VOLUMEVERTEX
{
    FLOAT      x, y, z;
    DWORD      color;
    FLOAT      tu, tv, tw;
};

#define D3DFVF_VOLUMEVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0))

VOLUMEVERTEX g_vVertices[4] =
{
    { 1.0f, 1.0f, 0.0f, 0xffffffff, 1.0f, 1.0f, 0.0f },
    {-1.0f, 1.0f, 0.0f, 0xffffffff, 0.0f, 1.0f, 0.0f },
    { 1.0f,-1.0f, 0.0f, 0xffffffff, 1.0f, 0.0f, 0.0f },
    {-1.0f,-1.0f, 0.0f, 0xffffffff, 0.0f, 0.0f, 0.0f }
};




//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    LPDIRECT3DVOLUMETEXTURE8 m_pVolumeTexture;
    LPDIRECT3DVERTEXBUFFER8  m_pVB;

public:
    void InitDeviceObjects();
    void RestoreDeviceObjects();
    void DeleteDeviceObjects();
    void Render();
    void FrameMove();

    CMyD3DApplication();
};


#if defined(PCBUILD)

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
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

    m_pVolumeTexture = NULL;
    m_pVB            = NULL;
}


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
void CMyD3DApplication::FrameMove()
{
    static FLOAT fAngle = 0.0f;
    fAngle += 0.1f;

    // Play with the volume texture coordinate
    VOLUMEVERTEX* pVertices = NULL;
    CheckHR(m_pVB->Lock( 0, 4*sizeof(VOLUMEVERTEX), (BYTE**)&pVertices, 0 ));
    for( int i=0; i<4; i++ )
        pVertices[i].tw = sinf(fAngle)*sinf(fAngle);
    CheckHR(m_pVB->Unlock());
}




//-----------------------------------------------------------------------------
// Name: App_Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
void CMyD3DApplication::Render()
{
    // Clear the viewport
    CheckHR(m_pDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x00000000, 1.0f, 0L ));

    // Begin the scene
    CheckHR(m_pDevice->BeginScene());
    
    // Draw the quad, with the volume texture
    CheckHR(m_pDevice->SetTexture( 0, m_pVolumeTexture ));
    CheckHR(m_pDevice->SetVertexShader( D3DFVF_VOLUMEVERTEX ));
    CheckHR(m_pDevice->SetStreamSource( 0, m_pVB, sizeof(VOLUMEVERTEX) ));
    CheckHR(m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2));

    // End the scene.
    CheckHR(m_pDevice->EndScene());
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
void CMyD3DApplication::InitDeviceObjects()
{
    // Create a 64x64x4 volume texture
    CheckHR((m_pDevice->CreateVolumeTexture( 16, 16, 16, 1, 0,
                                            D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
                                            &m_pVolumeTexture )));

    // Fill the volume texture
    {
        D3DLOCKED_BOX LockedBox;
        CheckHR(m_pVolumeTexture->LockBox( 0, &LockedBox, 0, 0 ));

        D3DVOLUME_DESC desc;
        m_pVolumeTexture->GetLevelDesc(0, &desc);
        PVOID pBits = LocalAlloc(0, 16 * 16 * 16 * 4);

        if (pBits == NULL)
        {
            m_pVolumeTexture->UnlockBox(0);
            return;
        }

        PVOID pTemp = pBits;

        for( UINT w=0; w<16; w++ )
        {
            BYTE* pSliceStart = (BYTE*)pTemp;

            for( UINT v=0; v<16; v++ )
            {
                for( UINT u=0; u<16; u++ )
                {
                    FLOAT du = (u-7.5f)/7.5f;
                    FLOAT dv = (v-7.5f)/7.5f;
                    FLOAT dw = (w-7.5f)/7.5f;
                    FLOAT fScale = sqrtf( du*du + dv*dv + dw*dw ) / sqrtf(1.0f);

                    if( fScale > 1.0f ) fScale = 0.0f;
                    else                fScale = 1.0f - fScale;

                                        DWORD r = (DWORD)((w<<4)*fScale);
                                        DWORD g = (DWORD)((v<<4)*fScale);
                                        DWORD b = (DWORD)((u<<4)*fScale);

                    ((DWORD*)pTemp)[u] = 0xff000000 + (r<<16) + (g<<8) + (b);
                }
                pTemp = (BYTE*)pTemp + LockedBox.RowPitch;
            }
            pTemp = pSliceStart + LockedBox.SlicePitch;
        }

        D3DBOX box = { 0, 0, 0, 16, 16, 16 };
        XGPOINT3D point = { 0, 0, 0 };

        XGSwizzleBox(pBits, 
                     LockedBox.RowPitch,
                     LockedBox.SlicePitch,
                     &box,
                     LockedBox.pBits,
                     16,
                     16,
                     16,
                     &point,
                     4);

        LocalFree(pBits);

        CheckHR(m_pVolumeTexture->UnlockBox( 0 ));
    }

    // Create a vertex buffer
    {
        CheckHR(m_pDevice->CreateVertexBuffer( 4*sizeof(VOLUMEVERTEX),
                                           D3DUSAGE_WRITEONLY,
                                           D3DFVF_VOLUMEVERTEX,
                                           D3DPOOL_MANAGED, &m_pVB ) );

        VOLUMEVERTEX* pVertices;
        CheckHR(m_pVB->Lock( 0, 4*sizeof(VOLUMEVERTEX), (BYTE**)&pVertices, 0 ));
        memcpy( pVertices, g_vVertices, sizeof(VOLUMEVERTEX)*4 );
        CheckHR(m_pVB->Unlock());
    }
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
void CMyD3DApplication::RestoreDeviceObjects()
{
    // Set the matrices
    D3DXVECTOR3 vEye( 0.0f, 0.0f,-3.0f );
    D3DXVECTOR3 vAt(  0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp(  0.0f, 1.0f, 0.0f );
    D3DXMATRIX matWorld, matView, matProj;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEye,&vAt, &vUp );
    FLOAT fAspect = m_BackBufferDesc.Width / (FLOAT)m_BackBufferDesc.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );
    CheckHR(m_pDevice->SetTransform( D3DTS_WORLD,      &matWorld ));
    CheckHR(m_pDevice->SetTransform( D3DTS_VIEW,       &matView ));
    CheckHR(m_pDevice->SetTransform( D3DTS_PROJECTION, &matProj ));

    // Set state
    CheckHR(m_pDevice->SetRenderState( D3DRS_DITHERENABLE, FALSE ));
    CheckHR(m_pDevice->SetRenderState( D3DRS_CULLMODE,     D3DCULL_NONE ));
    CheckHR(m_pDevice->SetRenderState( D3DRS_LIGHTING,     FALSE ));
    CheckHR(m_pDevice->SetRenderState( D3DRS_ZENABLE,      FALSE ));
    CheckHR(m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE ));

    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR ));
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exitting, or the device is being changed,
//       this function deletes any device dependant objects.
//-----------------------------------------------------------------------------
void CMyD3DApplication::DeleteDeviceObjects()
{
    SAFE_RELEASE( m_pVolumeTexture );
    SAFE_RELEASE( m_pVB );
}



