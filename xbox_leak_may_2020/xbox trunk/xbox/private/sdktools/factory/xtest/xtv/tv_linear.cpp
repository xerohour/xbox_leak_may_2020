// 
// (c) Copyright 2000-2000 Intel Corp. All rights reserved.
//   ntsccb.cpp: used to implement ntsc-m and ntsc-Japan color bars 
//   tests for composite and s-video output. The subtests include 
//	ntscmcompcb, ntscmsvcb, ntscjcompcb, and ntscjsvcb
//
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <wchar.h>
#include <string>
#include "..\stdafx.h"
#include "..\statkeys.h"
#include "XTV.h"

// A structure for our custom vertex type
struct TV_CUSTOMVERTEX
{
    FLOAT x, y, z, rhw; // The transformed position for the vertex
    DWORD color;        // The vertex color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_TV_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

//***************************
//  ** WARNING **
//
// The contents of this struct must match the contents of the same struct 
// contatined in the xtvcapture utility.
//BYTE align the data.
#pragma pack(1)
struct TV_LINEAR_SETUP_INFO
{
	int		pass;
	int		tolerance;
	int		line;
	int		column;
	int		expected;
	int		measured;
	int		difference;
	int		tv_mode;
	int		pattern;
	int 	color;
	int		maxRed;
	int		maxGreen;
	int		maxBlue;
};
#pragma pack()
//****************************

//TV color bar linear image for testing
IMPLEMENT_TESTLOCKED (TV, tv_linearity, 25)
{
	CHostResponse CHR;
	DWORD dwActualBytesReceived, dwErrorCodeFromHost;
	TV_LINEAR_SETUP_INFO setup;
	TV_LINEAR_SETUP_INFO *pReturned;
	TV_LINEAR_SETUP_INFO results;
	LPCTSTR tvMode;
	LPCTSTR tvPattern;
	int iError, tolerance;
	BOOL bPass=TRUE;
	

	
	if (CheckAbort (HERE)){ Cleanup(); return;};

	//Get the config parameters
	tolerance=GetCfgInt(L"tol", 10);
	m_bUseMemoryFill = GetCfgInt(L"use_memfill", 0);
	tvMode = GetCfgString( L"tv_mode", NULL);
	tvPattern = GetCfgString( L"pattern", NULL);
	m_dwDisplayHeight = g_d3dpp.BackBufferHeight;
	


	if( tvMode == NULL)
	{
		err_BADPARAMETER( L"tv_mode");
		Cleanup();
		return;
	}
	if( tvPattern == NULL)
	{
		err_BADPARAMETER( L"pattern");
		Cleanup();
		return;
	}

    //Init the setup struct parameters.
	memset(&setup, 0, sizeof(setup));
	memset(&results, 0, sizeof(results));
	setup.tolerance = tolerance;

	if(!wcsicmp(tvMode, L"ntsc_m"))
	{
		//NTSC_M mode - draw the pattern and continue.
		setup.tv_mode = NTSC_M;
		if(!wcsicmp(tvPattern, L"horizontal"))
		{
			setup.pattern = H_PATTERN;
			TVTestDrawHorizontalLinearityPattern();
		}
		else if(!wcsicmp(tvPattern, L"vertical"))
		{
			setup.pattern = V_PATTERN;
			TVTestDrawVerticalLinearityPattern();
		}
		else
		{
			err_BADPARAMETER( L"pattern");
			Cleanup();
			return;
		}
		TVTestRender(setup.pattern);
	}
	//TVTestReportSurfaceData();
	
	for(int attempts=0; attempts<m_testRetries; attempts++)
	{
		memset(&results, 0, sizeof(results));
		//Call the host utility message to take the measurement 
		if ((iError = g_host.iSendHost(MID_TV_LINEARITY, (char *)&setup, sizeof(setup), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, 180)) != EVERYTHINGS_OK)
		{

			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost);
				return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				err_HostCommunicationError(iError);
				return;
			}
		}
		
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
		{
			// The response buffer is empty, so take appropriate action (like an error message)
			err_RecordBufferIsEmpty();
			return;
		}
		
		else // The response is not NULL, and is not an error message, so process it
		{
			// Analyze the response data from the host
			pReturned = (TV_LINEAR_SETUP_INFO *)CHR.pvBufferFromHost;
			memcpy(&results, pReturned, sizeof(struct TV_LINEAR_SETUP_INFO));
		}

