//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Fog.h"
//#include "cimageloader.h"

//************************************************************************
// TableLinear Test functions

CTableLinearTest::CTableLinearTest()
{
	m_szTestName = TEXT("Linear Table Fog");
	m_szCommandKey = TEXT("TableLinear");

	// Tell parent class which Fog we are
	dwFog = TABLE_LINEAR;

    // Disable textures
    m_bTextures = false;
}

CTableLinearTest::~CTableLinearTest()
{
}

bool CTableLinearTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using Table Fog Linear
	SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_LINEAR);
	return true;
}

bool CTableLinearTest::SetDefaultMatrices(void)
{
	// Setup the projection matrix for Z-based pixel fog
    D3DMATRIX Matrix = IdentityMatrix();

    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
}

UINT CTableLinearTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;
	UINT Result = D3DTESTINIT_RUN;

	// Setup the Test range
	SetTestRange(1, TABLE_LINEAR_TESTS);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the appropriate Fog caps
	if (!(dwRasterCaps & D3DPRASTERCAPS_FOGTABLE))
	{
		WriteToLog("Device capability not supported: Fog Table.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	if (m_dwVersion >= 0x0700)
	{
		if (!(dwRasterCaps & D3DPRASTERCAPS_ZFOG))
		{
			WriteToLog("Device capability not supported: Z Fog.\n");
			Result = D3DTESTINIT_SKIPALL;
		}
	}

	// Initialize FogColor variable to Blue
	FogColor = RGB_MAKE(0,0,255);

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return Result;
}

//************************************************************************
// TableExp Test functions

CTableExpTest::CTableExpTest()
{
	m_szTestName = TEXT("Exp Table Fog");
	m_szCommandKey = TEXT("TableExp");

	// Tell parent class which Fog we are
	dwFog = TABLE_EXP;

    // Disable textures
    m_bTextures = false;
}

CTableExpTest::~CTableExpTest()
{
}

bool CTableExpTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using Table Fog Exp
	SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_EXP);
	return true;
}

bool CTableExpTest::SetDefaultMatrices(void)
{
	// Setup the projection matrix for Z-based pixel fog
    D3DMATRIX Matrix = IdentityMatrix();

    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
}

UINT CTableExpTest::TestInitialize(void)
{
//	UINT n = m_pMode->nSrcDevice;
	UINT Result = D3DTESTINIT_RUN;

    // Setup the Test range
	SetTestRange(1, TABLE_EXP_TESTS);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the appropriate Fog caps
	if (!(dwRasterCaps & D3DPRASTERCAPS_FOGTABLE))
	{
		WriteToLog("Device capability not supported: Fog Table.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	if (m_dwVersion >= 0x0700)
	{
		if (!(dwRasterCaps & D3DPRASTERCAPS_ZFOG))
		{
			WriteToLog("Device capability not supported: Z Fog.\n");
			Result = D3DTESTINIT_SKIPALL;
		}
	}

	// Initialize FogColor variable to Blue
	FogColor = RGB_MAKE(0,0,255);

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return Result;
}

//************************************************************************
// TableExp2 Test functions

CTableExp2Test::CTableExp2Test()
{
	m_szTestName = TEXT("Exp2 Table Fog");
	m_szCommandKey = TEXT("TableExp2");

	// Tell parent class which Fog we are
	dwFog = TABLE_EXP2;

    // Disable textures
    m_bTextures = false;
}

CTableExp2Test::~CTableExp2Test()
{
}

bool CTableExp2Test::SetDefaultRenderStates(void)
{
	// Make sure that we are using Table Fog Exp2
	SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_EXP2);
	return true;
}

bool CTableExp2Test::SetDefaultMatrices(void)
{
	// Setup the projection matrix for Z-based pixel fog
    D3DMATRIX Matrix = IdentityMatrix();

    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
}

UINT CTableExp2Test::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;
	UINT Result = D3DTESTINIT_RUN;

	// Setup the Test range
	SetTestRange(1, TABLE_EXP2_TESTS);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the appropriate Fog caps
	if (!(dwRasterCaps & D3DPRASTERCAPS_FOGTABLE))
	{
		WriteToLog("Device capability not supported: Fog Table.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	if (m_dwVersion >= 0x0700)
	{
		if (!(dwRasterCaps & D3DPRASTERCAPS_ZFOG))
		{
			WriteToLog("Device capability not supported: Z Fog.\n");
			Result = D3DTESTINIT_SKIPALL;
		}
	}

	// Initialize FogColor variable to Blue
	FogColor = RGB_MAKE(0,0,255);

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return Result;
}

//************************************************************************
// Texture TableLinear Test functions

CTextureTableLinearTest::CTextureTableLinearTest()
{
	m_szTestName = TEXT("Textured Linear Table Fog");
	m_szCommandKey = TEXT("TextureTableLinear");

	// Tell parent class which Fog we are
	dwFog = TABLE_LINEAR;

    // Disable textures
    m_bTextures = true;
}

CTextureTableLinearTest::~CTextureTableLinearTest()
{
}

bool CTextureTableLinearTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using Table Fog Linear
	SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_LINEAR);
	SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD) D3DTBLEND_DECAL);
	return true;
}

