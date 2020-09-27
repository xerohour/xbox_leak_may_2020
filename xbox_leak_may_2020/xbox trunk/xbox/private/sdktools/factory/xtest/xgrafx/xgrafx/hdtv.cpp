//-----------------------------------------------------------------------------
// File: hdtv.cpp
//
// Desc: 
//
// Copyright (c) 2000 Intel Corp. All rights reserved.
//-----------------------------------------------------------------------------
#include "..\..\stdafx.h"
#include "..\..\testobj.h"
#include "xgrafx.h"

// A structure for our custom vertex type
struct HDTV_CUSTOMVERTEX
{
    FLOAT x, y, z, rhw; // The transformed position for the vertex
    DWORD color;        // The vertex color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_HDTV_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
//Some defines for the tests.
#define HDTV_TIMINGS	0x0001
#define HDTV_PIXELS		0x0002
#define HDTV_DAC		0x0004
#define HDTV_TRILEVEL	0x0008

#define HDTV_DAC_LINES	384



//******************************************************************
// Title: HDTV Test
//
// Abstract: Generate a series of HDTV test patterns.  The output
//			signal will be analyzed by the HDVMU device attached to the 
//			host tester, and communications will take place via a small
//			interface app that will relay info between the DVMU and XBOX app.
//
//******************************************************************
IMPLEMENT_TESTLOCKED (Grafx, HDTV, 4)
{
	WORD tests = 0;

	if (CheckAbort (HERE)){ MainCleanup(); return;};
	
	if( m_HDTVTestPattern == NULL)
	{
		err_BADPARAMETER( m_modeString, L"test");
		MainCleanup();
		return;
	}
	
	ReportDebug(DEBUG_LEVEL1, _T("HDTV.CPP - Test Pattern = %s\n"), m_HDTVTestPattern);

	if(!wcsicmp(m_HDTVTestPattern, L"all"))//Do all tests.
		tests = (tests | HDTV_TIMINGS | HDTV_PIXELS | HDTV_DAC | HDTV_TRILEVEL);
	
	else if(!wcsicmp(m_HDTVTestPattern, L"pixels"))//Do the pixel count test.
		tests = tests | HDTV_PIXELS;

	else if(!wcsicmp(m_HDTVTestPattern, L"timings"))//Do the timings test.
		tests = tests | HDTV_TIMINGS;

	else if(!wcsicmp(m_HDTVTestPattern, L"dac"))//Do the DAC (analog colors) test.
		tests = tests | HDTV_DAC;

	else if(!wcsicmp(m_HDTVTestPattern, L"tlsp"))//Do the trilevel sync test.
		tests = tests | HDTV_TRILEVEL;

	if(!wcsicmp(m_HDTVmode, L"1080i"))
		m_bInterlaced = TRUE;
	else
		m_bInterlaced = FALSE;

	
	//Init the vertex buffer to be used for rendering.
	m_pHDTVPatternVB = NULL;
			
	if(g_pd3dDevice == NULL)
	{
		//Error - the global device was not created successfully
		//or has become lost or invalid.
		err_NODIRECT3D(m_modeString);
		MainCleanup();
		return;
	}

	if (CheckAbort (HERE)){ MainCleanup(); return;};
	if(tests & HDTV_PIXELS)//Do the pixel count test.
	{
		
		//if( m_PixelCountPattern == 0)
		//{
		//	err_BADPARAMETER( m_modeString, L"pattern");
		//	MainCleanup();
		//	return;
		//}
		
		if(!HDTVTestInitPixelCountPattern(m_PixelCountPattern) )
		{	//Fatal error - exit the grafx test.
			MainCleanup();
			return;
		}
		// Render the scene
		HDTVTestPixelCountRender();
		if(m_secondsToDisplay)
			Sleep(m_secondsToDisplay);	
		HDTVTestCheckPixelcountData();
		if (CheckAbort (HERE)){ MainCleanup(); return;};
	}

	
	SAFE_RELEASE(m_pHDTVPatternVB);
	m_pHDTVPatternVB = NULL;

	if(tests & HDTV_DAC)
	{
		HDTVTestInitDacPattern();
		HDTVTestAnalogColorsRender();
		if(m_secondsToDisplay)
			Sleep(m_secondsToDisplay);
		HDTVTestCheckAnalogColorsData();
		if (CheckAbort (HERE)){ MainCleanup(); return;};
	}

	
	SAFE_RELEASE(m_pHDTVPatternVB);
	m_pHDTVPatternVB = NULL;

	if(tests & HDTV_TIMINGS)//Do the timings test.
	{
		// Render the scene
		HDTVTestBackgroundRender(0,0,255);
		if(m_secondsToDisplay)
			Sleep(m_secondsToDisplay);
		HDTVTestCheckTimingsData();
		if (CheckAbort (HERE)){ MainCleanup(); return;};
	}

	SAFE_RELEASE(m_pHDTVPatternVB);
	m_pHDTVPatternVB = NULL;

	if(tests & HDTV_TRILEVEL)//Do the trilevel sync test.
	{
		// Render the scene
		HDTVTestBackgroundRender(255,255,255);
		if(m_secondsToDisplay)
			Sleep(m_secondsToDisplay);
		HDTVTestCheckTrilevelSyncData();
		if (CheckAbort (HERE)){ MainCleanup(); return;};
	}

	//Cleanup
	SAFE_RELEASE(m_pHDTVPatternVB);
	SAFE_DELETE(m_pHDTVPatternVB);
	MainCleanup();
}



//-----------------------------------------------------------------------------
// Name: InitPixelCountPattern()
// Desc: Creates a vertex buffer and fills it with our vertices. The vertex
//       buffer is basically just a chuck of memory that holds vertices. After
//       creating it, we must Lock()/Unlock() it to fill it. For indices, D3D
//       also uses index buffers. The special thing about vertex and index
//       buffers is that the ycan be created in device memory, allowing some
//       cards to process them in hardware, resulting in a dramatic
//       performance gain.
//-----------------------------------------------------------------------------
BOOL CXModuleGrafx::HDTVTestInitPixelCountPattern(int pattern)
{
    BOOL result;


	switch(pattern)
	{
		case 1:	result = HDTVTestPixelPattern01();	break;

		case 2:	result = HDTVTestPixelPattern02();	break;

		case 3:	result = HDTVTestPixelPattern03();	break;

		case 4:	result = HDTVTestPixelPattern04();	break;

		case 5:	result = HDTVTestPixelPattern05();	break;

		case 6:	result = HDTVTestPixelPattern06();	break;

		case 7:	result = HDTVTestPixelPattern07();	break;

		case 8:	result = HDTVTestPixelPattern08();	break;

		case 9:	result = HDTVTestPixelPattern09();	break;

		case 10: result = HDTVTestPixelPattern10();	break;

		case 11: result = HDTVTestPixelPattern11();	break;

		case 12: result = HDTVTestPixelPattern12();	break;

		case 13: result = HDTVTestPixelPattern13();	break;

		case 14: result = HDTVTestPixelPattern14();	break;

		default:
			result = FALSE;
			break;	
	}
	
    return result;
}


//---------------------------------------------
//Pattern01 is defined as follows:
//
//	Two full horizontal lines of blue
//	Alternating 1 green on, 1 black for remaining top half of screen
//	Alternating 1 black, 1 red on for second half of screen
//---------------------------------------------
BOOL CXModuleGrafx::HDTVTestPixelPattern01()
{
    
	HDTV_CUSTOMVERTEX* g_Vertices;
	int column = 0, width = 0, vert = 0;
	float startLine, firstRow, secondRow, thirdRow, fourthRow, fifthRow, height = 0.0f;
	BOOL bInterlaced = FALSE;
	

	//Check to see if we are in 1080i interlaced mode.
	if( (m_HDTVmode != NULL) && (!wcsicmp(m_HDTVmode, L"1080i")) )
		bInterlaced = TRUE;
	else
		bInterlaced = FALSE;


	width = (int)m_dwDisplayWidth;
	height = (float)m_dwDisplayHeight; //Make an easier variable to use.

	startLine = 0.0f;
	if(bInterlaced)
		firstRow  =  (height / 2.0f);
	else
		firstRow  =  (height / 2.0f) - 1.0f;
	secondRow = height - 2;
	thirdRow =  height;
	fourthRow = height;
	fifthRow =  height;
	

	m_PatternSections = 2;
	g_Vertices = new HDTV_CUSTOMVERTEX [(m_PatternSections * width * 2)+4];
	//g_Vertices = new HDTV_CUSTOMVERTEX [height * 2];

	//Draw a blue line across the top of the image to key the color counting engine.
	if(bInterlaced)
	{
		startLine = 14.0f;//5 for HDTV monitor, 14 for VGA
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
	}
	
	else
	{
		startLine = 2.0f;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
	}
	

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
    		vert++;
			column++;
		}
	  
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
		}
    }

	//Draw the Red pixel row
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;
		}
    }

	// Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( ((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_HDTV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pHDTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	HDTV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pHDTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, (((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX) ));
    m_pHDTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}


