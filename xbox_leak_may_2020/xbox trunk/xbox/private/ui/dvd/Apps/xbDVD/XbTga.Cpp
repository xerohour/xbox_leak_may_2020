////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// File: XBTga.cpp
//
// Desc: Support class for rendering a help image, which is an image of an Xbox
//       gamepad, with labelled callouts to each of the gamepad's controls.
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <tchar.h>
#include <stdio.h>
#include "XBTga.h"



//-----------------------------------------------------------------------------
// Name: CXBTga()
// Desc: Help class constructor
//-----------------------------------------------------------------------------
CXBTga::CXBTga()
{
    m_pd3dDevice      = NULL;
    m_pTgaTexture = NULL;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Creates the help class' internal objects
//-----------------------------------------------------------------------------
HRESULT CXBTga::Create( LPDIRECT3DDEVICE8 pd3dDevice, char* pFilename, RECT rect)
{
    // Keep track of the device
    m_pd3dDevice = pd3dDevice;
	m_pos = rect;

    // Create the gamepad texture
    if( FAILED( XBUtil_CreateTexture( m_pd3dDevice, pFilename,
                                      &m_pTgaTexture, D3DFMT_A8R8G8B8 ) ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Destroys the class' internal objects/
//-----------------------------------------------------------------------------
HRESULT CXBTga::Destroy()
{
    SAFE_RELEASE(m_pTgaTexture);
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the tga image
//-----------------------------------------------------------------------------
HRESULT CXBTga::Render()
{
    // First time around, create a vertex buffer
    static LPDIRECT3DVERTEXBUFFER8 g_pVB  = NULL;
    if( g_pVB == NULL )
    {
        m_pd3dDevice->CreateVertexBuffer( 4*6*sizeof(FLOAT), D3DUSAGE_WRITEONLY,
                                          0L, D3DPOOL_DEFAULT, &g_pVB );
        struct VERTEX { D3DXVECTOR4 p; FLOAT tu, tv; };
        VERTEX* v;
        g_pVB->Lock( 0, 0, (BYTE**)&v, NULL );
        v[0].p = D3DXVECTOR4( (float)  m_pos.left, (float) m_pos.top, 0.0f, 0.5f );
		v[0].tu = 0.0f;
		v[0].tv = 0.0f;
        v[1].p = D3DXVECTOR4( (float) m_pos.right, (float) m_pos.top, 0.0f, 0.5f );
		v[1].tu = 1.0f;
		v[1].tv = 0.0f;
        v[2].p = D3DXVECTOR4( (float) m_pos.left, (float) m_pos.bottom, 0.0f, 0.5f );
		v[2].tu = 0.0f;
		v[2].tv = 1.0f;
        v[3].p = D3DXVECTOR4( (float) m_pos.right, (float) m_pos.bottom, 0.0f, 0.5f );
		v[3].tu = 1.0f;
		v[3].tv = 1.0f;
        g_pVB->Unlock();
    }

    // Set state to render the tga image
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );

	m_pd3dDevice->SetTexture( 0, m_pTgaTexture );
//    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
//    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
//    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
//    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
//    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
//    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );

    m_pd3dDevice->SetStreamSource( 0, g_pVB, 6*sizeof(FLOAT) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW );

    return S_OK;
}



