//-----------------------------------------------------------------------------
// (c) Copyright 2000-2001 Intel Corp. All rights reserved.
//
// xgrafx.cpp - Graphics testing module for xmta.
//
//-----------------------------------------------------------------------------

#include "..\..\stdafx.h"
#include "..\..\testobj.h"
#include "xgrafx.h"
#include "crc32.h"
//********************************************************
//Some special code for performing the hardware CRC check.
//
DWORD g_DoChecksum = 0;
DWORD g_Channel = 0;
DWORD g_Value[3];


void __cdecl ChecksumCallback(DWORD Frame)
{
	if(g_DoChecksum)
	{
		if(g_Channel > 0)
		{
			//OutputDebugString(L"Local CRC callback");
			g_Value[g_Channel - 1] = *(volatile DWORD *)0xFD680840 & 0x00FFFFFF;
		}

		if(g_Channel < 3)
		{
			*(volatile DWORD *)0xFD680844 = 0x411 | (g_Channel << 8);
			*(volatile DWORD *)0xFD680844 = 0x410 | (g_Channel << 8);

			g_Channel++;
		}
		else
		{
			g_Channel = 0;
			g_DoChecksum = 0;
		}
	}
}


/*
void __cdecl ChecksumCallback(DWORD Frame)
{
	if(g_DoChecksum)
	{
		if(g_Channel < 3)
		{
			//OutputDebugString(L"Local CRC callback");
			g_Value[g_Channel ] = *(volatile DWORD *)0xFD680840 & 0x00FFFFFF;
		}

		if(g_Channel > 0)
		{
			*(volatile DWORD *)0xFD680844 = 0x411 | ((g_Channel-1) << 8);
			*(volatile DWORD *)0xFD680844 = 0x410 | ((g_Channel-1) << 8);

			g_Channel--;
		}
		else
		{
			g_Channel = 3;
			g_DoChecksum = 0;
		}
	}
}
*/


void GetScreenChecksum(DWORD *pRed, DWORD *pGreen, DWORD *pBlue)
{
	g_Value[0] = g_Value[1] = g_Value[2] = 0;
	g_DoChecksum = 1;
	g_Channel = 0;

	D3DDevice_BlockUntilVerticalBlank();
	
	g_DoChecksum = 1;
	g_Channel = 0;

	while(g_DoChecksum)
	{
		D3DDevice_BlockUntilVerticalBlank();
	}
	
	*pRed	= g_Value[0];
	*pGreen	= g_Value[1];
	*pBlue	= g_Value[2];
}



IMPLEMENT_MODULELOCKED (CXModuleGrafx);