//---------------------------------------------
//Pattern02 is defined as follows:
//
//	Two full horizontal lines of blue
//	Alternating 2 green on, 2 black for remaining top half of screen
//	Alternating 2 black, 2 red on for second half of screen
//---------------------------------------------
BOOL CXModuleGrafx::HDTVTestPixelPattern02()
{
    
	HDTV_CUSTOMVERTEX* g_Vertices;
	int column = 0, width = 0, vert = 0;
	float startLine, firstRow, secondRow, thirdRow, fourthRow, fifthRow, height = 0.0f;
	BOOL bInterlaced = FALSE;
	

	//Check to see if we are in 1080i interlaced mode.
	if( (m_HDTVmode != NULL) && (!wcsicmp(m_HDTVmode, L"1080i")) )
		bInterlaced = TRUE;
	else
		bInterlaced = FALSE;


	width = (int)m_dwDisplayWidth;
	height = (float)m_dwDisplayHeight; //Make an easier variable to use.

	startLine = 0.0f;
	if(bInterlaced)
		firstRow  =  (height / 2.0f);
	else
		firstRow  =  (height / 2.0f) - 1.0f;
	secondRow = height - 2;
	thirdRow =  height;
	fourthRow = height;
	fifthRow =  height;
	

	m_PatternSections = 2;
	g_Vertices = new HDTV_CUSTOMVERTEX [(m_PatternSections * width * 2)+4];

	//Draw a blue line across the top of the image to key the color counting engine.
	if(bInterlaced)
	{
		startLine = 14.0f;//5 for HDTV monitor, 14 for VGA
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
	}
	
	else
	{
		startLine = 2.0f;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
	}
	

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
    		vert++;
			column++;
		}
		if(column < width)
		{	//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
    		vert++;
			column++;
		}
		
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			column++;
		}
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
		}
    }

	
	
	//Draw the Red pixel row
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;
			column++;
		}
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;
		}
    }


	
	  // Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( ((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_HDTV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pHDTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	HDTV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pHDTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, (((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX) ));
    m_pHDTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}


