//-----------------------------------------------------------------------------
// (c) Copyright 2000-2000 Intel Corp. All rights reserved.
//
// 3dsurface.cpp - Graphics test (3DSurface) used to stress the video memory
//					functions of the graphics system.
//
// Member of:  Grafx Module for xmta test executive.
//
//-----------------------------------------------------------------------------
#include "..\..\stdafx.h"
#include "..\..\testobj.h"
#include "xgrafx.h"


#define D3DFVF_PROJTEXVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
#define FRAME_MOVES 150 //Number of frame moves to perform for each scene rendered.
#define SURFTEST_MAX_SCENES 9

//******************************************************************
// Title: Vidmemory
//
// Abstract: Renders a 3D scene to a surface and performs various 
//				memory transfer operations beetween multiple surfaces
//				to stress the video memory systems.
//
//******************************************************************
IMPLEMENT_TESTLOCKED (Grafx, 3DSurface, 1)
{
	DWORD crc = DEFAULT_CRC;//Default seed for crc calculations.
	D3DXMATRIX matWorld( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );

//#ifdef _DEBUG
	static int surfaceTestCount;
	surfaceTestCount++;
	ReportDebug(DEBUG_LEVEL1,_T("Surface Test - Loop %d"), surfaceTestCount);
//#endif

	if (CheckAbort (HERE)){ MainCleanup(); return;};
	
	
	//Check value of required parameters before proceeding.
	if(m_testScenes > SURFTEST_MAX_SCENES)
	{	//Error - invalid number of scenes for this test.
		err_BADPARAMETER( m_modeString, L"scenes");
		MainCleanup();
		return;
	}
	
	if(m_expectedCRC == 0xffffffff)
	{
		err_BADPARAMETER( m_modeString, L"crc" );
		MainCleanup();
		return;
	}
	
	if (CheckAbort (HERE)){ MainCleanup(); return;};
	//Check for an initialized D3D Device.
	if(g_pd3dDevice == NULL)
	{
		//Error - the global device was not created successfully
		//or has become lost or invalid.
		err_NODIRECT3D(m_modeString);
		MainCleanup();
		return;
	}
	
	// Initialize the app's device-dependant objects
    if(!SurfTestInitDisplayObjects() )
	{	//Fatal error - exit the grafx test.
		MainCleanup();
		return;
	}

	if (CheckAbort (HERE)){ MainCleanup(); return;};
	for(int loop=0; loop < m_testScenes; loop++)
	{
		//Stress the 3D rendering engine
		for(int innerLoop = 0; innerLoop < FRAME_MOVES; innerLoop++)
		{	// Frame move the scene
			if(!SurfTestFrameMove(&matWorld))
			{	//Fatal error - exit test.
				MainCleanup();
				return;
			}
			// Render the scene
			SurfTestRender( loop);
			// Show the frame on the primary surface.
			g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
			g_pd3dDevice->SetTexture( 1, NULL );//Release the texture or else a resource leak will happen.
		}

		if(m_hardwareCRC)
			crc = GetDisplayBufferCRC();
		else
			crc = SurfTestCalcCRC(crc);
		//Output the measured crc for each scene.
		ReportDebug(DEBUG_LEVEL1, _T("Surface CRC (loop %d) = 0x%x\n"), loop+1, crc);
		if (CheckAbort (HERE)){ MainCleanup(); return;};
	}
	
	
	ReportDebug(DEBUG_LEVEL1, _T("CRC value to insert into the ini file for the expected test value: = 0x%x\n"), crc);
	if(crc != m_expectedCRC)
	{	//Non-Fatal error, so check status of continue on error flag.
		err_BADCRC(m_modeString, crc, m_expectedCRC, L"");
		if (CheckAbort (HERE)){ MainCleanup(); return;};
	}

	//Cleanup
	MainCleanup();
}