		//Now look at the results returned and act accordingly.
		bPass=TRUE;
		if(results.pass != TRUE)
			bPass=FALSE;
		else
			break;
			//Made it to hear means the test passed - good job dude!

		if(attempts+1 < m_testRetries)
			ReportWarning(L"Attempt %d failed, retrying again...", attempts+1);
	
	}//end retry loop
	
	//Report out the stats.
	if(setup.pattern == H_PATTERN)
	{
		ReportStatistic(NTSCM_COMPOSITE_HORIZONTAL_LINEARITY_MAX_RED, _T("%d"), results.maxRed);
		ReportStatistic(NTSCM_COMPOSITE_HORIZONTAL_LINEARITY_MAX_GREEN, _T("%d"), results.maxGreen);
		ReportStatistic(NTSCM_COMPOSITE_HORIZONTAL_LINEARITY_MAX_BLUE, _T("%d"), results.maxBlue);
	}
	else if(setup.pattern == V_PATTERN)
	{
		ReportStatistic(NTSCM_COMPOSITE_VERTICAL_LINEARITY_MAX_RED, _T("%d"), results.maxRed);
		ReportStatistic(NTSCM_COMPOSITE_VERTICAL_LINEARITY_MAX_GREEN, _T("%d"), results.maxGreen);
		ReportStatistic(NTSCM_COMPOSITE_VERTICAL_LINEARITY_MAX_BLUE, _T("%d"), results.maxBlue);
	}
	
	if(!bPass)
	{
		//The PASS flag was set to FALSE.  We failed a linearity check.
		//Decode the returned data and report out the failure.
		//err_TVTestLinearity(LPCTSTR tvmode, LPCTSTR color, int line, int column, int meas, int exp, int diff, int tol)
		TCHAR mode[16];
		TCHAR color[8];
		TCHAR pattern[16];

		if(results.tv_mode == 1)
			swprintf(mode, L"NTSC_M");

		if(results.color == RED)
			swprintf(color,  L"RED");
		else if(results.color == GREEN)
			swprintf(color,  L"GREEN");
		else if(results.color == BLUE)
			swprintf(color,  L"BLUE");

		if(results.pattern == H_PATTERN)
			swprintf(pattern, L"HORIZONTAL");
		else if(results.pattern == V_PATTERN)
			swprintf(pattern, L"VERTICAL");
		
		/*
		ReportWarning(L"Results Data - Mode=%s\n"), mode);
		ReportWarning(L"Pass=%d\n", results.pass);
		ReportWarning(L"Color=%s\n", color);
		ReportWarning(L"Line=%d\n", results.line);
		ReportWarning(L"Column=%d\n", results.column);
		ReportWarning(L"Measured=%d\n", results.measured);
		ReportWarning(L"Expected=%d\n", results.expected);
		ReportWarning(L"Difference=%d\n", results.difference);
		ReportWarning(L"Tolerance=%d\n", results.tolerance);
		*/

		err_TVTestLinearity(mode, pattern, color, results.line, results.column, results.measured, results.expected, results.difference, results.tolerance);
		if (CheckAbort (HERE)){ Cleanup(); return;};
	}
		
	Cleanup();
	return;
}



BOOL CXModuleTV::TVTestDrawHorizontalLinearityPattern()
{
    
	TV_CUSTOMVERTEX* g_Vertices;
	int column = 0, width = 0, vert = 0;
	float startLine, firstRow, secondRow, thirdRow, height = 0.0f;
	
	width = m_dwDisplayWidth = (int)g_d3dpp.BackBufferWidth;
    height = (float)g_d3dpp.BackBufferHeight;
	

	startLine = 0.0f;
	firstRow  = (height / 3.0f) - 1.0f;
	secondRow = ((height / 3.0f)*2) - 1.0f;
	thirdRow = height - 2.0f;

	m_sectionsToDraw = 3;
	g_Vertices = new TV_CUSTOMVERTEX [(m_sectionsToDraw * width * 2)];
	

	startLine = 2.0f;
	

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//red shade
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(255,0,0);
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(0,0,0);
    		vert++;
		}
	  
    }

	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//green shade
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(0,255,0);
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(0,0,0);
    		vert++;
		}
    }

	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//blue shade
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(0,0,255);
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(0,0,0);
    		vert++;
		}
    }


	// Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( (m_sectionsToDraw * width * 2)*sizeof(TV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_TV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	TV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, ((m_sectionsToDraw * width * 2)*sizeof(TV_CUSTOMVERTEX) ));
    m_pTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}


