//-----------------------------------------------------------------------------
// (c) Copyright 2000-2000 Intel Corp. All rights reserved.
//
// vidmemory.cpp - Graphics test (Vidmemory) used to stress the video memory
//					functions of the graphics system.
//
// Member of:  Grafx Module for xmta test executive.
//
//-----------------------------------------------------------------------------
#include "..\..\stdafx.h"
#include "..\..\testobj.h"
#include "xgrafx.h"
//#include "rand32.h"

#define DEFAULT_RAND_SEED 0x6f39f1e5;
//#define VIDMEM_MAX_SCENES 3

//******************************************************************
// Title: Vidmemory
//
// Abstract: Renders a 3D scene to a surface and performs various 
//				memory transfer operations beetween multiple surfaces
//				to stress the video memory systems.
//
//******************************************************************
IMPLEMENT_TESTLOCKED (Grafx, Vidmemory_patterns, 5)
{
	DWORD  crc, crc2;
	int numSurfaces = 0;
	
	if (CheckAbort (HERE)) { MainCleanup(); return;}
	//Check for an initialized D3D Device.
	if(g_pd3dDevice == NULL)
	{
		//Error - the global device was not created successfully
		//or has become lost or invalid.
		err_NODIRECT3D(m_modeString);
		MainCleanup();
		return;
	}
			
	if(m_expectedCRC == 0xffffffff)
	{
		err_BADPARAMETER( m_modeString, L"crc" );
		MainCleanup();
		return;
	}

	if(m_mbytesTested < 2 || m_mbytesTested > 60)
	{
		err_BADPARAMETER( m_modeString, L"mbytes_tested");
		MainCleanup();
		return;
	}
	else//Make the value entered a 'megabyte' value
		m_mbytesTested *=1000000;
	
	if (CheckAbort (HERE)) { MainCleanup(); return;}	
	
	
	ReportWarning(L"Bit pattern used=0x%x", m_vidmemPattern);
	
	//Create additional surfaces to copy rendered image to.
	//This will stress the bit-blting processes and help verify as much video memory as possible.
	numSurfaces = VidmemPatternInitSurfaces();
	if(numSurfaces == 0)
		return;//Fatal error - exit test.

	//Init the crc's to default values.
	crc = crc2 = 0x00000001;
	
	if (CheckAbort (HERE)) { MainCleanup(); return;}
			
	//Render a scene based on loop count.
	if(!VidmemPatternRender())
	{	//Fatal error - exit test.
		MainCleanup();
		return;
	}
	
	//Present to force contents to frame buffer, then get the CRC.
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	
	if(m_secondsToDisplay)
		Sleep(m_secondsToDisplay);	
	
	
	//*** This section commented out 7/5/2001 to allow parallel test execution - DJM.
	/*	
	if(m_hardwareCRC)
		crc = GetDisplayBufferCRC();
	else
		crc = VidmemPatternCalcCRC();
	//Output the measured crc for each scene.
	ReportDebug(DEBUG_LEVEL1, _T("Starting CRC = 0x%x\n"), crc);
	
	if(crc != m_expectedCRC)
	{
		err_BADCRC(m_modeString, crc, m_expectedCRC, L"Starting CRC.");
		if (CheckAbort (HERE)) { MainCleanup(); return;}
	}
	*/
	
	//Copy the contents of the first surface through all of the new surfaces.
	if(!VidmemPatternCopySurfaces(numSurfaces))
	{	//Fatal error - exit test.
		ReportDebug(DEBUG_LEVEL1, _T("VIDMEMORY_PATTERN.CPP - Failed to CopySurfaces() from main test function!"));
		return;
	}
	
	//Output the final copied image to the screen.
	//To do this, copy the image info the the back buffer and then perform a present() to flip
	// the contents to the display.
	
	
	//Get the CRC value from the final surface.
	if(m_hardwareCRC)
		crc2 = GetDisplayBufferCRC();
	else
		crc2 = VidmemPatternCalcCRC(m_pD3Ds[numSurfaces-1]);
	
	ReportDebug(DEBUG_LEVEL1, _T("Ending CRC after %d surface blts = 0x%x\n"), numSurfaces, crc2);
	
	ReportDebug(DEBUG_LEVEL1, _T("If this was done on known good hardware, \nthe CRC value to insert into the ini file for the expected test value: = 0x%x\n"), crc2);
	if(crc2 != m_expectedCRC)
	{
		err_BADCRC(m_modeString, crc2, m_expectedCRC, L"Final CRC.");
		if (CheckAbort (HERE)) { MainCleanup(); return;}
	}
	
	//Cleanup
	MainCleanup();

}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
BOOL CXModuleGrafx::VidmemPatternRender()
{

	g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xFF000000, 1.0f, 0L );
    
	//Default seed for randowm number generator.
	//Use a different seed for each mode to help eliminate false measurements.

	// Begin the scene
    g_pd3dDevice->BeginScene();
    
	//Fill the background with suedo-randow noise based off of a known seed generated pattern.
	if(!FillSurfaceWithPattern(m_vidmemPattern))
		return FALSE;

    // End the scene.
    g_pd3dDevice->EndScene();
    return TRUE;
}


