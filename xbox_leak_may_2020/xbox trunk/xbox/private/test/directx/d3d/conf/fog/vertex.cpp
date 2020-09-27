//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Fog.h"
//#include "cimageloader.h"

//************************************************************************
// Vertex Test functions

CVertexTest::CVertexTest()
{
	m_szTestName = TEXT("Vertex Fog");
	m_szCommandKey = TEXT("Vertex");

	// Tell parent class which Fog we are
	dwFog = VERTEX;

    // Disable textures
    m_bTextures = false;
}

CVertexTest::~CVertexTest()
{
}

bool CVertexTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using Vertex Fog
	SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_NONE);
	return true;
}

UINT CVertexTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

    // Setup the Test range
	SetTestRange(1, VERTEX_TESTS);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the appropriate Fog caps
	if (!(dwRasterCaps & D3DPRASTERCAPS_FOGVERTEX))
	{
		WriteToLog("Device capability not supported: Fog Vertex.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Initialize FogColor variable to Black
	FogColor = RGB_MAKE(0,0,0);

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

    return D3DTESTINIT_RUN;
}

//************************************************************************
// Texture Vertex Test functions

CTextureVertexTest::CTextureVertexTest()
{
	m_szTestName = TEXT("Textured Vertex Fog");
	m_szCommandKey = TEXT("TextureVertex");

	// Tell parent class which Fog we are
	dwFog = VERTEX;

    // Enable textures
    m_bTextures = true;
}

CTextureVertexTest::~CTextureVertexTest()
{
}

bool CTextureVertexTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using Vertex Fog
	SetRenderState(D3DRENDERSTATE_FOGENABLE, (DWORD) TRUE);
	SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, (DWORD) D3DFOG_NONE);
	SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD) D3DTBLEND_DECAL);
	return true;
}

UINT CTextureVertexTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Request all texture formats except bumpmap
//	if (!RequestTextureFormats(PF_RGB | PF_FOURCC | PF_LUMINANCE))
//	{
//		WriteToLog("No texture formats found.\n");
//		return D3DTESTINIT_ABORT;
//	}

    // Setup the Test range
	SetTestRange(1, TEXTURE_VERTEX_TESTS * m_uCommonTextureFormats);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the appropriate Fog caps
	if (!(dwRasterCaps & D3DPRASTERCAPS_FOGVERTEX))
	{
		WriteToLog("Device capability not supported: Fog Vertex.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Initialize FogColor variable to Black
	FogColor = RGB_MAKE(0,0,0);

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

//************************************************************************
// Internal API:    VertexTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CFogTest::VertexTests(int nTest)
{
	int			FogValue[] = { 255, 128, 0 };
	int			nFog;
	D3DCOLOR	Color;
	char		szBuffer[80];

	// Init the status buffer
	szBuffer[0] = 0;

	// Calculate the Fog Value
	nFog = nTest % 4;

	if (nTest <= 4)
	{
		// Black Fog
		FogColor = RGB_MAKE(0, 0, 0);

		// White Triangle
		Color = RGBA_MAKE(255,255,255,255);

		// Update the color status buffer
		sprintf(szBuffer,"Fog: Black, Tri: White");
		WriteStatus("$gColor Values$y",szBuffer);

		sprintf(msgString, "%s%s",msgString, szBuffer);
	}
	else if (nTest <= 8)
	{
		// Green Fog
		FogColor = RGB_MAKE(0, 255, 0);

		// White Triangle
		Color = RGBA_MAKE(255,255,255,255);

		// Update the color status buffer
		sprintf(szBuffer,"Fog: Green, Tri: White");
		WriteStatus("$gColor Values$y",szBuffer);

		sprintf(msgString, "%s%s",msgString, szBuffer);
	}
	else
	{
		// White Fog
		FogColor = RGB_MAKE(255, 255, 255);

		// Red Triangle
		if (nTest == 9)
		{
			Color = RGBA_MAKE(255,0,0,255);

			// Update the color status buffer
			sprintf(szBuffer,"Fog: White, Tri: Red");
			WriteStatus("$gColor Values$y",szBuffer);

			sprintf(msgString, "%s%s",msgString, szBuffer);
		}
		// Green Triangle
		else if (nTest == 10)
		{
			Color = RGBA_MAKE(0,255,0,255);

			// Update the color status buffer
			sprintf(szBuffer,"Fog: White, Tri: Green");
			WriteStatus("$gColor Values$y",szBuffer);

			sprintf(msgString, "%s%s",msgString, szBuffer);
		}
		// Blue Triangle
		else if (nTest == 11)
		{
			Color = RGBA_MAKE(0,0,255,255);

			// Update the color status buffer
			sprintf(szBuffer,"Fog: White, Tri: Blue");
			WriteStatus("$gColor Values$y",szBuffer);

			sprintf(msgString, "%s%s",msgString, szBuffer);
		}
	}

	if (nFog)
	{
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   0.5f,  5.5f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[nFog-1]), 0.0f,0.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 240.5f,  5.5f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[nFog-1]), 1.0f,0.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 240.5f,245.5f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[nFog-1]), 1.0f,1.0f);

		// Update the value status buffer
		sprintf(szBuffer,"Left: %d, Right: %d, Bottom: %d", FogValue[nFog-1], FogValue[nFog-1], FogValue[nFog-1]);
		WriteStatus("$gFog Values$y",szBuffer);

		sprintf(msgString, "%s(%d, %d, %d), ",msgString, FogValue[nFog-1], FogValue[nFog-1], FogValue[nFog-1]);
	}
	else
	{
		if (nTest == 12)
		{
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   0.5f,  5.5f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), RGBA_MAKE(255,0,0,255), RGBA_MAKE(0, 0, 0, FogValue[2]), 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 240.5f,  5.5f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), RGBA_MAKE(0,255,0,255), RGBA_MAKE(0, 0, 0, FogValue[1]), 1.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 240.5f,245.5f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), RGBA_MAKE(0,0,255,255), RGBA_MAKE(0, 0, 0, FogValue[0]), 1.0f,1.0f);

			// Update the color and value status buffer
			sprintf(szBuffer,"Fog: White, Triangle: Red,Green,Blue");
			WriteStatus("$gColor Values$y",szBuffer);

			sprintf(szBuffer,"Left: %d, Right: %d, Bottom: %d", FogValue[2], FogValue[1], FogValue[0]);
			WriteStatus("$gFog Values$y",szBuffer);

			sprintf(msgString, "%sFog: White, Tri: RGB(%d, %d, %d), ",msgString, FogValue[2], FogValue[1], FogValue[0]);
		}
		else
		{
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   0.5f,  5.5f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[2]), 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 240.5f,  5.5f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[1]), 1.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 240.5f,245.5f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[0]), 1.0f,1.0f);

			// Update the value status buffer
			sprintf(szBuffer,"Left: %d, Right: %d, Bottom: %d", FogValue[2], FogValue[1], FogValue[0]);
			WriteStatus("$gFog Values$y",szBuffer);

			sprintf(msgString, "%s(%d, %d, %d), ",msgString, FogValue[2], FogValue[1], FogValue[0]);
		}

	}
}