//-----------------------------------------------------------------------------
// Name: InitDisplayObjects()
// Desc: 
//-----------------------------------------------------------------------------
BOOL CXModuleGrafx::SurfTestInitDisplayObjects()
{
    //Params for Vidmemory test.
	m_pSpotLightTexture = NULL;

    m_vTexEyePt    = D3DXVECTOR3(-2.0f, -2.0f, -7.0f );
    m_vTexLookatPt = D3DXVECTOR3( 0.0f,  0.0f,  0.0f );

	// Cube vertices
    m_vCube[ 0].p = D3DXVECTOR3( 3.0f, 3.0f,-3.0f); m_vCube[ 0].color = 0xff0080ff;
    m_vCube[ 1].p = D3DXVECTOR3(-3.0f, 3.0f,-3.0f); m_vCube[ 1].color = 0xff0080ff;
    m_vCube[ 2].p = D3DXVECTOR3(-3.0f, 3.0f, 3.0f); m_vCube[ 2].color = 0xff0080ff;
    m_vCube[ 3].p = D3DXVECTOR3( 3.0f, 3.0f, 3.0f); m_vCube[ 3].color = 0xff0080ff;
    m_vCube[ 4].p = D3DXVECTOR3( 3.0f, 3.0f,-3.0f); m_vCube[ 4].color = 0xff0080ff;
    m_vCube[ 5].p = D3DXVECTOR3(-3.0f, 3.0f, 3.0f); m_vCube[ 5].color = 0xff0080ff;

    m_vCube[ 6].p = D3DXVECTOR3(-3.0f,-3.0f,-3.0f); m_vCube[ 6].color = 0xffff00ff;
    m_vCube[ 7].p = D3DXVECTOR3( 3.0f,-3.0f,-3.0f); m_vCube[ 7].color = 0xffff00ff;
    m_vCube[ 8].p = D3DXVECTOR3( 3.0f,-3.0f, 3.0f); m_vCube[ 8].color = 0xffff00ff;
    m_vCube[ 9].p = D3DXVECTOR3(-3.0f,-3.0f, 3.0f); m_vCube[ 9].color = 0xffff00ff;
    m_vCube[10].p = D3DXVECTOR3(-3.0f,-3.0f,-3.0f); m_vCube[10].color = 0xffff00ff;
    m_vCube[11].p = D3DXVECTOR3( 3.0f,-3.0f, 3.0f); m_vCube[11].color = 0xffff00ff;

    m_vCube[12].p = D3DXVECTOR3( 3.0f,-3.0f,-3.0f); m_vCube[12].color = 0xff00ff00;
    m_vCube[13].p = D3DXVECTOR3( 3.0f, 3.0f,-3.0f); m_vCube[13].color = 0xff00ff00;
    m_vCube[14].p = D3DXVECTOR3( 3.0f, 3.0f, 3.0f); m_vCube[14].color = 0xff00ff00;
    m_vCube[15].p = D3DXVECTOR3( 3.0f,-3.0f, 3.0f); m_vCube[15].color = 0xff00ff00;
    m_vCube[16].p = D3DXVECTOR3( 3.0f,-3.0f,-3.0f); m_vCube[16].color = 0xff00ff00;
    m_vCube[17].p = D3DXVECTOR3( 3.0f, 3.0f, 3.0f); m_vCube[17].color = 0xff00ff00;

    m_vCube[18].p = D3DXVECTOR3(-3.0f, 3.0f,-3.0f); m_vCube[18].color = 0xff808000;
    m_vCube[19].p = D3DXVECTOR3(-3.0f,-3.0f,-3.0f); m_vCube[19].color = 0xff808000;
    m_vCube[20].p = D3DXVECTOR3(-3.0f,-3.0f, 3.0f); m_vCube[20].color = 0xff808000;
    m_vCube[21].p = D3DXVECTOR3(-3.0f, 3.0f, 3.0f); m_vCube[21].color = 0xff808000;
    m_vCube[22].p = D3DXVECTOR3(-3.0f, 3.0f,-3.0f); m_vCube[22].color = 0xff808000;
    m_vCube[23].p = D3DXVECTOR3(-3.0f,-3.0f, 3.0f); m_vCube[23].color = 0xff808000;

    m_vCube[24].p = D3DXVECTOR3( 3.0f,-3.0f,-3.0f); m_vCube[24].color = 0xff0000ff;
    m_vCube[25].p = D3DXVECTOR3(-3.0f,-3.0f,-3.0f); m_vCube[25].color = 0xff0000ff;
    m_vCube[26].p = D3DXVECTOR3(-3.0f, 3.0f,-3.0f); m_vCube[26].color = 0xff0000ff;
    m_vCube[27].p = D3DXVECTOR3( 3.0f, 3.0f,-3.0f); m_vCube[27].color = 0xff0000ff;
    m_vCube[28].p = D3DXVECTOR3( 3.0f,-3.0f,-3.0f); m_vCube[28].color = 0xff0000ff;
    m_vCube[29].p = D3DXVECTOR3(-3.0f, 3.0f,-3.0f); m_vCube[29].color = 0xff0000ff;

    m_vCube[30].p = D3DXVECTOR3(-3.0f,-3.0f, 3.0f); m_vCube[30].color = 0xffff0000;
    m_vCube[31].p = D3DXVECTOR3( 3.0f,-3.0f, 3.0f); m_vCube[31].color = 0xffff0000;
    m_vCube[32].p = D3DXVECTOR3( 3.0f, 3.0f, 3.0f); m_vCube[32].color = 0xffff0000;
    m_vCube[33].p = D3DXVECTOR3(-3.0f, 3.0f, 3.0f); m_vCube[33].color = 0xffff0000;
    m_vCube[34].p = D3DXVECTOR3(-3.0f,-3.0f, 3.0f); m_vCube[34].color = 0xffff0000;
    m_vCube[35].p = D3DXVECTOR3( 3.0f, 3.0f, 3.0f); m_vCube[35].color = 0xffff0000;
	  
	//Init the line vertex data.
	m_lineVertex[0].x = 0.0f;
	m_lineVertex[0].y = 0.0f;
	m_lineVertex[0].z = 0.0f;
	m_lineVertex[0].color = 0xffffffff;
	
	m_lineVertex[1].x = 0.75f;
	m_lineVertex[1].y = 0.75f;
	m_lineVertex[1].z = 0.75f;
	m_lineVertex[1].color = 0xffffffff;

	m_lineVertex[2].x = 0.0f;
	m_lineVertex[2].y = 0.0f;
	m_lineVertex[2].z = 0.0f;
	m_lineVertex[2].color = 0xffffffff;

	m_lineVertex[3].x = -0.5f;
	m_lineVertex[3].y = 0.5f;
	m_lineVertex[3].z = 0.5f;
	m_lineVertex[3].color = 0xffffffff;

	m_lineVertex[4].x = 0.0f;
	m_lineVertex[4].y = 0.0f;
	m_lineVertex[4].z = 0.0f;
	m_lineVertex[4].color = 0xffffffff;

	m_lineVertex[5].x = 0.5f;
	m_lineVertex[5].y = -0.5f;
	m_lineVertex[5].z = 0.5f;
	m_lineVertex[5].color = 0xffffffff;

	m_lineVertex[6].x = 0.0f;
	m_lineVertex[6].y = 0.0f;
	m_lineVertex[6].z = 0.0f;
	m_lineVertex[6].color = 0xffffffff;

	m_lineVertex[7].x = -0.5f;
	m_lineVertex[7].y = -0.5f;
	m_lineVertex[7].z = 0.5f;
	m_lineVertex[7].color = 0xffffffff;


	// Set the view matrix
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f,-15.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
    D3DXMATRIX  matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set the projection matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 200.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	// Set up a point light
    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_POINT, 0.0f, 0.0f, -10.0f );
    light.Attenuation0 = 0.0f;
    light.Attenuation1 = 0.0f;
    light.Attenuation2 = 1.0f;
	g_pd3dDevice->SetLight( 0, &light );
    g_pd3dDevice->LightEnable( 0, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00333333 );
	
	// Set a default white material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    g_pd3dDevice->SetMaterial( &mtrl );

    if(!CreateSpotLightTexture(TRUE))
		return FALSE;//Fatal error - exit the test.

    return TRUE;
}