DWORD CXModuleGrafx::VidmemPatternCalcCRC()
{
	
	D3DLOCKED_RECT pLockedRect;		//Locked rectangle struct for determing surface image data.
	LONG pitch;
	BYTE *pBits;
	HRESULT hr;
	LPDIRECT3DSURFACE8 lpSurface = NULL;
	int bytesPerLine;
	D3DSURFACE_DESC surfDesc;
	DWORD calcedCRC = 0;
	
	lpSurface = NULL;
	
	ReportDebug(DEBUG_LEVEL2, _T("Calculating CRC by obtaining front buffer\n"));
	
	//Since we have already presented the contents of the back buffer, we need
	//to get our data from the front buffer, hence the first argument of a -1. 
	if((hr = g_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &lpSurface)) != D3D_OK)
	{	//Fatal error - exit the test.
		ReportDebug(DEBUG_LEVEL1, _T("VIDMEM_PATTERN.CPP - Failed to GetFrontBuffer() for CRC calc!\n"));
		ReportD3DError(hr);
		if (CheckAbort (HERE))
		{
			SAFE_RELEASE(lpSurface);
			MainCleanup();
			return 0;
		}
	}
	
	lpSurface->GetDesc( &surfDesc);
	ReportDebug(DEBUG_LEVEL2, _T("Surface in CRC is %d bytes large\n"), surfDesc.Size);
	ReportDebug(DEBUG_LEVEL2, _T("Surface in CRC is %d pixels wide and %d pixels high\n"), surfDesc.Width, surfDesc.Height);
	
	//Lock the rect to get pointer to starting address.
	hr = lpSurface->LockRect( &pLockedRect, NULL, NULL );
	if(hr != D3D_OK)
	{
		ReportDebug(DEBUG_LEVEL1, _T("VIDMEMORY_PATTERN.CPP - Failed to LockRect() for surface CRC calc!\n"));
		ReportD3DError(hr);
		return 0;
	}
	pBits = (BYTE *)pLockedRect.pBits;
	pitch = pLockedRect.Pitch;
	bytesPerLine = ((m_bpp * (m_dwDisplayWidth)) + 7) >> 3;
	//bytesPerLine = ((m_bpp * (m_dwDisplayWidth)) ) >> 3;
	calcedCRC = UpdateImageRectCRC(m_dwDisplayHeight, pBits, pitch, bytesPerLine);
		
	//Unlock the locked surface.
	lpSurface->UnlockRect();
	SAFE_RELEASE(lpSurface);

	return calcedCRC;
}