//************************************************************************
// Internal API:    TextureVertexTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CFogTest::TextureVertexTests(int nTest)
{
	int			FogValue[] = { 255, 128, 0 };
	int			nFog;
	D3DCOLOR	Color;
	char		szBuffer[80];

	// Init the status buffer
	szBuffer[0] = 0;

	// Calculate the Fog Value
	nFog = nTest % 4;

	// Green Fog
	FogColor = RGB_MAKE(0, 255, 0);

	// White Triangle
	Color = RGBA_MAKE(255,255,255,255);

	// Update the color status buffer
	sprintf(szBuffer,"Fog: Green, Tri: White");
	WriteStatus("$gColor Values$y",szBuffer);

	sprintf(msgString, "%s%s",msgString, szBuffer);

	if (nFog)
	{
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   0.5f,  5.5f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[nFog-1]), 0.0f,0.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 240.5f,  5.5f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[nFog-1]), 1.0f,0.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 240.5f,245.5f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[nFog-1]), 1.0f,1.0f);

		// Update the value status buffer
		sprintf(szBuffer,"Left: %d, Right: %d, Bottom: %d", FogValue[nFog-1], FogValue[nFog-1], FogValue[nFog-1]);
		WriteStatus("$gFog Values$y",szBuffer);

		sprintf(msgString, "%s(%d, %d, %d)",msgString, FogValue[nFog-1], FogValue[nFog-1], FogValue[nFog-1]);
	}
	else
	{
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   0.5f,  5.5f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[2]), 0.0f,0.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 240.5f,  5.5f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[1]), 1.0f,0.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 240.5f,245.5f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), Color, RGBA_MAKE(0, 0, 0, FogValue[0]), 1.0f,1.0f);

		// Update the value status buffer
		sprintf(szBuffer,"Left: %d, Right: %d, Bottom: %d", FogValue[2], FogValue[1], FogValue[0]);
		WriteStatus("$gFog Values$y",szBuffer);

		sprintf(msgString, "%s(%d, %d, %d)",msgString, FogValue[2], FogValue[1], FogValue[0]);
	}
}