//-----------------------------------------------------------------------------
// Name: CreateSpotLightTexture()
// Desc: 
//-----------------------------------------------------------------------------
BOOL CXModuleGrafx::CreateSpotLightTexture( BOOL bBorder )
{
    HRESULT hr;
	
	// Release the old texture
    SAFE_RELEASE( m_pSpotLightTexture );

	// Create a new texture
    hr = g_pd3dDevice->CreateTexture( 128, 128, 1, 0, D3DFMT_X8R8G8B8, 
			                        D3DPOOL_DEFAULT, &m_pSpotLightTexture );
	if(hr != D3D_OK)
	{	//Fatal error - exit grafx test.
		ReportD3DError(hr);
		return FALSE;
	}
    	
	//Locking the area provides a pointer to the start of the data.
	D3DLOCKED_RECT lock;
 	if((hr = m_pSpotLightTexture->LockRect(0, &lock, NULL, 0L)) != D3D_OK)
	{	//Fatal error - exit grafx test.
		ReportD3DError(hr);
		return FALSE;
	}

	D3DSURFACE_DESC desc;
    m_pSpotLightTexture->GetLevelDesc( 0, &desc );
    DWORD* pBits    = (DWORD*)lock.pBits;
    DWORD* pLine    = (DWORD*)lock.pBits;
    DWORD  dwStride = lock.Pitch / sizeof(DWORD);

    // Generate the spotlight tex
    for( DWORD y = 0; y < 128; y++ )
    {
        for( DWORD x = 0; x < 128; x++ )
        {
            FLOAT dx = ( 64.0f - x + 0.5f ) / 64.0f;
            FLOAT dy = ( 64.0f - y + 0.5f ) / 64.0f;
            FLOAT r  = cosf( sqrtf( dx*dx + dy*dy ) * D3DX_PI / 2.0f );
            r = (r > 0.0f) ? r*r : 0.0f;
            int c = (int)min((r*r + 0.15f) * 0xff, 0xff);

            pLine[x] = 0xff000000 + 0x00010101*c;
        }

        pLine += dwStride;
    }
    if( bBorder )
    {
        // Draw border around spotlight texture
        DWORD* pLineX1 = (DWORD*)pBits + dwStride;
        DWORD* pLineX2 = (DWORD*)pBits + dwStride * (128 - 2);
        DWORD* pLineY1 = (DWORD*)pBits + 1;
        DWORD* pLineY2 = (DWORD*)pBits + dwStride - 2;

        for( DWORD x = 0; x < 128; x++ )
        {
            if( x != 0 && x != 127 )
            {
                *pLineX1 = *pLineX2 = 0xffffffff;
                *pLineY1 = *pLineY2 = 0xffffffff;
            }

            pLineX1 += 1;
            pLineX2 += 1;
            pLineY1 += dwStride;
            pLineY2 += dwStride;
        }
    }

    // Unlock the map so it can be used
    XBUtil_SwizzleTexture2D( &lock, &desc );
	if((hr = m_pSpotLightTexture->UnlockRect(0)) != D3D_OK)
	{	//Fatal error - exit grafx test.
		ReportD3DError(hr);
		return FALSE;
	}

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: ShowTexture()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXModuleGrafx::ShowTexture( LPDIRECT3DTEXTURE8 pTexture )
{
    D3DSURFACE_DESC d3dsd;
    pTexture->GetLevelDesc( 0, &d3dsd );

    struct SPRITEVERTEX
    {
        FLOAT sx, sy, sz, rhw;
        FLOAT tu, tv;
    };
    
    SPRITEVERTEX vSprite[4] =
    {
        {        0.0f,               0.0f,         0.99f, 1.0f, 0.0f, 0.0f },
        { (FLOAT)d3dsd.Width,        0.0f,         0.99f, 1.0f, 1.0f, 0.0f },
        { (FLOAT)d3dsd.Width, (FLOAT)d3dsd.Height, 0.99f, 1.0f, 1.0f, 1.0f },
        {        0.0f,        (FLOAT)d3dsd.Height, 0.99f, 1.0f, 0.0f, 1.0f },
    };

    // Set state
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,  D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,    D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,    D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    // Display the sprite
    g_pd3dDevice->SetTexture( 0, pTexture );
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vSprite, sizeof(SPRITEVERTEX) );
}