BOOL CXModuleTV::TVTestDrawVerticalLinearityPattern()
{
    
	TV_CUSTOMVERTEX* g_Vertices;
	int line, firstColumn, secondColumn, thirdColumn, width = 0, vert = 0;
	//float startLine, firstRow, secondRow, thirdRow, height = 0.0f;
	float startLine, height = 0.0f;

	width = m_dwDisplayWidth = (int)g_d3dpp.BackBufferWidth;
    height = (float)g_d3dpp.BackBufferHeight;
	

	startLine = 0.0f;
	firstColumn  = (width / 3) - 1;
	secondColumn = ((width / 3)*2) - 1;
	thirdColumn = width - 2;

	m_sectionsToDraw = 3;
	g_Vertices = new TV_CUSTOMVERTEX [(m_sectionsToDraw * (int)height * 2)];
	

	startLine = 2.0f;
	

	//Now start drawing the columns.
	for(line = 0; line < (int)height; line++)
    {
		//Red shade
		g_Vertices[vert].x = 0;
		g_Vertices[vert].y = (float)line;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(255,0,0);
		vert++;		
		g_Vertices[vert].x = (float)firstColumn;
		g_Vertices[vert].y = (float)line;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(0,0,0);
    	vert++;
    }

	for(line = 0; line < (int)height; line++)
    {
		//green shade
		g_Vertices[vert].x = (float)firstColumn+1;
		g_Vertices[vert].y = (float)line;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(0,255,0);
		vert++;		
		g_Vertices[vert].x = (float)secondColumn;
		g_Vertices[vert].y = (float)line;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(0,0,0);
    	vert++;
    }

	for(line = 0; line < (int)height; line++)
    {
		//blue shade
		g_Vertices[vert].x = (float)secondColumn+1;
		g_Vertices[vert].y = (float)line;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(0,0,255);
		vert++;		
		g_Vertices[vert].x = (float)thirdColumn;
		g_Vertices[vert].y = (float)line;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(0,0,0);
    	vert++;
	}

	// Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( (m_sectionsToDraw * (int)height * 2)*sizeof(TV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_TV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	TV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, ((m_sectionsToDraw * (int)height * 2)*sizeof(TV_CUSTOMVERTEX) ));
    m_pTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}

//******************************************************
//
// This function fills the display surface with a WORD bit pattern defined as
// a parameter from the INI file.
//
//******************************************************
BOOL CXModuleTV::FillSurfaceWithHorizontalPattern()
{
	// Fill the rectangle in the image with random data based on seed
	D3DLOCKED_RECT pLockedRect;		//Locked rectangle struct for determing surface image data.
	LONG pitch;
	BYTE *pBits;
	HRESULT hr;
	LPDWORD pData;
	LPDIRECT3DSURFACE8 lpd3dSurface;
	int bytesPerLine, r, g, b;
	DWORD x, y, line, Width, Height, Gradient;
	DWORD PitchAdjust;
	
	lpd3dSurface = NULL;
	Width=m_dwDisplayWidth;
	Height=m_dwDisplayHeight;
	
	if((hr = g_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &lpd3dSurface)) != D3D_OK)
	{	//Fatal error - exit the test.
		ReportDebug(0x1, _T("TV_LINEAR.CPP - Failed to GetBackBuffer() for FillPattern()!"));
		ReportD3DError(hr);
		if (CheckAbort (HERE))
		{
			SAFE_RELEASE(lpd3dSurface);
			Cleanup();
			return 0;
		}
	}
	
	hr = lpd3dSurface->LockRect( &pLockedRect, NULL, D3DLOCK_TILED);
	if(hr != D3D_OK)
	{
		ReportD3DError(hr);
		return FALSE;
	}
	
	//rand.seed(seed);