//---------------------------------------------
//Pattern03 is defined as follows:
//
//	Two full horizontal lines of blue
//	Alternating 1 white on, 1 black for remaining top 1/4 of screen
//	Alternating 1 blue on, 1 yellow on for second 1/4 of screen
//	Alternating 1 red on, 1 cyan on for third 1/4 of screen.
//	Alternating 1 green on, 1 black for 1/4 of the bottom 1/4 of screen.
//	Alternating 1 blue one, 1 black for 
//	Alternating 1 red on, 1 black for 
//---------------------------------------------
BOOL CXModuleGrafx::HDTVTestPixelPattern03()
{
    
	HDTV_CUSTOMVERTEX* g_Vertices;
	int column = 0, width = 0, vert = 0;
	float startLine, firstRow, secondRow, thirdRow, fourthRow, fifthRow, height = 0.0f;
	BOOL bInterlaced = FALSE;
	

	//Check to see if we are in 1080i interlaced mode.
	if( (m_HDTVmode != NULL) && (!wcsicmp(m_HDTVmode, L"1080i")) )
		bInterlaced = TRUE;
	else
		bInterlaced = FALSE;


	width = (int)m_dwDisplayWidth;
	height = (float)m_dwDisplayHeight; //Make an easier variable to use.
	


	startLine = 0.0f;
	if(bInterlaced)
		firstRow  =  (height / 4.0f);
	else
		firstRow  =  (height / 4.0f) - 1.0f;
	secondRow = ((height / 4.0f) * 2.0f)  - 1.0f;
	thirdRow =  ((height / 4.0f) * 3.0f)  - 1.0f;
	fourthRow = ((height / 4.0f) * 3.25f)  - 1.0f;
	fifthRow =  ((height / 4.0f) * 3.60f)  - 1.0f;
	
	m_PatternSections = 6;
	g_Vertices = new HDTV_CUSTOMVERTEX [(m_PatternSections * width * 2)+4];

	// Initialize 2 vertices for each line.
	//The order is as follows:
	//For the Red/Green pixels:  white, black
	//For the Green pixels:  red, red, cyan, cyan
	//For the Blue pixels:  blue, blue, yellow, yellow
	
	//Draw a blue line across the top of the image to key the color counting engine.
	if(bInterlaced)
	{
		startLine = 14.0f;//5 for HDTV monitor, 14 for VGA
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
	}
	
	else
	{
		startLine = 2.0f;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
	}
	

	//Now start drawing the columns.
	//Draw the white/black rows
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//white line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffffffff;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffffffff;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
		}
    }

	
	//Draw the blue/yellow rows
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//blue line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
    		vert++;
			column++;
		}
		if(column < width)
		{	//yellow line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffffff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffffff00;
    		vert++;
		}
    }


	//Draw the red/cyan rows
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//cyan line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ffff;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ffff;
    		vert++;
		}
    }


	//Draw the green/black rows
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
		}
    }

	//Draw the red/black rows
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
		}
    }

	//Draw the blue/black rows
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//blue line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = height-1;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = height-1;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
		}
    }

	
	// Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( ((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_HDTV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pHDTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	HDTV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pHDTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, (((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX) ));
    m_pHDTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}


//---------------------------------------------
//Pattern04 is defined as follows:
//
//	Two full horizontal lines of blue
//	Alternating 2 white on, 2 black for remaining top 1/4 of screen
//	Alternating 2 blue on, 2 yellow on for second 1/4 of screen
//	Alternating 2 red on, 2 cyan on for third 1/4 of screen.
//	Alternating 2 green on, 2 black for .25 of the bottom 1/4 of screen.
//	Alternating 2 blue one, 2 black for .60 of the bottom 1/4 of screen
//	Alternating 2 red on, 2 black   for .15 of the bottom 1/4 of screen
//---------------------------------------------
BOOL CXModuleGrafx::HDTVTestPixelPattern04()
{
    
	HDTV_CUSTOMVERTEX* g_Vertices;
	int column = 0, width = 0, vert = 0;
	float startLine, firstRow, secondRow, thirdRow, fourthRow, fifthRow, height = 0.0f;
	BOOL bInterlaced = FALSE;
	

	//Check to see if we are in 1080i interlaced mode.
	if( (m_HDTVmode != NULL) && (!wcsicmp(m_HDTVmode, L"1080i")) )
		bInterlaced = TRUE;
	else
		bInterlaced = FALSE;


	width = (int)m_dwDisplayWidth;
	height = (float)m_dwDisplayHeight; //Make an easier variable to use.
	


	startLine = 0.0f;
	if(bInterlaced)
		firstRow  =  (height / 4.0f);
	else
		firstRow  =  (height / 4.0f) - 1.0f;
	secondRow = ((height / 4.0f) * 2.0f)  - 1.0f;
	thirdRow =  ((height / 4.0f) * 3.0f)  - 1.0f;
	fourthRow = ((height / 4.0f) * 3.25f)  - 1.0f;
	fifthRow =  ((height / 4.0f) * 3.60f)  - 1.0f;
	
	m_PatternSections = 6;
	g_Vertices = new HDTV_CUSTOMVERTEX [(m_PatternSections * width * 2)+4];

	// Initialize 2 vertices for each line.
	//The order is as follows:
	//For the Red/Green pixels:  white, black
	//For the Green pixels:  red, red, cyan, cyan
	//For the Blue pixels:  blue, blue, yellow, yellow
	
	//Draw a blue line across the top of the image to key the color counting engine.
	if(bInterlaced)
	{
		startLine = 14.0f;//5 for HDTV monitor, 14 for VGA
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
	}
	
	else
	{
		startLine = 2.0f;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
	}
	

	//Now start drawing the columns.
	//Draw the white/black rows
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//white line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffffffff;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffffffff;
    		vert++;
			column++;
		}
		if(column < width)
		{	//white line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffffffff;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffffffff;
    		vert++;
			column++;
		}

		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
		}
    }

	
	//Draw the blue/yellow rows
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//blue line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
    		vert++;
			column++;
		}
		if(column < width)
		{	//blue line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
    		vert++;
			column++;
		}

		if(column < width)
		{	//yellow line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffffff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffffff00;
    		vert++;
			column++;
		}

		if(column < width)
		{	//yellow line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffffff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffffff00;
    		vert++;
		}
    }


	//Draw the red/cyan rows
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
    		vert++;
			column++;
		}

		if(column < width)
		{	//cyan line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ffff;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ffff;
    		vert++;
			column++;
		}
		if(column < width)
		{	//cyan line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ffff;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ffff;
    		vert++;
		}
    }


	//Draw the green/black rows
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
    		vert++;
			column++;
		}
		if(column < width)
		{	//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
    		vert++;
			column++;
		}

		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
		}
    }

	//Draw the red/black rows
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
    		vert++;
			column++;
		}

		
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
		}
    }

	//Draw the blue/black rows
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//blue line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = height-1;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
    		vert++;
			column++;
		}
		if(column < width)
		{	//blue line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = height-1;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
    		vert++;
			column++;
		}

		
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = height-1;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = height-1;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
		}
    }

	
	// Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( ((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_HDTV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pHDTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	HDTV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pHDTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, (((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX) ));
    m_pHDTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}