//-----------------------------------------------------------------------------
// Name: CalculateTexProjMatrix()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXModuleGrafx::CalculateTexProjMatrix()
{
	// Get the current view matrix
    D3DXMATRIX matView, matInvView;
    g_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
    D3DXMatrixInverse( &matInvView,  NULL, &matView );

    // Compute view matrix
    D3DXMATRIX matTexView;
    D3DXMatrixLookAtLH( &matTexView, &m_vTexEyePt, &m_vTexLookatPt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
    
    // Compute projection matrix
    D3DXMATRIX matTexProj;
    D3DXMatrixPerspectiveFovLH( &matTexProj, D3DX_PI/6, 1.0f, 1.0f, 200.0f );

    D3DXMATRIX matScale;
    D3DXMatrixIdentity( &matScale );
    matScale._11 = 0.5f;
    matScale._22 = 0.5f;

    D3DXMATRIX matBias;
    D3DXMatrixIdentity( &matBias );
    matBias._31 = 0.5f;
    matBias._32 = 0.5f;

    D3DXMatrixIdentity( &m_matTexProj );
    D3DXMatrixMultiply( &m_matTexProj, &m_matTexProj, &matInvView ); // Transform cameraspaceposition to worldspace
    D3DXMatrixMultiply( &m_matTexProj, &m_matTexProj, &matTexView ); // Transform to worldspace
    D3DXMatrixMultiply( &m_matTexProj, &m_matTexProj, &matTexProj ); // Situate verts relative to projector's view
    D3DXMatrixMultiply( &m_matTexProj, &m_matTexProj, &matScale );   // Scale and bias to map the near clipping plane to texcoords
    D3DXMatrixMultiply( &m_matTexProj, &m_matTexProj, &matBias );    // Scale and bias to map the near clipping plane to texcoords

    // Convert from homogeneous texmap coords to worldspace
    D3DXMATRIX matInvTexView, matInvTexProj;
    D3DXMatrixInverse( &matInvTexView, NULL, &matTexView );
    D3DXMatrixInverse( &matInvTexProj, NULL, &matTexProj );          

    for( int i = 0; i < 8; i++ )
    {
        D3DXVECTOR4 vT( 0.0f, 0.0f, 0.0f, 1.0f );
        vT.x = (i%2) * ( i&0x2 ? -1.0f : 1.0f );
        vT.y = (i%2) * ( i&0x4 ? -1.0f : 1.0f );
        vT.z = (i%2) * ( 1.0f );

        D3DXVec4Transform( &vT, &vT, &matInvTexProj );
        D3DXVec4Transform( &vT, &vT, &matInvTexView );

        m_lineVertex[i].x = vT.x / vT.w;
        m_lineVertex[i].y = vT.y / vT.w;
        m_lineVertex[i].z = vT.z / vT.w;
    }
}


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
BOOL CXModuleGrafx::SurfTestFrameMove(D3DXMATRIX* pMatWorld)
{
	HRESULT hr;
    D3DXMATRIX matRotate;
	
	//Rotate the cube on the x & y axis.
	FLOAT fXRotate = 0.03f;
    FLOAT fYRotate = 0.02f;
    D3DXMatrixRotationYawPitchRoll( &matRotate, -fXRotate, -fYRotate, 0.0f );
    D3DXMatrixMultiply( pMatWorld, pMatWorld, &matRotate );
    if((hr = g_pd3dDevice->SetTransform( D3DTS_WORLD, pMatWorld )) != D3D_OK)
	{	//Fatal error - exit test.
		ReportDebug(DEBUG_LEVEL1, _T("3DSURFACE.CPP - Failed to setTransform in FrameMove()!"));
		//OutputDebugString( _T("3DSURFACE.CPP - Failed to setTransform in FrameMove()!"));
		ReportD3DError(hr);
		return FALSE;
	}
    
	//Move the eye point that projects the light onto the cube.
	m_vTexEyePt.x += 0.005f;
    m_vTexEyePt.y += 0.006f;

	//Keep the projected spotlight texture over the surface fo the cube.
	m_vTexLookatPt.x += 0.003f;
    m_vTexLookatPt.y += 0.003f;

    // Recalculate our texture projection matrix
    CalculateTexProjMatrix();

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
VOID CXModuleGrafx::SurfTestRender(int scene)
{
	
	// Begin the scene
    g_pd3dDevice->BeginScene();
	
	g_pd3dDevice->Clear( 0, NULL,
						D3DCLEAR_TARGET|D3DCLEAR_TARGET_R|D3DCLEAR_TARGET_G|D3DCLEAR_TARGET_G|
						D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
	
	
	DisplayGradientBackground(scene);
		
	// Set default render states
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );
		
	//Set lighting params
	g_pd3dDevice->SetTexture( 1, m_pSpotLightTexture );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADD );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

	// Set up texture and states to draw the projected spotlight
    g_pd3dDevice->SetTransform( D3DTS_TEXTURE1, &m_matTexProj );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX,         D3DTSS_TCI_CAMERASPACEPOSITION );

    // Draw the cube
    g_pd3dDevice->SetVertexShader( D3DFVF_PROJTEXVERTEX );
    g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 12, 
                                   m_vCube, sizeof(PROJTEXVERTEX) );
	
    // Draw texture projection frustrum
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZ|D3DFVF_DIFFUSE );
    g_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 4, m_lineVertex, sizeof(m_lineVertex[0]) );

    // End the scene.
    g_pd3dDevice->EndScene();
}


