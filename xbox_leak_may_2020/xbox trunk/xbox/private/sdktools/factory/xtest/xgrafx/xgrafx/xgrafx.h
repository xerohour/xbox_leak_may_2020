//-----------------------------------------------------------------------------
// (c) Copyright 2000-2000 Intel Corp. All rights reserved.
//
// xgrafx.h - Graphics testing module for xmta.
//
//-----------------------------------------------------------------------------
#ifndef _XGRAFX_H_
#define _XGRAFX_H_

#include "..\..\parameter.h"
#include "XBUtil.h"
#include <xgraphics.h>

//Disable futile warnings...
#pragma warning (disable:4238)//non-standard extensions
#pragma warning (disable:4245)//type conversion for GetBackBuffer() with -1 as argument.

//Debug levels for message output.
#define DEBUG_LEVEL1 0x1		//High level, few messages
#define DEBUG_LEVEL2 0x2		//Moderate level, more messages
#define DEBUG_LEVEL3 0x4		//Low Level, very detailed, lots of messages
#define MAX_SURFACES 49			//Max number of surafces that can safely be created.
#define DEFAULT_CRC  0x8a2d91f5	//Default starting CRC value.

#define MAX_VIDMODES     9		//Max number of video modes to be tested.


#ifndef PASS
#define PASS 0
#endif
#ifndef FAIL
#define FAIL 0xFFFFFFFF
#endif


// A structure for our custom vertex type
struct SHAPES_TEST_CUSTOMVERTEX
{
    FLOAT x, y, z;      // The untransformed, 3D position for the vertex
    DWORD color;        // The vertex color
};

//Struct for the projected vertex.
struct PROJTEXVERTEX
{
    D3DXVECTOR3 p;
    D3DCOLOR    color;
};

// projection frustrum
struct LINEVERTEX
{
    FLOAT x, y, z;
    DWORD color;
};


extern "C"
{
void __cdecl D3DTest_ChecksumCallback(DWORD Frame);

void __stdcall D3DTest_GetScreenChecksum( DWORD *pRed, DWORD *pGreen, DWORD *pBlue);

}



class CXModuleGrafx : public CTestObj
{
public:
	DECLARE_XMTAMODULE (CXModuleGrafx, "grafx", MODNUM_GRAFX);


protected:	
    //-----------------------------------
	//Parameters used by all grafx tests.
	//-----------------------------------
	D3DPRESENT_PARAMETERS	m_d3dpp;
	DWORD					m_dwDisplayWidth;
    DWORD					m_dwDisplayHeight;
    D3DFORMAT				m_DepthBufferFormat; // Format used to create depthbuffer
    D3DFORMAT				m_BackBufferFormat;  // Format used to create depthbuffer
	int						m_vidMode;
	int						m_bpp;
	DWORD					m_expectedCRC;
	int						m_testScenes;
	TCHAR					m_debugMsg[128]; //Used to output debug message strings
	TCHAR					m_errorMsg[128]; 
	static int				m_surfaceTestCount;
	static int				m_vidmemTestCount;
	static int				m_shapesTestCount;
	BOOL					m_hardwareCRC;
	TCHAR					m_modeString[64];


	virtual bool InitializeParameters();
//	BOOL	InitializeDisplay(int mode);
	DWORD   UpdateImageRectCRC(LONG height, BYTE *pBits, LONG pitch, int bytesPerLine);
	DWORD	GetDisplayBufferCRC();
	VOID	ReportD3DError(DWORD error);
	VOID	MainCleanup();

	
	//-------------------------------------
	//Paremeters used by the 3DSurface Test
	//-------------------------------------
	WORD                m_wIndex[32];					//Triangle light index data
	LPDIRECT3DTEXTURE8  m_pSpotLightTexture;			//procedurally generated spotlight texture
	D3DXMATRIX          m_matTexProj;					//texture projection matrix
	D3DXVECTOR3         m_vTexEyePt;					//texture eye pt.
	D3DXVECTOR3         m_vTexLookatPt;					//texture lookat pt.
	LPDIRECT3DVERTEXBUFFER8 m_pBackgroundVB;	
	PROJTEXVERTEX       m_vCube[36];
	LINEVERTEX			m_lineVertex[8];

	BOOL	SurfTestInitDisplayObjects();
	BOOL	SurfTestFrameMove(D3DXMATRIX* pMatWorld);
    VOID	SurfTestRender(int scene);
	DWORD   SurfTestCalcCRC(DWORD crc);
	BOOL	CreateSpotLightTexture(BOOL bBorder);
	VOID	ShowTexture( LPDIRECT3DTEXTURE8 pTexture );
	VOID	CalculateTexProjMatrix();
	HRESULT DisplayGradientBackground(int scene);

	//-------------------------------------
	//Paremeters used by the Vidmemory Test
	//-------------------------------------
	//LPDIRECT3DSURFACE8  m_pD3DsVidMemory;
	LPDIRECT3DSURFACE8	m_pBackBuffer;  
	LPDIRECT3DSURFACE8*	m_pD3Ds;		//Surfaces to be used to copy to.
	int					m_newSurfaces;
	DWORD				m_mbytesTested;
	