DWORD CXModuleGrafx::VidmemPatternCalcCRC(LPDIRECT3DSURFACE8 lpSurface)
{
	
	D3DLOCKED_RECT pLockedRect;		//Locked rectangle struct for determing surface image data.
	LONG pitch;
	BYTE *pBits;
	HRESULT hr;
	int bytesPerLine;
	D3DSURFACE_DESC surfDesc;
	DWORD calcedCRC = 0;
	
	  
	ReportDebug(DEBUG_LEVEL2, _T("Calculatiing CRC with surface as argument\n"));

	lpSurface->GetDesc( &surfDesc);
	ReportDebug(DEBUG_LEVEL2, _T("Surface in CRC is %d bytes large\n"), surfDesc.Size);
	ReportDebug(DEBUG_LEVEL2, _T("Surface in CRC is %d pixels wide and %d pixels high\n"), surfDesc.Width, surfDesc.Height);
	
	//Lock the rect to get pointer to starting address.
	hr = lpSurface->LockRect( &pLockedRect, NULL, NULL );
	if(hr != D3D_OK)
	{
		ReportDebug(DEBUG_LEVEL1, _T("VIDMEMORY_PATTERN.CPP - Failed to LockRect() for surface CRC calc!\n"));
		ReportD3DError(hr);
		return 0;
	}
	pBits = (BYTE *)pLockedRect.pBits;
	pitch = pLockedRect.Pitch;
	bytesPerLine = ((m_bpp * (m_dwDisplayWidth)) + 7) >> 3;
	//bytesPerLine = ((m_bpp * (m_dwDisplayWidth)) ) >> 3;
	ReportDebug(DEBUG_LEVEL1, _T("pitch= %d, bpl=%d\n"), pitch, bytesPerLine);
	calcedCRC = UpdateImageRectCRC(m_dwDisplayHeight, pBits, pitch, bytesPerLine);
		
	//Unlock the locked surface.
	lpSurface->UnlockRect();

	return calcedCRC;
}


//----------------------------------------------------------
//
// VidMemInitSurfaces()
//
//----------------------------------------------------------
int CXModuleGrafx::VidmemPatternInitSurfaces()
{
	HRESULT hr;
	DWORD surfaceSize;
	MEMORYSTATUS stat;
	D3DSURFACE_DESC surfDesc;
    LPDIRECT3DSURFACE8 pBackBuffer;


	surfaceSize = 0;
	pBackBuffer = NULL;
	
	// Get render target surface desc
    g_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
    pBackBuffer->GetDesc( &surfDesc);
	SAFE_RELEASE(pBackBuffer);

	ReportDebug(DEBUG_LEVEL2, _T("Size of reference surface from descrip struct = %d\n"), surfDesc.Size);
	//Get size of reference surface.
	surfaceSize = surfDesc.Size;
	
	GlobalMemoryStatus (&stat);
		
	if(m_mbytesTested > stat.dwAvailPhys)
	{
		ReportWarning( _T("The amount of memory specified to test exceeds\n the amount of available free memory.\nA safe amount will be calculated to use.\n"));
		m_newSurfaces = (stat.dwAvailPhys / surfaceSize);
		if(m_newSurfaces > MAX_SURFACES)
		{
			//WARNING - more room for surfaces available than expected.
			//Report a warning that the number being used is less than the max available.
			ReportDebug(BIT0, _T("Using MAX_SURFACES value even though more memory may be available.\n"));
			m_newSurfaces = MAX_SURFACES;
		}
		ReportDebug(BIT0, _T("The following calculated safe\nvalues will be used for the test:\n"));
		ReportDebug(BIT0, _T("Surfaces=%d, Total memory tested=%lu bytes.\n"), m_newSurfaces, m_newSurfaces*surfaceSize);
	}
	else
	{
		m_newSurfaces = (m_mbytesTested / surfaceSize);
		if(m_newSurfaces > MAX_SURFACES)
		{
			//WARNING - more room for surfaces available than expected.
			//Report a warning that the number being used is less than the max available.
			ReportDebug(BIT0, _T("Using MAX_SURFACES value even though more memory may be available.\n"));
			m_newSurfaces = MAX_SURFACES;
		}
		ReportDebug(DEBUG_LEVEL1, _T("Surfaces=%d, Total memory tested=%lu bytes.\n"), m_newSurfaces, m_newSurfaces*surfaceSize);
		
	}
	
	//Create as many new surfaces as possible.
	//For now we will ignore any remainder resulting from the division
	//since this is a unified memory architecture.  The regular system 
	//memory tests will cover any memory we may miss here.
	//Create the surfaces required for the test.
	m_pD3Ds = new LPDIRECT3DSURFACE8 [m_newSurfaces];
	for(int i = 0; i < m_newSurfaces; i++)
	{
		m_pD3Ds[i] = NULL;
		if((hr = g_pd3dDevice->CreateImageSurface(	m_dwDisplayWidth,
													m_dwDisplayHeight,
													m_BackBufferFormat,
													&m_pD3Ds[i])) != D3D_OK)
		{
			//Error creating a surface
			ReportDebug(DEBUG_LEVEL1, _T("VIDMEMORY_PATTERN.CPP - Failed creating new surfaces!\n"));
			ReportD3DError(hr);
			return 0;
		}

		m_pD3Ds[i]->GetDesc( &surfDesc);
		ReportDebug(DEBUG_LEVEL2, _T("Surface %d is %d bytes large\n"), i, surfDesc.Size);
		ReportDebug(DEBUG_LEVEL2, _T("Surface %d is %d pixels wide and %d pixels high\n"), i, surfDesc.Width, surfDesc.Height);
		
		GlobalMemoryStatus (&stat);
		ReportDebug(DEBUG_LEVEL2, _T("There are %lu TOTAL bytes of physical memory.\n"), stat.dwTotalPhys);
		ReportDebug(DEBUG_LEVEL2, _T("There are %lu FREE bytes of physical memory.\n"), stat.dwAvailPhys);
		ReportDebug(DEBUG_LEVEL2, _T("%.2f percent of memory is available.\n"), ((float)stat.dwAvailPhys/(float)stat.dwTotalPhys)*100.0f);
	}
	
	return m_newSurfaces;
}