//---------------------------------------------
//Pattern05 is defined as follows:
//
//	Two full horizontal lines of blue
//	Alternating 1 red on, 1 black for remaining height of screen
//
//---------------------------------------------
BOOL CXModuleGrafx::HDTVTestPixelPattern05()
{
    
	HDTV_CUSTOMVERTEX* g_Vertices;
	int column = 0, width = 0, vert = 0;
	float startLine, firstRow = 0.0f, height = 0.0f;
	BOOL bInterlaced = FALSE;
	

	//Check to see if we are in 1080i interlaced mode.
	if( (m_HDTVmode != NULL) && (!wcsicmp(m_HDTVmode, L"1080i")) )
		bInterlaced = TRUE;
	else
		bInterlaced = FALSE;


	width = (int)m_dwDisplayWidth;
	height = (float)m_dwDisplayHeight; //Make an easier variable to use.

	startLine = 0.0f;
	if(bInterlaced)
		firstRow  =  height;
	else
		firstRow  =  height - 1.0f;
	

	m_PatternSections = 1;
	g_Vertices = new HDTV_CUSTOMVERTEX [(m_PatternSections * width * 2)+4];
	//g_Vertices = new HDTV_CUSTOMVERTEX [height * 2];

	//Draw a blue line across the top of the image to key the color counting engine.
	if(bInterlaced)
	{
		startLine = 14.0f;//5 for HDTV monitor, 14 for VGA
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
	}
	
	else
	{
		startLine = 2.0f;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
	}
	

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
    		vert++;
			column++;
		}
	  
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
		}
    }

	
	// Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( ((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_HDTV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pHDTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	HDTV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pHDTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, (((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX) ));
    m_pHDTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}


//---------------------------------------------
//Pattern06 is defined as follows:
//
//	Two full horizontal lines of blue
//	Alternating 2 red on, 2 black for remaining height of screen
//
//---------------------------------------------
BOOL CXModuleGrafx::HDTVTestPixelPattern06()
{
    
	HDTV_CUSTOMVERTEX* g_Vertices;
	int column = 0, width = 0, vert = 0;
	float startLine, firstRow = 0.0f, height = 0.0f;
	BOOL bInterlaced = FALSE;
	

	//Check to see if we are in 1080i interlaced mode.
	if( (m_HDTVmode != NULL) && (!wcsicmp(m_HDTVmode, L"1080i")) )
		bInterlaced = TRUE;
	else
		bInterlaced = FALSE;


	width = (int)m_dwDisplayWidth;
	height = (float)m_dwDisplayHeight; //Make an easier variable to use.

	startLine = 0.0f;
	if(bInterlaced)
		firstRow  =  height;
	else
		firstRow  =  height - 1.0f;
	

	m_PatternSections = 1;
	g_Vertices = new HDTV_CUSTOMVERTEX [(m_PatternSections * width * 2)+4];
	//g_Vertices = new HDTV_CUSTOMVERTEX [height * 2];

	//Draw a blue line across the top of the image to key the color counting engine.
	if(bInterlaced)
	{
		startLine = 14.0f;//5 for HDTV monitor, 14 for VGA
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
	}
	
	else
	{
		startLine = 2.0f;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
	}
	

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
    		vert++;
			column++;
		}
	  
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			column++;
		}
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
		}
    }

	
	// Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( ((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_HDTV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pHDTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	HDTV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pHDTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, (((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX) ));
    m_pHDTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}


//---------------------------------------------
//Pattern07 is defined as follows:
//
//	Two full horizontal lines of blue
//	Alternating 1 green on, 1 black for remaining height of screen
//
//---------------------------------------------
BOOL CXModuleGrafx::HDTVTestPixelPattern07()
{
    
	HDTV_CUSTOMVERTEX* g_Vertices;
	int column = 0, width = 0, vert = 0;
	float startLine, firstRow = 0.0f, height = 0.0f;
	BOOL bInterlaced = FALSE;
	

	//Check to see if we are in 1080i interlaced mode.
	if( (m_HDTVmode != NULL) && (!wcsicmp(m_HDTVmode, L"1080i")) )
		bInterlaced = TRUE;
	else
		bInterlaced = FALSE;


	width = (int)m_dwDisplayWidth;
	height = (float)m_dwDisplayHeight; //Make an easier variable to use.

	startLine = 0.0f;
	if(bInterlaced)
		firstRow  =  height;
	else
		firstRow  =  height - 1.0f;
	

	m_PatternSections = 1;
	g_Vertices = new HDTV_CUSTOMVERTEX [(m_PatternSections * width * 2)+4];
	//g_Vertices = new HDTV_CUSTOMVERTEX [height * 2];

	//Draw a blue line across the top of the image to key the color counting engine.
	if(bInterlaced)
	{
		startLine = 14.0f;//5 for HDTV monitor, 14 for VGA
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
	}
	
	else
	{
		startLine = 2.0f;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
	}
	

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
    		vert++;
			column++;
		}
	  
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
		}
    }

	
	// Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( ((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_HDTV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pHDTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	HDTV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pHDTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, (((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX) ));
    m_pHDTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}


//---------------------------------------------
//Pattern08 is defined as follows:
//
//	Two full horizontal lines of blue
//	Alternating 1 green on, 1 black for remaining height of screen
//
//---------------------------------------------
BOOL CXModuleGrafx::HDTVTestPixelPattern08()
{
    
	HDTV_CUSTOMVERTEX* g_Vertices;
	int column = 0, width = 0, vert = 0;
	float startLine, firstRow = 0.0f, height = 0.0f;
	BOOL bInterlaced = FALSE;
	

	//Check to see if we are in 1080i interlaced mode.
	if( (m_HDTVmode != NULL) && (!wcsicmp(m_HDTVmode, L"1080i")) )
		bInterlaced = TRUE;
	else
		bInterlaced = FALSE;


	width = (int)m_dwDisplayWidth;
	height = (float)m_dwDisplayHeight; //Make an easier variable to use.

	startLine = 0.0f;
	if(bInterlaced)
		firstRow  =  height;
	else
		firstRow  =  height - 1.0f;
	

	m_PatternSections = 1;
	g_Vertices = new HDTV_CUSTOMVERTEX [(m_PatternSections * width * 2)+4];
	//g_Vertices = new HDTV_CUSTOMVERTEX [height * 2];

	//Draw a blue line across the top of the image to key the color counting engine.
	if(bInterlaced)
	{
		startLine = 14.0f;//5 for HDTV monitor, 14 for VGA
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
	}
	
	else
	{
		startLine = 2.0f;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
	}
	

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
    		vert++;
			column++;
		}
		if(column < width)
		{	//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
    		vert++;
			column++;
		}
	  
		
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			column++;
		}
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
		}
    }

	
	// Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( ((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_HDTV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pHDTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	HDTV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pHDTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, (((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX) ));
    m_pHDTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}