    BOOL	VidmemRender(int mode);
	DWORD   VidmemCalcCRC();
	DWORD   VidmemCalcCRC(LPDIRECT3DSURFACE8 lpd3dsurface);
	int		VidmemInitSurfaces();
	BOOL	CopySurfaces(int numSurfaces);
	BOOL	FillRand(DWORD seed);


	//---------------------------------------------
	//Paremeters used by the Vidmemory Pattern Test
	//---------------------------------------------
	BYTE	m_vidmemPattern;//Bit pattern used to fill surface.
	
	BOOL	VidmemPatternRender();
	DWORD   VidmemPatternCalcCRC();
	DWORD   VidmemPatternCalcCRC(LPDIRECT3DSURFACE8 lpd3dsurface);
	int		VidmemPatternInitSurfaces();
	BOOL	VidmemPatternCopySurfaces(int numSurfaces);
	BOOL	FillSurfaceWithPattern(BYTE pattern);
	void	FillBuffer(unsigned long *start_address, int count, unsigned char pattern);
	void	FillBuffer(unsigned char *start_address, int count, unsigned char pattern);


	//-------------------------------------
	//Paremeters used by the Shapes Test
	//-------------------------------------
	LPDIRECT3DVERTEXBUFFER8		m_pVB; // Buffer to hold triangle vertices
	LPDIRECT3DVERTEXBUFFER8		m_pVB2; // Buffer to hold triangle vertices
	LPDIRECT3DVERTEXBUFFER8		m_pVB3; // Buffer to hold triangle vertices
	SHAPES_TEST_CUSTOMVERTEX*	m_pTriangles;
	SHAPES_TEST_CUSTOMVERTEX*	m_pTriangles2;
	SHAPES_TEST_CUSTOMVERTEX*	m_pTriangles3;
	int							m_numTriangles;
	
	BOOL	ShapeTestInitD3D();
	VOID	ShapeTestRender(float angle);
	DWORD	ShapeTestCalcCRC(DWORD crc);
	BOOL	ShapeTestInitGeometry();
	VOID	ShapeTestSetupMatrices(float angle);
	VOID	ShapeTestInitTriangles();
	
	//-------------------------------------
	//Paremeters used by the HDTV Test
	//-------------------------------------
	LPDIRECT3DVERTEXBUFFER8 m_pHDTVPatternVB; // Buffer to hold vertices
	LPCTSTR m_HDTVTestPattern;
	LPCTSTR m_HDTVmode;
	DWORD	m_dwTolerance;
	int		m_testRetries;
	int		m_secondsToDisplay;
	
	WORD	m_wPixelThreshold, m_wThresholdRangeStep, m_wHSyncThreshold, m_wVSyncThreshold;
	DWORD	m_dwExpectedGreenCount, m_dwExpectedRedCount, m_dwExpectedBlueCount;
	int		m_PixelCountPattern, m_PatternSections;
	BOOL	m_bBitmapFromFile;
	TCHAR	m_BitmapFileName[64];
	
	BYTE	m_VertFrontPorch1, m_VertBackPorch1, m_VertFrontPorch2, m_VertBackPorch2;
	WORD	m_wVertSyncLow, m_wVertSyncHigh;
	WORD	m_wHorizBackPorch, m_wHorizFrontPorch;
	WORD	m_wHorizSyncLow, m_wHorizSyncHigh;
	
	
	//DAC parameters
	WORD	m_wStartLine, m_wNumLines, m_wNumSamples;
	BYTE	m_hDelay;
	BOOL	m_bInterlaced;
	WORD	m_GY_LowerThreshold, m_BPb_LowerThreshold, m_RPr_LowerThreshold;
	WORD	m_GY_UpperThreshold, m_BPb_UpperThreshold, m_RPr_UpperThreshold;
	WORD	m_wLinearTolerance;
	int		m_totalDacLines;
	struct COLOR_SAMPLE *pColorData;

	//TriLevel Sync parameters
	int		m_syncMode;
	LPCTSTR m_syncType;

	
	BOOL	HDTVTestInitPixelCountPattern(int pattern);
	VOID	HDTVTestGetBitmapFromHost();
	BOOL	HDTVTestPixelPattern01();
	BOOL	HDTVTestPixelPattern02();
	BOOL	HDTVTestPixelPattern03();
	BOOL	HDTVTestPixelPattern04();
	BOOL	HDTVTestPixelPattern05();
	BOOL	HDTVTestPixelPattern06();
	BOOL	HDTVTestPixelPattern07();
	BOOL	HDTVTestPixelPattern08();
	BOOL	HDTVTestPixelPattern09();
	BOOL	HDTVTestPixelPattern10();
	BOOL	HDTVTestPixelPattern11();
	BOOL	HDTVTestPixelPattern12();
	BOOL	HDTVTestPixelPattern13();
	BOOL	HDTVTestPixelPattern14();
	VOID	HDTVTestPixelCountRender();
	BOOL	HDTVTestGetPixelcounts(struct GET_ANALOG_PIXEL_COUNT_DATA *pReturnedData);
	BOOL    HDTVTestCheckPixelcountData();
	BOOL	HDTVTestInitDacPattern();
	VOID	HDTVTestAnalogColorsRender();
	VOID	HDTVTestBackgroundRender(int r, int g, int b);
	BOOL	HDTVTestGetTimings(struct GET_TIMINGS_DATA *pReturnedData);
	BOOL	HDTVTestCheckTimingsData();
	BOOL	HDTVTestCheckMeasuredTimings(bool reportError, struct GET_TIMINGS_DATA *data);
	BOOL	HDTVTestCheckTimingResult(bool reportError, WORD val, WORD ref, DWORD tol, TCHAR *msg);
	BOOL	HDTVTestGetAnalogColors();
	BOOL	HDTVTestCheckAnalogColorsData();
	BOOL	HDTVTestGetTrilevelSync();
	BOOL	HDTVTestCheckTrilevelSyncData();