BOOL CXModuleGrafx::VidmemPatternCopySurfaces(int numSurfaces)
{
	HRESULT hr;
	LPDIRECT3DSURFACE8 pFrontBuffer;
	LPDIRECT3DSURFACE8 pBackBuffer;

    
	//Flip the front buffer to the back buffer.
	
	//This get the originally rendered contents back into a buffer we can safely manipulate.
	//g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	g_pd3dDevice->GetBackBuffer( -1, D3DBACKBUFFER_TYPE_MONO, &pFrontBuffer);
	
	//XGWriteSurfaceToFile( IDirect3DSurface8* pSurf, const char* cPath);
	XGWriteSurfaceToFile( pFrontBuffer, "z:\\surface.bmp");
	
	//Copy the backbuffer surface contents into the first new surface
	hr = g_pd3dDevice->CopyRects(pFrontBuffer, NULL, 0, m_pD3Ds[0], NULL);
	if(hr != D3D_OK)
	{	//Fatal error - exit test.
		SAFE_RELEASE(pFrontBuffer);
		ReportDebug(DEBUG_LEVEL1, _T("VIDMEMORY_PATTERN.CPP - Failed trying to perform first copySurfaces() for test!\n"));
		ReportD3DError(hr);
		MainCleanup();
		return FALSE;
	}

	//Present again so we don't have a distorted display.
	//g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	//Save the contents of this surface to compare to the original.
	XGWriteSurfaceToFile( m_pD3Ds[0], "z:\\copied_surface.bmp");

	if (CheckAbort (HERE)) { MainCleanup(); return FALSE;}

	for(int i = 0; i < numSurfaces-1; i++)
	{
		hr = g_pd3dDevice->CopyRects(m_pD3Ds[i], NULL, 0, m_pD3Ds[i+1], NULL);
		if(hr != D3D_OK)
		{
			ReportDebug(DEBUG_LEVEL1, _T("VIDMEMORY_PATTERNS.CPP - Failed trying to perform subsequent copySurfaces() for test!\n"));
			ReportD3DError(hr);
			if (CheckAbort (HERE)) { MainCleanup(); return FALSE;}
		}
		ReportDebug(DEBUG_LEVEL2, _T("VIDMEMORY_PATTERNS.CPP - Copied surface %d to surface %d successfully!\n"), i, i+1);
	}

//	XGWriteSurfaceToFile( m_pD3Ds[numSurfaces-1], "d:\\final_copied_surface.bmp");
	
	g_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	//Copy the final surface contents into the backbuffer so we can present it to the display buffer.
	hr = g_pd3dDevice->CopyRects(m_pD3Ds[numSurfaces-1], NULL, 0, pBackBuffer, NULL);
	if(hr != D3D_OK)
	{	//Fatal error - exit test.
		SAFE_RELEASE(pBackBuffer);
		ReportDebug(DEBUG_LEVEL1, _T("VIDMEMORY_PATTERNS.CPP - Failed trying to copySurfaces() from final copied surface to backBuffer!\n"));
		ReportD3DError(hr);
		MainCleanup();
		return FALSE;
	}
	
	//Present the contents to the display buffer so we can get a CRC check.
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	  
	SAFE_RELEASE(pBackBuffer);
	SAFE_RELEASE(pFrontBuffer);

	return TRUE;
}



