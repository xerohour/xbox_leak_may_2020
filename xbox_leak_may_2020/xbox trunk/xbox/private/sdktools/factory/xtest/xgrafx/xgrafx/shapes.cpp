//-------------------------------------------------------------
//
// Shapes.Cpp - Test module to stress the polygon fill rate for
//				various shapes using random sizes and depths.
//
//-------------------------------------------------------------
#include "..\..\stdafx.h"
#include "..\..\testobj.h"
#include "xgrafx.h"


// Our custom FVF, which describes our custom vertex structure
//NOTE:  The maximum number of vertices that the current hardware can support
// at once is 0xffff (65,535), which equals 21,845 triangles.

#define D3DFVF_SHAPES_TEST_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
#define NUM_COLORS 32

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
//Lookup table for colors to be selected at random for creating shapes.
DWORD shapeColor[] = 
{
	0xFFFFFFFF, 0xFF000000, 0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFF00FF, 0xFF00FFFF, 0xFFFFFF00,
	0xFF0F0F0F, 0xFF6F6F6F, 0xFFFF6F6F, 0xFFFF6F00,	0xFF606F00, 0xFFFF666F, 0xFF663F6F, 0xFFFFFF66,
	0xFF6666ff, 0xFF4F9F2F, 0xFF8F4F0F, 0xFF0F3FcF,	0xFF0F7FdF, 0xFFaF4F8F, 0xFFeF006F, 0xFF1F11F0,
	0xFF054F1F, 0xFF3F4F5F, 0xFF5F4F3F, 0xFF1F6F9F,	0xFF9F1F6F, 0xFF4F8F1F, 0xFF5F1F9F, 0xFFdFbFaF
};