	//---------------------------------------
	// Error messages declared here
	//(Note: 0x000 - 0x00f reserved for XMTA)
	//---------------------------------------
	bool err_BADPARAMETER (LPCTSTR mode, LPCTSTR s1)
	{
		ReportError (0x090, L"(%s) Failed to find or invalid value for configuration parameter \"%s\"\nThis configuration parameter doesn't exist or is invalid", mode, s1); return (CheckAbort (HERE));
	}
	bool err_NODIRECT3D (LPCTSTR mode)
	{
		ReportError (0x091, L"(%s)Failed to create the Direct3D Device - check for proper DirectX runtime installation.", mode); return (CheckAbort (HERE));
	}
	bool err_NOCREATEWINDOW (LPCTSTR mode)
	{
		ReportError (0x092, L"(%s)Failed to create the primary window to render to."); return (CheckAbort (HERE));
	}
	bool err_BADVIDEOMODE (int mode)
	{
		ReportError (0x093, L"The videomode selected (mode=%d) does not exist.\n Check the 'videomode' parameter in the ini file.", mode); return (CheckAbort (HERE));
	}
	bool err_BADCRC (LPCTSTR mode, DWORD dwCalc, DWORD dwExpect, LPCTSTR s1)
	{
		ReportError (0x094, L"(%s)The CRC calculated (0x%x) did not match the expected value (0x%x) - %s", mode, dwCalc, dwExpect, s1); return (CheckAbort (HERE));
	}
	bool err_NOCREATESURFACE (LPCTSTR mode, LPCTSTR s1)
	{
		ReportError (0x095, L"(%s)Failed to create a D3D Surface - (%s)", mode, s1); return (CheckAbort (HERE));
	}
	bool err_HostCommunicationError (LPCTSTR mode, int i1)
	{
		ReportError (0x096, L"(%s)iSendHost communication routine returned an error code of 0x%x", mode, i1); return (CheckAbort (HERE)); 
	}
	bool err_HostResponseBufferIsNull (LPCTSTR mode)
	{
		ReportError (0x097, L"(%s)The response buffer received from the host command is empty", mode); return (CheckAbort (HERE)); 
	}
	bool err_BufferContentsMismatch (LPCTSTR s1, LPCTSTR s2)
	{
		ReportError (0x098, L"Echo mismatch!  Sent string does not match response string.\nSent string: %s\nResponse string: %s", s1, s2); return (CheckAbort (HERE)); 
	}
	bool err_HDVMUBadPixelCount (LPCTSTR mode, LPCTSTR s1)
	{
		ReportError (0x099, L"(%s)HDVMU Pixelcount Error - %s", mode, s1); return (CheckAbort (HERE)); 
	}
	bool err_HDVMUBadTiming (LPCTSTR mode, LPCTSTR s1)
	{
		ReportError (0x09A, L"(%s)HDVMU Timings Error - %s", mode, s1); return (CheckAbort (HERE));
	}
	bool err_HDVMUBadDAC (LPCTSTR mode, LPCTSTR s1)
	{
		ReportError (0x09B, L"(%s)HDVMU DAC Error - %s", mode, s1); return (CheckAbort (HERE));
	}
	bool err_HDVMUBadTrilevelSync (LPCTSTR mode, LPCTSTR s1)
	{
		ReportError (0x09C, L"(%s)HDVMU TrilevelSync Error - %s", mode, s1); return (CheckAbort (HERE));
	}
	bool err_HostResponseError (LPCTSTR mode, DWORD dwErrorCodeFromHost, LPCTSTR s1)
	{
		ReportError ((unsigned short)dwErrorCodeFromHost, L"(%s)The host responded with the following error message:\n%s", mode, s1); return (CheckAbort (HERE)); 
	}
	//Default message for general DirectX API error.
	bool err_DIRECTX_API (LPCTSTR mode, HRESULT hr, LPCTSTR s1) //Default DirectX API error message
	{
		ReportError (0x09F, L"(%s)A DirectX API returned error 0x%08lx (%s).", mode, hr, s1 ); return (CheckAbort (HERE));
	}
	

};

#endif //_XGRAFX_H