//******************************************************
//
// This function fills the display surface with a WORD bit pattern defined as
// a parameter from the INI file.
//
//******************************************************
BOOL CXModuleGrafx::FillSurfaceWithPattern(BYTE pattern)
{
	// Fill the rectangle in the image with random data based on seed
	D3DLOCKED_RECT pLockedRect;		//Locked rectangle struct for determing surface image data.
	LONG pitch;
	BYTE *pBits;
	HRESULT hr;
	LPDIRECT3DSURFACE8 lpd3dSurface;
	int bytesPerLine;
//	CRand32 rand;
	
	lpd3dSurface = NULL;
	
	if((hr = g_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &lpd3dSurface)) != D3D_OK)
	{	//Fatal error - exit the test.
		ReportDebug(DEBUG_LEVEL1, _T("VIDMEMORY_PATTERN.CPP - Failed to GetBackBuffer() for FillRand()!"));
		ReportD3DError(hr);
		if (CheckAbort (HERE))
		{
			SAFE_RELEASE(lpd3dSurface);
			MainCleanup();
			return 0;
		}
	}
	
	hr = lpd3dSurface->LockRect( &pLockedRect, NULL, NULL);
	if(hr != D3D_OK)
	{
		ReportD3DError(hr);
		return FALSE;
	}
	pBits = (BYTE *)pLockedRect.pBits;
	pitch = pLockedRect.Pitch;
	bytesPerLine = ((m_bpp * (m_dwDisplayWidth)) + 7) >> 3;
	//bytesPerLine = ((m_bpp * (m_dwDisplayWidth)) ) >> 3;
	
	//rand.seed(seed);

	// Fill each line with random data, masking the first and last bytes as needed
	for (LONG line=0; line<(LONG)m_dwDisplayHeight; line++)
	{
		FillBuffer(pBits, bytesPerLine, pattern);
		pBits += pitch;
	}

	lpd3dSurface->UnlockRect();
	SAFE_RELEASE(lpd3dSurface);
		
	return TRUE;
}



//Fills in the DWORD aligned part of the buffer.
void CXModuleGrafx::FillBuffer(unsigned long *start_address, int count, unsigned char pattern)
{
	unsigned long data = 0, temp = 0;
	
	if (count <= 0) return;

	temp=(unsigned long)pattern;

	data = data | (pattern<<24);
	data = data | (pattern<<16);
	data = data | (pattern<<8);
	data = data | pattern;

	while (count--)
	{
		*start_address++ =  data;
		//*start_address++ =  ((unsigned long)pattern << 17) ^ ((unsigned long)pattern << 6) ^ (unsigned long)pattern;
	}
}


void CXModuleGrafx::FillBuffer(unsigned char *start_address, int count, unsigned char pattern)
{
	int byteCount;

	//Avoid warnings.
	byteCount	= 0;
	
	if (count <= 0) return;
	// Be sure start_address is DWORD aligned
	if (byteCount = ((int)start_address & 3))
	{
		byteCount = 4 - byteCount;
		if (byteCount > count) byteCount = count;
		while (byteCount--)
		{
			*start_address++ = pattern;
			count--;
		}
	}

	// Fill DWORD aligned part of buffer
	count >>= 2;
	FillBuffer((unsigned long *)start_address, count, pattern);

	// Fill trailing bytes
	start_address += count << 2;
	while (byteCount--)
	{
		*start_address++ = pattern;
	}
}