bool CTextureTableLinearTest::SetDefaultMatrices(void)
{
	// Setup the projection matrix for Z-based pixel fog
    D3DMATRIX Matrix = IdentityMatrix();

    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
}

UINT CTextureTableLinearTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;
	UINT Result = D3DTESTINIT_RUN;

	// Request all texture formats except bumpmap
//	if (!RequestTextureFormats(PF_RGB | PF_FOURCC | PF_LUMINANCE))
//	{
//		WriteToLog("No texture formats found.\n");
//		return D3DTESTINIT_ABORT;
//	}

    // Setup the Test range
	SetTestRange(1, TEXTURE_TABLE_TESTS * m_uCommonTextureFormats);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the appropriate Fog caps
	if (!(dwRasterCaps & D3DPRASTERCAPS_FOGTABLE))
	{
		WriteToLog("Device capability not supported: Fog Table.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	if (m_dwVersion >= 0x0700)
	{
		if (!(dwRasterCaps & D3DPRASTERCAPS_ZFOG))
		{
			WriteToLog("Device capability not supported: Z Fog.\n");
			Result = D3DTESTINIT_SKIPALL;
		}
	}

	// Initialize FogColor variable to Blue
	FogColor = RGB_MAKE(0,0,255);

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

    return Result;
}

//************************************************************************
// Texture TableExp Test functions

CTextureTableExpTest::CTextureTableExpTest()
{
	m_szTestName = TEXT("Textured Exp Table Fog");
	m_szCommandKey = TEXT("TextureTableExp");

	// Tell parent class which Fog we are
	dwFog = TABLE_EXP;

    // Disable textures
    m_bTextures = true;
}

CTextureTableExpTest::~CTextureTableExpTest()
{
}

bool CTextureTableExpTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using Table Fog Exp
	SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_EXP);
	SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD) D3DTBLEND_DECAL);
	return true;
}

bool CTextureTableExpTest::SetDefaultMatrices(void)
{
	// Setup the projection matrix for Z-based pixel fog
    D3DMATRIX Matrix = IdentityMatrix();

    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
}

UINT CTextureTableExpTest::TestInitialize(void)
{
//	UINT n = m_pMode->nSrcDevice;
	UINT Result = D3DTESTINIT_RUN;

	// Request all texture formats except bumpmap
//	if (!RequestTextureFormats(PF_RGB | PF_FOURCC | PF_LUMINANCE))
//	{
//		WriteToLog("No texture formats found.\n");
//		return D3DTESTINIT_ABORT;
//	}

    // Setup the Test range
	SetTestRange(1, TEXTURE_TABLE_TESTS * m_uCommonTextureFormats);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the appropriate Fog caps
	if (!(dwRasterCaps & D3DPRASTERCAPS_FOGTABLE))
	{
		WriteToLog("Device capability not supported: Fog Table.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	if (m_dwVersion >= 0x0700)
	{
		if (!(dwRasterCaps & D3DPRASTERCAPS_ZFOG))
		{
			WriteToLog("Device capability not supported: Z Fog.\n");
			Result = D3DTESTINIT_SKIPALL;
		}
	}

	// Initialize FogColor variable to Blue
	FogColor = RGB_MAKE(0,0,255);

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

    return Result;
}

//************************************************************************
// Texture TableExp2 Test functions

CTextureTableExp2Test::CTextureTableExp2Test()
{
	m_szTestName = TEXT("Textured Exp2 Table Fog");
	m_szCommandKey = TEXT("TextureTableExp2");

	// Tell parent class which Fog we are
	dwFog = TABLE_EXP2;

    // Disable textures
    m_bTextures = true;
}

CTextureTableExp2Test::~CTextureTableExp2Test()
{
}

bool CTextureTableExp2Test::SetDefaultRenderStates(void)
{
	// Make sure that we are using Table Fog Exp2
	SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_EXP2);
	SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD) D3DTBLEND_DECAL);
	return true;
}