//---------------------------------------------
//Pattern09 is defined as follows:
//
//	Two full horizontal lines of blue
//	Alternating 1 green on, 1 black for remaining height of screen
//
//---------------------------------------------
BOOL CXModuleGrafx::HDTVTestPixelPattern09()
{
    
	HDTV_CUSTOMVERTEX* g_Vertices;
	int column = 0, width = 0, vert = 0;
	float startLine, firstRow = 0.0f, height = 0.0f;
	BOOL bInterlaced = FALSE;
	

	//Check to see if we are in 1080i interlaced mode.
	if( (m_HDTVmode != NULL) && (!wcsicmp(m_HDTVmode, L"1080i")) )
		bInterlaced = TRUE;
	else
		bInterlaced = FALSE;


	width = (int)m_dwDisplayWidth;
	height = (float)m_dwDisplayHeight; //Make an easier variable to use.

	startLine = 0.0f;
	if(bInterlaced)
		firstRow  =  height;
	else
		firstRow  =  height - 1.0f;
	

	m_PatternSections = 1;
	g_Vertices = new HDTV_CUSTOMVERTEX [(m_PatternSections * width * 2)+4];
	//g_Vertices = new HDTV_CUSTOMVERTEX [height * 2];

	//Draw a blue line across the top of the image to key the color counting engine.
	if(bInterlaced)
	{
		startLine = 14.0f;//5 for HDTV monitor, 14 for VGA
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
	}
	
	else
	{
		startLine = 2.0f;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
	}
	

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//blue line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
    		vert++;
			column++;
		}
	  
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
		}
    }

	
	// Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( ((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_HDTV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pHDTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	HDTV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pHDTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, (((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX) ));
    m_pHDTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}



//---------------------------------------------
//Pattern10 is defined as follows:
//
//	Two full horizontal lines of blue
//	Alternating 1 green on, 1 black for remaining height of screen
//
//---------------------------------------------
BOOL CXModuleGrafx::HDTVTestPixelPattern10()
{
    
	HDTV_CUSTOMVERTEX* g_Vertices;
	int column = 0, width = 0, vert = 0;
	float startLine, firstRow = 0.0f, height = 0.0f;
	BOOL bInterlaced = FALSE;
	

	//Check to see if we are in 1080i interlaced mode.
	if( (m_HDTVmode != NULL) && (!wcsicmp(m_HDTVmode, L"1080i")) )
		bInterlaced = TRUE;
	else
		bInterlaced = FALSE;


	width = (int)m_dwDisplayWidth;
	height = (float)m_dwDisplayHeight; //Make an easier variable to use.

	startLine = 0.0f;
	if(bInterlaced)
		firstRow  =  height;
	else
		firstRow  =  height - 1.0f;
	

	m_PatternSections = 1;
	g_Vertices = new HDTV_CUSTOMVERTEX [(m_PatternSections * width * 2)+4];
	//g_Vertices = new HDTV_CUSTOMVERTEX [height * 2];

	//Draw a blue line across the top of the image to key the color counting engine.
	if(bInterlaced)
	{
		startLine = 14.0f;//5 for HDTV monitor, 14 for VGA
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
	}
	
	else
	{
		startLine = 2.0f;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
	}
	

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//blue line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
    		vert++;
			column++;
		}
		if(column < width)
		{	//blue line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
    		vert++;
			column++;
		}
	  
		
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			column++;
		}
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
		}
    }

	
	// Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( ((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_HDTV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pHDTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	HDTV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pHDTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, (((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX) ));
    m_pHDTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}



//---------------------------------------------
//Pattern11 is defined as follows:
//
//	Two full horizontal lines of blue
//	Alternating 2 green on, 2 light black for remaining top half of screen
//	Alternating 2 light black, 2 red on for second half of screen
//---------------------------------------------
BOOL CXModuleGrafx::HDTVTestPixelPattern11()
{
    
	HDTV_CUSTOMVERTEX* g_Vertices;
	int column = 0, width = 0, vert = 0;
	float startLine, firstRow, secondRow, thirdRow, fourthRow, height = 0.0f;
	BOOL bInterlaced = FALSE;
	

	//Check to see if we are in 1080i interlaced mode.
	if( (m_HDTVmode != NULL) && (!wcsicmp(m_HDTVmode, L"1080i")) )
		bInterlaced = TRUE;
	else
		bInterlaced = FALSE;


	width = (int)m_dwDisplayWidth;
	height = (float)m_dwDisplayHeight; //Make an easier variable to use.

	startLine = 0.0f;
	if(bInterlaced)
		firstRow  =  (height / 4.0f);
	else
		firstRow  =  (height / 4.0f) - 1.0f;

	secondRow = ((height / 4.0f) * 2.0f)  - 1.0f;
	thirdRow =  ((height / 4.0f) * 3.0f)  - 1.0f;
	fourthRow = height - 2.0f;
	


	m_PatternSections = 4;
	g_Vertices = new HDTV_CUSTOMVERTEX [(m_PatternSections * width * 2)+4];

	//Draw a blue line across the top of the image to key the color counting engine.
	if(bInterlaced)
	{
		startLine = 14.0f;//5 for HDTV monitor, 14 for VGA
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
	}
	
	else
	{
		startLine = 2.0f;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
	}
	

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
    		vert++;
			column++;
		}
		if(column < width)
		{
			//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;
			column++;
		}
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			column++;
		}
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
		}
    }

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			column++;
		}
		if(column < width)
		{
			//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;
			column++;
		}
		if(column < width)
		{
			//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;
		}
    }

	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
		}
    }

	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;
		}
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;
			column++;
		}
    }

	// Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( ((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_HDTV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pHDTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	HDTV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pHDTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, (((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX) ));
    m_pHDTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}



