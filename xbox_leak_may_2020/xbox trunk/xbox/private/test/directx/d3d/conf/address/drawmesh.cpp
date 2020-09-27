//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "Address.h"

//************************************************************************
// Internal API:    DrawGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CAddressTest::DrawGrid(int nTest)
{
	int nRot, nTile, nGroup;
	float fZero, fOne;
	static DWORD dwDefault = 99;

	// Decide which group of tests we are in
	nGroup = nTest % 20;

	if (nGroup == 0)
		nGroup = 20;

	// Decide which rotation to use
	nRot = (nGroup-1) / 5;

	// Decide how much tiling to use
	nTile = nGroup - (nRot * 5);
 
	if (((nTest-1) / 20) == 0)
	{
		// Use (0,0) to (1,1) mapping
		fOne =  nTile * 1.0f;
		fZero = 0.0f;
	}
	else
	{
		// Use (-1,-1) to (1,1) mapping
		fOne =  nTile * 1.0f;
		fZero = nTile * -1.0f;
	}

	// Setup the mesh
	switch (nRot)
	{
		case (0): // Use ((1,0)(1,1)(0,0)(0,1)) mapping, White Mesh
			if (bBorder)
			{
				if (dwDefault == 99)
				{
					if (m_dwVersion >= 0x0700)
						GetTextureStageState(0, D3DTSS_BORDERCOLOR, NULL, &dwDefault, CD3D_REF);
//					else
//						m_pRefDevice3->GetRenderState(D3DRENDERSTATE_BORDERCOLOR, &dwDefault);
				}

				wsprintf(msgString, TEXT("%sDefault, "),msgString);
                m_pDevice->SetTextureStageState(0, D3DTSS_BORDERCOLOR, dwDefault);
//				SetRenderState(D3DRENDERSTATE_BORDERCOLOR, dwDefault);
			}

			if (fZero == 0.0f)
				wsprintf(msgString, TEXT("%sMap:(%d,0)(%d,%d)(0,0)(0,%d), "),msgString,nTile,nTile,nTile,nTile);
			else
				wsprintf(msgString, TEXT("%sMap:(%d,%d)(%d,%d)(%d,%d)(%d,%d), "),msgString,nTile,-nTile,nTile,nTile,-nTile,-nTile,-nTile,nTile);

			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			break;
		case (1): // Use standard ((0,0)(1,0)(0,1)(1,1)) mapping, White Mesh
			if (bBorder)
			{
				wsprintf(msgString, TEXT("%sBlue, "),msgString);
                m_pDevice->SetTextureStageState(0, D3DTSS_BORDERCOLOR, RGBA_MAKE(0,0,255,255));
//				SetRenderState(D3DRENDERSTATE_BORDERCOLOR, RGBA_MAKE(0,0,255,255));
			}

			if (fZero == 0.0f)
				wsprintf(msgString, TEXT("%sMap:(0,0)(%d,0)(0,%d)(%d,%d), "),msgString,nTile,nTile,nTile,nTile);
			else
				wsprintf(msgString, TEXT("%sMap:(%d,%d)(%d,%d)(%d,%d)(%d,%d), "),msgString,-nTile,-nTile,nTile,-nTile,-nTile,nTile,nTile,nTile);

			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			break;
		case (2): // Use ((0,1)(0,0)(1,1)(1,0)) mapping, White Mesh
			if (bBorder)
			{
				wsprintf(msgString, TEXT("%sRed, "),msgString);
                m_pDevice->SetTextureStageState(0, D3DTSS_BORDERCOLOR, RGBA_MAKE(255,0,0,255));
//				SetRenderState(D3DRENDERSTATE_BORDERCOLOR, RGBA_MAKE(255,0,0,255));
			}

			if (fZero == 0.0f)
				wsprintf(msgString, TEXT("%sMap:(0,%d)(0,0)(%d,%d)(%d,0), "),msgString,nTile,nTile,nTile,nTile);
			else
				wsprintf(msgString, TEXT("%sMap:(%d,%d)(%d,%d)(%d,%d)(%d,%d), "),msgString,-nTile,nTile,-nTile,-nTile,nTile,nTile,nTile,-nTile);

			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			break;
		case (3): // Use ((1,1)(0,1)(1,0)(0,0)) mapping, White Mesh
			if (bBorder)
			{
				wsprintf(msgString, TEXT("%sGreen, "),msgString);
                m_pDevice->SetTextureStageState(0, D3DTSS_BORDERCOLOR, RGBA_MAKE(0,255,0,255));
//				SetRenderState(D3DRENDERSTATE_BORDERCOLOR, RGBA_MAKE(0,255,0,255));
			}

			if (fZero == 0.0f)
				wsprintf(msgString, TEXT("%sMap:(%d,%d)(0,%d)(%d,0)(0,0), "),msgString,nTile,nTile,nTile,nTile);
			else
				wsprintf(msgString, TEXT("%sMap:(%d,%d)(%d,%d)(%d,%d)(%d,%d), "),msgString,nTile,nTile,-nTile,nTile,-nTile,nTile,-nTile,-nTile);

			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne,fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			break;
	}
}

//************************************************************************
// Internal API:    DrawWrapGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CAddressTest::DrawWrapGrid(int nTest)
{
	float fZero, fOne;

	// Use (0,0) to (1,1) mapping
	fOne =  0.9f;
	fZero = 0.1f;

	// Setup the mesh
	switch (nTest)
	{
		case (1): // Use standard ((0,0)(1,0)(0,1)(1,1)) mapping, White Mesh
			wsprintf(msgString, TEXT("%sMap:(0,0)(1,0)(0,1)(1,1), "),msgString);

			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			break;
		case (2): // Use ((0,1)(0,0)(1,1)(1,0)) mapping, White Mesh
			wsprintf(msgString, TEXT("%sMap:(0,1)(0,0)(1,1)(1,0), "),msgString);

			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			break;
		case (3): // Use ((1,1)(0,1)(1,0)(0,0)) mapping, White Mesh
			wsprintf(msgString, TEXT("%sMap:(1,1)(0,1)(1,0)(0,0), "),msgString);

			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			break;
		case (4): // Use ((1,0)(1,1)(0,0)(0,1)) mapping, White Mesh
			wsprintf(msgString, TEXT("%sMap:(1,0)(1,1)(0,0)(0,1), "),msgString);

			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(  9.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fZero);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X,   9.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fOne, fOne);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(265.5f + (float)m_vpTest.X, 265.5f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, fZero,fOne);
			break;
	}
}
