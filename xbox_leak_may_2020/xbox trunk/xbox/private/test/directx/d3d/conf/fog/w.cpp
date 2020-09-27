#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Fog.h"
//#include "cimageloader.h"

//************************************************************************
// WLinear Test functions

CWLinearTest::CWLinearTest()
{
	m_szTestName = TEXT("Linear W Fog");
	m_szCommandKey = TEXT("WLinear");

	// Tell parent class which Fog we are
	dwFog = W_LINEAR;

    // Disable textures
    m_bTextures = false;
}

CWLinearTest::~CWLinearTest()
{
}

bool CWLinearTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using W Fog Linear
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_LINEAR);
	return true;
}

bool CWLinearTest::SetDefaultMatrices(void)
{
	// Setup the projection matrix for Eye-Relative pixel fog
	D3DMATRIX Matrix = ProjectionMatrix(1.0f, 10.0f, pi/3);

	Matrix = MatrixMult(Matrix, 2.0f);

    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
}

UINT CWLinearTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;
	UINT Result = D3DTESTINIT_RUN;

	// Setup the Test range
	SetTestRange(1, W_LINEAR_TESTS);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the appropriate Fog caps
	if (!(dwRasterCaps & D3DPRASTERCAPS_FOGTABLE))
	{
		WriteToLog("Device capability not supported: Fog Table.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	if (!(dwRasterCaps & D3DPRASTERCAPS_WFOG))
	{
		WriteToLog("Device capability not supported: W Fog.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	// Initialize FogColor variable to Red
	FogColor = RGB_MAKE(255,0,0);

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return Result;
}

//************************************************************************
// WExp Test functions

CWExpTest::CWExpTest()
{
	m_szTestName = TEXT("Exp W Fog");
	m_szCommandKey = TEXT("WExp");

	// Tell parent class which Fog we are
	dwFog = W_EXP;

    // Disable textures
    m_bTextures = false;
}

CWExpTest::~CWExpTest()
{
}

bool CWExpTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using W Fog Exp
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_EXP);
	return true;
}

bool CWExpTest::SetDefaultMatrices(void)
{
	// Setup the projection matrix for Eye-Relative pixel fog
	D3DMATRIX Matrix = ProjectionMatrix(1.0f, 10.0f, pi/3);

	Matrix = MatrixMult(Matrix, 2.0f);

    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
}

UINT CWExpTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;
	UINT Result = D3DTESTINIT_RUN;

	// Setup the Test range
	SetTestRange(1, W_EXP_TESTS);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the appropriate Fog caps
	if (!(dwRasterCaps & D3DPRASTERCAPS_FOGTABLE))
	{
		WriteToLog("Device capability not supported: Fog Table.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	if (!(dwRasterCaps & D3DPRASTERCAPS_WFOG))
	{
		WriteToLog("Device capability not supported: W Fog.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	// Initialize FogColor variable to Red
	FogColor = RGB_MAKE(255,0,0);

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return Result;
}

//************************************************************************
// WExp2 Test functions

CWExp2Test::CWExp2Test()
{
	m_szTestName = TEXT("Exp2 W Fog");
	m_szCommandKey = TEXT("WExp2");

	// Tell parent class which Fog we are
	dwFog = W_EXP2;

    // Disable textures
    m_bTextures = false;
}

CWExp2Test::~CWExp2Test()
{
}

bool CWExp2Test::SetDefaultRenderStates(void)
{
	// Make sure that we are using W Fog Exp2
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_EXP2);
	return true;
}

bool CWExp2Test::SetDefaultMatrices(void)
{
	// Setup the projection matrix for Eye-Relative pixel fog
	D3DMATRIX Matrix = ProjectionMatrix(1.0f, 10.0f, pi/3);

	Matrix = MatrixMult(Matrix, 2.0f);

    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
}

UINT CWExp2Test::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;
	UINT Result = D3DTESTINIT_RUN;

	// Setup the Test range
	SetTestRange(1, W_EXP2_TESTS);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the appropriate Fog caps
	if (!(dwRasterCaps & D3DPRASTERCAPS_FOGTABLE))
	{
		WriteToLog("Device capability not supported: Fog Table.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	if (!(dwRasterCaps & D3DPRASTERCAPS_WFOG))
	{
		WriteToLog("Device capability not supported: W Fog.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	// Initialize FogColor variable to Red
	FogColor = RGB_MAKE(255,0,0);

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return Result;
}

//************************************************************************
// Texture WLinear Test functions

CTextureWLinearTest::CTextureWLinearTest()
{
	m_szTestName = TEXT("Textured Linear W Fog");
	m_szCommandKey = TEXT("TextureWLinear");

	// Tell parent class which Fog we are
	dwFog = W_LINEAR;

    // Disable textures
    m_bTextures = true;
}

CTextureWLinearTest::~CTextureWLinearTest()
{
}

bool CTextureWLinearTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using W Fog Linear
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_LINEAR);
	SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD) D3DTBLEND_DECAL);
	return true;
}

bool CTextureWLinearTest::SetDefaultMatrices(void)
{
	// Setup the projection matrix for Eye-Relative pixel fog
	D3DMATRIX Matrix = ProjectionMatrix(1.0f, 10.0f, pi/3);

	Matrix = MatrixMult(Matrix, 2.0f);

    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
}

UINT CTextureWLinearTest::TestInitialize(void)
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
	SetTestRange(1, TEXTURE_W_TESTS * m_uCommonTextureFormats);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the appropriate Fog caps
	if (!(dwRasterCaps & D3DPRASTERCAPS_FOGTABLE))
	{
		WriteToLog("Device capability not supported: Fog Table.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	if (!(dwRasterCaps & D3DPRASTERCAPS_WFOG))
	{
		WriteToLog("Device capability not supported: W Fog.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	// Initialize FogColor variable to Red
	FogColor = RGB_MAKE(255,0,0);

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
// Texture WExp Test functions

CTextureWExpTest::CTextureWExpTest()
{
	m_szTestName = TEXT("Textured Exp W Fog");
	m_szCommandKey = TEXT("TextureWExp");

	// Tell parent class which Fog we are
	dwFog = W_EXP;

    // Disable textures
    m_bTextures = true;
}

CTextureWExpTest::~CTextureWExpTest()
{
}

bool CTextureWExpTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using W Fog Exp
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_EXP);
	SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD) D3DTBLEND_DECAL);
	return true;
}

bool CTextureWExpTest::SetDefaultMatrices(void)
{
	// Setup the projection matrix for Eye-Relative pixel fog
	D3DMATRIX Matrix = ProjectionMatrix(1.0f, 10.0f, pi/3);

	Matrix = MatrixMult(Matrix, 2.0f);

    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
}

UINT CTextureWExpTest::TestInitialize(void)
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
	SetTestRange(1, TEXTURE_W_TESTS * m_uCommonTextureFormats);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the appropriate Fog caps
	if (!(dwRasterCaps & D3DPRASTERCAPS_FOGTABLE))
	{
		WriteToLog("Device capability not supported: Fog Table.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	if (!(dwRasterCaps & D3DPRASTERCAPS_WFOG))
	{
		WriteToLog("Device capability not supported: W Fog.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	// Initialize FogColor variable to Red
	FogColor = RGB_MAKE(255,0,0);

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
// Texture WExp2 Test functions

CTextureWExp2Test::CTextureWExp2Test()
{
	m_szTestName = TEXT("Textured Exp2 W Fog");
	m_szCommandKey = TEXT("TextureWExp2");

	// Tell parent class which Fog we are
	dwFog = W_EXP2;

    // Disable textures
    m_bTextures = true;
}

CTextureWExp2Test::~CTextureWExp2Test()
{
}

bool CTextureWExp2Test::SetDefaultRenderStates(void)
{
	// Make sure that we are using W Fog Exp2
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_EXP2);
	SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD) D3DTBLEND_DECAL);
	return true;
}

bool CTextureWExp2Test::SetDefaultMatrices(void)
{
	// Setup the projection matrix for Eye-Relative pixel fog
	D3DMATRIX Matrix = ProjectionMatrix(1.0f, 10.0f, pi/3);

	Matrix = MatrixMult(Matrix, 2.0f);

    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
}

UINT CTextureWExp2Test::TestInitialize(void)
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
	SetTestRange(1, TEXTURE_W_TESTS * m_uCommonTextureFormats);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the appropriate Fog caps
	if (!(dwRasterCaps & D3DPRASTERCAPS_FOGTABLE))
	{
		WriteToLog("Device capability not supported: Fog Table.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	if (!(dwRasterCaps & D3DPRASTERCAPS_WFOG))
	{
		WriteToLog("Device capability not supported: W Fog.\n");
		Result = D3DTESTINIT_SKIPALL;
	}

	// Initialize FogColor variable to Red
	FogColor = RGB_MAKE(255,0,0);

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
// Internal API:    WTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CFogTest::WTests(void)
{
	// Thin Triangle Strip on the top
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  16.5f, 56.5f + (float)m_vpTest.Y, 0.000f), 0.999f, RGBA_MAKE(0,255,0,255),0, 0.0f, 0.125f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  16.5f, 28.5f + (float)m_vpTest.Y, 0.000f), 0.999f, RGBA_MAKE(0,255,0,255),0, 0.0f, 0.000f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 304.5f, 56.5f + (float)m_vpTest.Y, 0.999f), 0.100f, RGBA_MAKE(0,255,0,255),0, 1.0f, 0.125f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 304.5f, 28.5f + (float)m_vpTest.Y, 0.999f), 0.100f, RGBA_MAKE(0,255,0,255),0, 1.0f, 0.000f);

	// Thin Triangle Strip on the bottom
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  16.5f, 252.5f + (float)m_vpTest.Y, 0.999f), 0.100f, RGBA_MAKE(0,255,0,255),0, 0.0f, 1.000f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  16.5f, 224.5f + (float)m_vpTest.Y, 0.999f), 0.100f, RGBA_MAKE(0,255,0,255),0, 0.0f, 0.875f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 304.5f, 252.5f + (float)m_vpTest.Y, 0.000f), 0.999f, RGBA_MAKE(0,255,0,255),0, 1.0f, 1.000f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 304.5f, 224.5f + (float)m_vpTest.Y, 0.000f), 0.999f, RGBA_MAKE(0,255,0,255),0, 1.0f, 0.875f);

	// Four white Triangles in the middle
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  16.5f, 210.5f + (float)m_vpTest.Y, 0.2f), (1.0f - 0.2f), RGBA_MAKE(255,255,255,255),0, 0.0000f, 0.8125f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  64.5f,  70.5f + (float)m_vpTest.Y, 0.2f), (1.0f - 0.2f), RGBA_MAKE(255,255,255,255),0, 0.1667f, 0.1875f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 112.5f, 210.5f + (float)m_vpTest.Y, 0.2f), (1.0f - 0.2f), RGBA_MAKE(255,255,255,255),0, 0.3333f, 0.8125f);

	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 128.5f, 210.5f + (float)m_vpTest.Y, 0.4f), (1.0f - 0.4f), RGBA_MAKE(255,255,255,255),0, 0.3889f, 0.8125f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  80.5f,  70.5f + (float)m_vpTest.Y, 0.4f), (1.0f - 0.4f), RGBA_MAKE(255,255,255,255),0, 0.2222f, 0.1875f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 176.5f,  70.5f + (float)m_vpTest.Y, 0.4f), (1.0f - 0.4f), RGBA_MAKE(255,255,255,255),0, 0.5556f, 0.1875f);

	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 144.5f, 210.5f + (float)m_vpTest.Y, 0.6f), (1.0f - 0.6f), RGBA_MAKE(255,255,255,255),0, 0.4444f, 0.8125f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 192.5f,  70.5f + (float)m_vpTest.Y, 0.6f), (1.0f - 0.6f), RGBA_MAKE(255,255,255,255),0, 0.6111f, 0.1875f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 240.5f, 210.5f + (float)m_vpTest.Y, 0.6f), (1.0f - 0.6f), RGBA_MAKE(255,255,255,255),0, 0.7778f, 0.8125f);

	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 256.5f, 210.5f + (float)m_vpTest.Y, 0.8f), (1.0f - 0.8f), RGBA_MAKE(255,255,255,255),0, 0.8333f, 0.8125f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 208.5f,  70.5f + (float)m_vpTest.Y, 0.8f), (1.0f - 0.8f), RGBA_MAKE(255,255,255,255),0, 0.6667f, 0.1875f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 304.5f,  70.5f + (float)m_vpTest.Y, 0.8f), (1.0f - 0.8f), RGBA_MAKE(255,255,255,255),0, 1.0000f, 0.1875f);
}