bool CXModuleGrafx::InitializeParameters ()
{
	if (!CTestObj::InitializeParameters ())
		return false;

	//Init params before attempting to retrieve.
	//Params for all tests
	m_pBackBuffer       = NULL;
    m_BackBufferFormat  = g_d3dpp.BackBufferFormat;
    m_DepthBufferFormat = g_d3dpp.AutoDepthStencilFormat;
	m_hardwareCRC = GetCfgInt( L"hardware_crc", 1);

	//Params for 3DSurface Test
	m_pBackgroundVB		= NULL;
	m_pSpotLightTexture = NULL;
	m_dwDisplayWidth    = g_d3dpp.BackBufferWidth;
    m_dwDisplayHeight   = g_d3dpp.BackBufferHeight;
	_stprintf(m_modeString, _T("%lux%lu"), m_dwDisplayWidth, m_dwDisplayHeight);
	m_bpp				= 32;
	


	//Params for vidmemory test
	m_pD3Ds = NULL;
	m_newSurfaces = 0;
	
	//Params for shapes test
	m_pVB = NULL;
	m_pVB2 = NULL;
	m_pVB3 = NULL;
	m_pTriangles = NULL;
	m_pTriangles2 = NULL;
	m_pTriangles3 = NULL;

	m_numTriangles = 0;

	if (CheckAbort (HERE)) return true;

	//Params required for all tests.
	m_expectedCRC = GetCfgUint( L"crc", 0xffffffff);
	m_testScenes = GetCfgInt( L"scenes", 1);
		
	//Video memory test
	m_mbytesTested = GetCfgUint( L"mbytes_tested", 32);

	//Video memory patern test
	m_vidmemPattern = (unsigned char)GetCfgInt( L"bit_pattern", 0xAA);
		
	//Params for Shapes test
	m_numTriangles = GetCfgInt(L"triangles", 21800);
		
	
	//---------------------------
	// ** Params for HDTV Test **
	//
	//Global params.
	m_HDTVTestPattern = GetCfgString( L"test", NULL);

	m_HDTVmode = GetCfgString( L"hdtv_mode", NULL);
	m_wPixelThreshold = (WORD)GetCfgUint( L"pixel_thresh", 0);
	m_wThresholdRangeStep = (WORD)GetCfgUint( L"range_step", 10);
	m_wHSyncThreshold = (WORD)GetCfgUint( L"hsync_thresh", 0);
	m_wVSyncThreshold = (WORD)GetCfgUint( L"vsync_thresh", 0);
	m_dwTolerance = GetCfgUint( L"tol", 0);
	m_testRetries = GetCfgInt( L"retries", 1);
	m_secondsToDisplay = 1000 * (GetCfgInt( L"std", 0));

	//Pixelcount test params.
	m_dwExpectedGreenCount = GetCfgUint( L"green", 0);
	m_dwExpectedRedCount = GetCfgUint( L"red", 0);
	m_dwExpectedBlueCount = GetCfgUint( L"blue", 0);
	m_PixelCountPattern = GetCfgInt( L"pattern", 2);
	m_bBitmapFromFile = GetCfgInt(L"bitmap_from_file", 0);
	_stprintf(m_BitmapFileName, _T("%s_pattern.bmp"),m_modeString);
	
	//Timings test params.
	m_VertFrontPorch1 = (BYTE)GetCfgInt( L"vfp1", -1);
	m_VertBackPorch1 = (BYTE)GetCfgInt( L"vbp1", -1);
	m_VertFrontPorch2 = (BYTE)GetCfgInt( L"vfp2", -1);
	m_VertBackPorch2 = (BYTE)GetCfgInt( L"vbp2", -1);
	m_wVertSyncLow = (WORD)GetCfgUint( L"vsl", -1);
	m_wVertSyncHigh = (WORD)GetCfgUint( L"vsh", -1);
	m_wHorizBackPorch = (WORD)GetCfgUint( L"hbp", -1);
	m_wHorizFrontPorch = (WORD)GetCfgUint( L"hfp", -1);
	m_wHorizSyncLow = (WORD)GetCfgUint( L"hsl", -1);
	m_wHorizSyncHigh = (WORD)GetCfgUint( L"hsh", -1);
	
	//AnalogColors test params.
	m_wNumLines = (WORD)GetCfgUint( L"lines", -1);
	m_wNumSamples = (WORD)GetCfgUint( L"samples", -1);
	m_wStartLine = (WORD)GetCfgUint( L"start_line", -1);
	m_hDelay = (BYTE)GetCfgInt( L"h_delay", -1);
	m_wLinearTolerance = (WORD)GetCfgUint( L"dac_tolerance", -1);
	m_GY_LowerThreshold = (WORD)GetCfgUint( L"gy_lower", -1);
	m_BPb_LowerThreshold = (WORD)GetCfgUint( L"bpb_lower", -1);
	m_RPr_LowerThreshold = (WORD)GetCfgUint( L"rpr_lower", -1);
	m_GY_UpperThreshold = (WORD)GetCfgUint( L"gy_upper", -1);
	m_BPb_UpperThreshold = (WORD)GetCfgUint( L"bpb_upper", -1);
	m_RPr_UpperThreshold = (WORD)GetCfgUint( L"rpr_upper", -1);
	
	//TriLevel Sync parameters
	m_syncMode = GetCfgInt(L"sync_mode", -1);
	m_syncType = GetCfgString( L"sync_type", NULL);
		
	//pColorData is used by both the DAC and TriLevel tests.
	pColorData = NULL;

		
	if(m_hardwareCRC)//Use hardware CRC.
	{
		ReportDebug(DEBUG_LEVEL1, L"Test will use HARDWARE CRC calculations.\n");
		D3DDevice_SetVerticalBlankCallback(ChecksumCallback);
		//D3DDevice_SetVerticalBlankCallback(D3DTest_ChecksumCallback);
	}

    return TRUE;
}


DWORD CXModuleGrafx::UpdateImageRectCRC(LONG height, BYTE *pBits, LONG pitch, int bytesPerLine)
{
	// Update the CRC with pixel data using the provided memory parameters
	DWORD crc = DEFAULT_CRC;
	LONG line;
	
	for (line=0; line<height; line++) {
		crc = ::updateCRC32(pBits, bytesPerLine, crc);
		pBits += pitch;
		ReportDebug(DEBUG_LEVEL2, _T("Line%d, CRC=0x%x\n"),line, crc);
	}
	
	return crc;
}

DWORD CXModuleGrafx::GetDisplayBufferCRC()
{
	// Update the CRC with pixel data using the provided memory parameters
	DWORD dwRed = 0;
	DWORD dwGreen = 0;
	DWORD dwBlue = 0;
	DWORD dwCompositeCRC = 0;

	//D3DTest_GetScreenChecksum(&dwRed, &dwGreen, &dwBlue);
	GetScreenChecksum(&dwRed, &dwGreen, &dwBlue);
	dwCompositeCRC = dwRed*dwGreen*dwBlue;
	
	ReportDebug(DEBUG_LEVEL1, _T("HardwareCRC: Red=0x%x, Green=0x%x, Blue=0x%x\n"),dwRed, dwGreen, dwBlue);
	ReportDebug(DEBUG_LEVEL1, _T("Composite HardwareCRC=0x%x\n"), dwCompositeCRC);
	
	return dwCompositeCRC;
}