//---------------------------------------------
//Pattern12 is defined as follows:
//
//	Two full horizontal lines of blue
//	Alternating 2 green on, 2 light black for remaining top half of screen
//	Alternating 2 light black, 2 red on for second half of screen
//---------------------------------------------
BOOL CXModuleGrafx::HDTVTestPixelPattern12()
{
    
	HDTV_CUSTOMVERTEX* g_Vertices;
	int column = 0, width = 0, vert = 0;
	float startLine, firstRow, secondRow, thirdRow, fourthRow, fifthRow, sixthRow, height = 0.0f;
	BOOL bInterlaced = FALSE;
	

	//Check to see if we are in 1080i interlaced mode.
	if( (m_HDTVmode != NULL) && (!wcsicmp(m_HDTVmode, L"1080i")) )
		bInterlaced = TRUE;
	else
		bInterlaced = FALSE;


	width = (int)m_dwDisplayWidth;
	height = (float)m_dwDisplayHeight; //Make an easier variable to use.

	startLine = 0.0f;
	if(bInterlaced)
		firstRow  =  (height / 6.0f);
	else
		firstRow  =  (height / 6.0f) - 1.0f;

	secondRow = ((height / 6.0f) * 2.0f)  - 1.0f;
	thirdRow =  ((height / 6.0f) * 3.0f)  - 1.0f;
	fourthRow = ((height / 6.0f) * 4.0f)  - 1.0f;
	fifthRow  = ((height / 6.0f) * 5.0f)  - 1.0f;
	sixthRow  = height - 2.0f;
	


	m_PatternSections = 6;
	g_Vertices = new HDTV_CUSTOMVERTEX [(m_PatternSections * width * 2)+4];

	//Draw a blue line across the top of the image to key the color counting engine.
	if(bInterlaced)
	{
		startLine = 14.0f;//5 for HDTV monitor, 14 for VGA
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
	}
	
	else
	{
		startLine = 2.0f;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
	}
	

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
    		vert++;
			column++;
		}
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			column++;
		}
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
		}
    }

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			column++;
		}
		if(column < width)
		{
			//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;
		}
    }

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		if(column < width)
		{
			//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;
			column++;
		}
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
		}
    }




	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
		}
    }

	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;
		}
    }

	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = sixthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = sixthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
    		vert++;
			column++;
		}
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fifthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = sixthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff000000;
			vert++;
		}
    }
	
	  // Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( ((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_HDTV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pHDTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	HDTV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pHDTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, (((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX) ));
    m_pHDTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}



//---------------------------------------------
//Pattern13 is defined as follows:
//
//	Two full horizontal lines of blue
//	Alternating 2 green on, 2 light black for remaining top half of screen
//	Alternating 2 light black, 2 red on for second half of screen
//---------------------------------------------
BOOL CXModuleGrafx::HDTVTestPixelPattern13()
{
    
	HDTV_CUSTOMVERTEX* g_Vertices;
	int column = 0, width = 0, vert = 0;
	float startLine, firstRow, secondRow, thirdRow, fourthRow, height = 0.0f;
	BOOL bInterlaced = FALSE;
	

	//Check to see if we are in 1080i interlaced mode.
	if( (m_HDTVmode != NULL) && (!wcsicmp(m_HDTVmode, L"1080i")) )
		bInterlaced = TRUE;
	else
		bInterlaced = FALSE;


	width = (int)m_dwDisplayWidth;
	height = (float)m_dwDisplayHeight; //Make an easier variable to use.

	startLine = 0.0f;
	if(bInterlaced)
		firstRow  =  (height / 4.0f);
	else
		firstRow  =  (height / 4.0f) - 1.0f;

	secondRow = ((height / 4.0f) * 2.0f)  - 1.0f;
	thirdRow =  ((height / 4.0f) * 3.0f)  - 1.0f;
	fourthRow = height - 2.0f;
	


	m_PatternSections = 4;
	g_Vertices = new HDTV_CUSTOMVERTEX [(m_PatternSections * width * 2)+4];

	//Draw a blue line across the top of the image to key the color counting engine.
	if(bInterlaced)
	{
		startLine = 14.0f;//5 for HDTV monitor, 14 for VGA
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
	}
	
	else
	{
		startLine = 2.0f;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
	}
	

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
    		vert++;
			column++;
		}
		
		if(column < width)
		{
			//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff505050;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff505050;
			vert++;
		}
    }

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff00ff00;
    		vert++;
			column++;
		}
		if(column < width)
		{
			//green line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff505050;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff505050;
			vert++;
		}
    }

	
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
    		vert++;
			column++;
		}
		
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff505050;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff505050;
			vert++;
		}
    }

	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff505050;
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff505050;
    		vert++;
			column++;
		}
		if(column < width)
		{	//red line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = fourthRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xffff0000;
			vert++;
		}
    }
	
	  // Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( ((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_HDTV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pHDTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	HDTV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pHDTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, (((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX) ));
    m_pHDTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}