//******************************************************************
// Title: Shapes
//
// Abstract: Renders a 3D scene comprised of random sizes of triangles
//				and rectangles filled with various colors.  Will stress
//				fill rate and ability to calc projections, intercepts,
//				and depths through a variety of angles.
//
//******************************************************************
IMPLEMENT_TESTLOCKED (Grafx, Shapes, 3)
{
	DWORD crc = DEFAULT_CRC;//Default seed for crc calculations.

//#ifdef _DEBUG
	static int shapesTestCount;
	shapesTestCount++;
	ReportDebug(DEBUG_LEVEL1, _T("Shapes Test - Loop %d"), shapesTestCount);
//#endif
	
	if (CheckAbort (HERE)){ MainCleanup(); return;};
	
	if(m_expectedCRC == 0xffffffff)
	{
		err_BADPARAMETER( m_modeString, L"crc" );
		MainCleanup();
		return;
	}
	
	if(m_testScenes < 1 || m_testScenes > 20)
	{
		err_BADPARAMETER( m_modeString, L"scenes" );
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
    if(!ShapeTestInitD3D() )
	{	//Fatal error - exit the grafx test.
		MainCleanup();
		return;
	}

    //Initialize geometric shapes.
	if(!ShapeTestInitGeometry() )
	{	//Fatal error - exit the grafx test.
		MainCleanup();
		return;
	}

	if (CheckAbort (HERE)){ MainCleanup(); return;};
	for(int loop=0; loop < m_testScenes; loop++)
	{
		float rotateAngle = 0.0f;
		rotateAngle = 0.30f * (float)loop;
		
		for(float loop2=0.0; loop2 < 0.30f; loop2+= 0.0025f)
		{	
			// Render the scene
			ShapeTestRender(rotateAngle + loop2);
			// Show the frame on the primary surface.
			g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		}		
		if(m_hardwareCRC)
			crc = GetDisplayBufferCRC();
		else
			crc = ShapeTestCalcCRC(crc);
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
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
BOOL CXModuleGrafx::ShapeTestInitD3D()
{
   
    // Turn off culling, so we see the front and back of the triangle
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Turn off D3D lighting, since we are providing our own vertex colors
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Creates the scene geometry
//-----------------------------------------------------------------------------
BOOL CXModuleGrafx::ShapeTestInitGeometry()
{
	HRESULT hr;
	
	//Create some vertices to use for drawing the shapes.
	m_pTriangles  = new SHAPES_TEST_CUSTOMVERTEX [m_numTriangles * 3];
	m_pTriangles2  = new SHAPES_TEST_CUSTOMVERTEX [m_numTriangles * 3];
	m_pTriangles3  = new SHAPES_TEST_CUSTOMVERTEX [m_numTriangles * 3];
	
	ShapeTestInitTriangles();
	
	// Create the vertex buffer.
    if((hr = g_pd3dDevice->CreateVertexBuffer( m_numTriangles*3*sizeof(SHAPES_TEST_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_SHAPES_TEST_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pVB )) != D3D_OK)
	{
		ReportDebug(DEBUG_LEVEL1, _T("SHAPES.CPP - Failed to CreateVertexBuffer for trinagles in InitGeometry()!"));
		ReportD3DError(hr);
		return FALSE;
	}
	
	// Fill the vertex buffer.
    SHAPES_TEST_CUSTOMVERTEX* pVertices;
    if((hr = m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 )) != D3D_OK)
	{
        ReportDebug(DEBUG_LEVEL1, _T("SHAPES.CPP - Failed to LockVertexBuffer for triangles in InitGeometry()!"));
		ReportD3DError(hr);
		return FALSE;
	}

    memcpy( pVertices, m_pTriangles, m_numTriangles*3*sizeof(SHAPES_TEST_CUSTOMVERTEX) );
    m_pVB->Unlock();


    
	// Create the second vertex buffer.
    if((hr = g_pd3dDevice->CreateVertexBuffer( m_numTriangles*3*sizeof(SHAPES_TEST_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_SHAPES_TEST_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pVB2 )) != D3D_OK)
	{
		ReportDebug(DEBUG_LEVEL1, _T("SHAPES.CPP - Failed to CreateVertexBuffer for trinagles in InitGeometry()!"));
		ReportD3DError(hr);
		return FALSE;
	}
	
	// Fill the vertex buffer.
    SHAPES_TEST_CUSTOMVERTEX* pVertices2;
    if((hr = m_pVB2->Lock( 0, 0, (BYTE**)&pVertices2, 0 )) != D3D_OK)
	{
        ReportDebug(DEBUG_LEVEL1, _T("SHAPES.CPP - Failed to LockVertexBuffer for triangles in InitGeometry()!"));
		ReportD3DError(hr);
		return FALSE;
	}

    memcpy( pVertices2, m_pTriangles2, m_numTriangles*3*sizeof(SHAPES_TEST_CUSTOMVERTEX) );
    m_pVB2->Unlock();




	// Create the third vertex buffer.
    if((hr = g_pd3dDevice->CreateVertexBuffer( m_numTriangles*3*sizeof(SHAPES_TEST_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_SHAPES_TEST_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pVB3 )) != D3D_OK)
	{
		ReportDebug(DEBUG_LEVEL1, _T("SHAPES.CPP - Failed to CreateVertexBuffer for trinagles in InitGeometry()!"));
		ReportD3DError(hr);
		return FALSE;
	}
	
	// Fill the vertex buffer.
    SHAPES_TEST_CUSTOMVERTEX* pVertices3;
    if((hr = m_pVB3->Lock( 0, 0, (BYTE**)&pVertices3, 0 )) != D3D_OK)
	{
        ReportDebug(DEBUG_LEVEL1, _T("SHAPES.CPP - Failed to LockVertexBuffer for triangles in InitGeometry()!"));
		ReportD3DError(hr);
		return FALSE;
	}

    memcpy( pVertices3, m_pTriangles3, m_numTriangles*3*sizeof(SHAPES_TEST_CUSTOMVERTEX) );
    m_pVB3->Unlock();
	
	return TRUE;
}




//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID CXModuleGrafx::ShapeTestSetupMatrices(float angle)
{
    // For our world matrix, we will just rotate the object about the y-axis.
    D3DXMATRIX matWorld1;
    D3DXMatrixRotationY( &matWorld1, angle );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld1 );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView1;
    D3DXMatrixLookAtLH( &matView1, &D3DXVECTOR3( 0.0f, 3.0f,-5.0f ),
                                  &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
                                  &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView1 );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIX matProj1;
    D3DXMatrixPerspectiveFovLH( &matProj1, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj1 );
}



//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID CXModuleGrafx::ShapeTestRender(float angle)
{
		
	g_pd3dDevice->Clear( 0, NULL,
						D3DCLEAR_TARGET|D3DCLEAR_TARGET_R|D3DCLEAR_TARGET_G|D3DCLEAR_TARGET_G|
						D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,85,170), 1.0f, 0 );
		// Begin the scene
	g_pd3dDevice->BeginScene();
	
	// Setup the world, view, and projection matrices
	ShapeTestSetupMatrices(angle);
	
	// Render the vertex buffer contents for triangles
	g_pd3dDevice->SetTexture( 0, NULL );
	g_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(SHAPES_TEST_CUSTOMVERTEX) );
	g_pd3dDevice->SetVertexShader( D3DFVF_SHAPES_TEST_CUSTOMVERTEX );
	g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_numTriangles );
	

	// Render the vertex buffer contents for triangles
	g_pd3dDevice->SetTexture( 0, NULL );
	g_pd3dDevice->SetStreamSource( 0, m_pVB2, sizeof(SHAPES_TEST_CUSTOMVERTEX) );
	g_pd3dDevice->SetVertexShader( D3DFVF_SHAPES_TEST_CUSTOMVERTEX );
	g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_numTriangles );

	
	// Render the vertex buffer contents for triangles
	g_pd3dDevice->SetTexture( 0, NULL );
	g_pd3dDevice->SetStreamSource( 0, m_pVB3, sizeof(SHAPES_TEST_CUSTOMVERTEX) );
	g_pd3dDevice->SetVertexShader( D3DFVF_SHAPES_TEST_CUSTOMVERTEX );
	g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_numTriangles );
	

	// End the scene
	g_pd3dDevice->EndScene();
}


