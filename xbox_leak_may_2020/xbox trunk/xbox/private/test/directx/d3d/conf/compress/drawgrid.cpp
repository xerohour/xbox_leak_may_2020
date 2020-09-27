//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Compress.h"

//************************************************************************
// Internal API:    DrawBasicGrid
//
// Purpose:         
//
// Return:          ERROR_SUCCESS if the vertices were setup properly
//************************************************************************

void CCompressTest::DrawBasicGrid(int nTest, int nTexFormats)
{
	char	szBuffer[80];
	int nScale, nMapping;
	float fZstart, fZend, fScale;
	bool bLogMap;

	// Initialize the interesting variables
	nMapping = nScale = 1;
	fScale = 1.0f * nScale;
	fZstart = fZend = 0.9f;
	bLogMap = false;

	switch (nTest)
	{
		case (1):	// Texture Mapping cases
		case (2):
		case (3):
		case (4):
			{
				nMapping = nTest;
				bLogMap = true;
			}
			break;
		case (5):	// Texture 2x2 Tiling case
			{
				nScale = 2;
				fScale = 1.0f * nScale;
			}
			break;
		case (6):	// Texture 4x4 Tiling case
			{
				nScale = 4;
				fScale = 1.0f * nScale;
			}
			break;
		default:	// Texture Perspective cases
			{
				fZstart = 1.0f - ((nTest-6) * 0.25f);

				if (!fZstart)
					fZstart = 0.1f;
			}
			break;
	}

	switch (nMapping)
	{
		case (1): // Use standard ((0,0)(1,0)(0,1)(1,1)) mapping, White Mesh
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f,   9.5f + (float)m_vpTest.Y, fZstart),(!fZstart ? 1.0f / 0.00001f : 1.0f / fZstart), RGBA_MAKE(255,255,255,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 265.5f, 265.5f + (float)m_vpTest.Y, fZend),  (1.0f / fZend), RGBA_MAKE(255,255,255,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f, 265.5f + (float)m_vpTest.Y, fZend),  (1.0f / fZend), RGBA_MAKE(255,255,255,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f,   9.5f + (float)m_vpTest.Y, fZstart),(!fZstart ? 1.0f / 0.00001f : 1.0f / fZstart), RGBA_MAKE(255,255,255,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 265.5f,   9.5f + (float)m_vpTest.Y, fZstart),(!fZstart ? 1.0f / 0.00001f : 1.0f / fZstart), RGBA_MAKE(255,255,255,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 265.5f, 265.5f + (float)m_vpTest.Y, fZend),  (1.0f / fZend), RGBA_MAKE(255,255,255,255),0, fScale,fScale);

			szBuffer[0] = 0;
			sprintf(szBuffer,"White Mesh, %dx%d Tiling, %.2ff Z top - %.2ff Z bottom",nScale,nScale,fZstart,fZend);
			sprintf(szStatus, "%s%s",szStatus, szBuffer);

			sprintf(msgString, "%sMesh: %dx%d White, Z: %.2ff - %.2ff",msgString,nScale,nScale,fZstart,fZend);
			break;
		case (2): // Use ((0,1)(0,0)(1,1)(1,0)) mapping, Red Mesh
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 265.5f, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 265.5f,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 265.5f, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255),0, fScale,  0.0f);

			szBuffer[0] = 0;
			sprintf(szBuffer,"Red Mesh, %dx%d Tiling, %.2ff Z top - %.2ff Z bottom",nScale,nScale,fZstart,fZend);
			sprintf(szStatus, "%s%s",szStatus, szBuffer);

			sprintf(msgString, "%sMesh: %dx%d Red, Z: %.2ff - %.2ff",msgString,nScale,nScale,fZstart,fZend);
			break;
		case (3): // Use ((1,1)(0,1)(1,0)(0,0)) mapping, Green Mesh
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 265.5f, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 265.5f,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 265.5f, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,255,0,255),0, 0.0f,    0.0f);

			szBuffer[0] = 0;
			sprintf(szBuffer,"Green Mesh, %dx%d Tiling, %.2ff Z top - %.2ff Z bottom",nScale,nScale,fZstart,fZend);
			sprintf(szStatus, "%s%s",szStatus, szBuffer);

			sprintf(msgString, "%sMesh: %dx%d Green, Z: %.2ff - %.2ff",msgString,nScale,nScale,fZstart,fZend);
			break;
		case (4): // Use ((1,0)(1,1)(0,0)(0,1)) mapping, Blue Mesh
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 265.5f, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, 0.0f,  fScale);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, 0.0f,    0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, fScale,  0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 265.5f,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, fScale,fScale);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 265.5f, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, 0.0f,  fScale);

			szBuffer[0] = 0;
			sprintf(szBuffer,"Blue Mesh, %dx%d Tiling, %.2ff Z top - %.2ff Z bottom",nScale,nScale,fZstart,fZend);
			sprintf(szStatus, "%s%s",szStatus, szBuffer);

			sprintf(msgString, "%sMesh: %dx%d Blue, Z: %.2ff - %.2ff",msgString,nScale,nScale,fZstart,fZend);
			break;
	}
}

//************************************************************************
// Internal API:    DrawBlendingGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CCompressTest::DrawBlendingGrid(int nAlpha)
{
	// Use standard ((0,0)(1,0)(0,1)(1,1)) mapping, White Mesh
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,nAlpha),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 265.5f, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255)   ,0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,nAlpha),0, 0.0f,1.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,nAlpha),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 265.5f,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255)   ,0, 1.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 265.5f, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255)   ,0, 1.0f,1.0f);

}
