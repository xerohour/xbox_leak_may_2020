//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "Perspective.h"

//************************************************************************
// Perspective Texture Test functions

CTextureTest::CTextureTest()
{
	m_szTestName = TEXT("Perspective Texture");
	m_szCommandKey = TEXT("Texture");

    m_ModeOptions.fTextures = true;
}

CTextureTest::~CTextureTest()
{
}

UINT CTextureTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Setup the Test range
	SetTestRange(1, (MAX_GRID_TESTS * 4) + MAX_TRI_TESTS);

	// Check the caps we are interested in
	DWORD dwTextureCaps = m_d3dcaps.TextureCaps;

	// Check the appropriate Texture cap
	if (!(dwTextureCaps & D3DPTEXTURECAPS_PERSPECTIVE))
	{
		WriteToLog("Device capability not supported: Texture Perspective.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	// Setup the colors for the Stripes array
//	D3DCOLOR		Stripes[8];

	Stripes[0] = RGBA_MAKE(255,255,255,255);// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);	// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);	// Green
	Stripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);// Grey

	// Create the stripe image data
//    CImageLoader    Image;

//    pImage = Image.LoadStripes(64,64,8,Stripes,false); 

	return D3DTESTINIT_RUN;
}