DWORD CXModuleGrafx::SurfTestCalcCRC(DWORD crc)
{
	D3DLOCKED_RECT pLockedRect;		//Locked rectangle struct for determing surface image data.
	LPDIRECT3DSURFACE8 lpd3dSurface = NULL;
	LONG pitch;
	BYTE *pBits;
	HRESULT hr;
	int bytesPerLine;
	
	if((hr = g_pd3dDevice->GetBackBuffer( -1, D3DBACKBUFFER_TYPE_MONO, &lpd3dSurface)) != D3D_OK)
	{	//Fatal error - exit the test.
		ReportDebug(DEBUG_LEVEL1, _T("3DSURFACE.CPP - Failed to GetFrontBuffer() for CRC calc!\n"));
		ReportD3DError(hr);
		if (CheckAbort (HERE))
		{
			SAFE_RELEASE(lpd3dSurface);
			MainCleanup();
			return 0;
		}
	}
	if((hr = lpd3dSurface->LockRect( &pLockedRect, NULL, NULL )) != D3D_OK)
	{	//Fatal error - exit the test.
		ReportDebug(DEBUG_LEVEL1, _T("3DSURFACE.CPP - Failed to LockRect() for CRC calc!\n"));
		ReportD3DError(hr);
		if (CheckAbort (HERE))
		{
			SAFE_RELEASE(lpd3dSurface);
			MainCleanup();
			return 0;
		}
	}
	pBits = (BYTE *)pLockedRect.pBits;
	pitch = pLockedRect.Pitch;
	bytesPerLine = ((m_bpp * (m_dwDisplayWidth)) + 7) >> 3;
	
	crc = UpdateImageRectCRC(m_dwDisplayHeight, pBits, pitch, bytesPerLine);
	
	//Unlock and release the surface.
	lpd3dSurface->UnlockRect();
	SAFE_RELEASE(lpd3dSurface);

	return crc;
}