DWORD CXModuleGrafx::ShapeTestCalcCRC(DWORD crc)
{
	
	D3DLOCKED_RECT pLockedRect;		//Locked rectangle struct for determing surface image data.
	LPDIRECT3DSURFACE8 lpd3dSurface = NULL;
	LONG pitch;
	BYTE *pBits;
	HRESULT hr;
	int bytesPerLine;
	
	//Get the contents of the FRONT BUFFER - this checks that the data we rendered to the 
	//back buffer was correctly blted to the display.
	//if((hr = g_pd3dDevice->GetBackBuffer( -1, D3DBACKBUFFER_TYPE_MONO, &lpd3dSurface)) != D3D_OK)
	
	//Flip the primary surface to the back buffer.
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	if((hr = g_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &lpd3dSurface)) != D3D_OK)
	{	//Fatal error - exit the test.
		ReportDebug(DEBUG_LEVEL1, _T("SHAPES.CPP - Failed to GetFrontBuffer() for CRC calc!"));
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
		ReportDebug(DEBUG_LEVEL1, _T("SHAPES.CPP - Failed to LockRect() for CRC calc!"));
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



VOID CXModuleGrafx::ShapeTestInitTriangles()
{
	int total = 0;
	int loop = 0;
	
	srand(1);
	
	
	
	for(loop = 0; loop < m_numTriangles; loop++)
	{
		m_pTriangles[total].x = (8 * ((float)rand()/RAND_MAX) - 4);
		m_pTriangles[total].y = (8 * ((float)rand()/RAND_MAX) - 4);
		m_pTriangles[total].z = (6 * ((float)rand()/RAND_MAX) - 3);
		m_pTriangles[total].color = shapeColor[(int)(NUM_COLORS * (float)rand()/RAND_MAX)];
		total++;
		
		m_pTriangles[total].x = m_pTriangles[total-1].x + (0.45f * ((float)rand()/RAND_MAX) - 0.225f);
		m_pTriangles[total].y = m_pTriangles[total-1].y - (0.45f * ((float)rand()/RAND_MAX) - 0.225f);//(1 * ((float)rand()/RAND_MAX) - 0.5f);
		m_pTriangles[total].z = m_pTriangles[total-1].z - (0.45f * ((float)rand()/RAND_MAX) - 0.225f);//(1 * ((float)rand()/RAND_MAX) - 0.5f);
		m_pTriangles[total].color = shapeColor[(int)(NUM_COLORS * (float)rand()/RAND_MAX)];
		total++;

		m_pTriangles[total].x = m_pTriangles[total-1].x - (0.25f * ((float)rand()/RAND_MAX) - 0.125f);//(1 * ((float)rand()/RAND_MAX) - 0.5f);
		m_pTriangles[total].y = m_pTriangles[total-1].y - (0.25f * ((float)rand()/RAND_MAX) - 0.125f);//(1 * ((float)rand()/RAND_MAX) - 0.5f);
		m_pTriangles[total].z = m_pTriangles[total-1].z + (0.25f * ((float)rand()/RAND_MAX) - 0.125f);//(1 * ((float)rand()/RAND_MAX) - 0.5f);
		m_pTriangles[total].color = shapeColor[(int)(NUM_COLORS * (float)rand()/RAND_MAX)];
		total++;
	}
    

	total = 0;
	srand(2);
	for(loop = 0; loop < m_numTriangles; loop++)
	{
		m_pTriangles2[total].x = (8 * ((float)rand()/RAND_MAX) - 4);
		m_pTriangles2[total].y = (8 * ((float)rand()/RAND_MAX) - 4);
		m_pTriangles2[total].z = (6 * ((float)rand()/RAND_MAX) - 3);
		m_pTriangles2[total].color = shapeColor[(int)(NUM_COLORS * (float)rand()/RAND_MAX)];
		total++;
		
		m_pTriangles2[total].x = m_pTriangles2[total-1].x + (0.4f * ((float)rand()/RAND_MAX) - 0.2f);
		m_pTriangles2[total].y = m_pTriangles2[total-1].y - (0.4f * ((float)rand()/RAND_MAX) - 0.2f);//(1 * ((float)rand()/RAND_MAX) - 0.5f);
		m_pTriangles2[total].z = m_pTriangles2[total-1].z - (0.4f * ((float)rand()/RAND_MAX) - 0.2f);//(1 * ((float)rand()/RAND_MAX) - 0.5f);
		m_pTriangles2[total].color = shapeColor[(int)(NUM_COLORS * (float)rand()/RAND_MAX)];
		total++;

		m_pTriangles2[total].x = m_pTriangles2[total-1].x - (0.2f * ((float)rand()/RAND_MAX) - 0.1f);//(1 * ((float)rand()/RAND_MAX) - 0.5f);
		m_pTriangles2[total].y = m_pTriangles2[total-1].y - (0.2f * ((float)rand()/RAND_MAX) - 0.1f);//(1 * ((float)rand()/RAND_MAX) - 0.5f);
		m_pTriangles2[total].z = m_pTriangles2[total-1].z + (0.2f * ((float)rand()/RAND_MAX) - 0.1f);//(1 * ((float)rand()/RAND_MAX) - 0.5f);
		m_pTriangles2[total].color = shapeColor[(int)(NUM_COLORS * (float)rand()/RAND_MAX)];
		total++;
	}

	
	total = 0;
	srand(3);
	for(loop = 0; loop < m_numTriangles; loop++)
	{
		m_pTriangles3[total].x = (8 * ((float)rand()/RAND_MAX) - 4);
		m_pTriangles3[total].y = (8 * ((float)rand()/RAND_MAX) - 4);
		m_pTriangles3[total].z = (6 * ((float)rand()/RAND_MAX) - 3);
		m_pTriangles3[total].color = shapeColor[(int)(NUM_COLORS * (float)rand()/RAND_MAX)];
		total++;
		
		m_pTriangles3[total].x = m_pTriangles3[total-1].x + (0.3f * ((float)rand()/RAND_MAX) - 0.15f);
		m_pTriangles3[total].y = m_pTriangles3[total-1].y - (0.3f * ((float)rand()/RAND_MAX) - 0.15f);//(1 * ((float)rand()/RAND_MAX) - 0.5f);
		m_pTriangles3[total].z = m_pTriangles3[total-1].z - (0.3f * ((float)rand()/RAND_MAX) - 0.15f);//(1 * ((float)rand()/RAND_MAX) - 0.5f);
		m_pTriangles3[total].color = shapeColor[(int)(NUM_COLORS * (float)rand()/RAND_MAX)];
		total++;

		m_pTriangles3[total].x = m_pTriangles3[total-1].x - (0.1f * ((float)rand()/RAND_MAX) - 0.05f);//(1 * ((float)rand()/RAND_MAX) - 0.5f);
		m_pTriangles3[total].y = m_pTriangles3[total-1].y - (0.1f * ((float)rand()/RAND_MAX) - 0.05f);//(1 * ((float)rand()/RAND_MAX) - 0.5f);
		m_pTriangles3[total].z = m_pTriangles3[total-1].z + (0.1f * ((float)rand()/RAND_MAX) - 0.05f);//(1 * ((float)rand()/RAND_MAX) - 0.5f);
		m_pTriangles3[total].color = shapeColor[(int)(NUM_COLORS * (float)rand()/RAND_MAX)];
		total++;
	}
	
}

