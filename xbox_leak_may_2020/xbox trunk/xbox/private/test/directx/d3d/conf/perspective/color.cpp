//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "Perspective.h"

//************************************************************************
// Perspective Color Test functions

CColorTest::CColorTest()
{
	m_szTestName = TEXT("Perspective Color");
	m_szCommandKey = TEXT("Color");

    // Disable textures
    m_ModeOptions.fTextures = false;
}

CColorTest::~CColorTest()
{
}

UINT CColorTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Setup the Test range
	SetTestRange(1, MAX_GRID_TESTS + MAX_TRI_TESTS);

	// Make sure we are on at lease DX8
//	if (m_dwVersion <= 0x0700)
//	{
//		WriteToLog("This test requires at least Dx8.\n");
//		return D3DTESTINIT_SKIPALL;
//	}

    // Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the appropriate Raster cap
	if (!(dwRasterCaps & D3DPRASTERCAPS_COLORPERSPECTIVE))
	{
		WriteToLog("Device capability not supported: Raster ColorPerspective.\n");
		return D3DTESTINIT_SKIPALL;
    }

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return D3DTESTINIT_RUN;
}