//-----------------------------------------------------------------------------
// Name: DisplayGradientBackground()
// Desc: Draws a gradient filled background
//-----------------------------------------------------------------------------
HRESULT CXModuleGrafx::DisplayGradientBackground( int scene)
{
    DWORD bgColor[] = 
	{
		0xFFFF0000, 0xFF00FFFF, 0xFF00FF00, 0xFFFF00FF,
		0xFFFFFF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
		0xFFFF00FF, 0xFF0000FF
	};

	// First time around, allocate a vertex buffer
    if( m_pBackgroundVB == NULL )
	{
        g_pd3dDevice->CreateVertexBuffer( 4*5*sizeof(FLOAT), D3DUSAGE_WRITEONLY, 
                                          0L, D3DPOOL_DEFAULT, &m_pBackgroundVB );
	}

	struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
	BACKGROUNDVERTEX* v;
	m_pBackgroundVB->Lock( 0, 0, (BYTE**)&v, 0L);
	v[0].p = D3DXVECTOR4(   0,   0, 1.0f, 1.0f ); v[0].color = bgColor[scene];
	v[1].p = D3DXVECTOR4( (float)m_dwDisplayWidth,   0, 1.0f, 1.0f ); v[1].color = bgColor[scene];
	v[2].p = D3DXVECTOR4(   0, (float)m_dwDisplayHeight, 1.0f, 1.0f ); v[2].color = bgColor[scene+1];
	v[3].p = D3DXVECTOR4( (float)m_dwDisplayWidth, (float)m_dwDisplayHeight, 1.0f, 1.0f ); v[3].color = bgColor[scene+1];
	m_pBackgroundVB->Unlock();
	
    // Set states
    g_pd3dDevice->SetTexture( 0, NULL );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
    g_pd3dDevice->SetStreamSource( 0, m_pBackgroundVB, 5*sizeof(FLOAT) );

    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	// Clear the zbuffer
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L );
	
    return S_OK;
}