//---------------------------------------------
//Pattern14 is defined as follows:
//
//	Two full horizontal lines of blue
//	Alternating 2 green on, 2 light black for remaining top half of screen
//	Alternating 2 light black, 2 red on for second half of screen
//---------------------------------------------
BOOL CXModuleGrafx::HDTVTestPixelPattern14()
{
    
	HDTV_CUSTOMVERTEX* g_Vertices;
	int column = 0, width = 0, vert = 0;
	float startLine, firstRow, secondRow, thirdRow, height = 0.0f;
	BOOL bInterlaced = FALSE;
	

	//Check to see if we are in 1080i interlaced mode.
	if( (m_HDTVmode != NULL) && (!wcsicmp(m_HDTVmode, L"1080i")) )
		bInterlaced = TRUE;
	else
		bInterlaced = FALSE;


	width = (int)m_dwDisplayWidth;
	height = (float)m_dwDisplayHeight; //Make an easier variable to use.

	startLine = 0.0f;
	if(bInterlaced)
		firstRow  =  (height / 3.0f);
	else
		firstRow  =  (height / 3.0f) - 1.0f;

	secondRow = ((height / 3.0f) * 2.0f)  - 1.0f;
	thirdRow =  height - 1.0f;
	


	m_PatternSections = 3;
	g_Vertices = new HDTV_CUSTOMVERTEX [(m_PatternSections * width * 2)+4];

	//Draw a blue line across the top of the image to key the color counting engine.
	if(bInterlaced)
	{
		startLine = 14.0f;//5 for HDTV monitor, 14 for VGA
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 12.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 13.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = 0xff0000ff;
		vert++;
	}
	
	else
	{
		startLine = 2.0f;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 0.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
		g_Vertices[vert].x = 0.0f;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
		vert++;		
		g_Vertices[vert].x = (float)width;
		g_Vertices[vert].y = 1.0f;
		g_Vertices[vert].z = 0.0f;
		g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = 0xff0000ff;
   		vert++;
	}
	

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//red to black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = startLine;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(255,0,0);
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(127,0,0);
    		vert++;
		}
		
    }

	//Now start drawing the columns.
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//green to black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = firstRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(0,255,0);
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(0,127,0);
    		vert++;
		}
    }

	
	for(column = 0; column < width; column++)
    {
		if(column < width)
		{	//blue to black line
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = secondRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(0,0,255);
			vert++;		
			g_Vertices[vert].x = (float)column;
			g_Vertices[vert].y = thirdRow;
			g_Vertices[vert].z = 0.0f;
			g_Vertices[vert].rhw = 1.0f;	g_Vertices[vert].color = D3DCOLOR_XRGB(0,0,127);
    		vert++;
		}
    }

	
	  // Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( ((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_HDTV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pHDTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	HDTV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pHDTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}
    
	memcpy( pVertices, g_Vertices, (((m_PatternSections * width * 2)+4)*sizeof(HDTV_CUSTOMVERTEX) ));
    m_pHDTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);
	
    return TRUE;
}






//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID CXModuleGrafx::HDTVTestPixelCountRender()
{
	//HRESULT hr;
	//LPDIRECT3DSURFACE8 pBackBuffer;

	// Clear the backbuffer
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
	
	// Begin the scene
    g_pd3dDevice->BeginScene();

    if(m_bBitmapFromFile)
	{
		HDTVTestGetBitmapFromHost();
		/*
		g_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
		hr = D3DXLoadSurfaceFromFile( pBackBuffer,
											NULL,
											NULL,
											"d:\\pattern.bmp",
											NULL,
											D3DX_FILTER_NONE,
											0,
											NULL );
		SAFE_RELEASE(pBackBuffer);
		*/

	}
	else
	{
		//Draw the lines for the pixel count
		g_pd3dDevice->SetStreamSource( 0, m_pHDTVPatternVB, sizeof(HDTV_CUSTOMVERTEX) );
		g_pd3dDevice->SetVertexShader( D3DFVF_HDTV_CUSTOMVERTEX );
		g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, ((int)m_dwDisplayWidth * m_PatternSections) + 2);
	}
	
	// End the scene
    g_pd3dDevice->EndScene();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


VOID CXModuleGrafx::HDTVTestGetBitmapFromHost()
{
	DWORD dwActualBytesReceived = 0, dwErrorCodeFromHost = 0;
	int iError;
	DWORD *pdwReturnedBuffer = NULL;

	{
		CHostResponse CHR;
		
		if ((iError = g_host.iSendHost(MID_GET_FILE, (char *)&m_BitmapFileName[0], (_tcslen(m_BitmapFileName) + 1) * sizeof(TCHAR), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, 180)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(m_modeString, dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(m_modeString, iError))
					return;
			}
		}
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
		{
			// The response buffer is empty, so take appropriate action (like an error message)
			if (err_HostCommunicationError(m_modeString, iError))
				return;
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			HRESULT hr;
			LPDIRECT3DSURFACE8 pBackBuffer;
			g_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
			
			pdwReturnedBuffer = (DWORD *)CHR.pvBufferFromHost;

			/*
			D3DXLoadSurfaceFromFileInMemory( LPDIRECT3DSURFACE8 pDestSurface,
											  CONST PALETTEENTRY* pDestPalette,
											  CONST RECT* pDestRect,
											  LPCVOID pSrcData,
											  UINT SrcData,
											  CONST RECT* pSrcRect,
											  DWORD Filter,
											  D3DCOLOR ColorKey,
											  D3DXIMAGE_INFO* pSrcInfo);*/

			hr = D3DXLoadSurfaceFromFileInMemory( pBackBuffer,
												  NULL,
												  NULL,
												  pdwReturnedBuffer,
												  dwActualBytesReceived,
												  NULL,
												  D3DX_FILTER_NONE,
												  0,
												  NULL);

			SAFE_RELEASE(pBackBuffer);



			/*
			HANDLE hFile = CreateFile( "z:\\pattern.bmp",
										GENERIC_WRITE,
										FILE_SHARE_WRITE,
										NULL,
										CREATE_ALWAYS,
										0,
										NULL );
			
			
			
			WriteFile(hFile,
						pdwReturnedBuffer,
						dwActualBytesReceived,
						&dwBytesWritten,
						NULL);
			ReportWarning(L"bytes received-written = %lu %lu\n", dwActualBytesReceived, dwBytesWritten);
			CloseHandle(hFile);
			*/
		}
	}

}