//	ReportWarning(L"pitch=%d, bpl=%d", pitch, bytesPerLine);

	PitchAdjust = pLockedRect.Pitch / 4 - m_dwDisplayWidth;

    pData = (LPDWORD)pLockedRect.pBits;

	Gradient = 256 / (Height/3);
	
        
	//Draw the red gradient first.
	for(line=0; line < (Height / 3); line++)
	{
		r = ( (line * Gradient)>255)?255 : (line * Gradient);
		g=b=0;
		for (x = 0; x < Width; x++)
		{
			*pData++ = D3DCOLOR_ARGB(0, r, g, b);
		}
		pData += PitchAdjust;
	}

	//Draw the green gradient second.
	for(line=0; line < (Height / 3); line++)
	{
		g = ( (line * Gradient)>255)?255 : (line * Gradient);
		r=b=0;
		for (x = 0; x < Width; x++)
		{
			*pData++ = D3DCOLOR_ARGB(0, r, g, b);
		}
		pData += PitchAdjust;
	}

	//Draw the green gradient second.
	for(line=0; line < (Height / 3); line++)
	{
		b = ( (line * Gradient)>255)?255 : (line * Gradient);
		r=g=0;
		for (x = 0; x < Width; x++)
		{
			*pData++ = D3DCOLOR_ARGB(0, r, g, b);
		}
		pData += PitchAdjust;
	}

	lpd3dSurface->UnlockRect();
	SAFE_RELEASE(lpd3dSurface);
		
	return TRUE;
}



//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID CXModuleTV::TVTestRender(int pattern)
{
	HRESULT hr;
	LPDIRECT3DSURFACE8 pFrontBuffer;
	LPDIRECT3DSURFACE8 pBackBuffer;


	// Clear the backbuffer
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
	
	// Begin the scene
    g_pd3dDevice->BeginScene();
	
	/*
	if(m_bUseBitmap)
	{
		//Draw the lines for the pixel count
		g_pd3dDevice->SetStreamSource( 0, m_pTVPatternVB, sizeof(TV_CUSTOMVERTEX) );
		g_pd3dDevice->SetVertexShader( D3DFVF_TV_CUSTOMVERTEX );
		if(pattern == H_PATTERN)
			g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, ((int)m_dwDisplayWidth * m_sectionsToDraw));
		else if(pattern == V_PATTERN)
			g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, ((int)m_dwDisplayHeight * m_sectionsToDraw));
		
		// End the scene
		g_pd3dDevice->EndScene();

		// Present the backbuffer contents to the display
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
			
		g_pd3dDevice->GetBackBuffer( -1, D3DBACKBUFFER_TYPE_MONO, &pFrontBuffer);
		hr = XGWriteSurfaceToFile( pFrontBuffer, "z:\\pattern.bmp");
		SAFE_RELEASE(pFrontBuffer);
		
		if(hr != S_OK)
			err_DIRECTX_API ( hr, L"Failed saving surface to bitmap file.");

		// Clear the backbuffer
		g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
		// Begin the scene
		g_pd3dDevice->BeginScene();
		// End the scene
		g_pd3dDevice->EndScene();
		// Present the backbuffer contents to the display
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );


		g_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
		// Begin the scene
	    g_pd3dDevice->BeginScene();

		
		hr = D3DXLoadSurfaceFromFile(pBackBuffer,
								NULL,
								NULL,
								"z:\\pattern.bmp",
								NULL,
								D3DX_FILTER_NONE,
								0,
								NULL);
		SAFE_RELEASE(pBackBuffer);

		if(hr != D3D_OK)
			err_DIRECTX_API ( hr, L"Failed loading surface from bitmap file.");

	}
	*/
	if(m_bUseMemoryFill)
	{
		FillSurfaceWithHorizontalPattern();

	}


	else
	{

		//Draw the lines for the pixel count
		g_pd3dDevice->SetStreamSource( 0, m_pTVPatternVB, sizeof(TV_CUSTOMVERTEX) );
		g_pd3dDevice->SetVertexShader( D3DFVF_TV_CUSTOMVERTEX );
		if(pattern == H_PATTERN)
			g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, ((int)m_dwDisplayWidth * m_sectionsToDraw));
		else if(pattern == V_PATTERN)
			g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, ((int)m_dwDisplayHeight * m_sectionsToDraw));
	}
	
	// End the scene
    g_pd3dDevice->EndScene();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