VOID CXModuleGrafx::ReportD3DError(DWORD error)
{

	switch(error)
	{
		case D3D_OK:
			err_DIRECTX_API (m_modeString, error, L"D3D_OK - No error occurred");
			break;

		case D3DERR_CONFLICTINGRENDERSTATE:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_CONFLICTINGRENDERSTATE - Currently set render states can't be used together");
			break;

		case D3DERR_CONFLICTINGTEXTUREFILTER:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_CONFLICTINGTEXTUREFILTER - Current texture filters can't be used together");
			break;

		case D3DERR_CONFLICTINGTEXTUREPALETTE:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_CONFLICTINGTEXTUREPALETTE - Current textures can't be used simultaneously");
			break;

		case D3DERR_DEVICELOST:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_DEVICELOST - Device is lost and can't be restored, so rendering is not possible");
			break;

		case D3DERR_DEVICENOTRESET:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_DEVICENOTRESET - Device cannot be reset");
			break;

		case D3DERR_DRIVERINTERNALERROR:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_DRIVERINTERNALERROR - Internal driver error");
			break;

		case D3DERR_INVALIDCALL:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_INVALIDCALL - The method call is invalid. For example, a method's parameter may have an invalid value");
			break;

		case D3DERR_INVALIDDEVICE:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_INVALIDDEVICE - The requested device type is not valid");
			break;

		case D3DERR_MOREDATA:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_MOREDATA - There is more data available than the specified buffer size can hold");
			break;

		case D3DERR_NOTAVAILABLE:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_NOTAVAILABLE - This device does not support the queried technique");
			break;

		case D3DERR_NOTFOUND:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_NOTFOUND - The requested item was not found");
			break;

		case D3DERR_OUTOFVIDEOMEMORY:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_OUTOFVIDEOMEMORY - Direct3D does not have enough display memory to perform the operation");
			break;

		case D3DERR_TOOMANYOPERATIONS:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_TOOMANYOPERATIONS - Application is requesting more texture-filtering operations than the device supports");
			break;

		case D3DERR_UNSUPPORTEDALPHAARG:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_UNSUPPORTEDALPHAARG - Device does not support a specified texture-blending argument for the alpha channel");
			break;

		case D3DERR_UNSUPPORTEDALPHAOPERATION:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_UNSUPPORTEDALPHAOPERATION - Device does not support a specified texture-blending operation for the alpha channel");
			break;

		case D3DERR_UNSUPPORTEDCOLORARG:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_UNSUPPORTEDCOLORARG - Device does not support a specified texture-blending argument for color values");
			break;

		case D3DERR_UNSUPPORTEDCOLOROPERATION:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_UNSUPPORTEDCOLOROPERATION - Device does not support a specified texture-blending operation for color values");
			break;

		case D3DERR_UNSUPPORTEDFACTORVALUE:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_UNSUPPORTEDFACTORVALUE - Device does not support the specified texture factor value");
			break;

		case D3DERR_UNSUPPORTEDTEXTUREFILTER:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_UNSUPPORTEDTEXTUREFILTER - Device does not support the specified texture filter");
			break;

		case D3DERR_WRONGTEXTUREFORMAT:
			err_DIRECTX_API (m_modeString, error, L"D3DERR_WRONGTEXTUREFORMAT - Pixel format of the texture surface is not valid");
			break;

		case E_FAIL:
			err_DIRECTX_API (m_modeString, error, L"E_FAIL - An undetermined error occurred inside the Direct3D subsystem");
			break;

		case E_INVALIDARG:
			err_DIRECTX_API (m_modeString, error, L"E_INVALIDARG - An invalid parameter was passed to the returning function");
			break;

//		case E_INVALIDCALL:
//			err_DIRECTX_API (error, L"E_INVALIDCALL - The method call is invalid. For example, a method's parameter may have an invalid value.");
//			break;

		case E_OUTOFMEMORY:
			err_DIRECTX_API (m_modeString, error, L"E_OUTOFMEMORY - Direct3D could not allocate sufficient memory to complete the call");
			break;
	}

}


VOID CXModuleGrafx::MainCleanup()
{  //Called befor exiting under any condition.

	//Release all additional surfaces that may have been created.
	for(int i = 0; i < m_newSurfaces; i++)
	{
		SAFE_RELEASE(m_pD3Ds[i]);
	}

	SAFE_DELETE_ARRAY(m_pD3Ds);
	SAFE_DELETE_ARRAY(m_pTriangles);
	SAFE_DELETE_ARRAY(m_pTriangles2);
	SAFE_DELETE_ARRAY(m_pTriangles3);
	SAFE_RELEASE(m_pBackgroundVB);
	SAFE_RELEASE( m_pVB);
	SAFE_RELEASE( m_pVB2);
	SAFE_RELEASE( m_pVB3);
	SAFE_RELEASE( m_pBackBuffer);
	SAFE_RELEASE( m_pSpotLightTexture);
}