//-----------------------------------------------------------------------------
// Name: InitDacPattern()
// Desc: Creates a vertex buffer and fills it with our vertices. The vertex
//       buffer is basically just a chuck of memory that holds vertices. After
//       creating it, we must Lock()/Unlock() it to fill it. For indices, D3D
//       also uses index buffers. The special thing about vertex and index
//       buffers is that the ycan be created in device memory, allowing some
//       cards to process them in hardware, resulting in a dramatic
//       performance gain.
//-----------------------------------------------------------------------------
BOOL CXModuleGrafx::HDTVTestInitDacPattern()
{
    
	HDTV_CUSTOMVERTEX* g_Vertices;
	int vert, blueLines, currentLine, loop = 0;
	float firstRow, secondRow, width, height = 0.0f;
	int r, g, b=0;


	width = (float)m_dwDisplayWidth;
	height = (float)m_dwDisplayHeight; //Make an easier variable to use.

	firstRow  = (height / 3.0f) - 1.0f;
	secondRow = ((height / 3.0f) * 2.0f) - 1.0f;
	blueLines = (m_bInterlaced)? 30 : 15;
	m_totalDacLines = HDTV_DAC_LINES + (blueLines * 2);


	g_Vertices = new HDTV_CUSTOMVERTEX [m_totalDacLines * 2];

	
	ReportDebug(DEBUG_LEVEL1, L"BlueLines=%d, Interlaced=%d",blueLines, m_bInterlaced);
	
	// Initialize 2 vertices for each line.
	currentLine = 0;
	vert=0;
	for(loop=0; loop < blueLines; loop++)
    {
		//Draw the BLUE section of the first lines.
		g_Vertices[vert].x = 0.0f;				g_Vertices[vert].y = (float)currentLine;
		g_Vertices[vert].z = 0.0f;				g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = D3DCOLOR_XRGB(0,0,255) ;
		vert++;		
		g_Vertices[vert].x = width - 10.0f;		g_Vertices[vert].y = (float)currentLine;
		g_Vertices[vert].z = 0.0f;				g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = D3DCOLOR_XRGB(0,0,255) ;
    	vert++;

		//Draw the YELLOW section of the first lines.
		g_Vertices[vert].x = width-10.0f;		g_Vertices[vert].y = (float)currentLine;
		g_Vertices[vert].z = 0.0f;				g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = D3DCOLOR_XRGB(255,255,0) ;
		vert++;		
		g_Vertices[vert].x = width;				g_Vertices[vert].y = (float)currentLine;
		g_Vertices[vert].z = 0.0f;				g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = D3DCOLOR_XRGB(255,255,0) ;
    	vert++;
		currentLine++;
	}
			
	ReportDebug(DEBUG_LEVEL1, L"CurrentLine=%d before white starts to draw.\n",currentLine);	
	//Now draw the Y gradient.
	//Start with WHITE and fade to BLACK.
	r=g=b=255;
	for(loop=0; loop < 128; loop++)
    {
		g_Vertices[vert].x = 0.0f;				g_Vertices[vert].y = (float)currentLine;
		g_Vertices[vert].z = 0.0f;				g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = D3DCOLOR_XRGB(r,g,b) ;
		vert++;		
		g_Vertices[vert].x = width;				g_Vertices[vert].y = (float)currentLine;
		g_Vertices[vert].z = 0.0f;				g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = D3DCOLOR_XRGB(r,g,b) ;
    	vert++;
		
		r=g=b-=2;
		if(r <=0)
			r=g=b=0;
		
		currentLine++;
	}		
	


	//Now draw the RPr gradient.
	//Start with RED and fade to CYAN.
	r=255;
	g=b=0;
	for(loop=0; loop < 128; loop++)
    {
		g_Vertices[vert].x = 0.0f;				g_Vertices[vert].y = (float)currentLine;
		g_Vertices[vert].z = 0.0f;				g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = D3DCOLOR_XRGB(r,g,b) ;
		vert++;		
		g_Vertices[vert].x = width;				g_Vertices[vert].y = (float)currentLine;
		g_Vertices[vert].z = 0.0f;				g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = D3DCOLOR_XRGB(r,g,b) ;
    	vert++;
		
		r-=2;

		g=b+=2;

		currentLine++;
	}		
			
	//Now draw the BPb gradient.
	//Start with BLUE and fade to YELLOW.
	b=255;
	r=g=0;
	for(loop=0; loop < 128; loop++)
    {
		g_Vertices[vert].x = 0.0f;				g_Vertices[vert].y = (float)currentLine;
		g_Vertices[vert].z = 0.0f;				g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = D3DCOLOR_XRGB(r,g,b) ;
		vert++;		
		g_Vertices[vert].x = width;				g_Vertices[vert].y = (float)currentLine;
		g_Vertices[vert].z = 0.0f;				g_Vertices[vert].rhw = 1.0f;
		g_Vertices[vert].color = D3DCOLOR_XRGB(r,g,b) ;
    	vert++;
		
		b-=2;

		r=g+=2;

		currentLine++;
	}		
			

	// Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( m_totalDacLines*2*sizeof(HDTV_CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY,
												  D3DFVF_HDTV_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &m_pHDTVPatternVB ) ) )
    {
		SAFE_DELETE_ARRAY(g_Vertices);
		return FALSE;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
	HDTV_CUSTOMVERTEX* pVertices;
    if( FAILED( m_pHDTVPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
		SAFE_DELETE_ARRAY(g_Vertices);
        return FALSE;
	}

    memcpy( pVertices, g_Vertices, m_totalDacLines*2*sizeof(HDTV_CUSTOMVERTEX) );
    m_pHDTVPatternVB->Unlock();
	SAFE_DELETE_ARRAY(g_Vertices);

    return TRUE;
}





//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID CXModuleGrafx::HDTVTestAnalogColorsRender()
{
    ReportDebug(DEBUG_LEVEL1, L"HDTV.CPP - Attempting to Render the AnalogColors scene...");
	
	// Clear the backbuffer to a blue color
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
		
    // Begin the scene
    g_pd3dDevice->BeginScene();

    //Draw the lines for the dac pattern
	g_pd3dDevice->SetStreamSource( 0, m_pHDTVPatternVB, sizeof(HDTV_CUSTOMVERTEX) );
    g_pd3dDevice->SetVertexShader( D3DFVF_HDTV_CUSTOMVERTEX );
    g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, m_totalDacLines);
	
	// End the scene
    g_pd3dDevice->EndScene();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}
	
	  
//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID CXModuleGrafx::HDTVTestBackgroundRender(int r, int g, int b)
{
	// Clear the backbuffer to a blue color
    g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(r,g,b), 1.0f, 0L );

	// Begin the scene
    g_pd3dDevice->BeginScene();

	// End the scene
    g_pd3dDevice->EndScene();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}