bool CTextureTableExp2Test::SetDefaultMatrices(void)
{
	// Setup the projection matrix for Z-based pixel fog
    D3DMATRIX Matrix = IdentityMatrix();

    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
}

UINT CTextureTableExp2Test::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;
	UINT Result = D3DTESTINIT_RUN;

	// Request all texture formats except bumpmap
//	if (!RequestTextureFormats(PF_RGB | PF_FOURCC | PF_LUMINANCE))
//	{
//		WriteToLog("No texture formats found.\n");
//		return D3DTESTINIT_ABORT;
//	}

    // Setup the Test range
	SetTestRange(1, TEXTURE_TABLE_TESTS * m_uCommonTextureFormats);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the appropriate Fog caps
	if (!(dwRasterCaps & D3DPRASTERCAPS_FOGTABLE))
	{
		WriteToLog("Device capability not supported: Fog Table.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	if (m_dwVersion >= 0x0700)
	{
		if (!(dwRasterCaps & D3DPRASTERCAPS_ZFOG))
		{
			WriteToLog("Device capability not supported: Z Fog.\n");
			Result = D3DTESTINIT_SKIPALL;
		}
	}

	// Initialize FogColor variable to Blue
	FogColor = RGB_MAKE(0,0,255);

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

	return Result;
}

//************************************************************************
// Internal API:    TableTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CFogTest::TableTests(void)
{
	// Thin Triangle Strip on the top
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  16.5f, 56.5f + (float)m_vpTest.Y, 0.0000f), 1.0f, RGBA_MAKE(255,0,0,255),0, 0.0f, 0.125f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  16.5f, 28.5f + (float)m_vpTest.Y, 0.0000f), 1.0f, RGBA_MAKE(255,0,0,255),0, 0.0f, 0.000f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 304.5f, 56.5f + (float)m_vpTest.Y, 0.9999f), 1.0f, RGBA_MAKE(255,0,0,255),0, 1.0f, 0.125f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 304.5f, 28.5f + (float)m_vpTest.Y, 0.9999f), 1.0f, RGBA_MAKE(255,0,0,255),0, 1.0f, 0.000f);

	// Thin Triangle Strip on the bottom
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  16.5f, 252.5f + (float)m_vpTest.Y, 0.9999f), 1.0f, RGBA_MAKE(255,0,0,255),0, 0.0f, 1.000f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  16.5f, 224.5f + (float)m_vpTest.Y, 0.9999f), 1.0f, RGBA_MAKE(255,0,0,255),0, 0.0f, 0.875f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 304.5f, 252.5f + (float)m_vpTest.Y, 0.0000f), 1.0f, RGBA_MAKE(255,0,0,255),0, 1.0f, 1.000f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 304.5f, 224.5f + (float)m_vpTest.Y, 0.0000f), 1.0f, RGBA_MAKE(255,0,0,255),0, 1.0f, 0.875f);

	// Four white Triangles in the middle
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  16.5f, 210.5f + (float)m_vpTest.Y, 0.2f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.0000f, 0.8125f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  64.5f,  70.5f + (float)m_vpTest.Y, 0.2f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.1667f, 0.1875f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 112.5f, 210.5f + (float)m_vpTest.Y, 0.2f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.3333f, 0.8125f);

	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 128.5f, 210.5f + (float)m_vpTest.Y, 0.4f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.3889f, 0.8125f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  80.5f,  70.5f + (float)m_vpTest.Y, 0.4f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.2222f, 0.1875f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 176.5f,  70.5f + (float)m_vpTest.Y, 0.4f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.5556f, 0.1875f);
                                                                                                                                                                                         
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 144.5f, 210.5f + (float)m_vpTest.Y, 0.6f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.4444f, 0.8125f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 192.5f,  70.5f + (float)m_vpTest.Y, 0.6f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.6111f, 0.1875f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 240.5f, 210.5f + (float)m_vpTest.Y, 0.6f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.7778f, 0.8125f);
                                                                                                                                                                                         
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 256.5f, 210.5f + (float)m_vpTest.Y, 0.8f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.8333f, 0.8125f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 208.5f,  70.5f + (float)m_vpTest.Y, 0.8f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.6667f, 0.1875f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 304.5f,  70.5f + (float)m_vpTest.Y, 0.8f), 1.0f, RGBA_MAKE(255,255,255,255),0, 1.0000f, 0.1875f);
}
