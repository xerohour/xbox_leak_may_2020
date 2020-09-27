//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "AlphaBld.h"
#include "AlphaBld.h"

//************************************************************************
// Internal API:    DrawBackground
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CAlphaBldTest::DrawBackground(void)
{
	float			fWidth, fHeight, fX, fY;
	D3DCOLOR		Color[2] = {RGBA_MAKE(0,0,0,255),
								RGBA_MAKE(255,255,255,255)};
	DWORD			dwCount;
	bool			bWhite;
	int				i, j;

	dwCount = 0;
	bWhite = true;
	fWidth = (float)m_vpTest.Width / 10.0f;//m_pSrcTarget->m_dwWidth / 10.0f;
	fHeight = (float)m_vpTest.Height / 10.0f;//m_pSrcTarget->m_dwHeight / 10.0f;

	for (i=0; i < 10; i++)
	{
		fX = fWidth*i + (float)m_vpTest.X;
		bWhite = !bWhite;

		for (j=0; j < 10; j++)
		{
			fY = fHeight*j + (float)m_vpTest.Y;
			bWhite = !bWhite;

			BackgroundList[dwCount++] = cD3DTLVERTEX(cD3DVECTOR(fX+fWidth,fY,         0.9f), (1.0f / 0.9f), Color[bWhite],0, 0.0f,0.0f);
			BackgroundList[dwCount++] = cD3DTLVERTEX(cD3DVECTOR(fX+fWidth,fY+fHeight, 0.9f), (1.0f / 0.9f), Color[bWhite],0, 0.0f,0.0f);
			BackgroundList[dwCount++] = cD3DTLVERTEX(cD3DVECTOR(fX,       fY,         0.9f), (1.0f / 0.9f), Color[bWhite],0, 0.0f,0.0f);
			BackgroundList[dwCount++] = cD3DTLVERTEX(cD3DVECTOR(fX,       fY+fHeight, 0.9f), (1.0f / 0.9f), Color[bWhite],0, 0.0f,0.0f);
		}
	}
}

//************************************************************************
// Internal API:    DrawGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CAlphaBldTest::DrawGrid(int nTest, int nMesh)
{
	DWORD dwWidth = m_vpTest.Width;//m_pSrcTarget->m_dwWidth;
	DWORD dwHeight = m_vpTest.Height;//m_pSrcTarget->m_dwHeight;

	// Multicolored grid
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.10f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.099f * (float)dwHeight)) + (float)m_vpTest.Y, 0.6f), (1.0f / 0.6f), RGBA_MAKE(255,0,0,nAlphaValue[nMesh]),  0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.10f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.9f * (float)dwHeight)) + (float)m_vpTest.Y,   0.6f), (1.0f / 0.6f), RGBA_MAKE(255,128,0,nAlphaValue[nMesh]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.26f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.099f * (float)dwHeight)) + (float)m_vpTest.Y, 0.6f), (1.0f / 0.6f), RGBA_MAKE(128,128,0,nAlphaValue[nMesh]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.42f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.9f * (float)dwHeight)) + (float)m_vpTest.Y,   0.6f), (1.0f / 0.6f), RGBA_MAKE(128,255,0,nAlphaValue[nMesh]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.58f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.099f * (float)dwHeight)) + (float)m_vpTest.Y, 0.6f), (1.0f / 0.6f), RGBA_MAKE(0,255,128,nAlphaValue[nMesh]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.74f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.9f * (float)dwHeight)) + (float)m_vpTest.Y,   0.6f), (1.0f / 0.6f), RGBA_MAKE(0,128,128,nAlphaValue[nMesh]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.901f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.099f * (float)dwHeight)) + (float)m_vpTest.Y, 0.6f), (1.0f / 0.6f), RGBA_MAKE(0,128,255,nAlphaValue[nMesh]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.901f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.9f * (float)dwHeight)) + (float)m_vpTest.Y,   0.6f), (1.0f / 0.6f), RGBA_MAKE(0,0,255,nAlphaValue[nMesh]),  0, 0.0f,0.0f);

	// White triangle (Bottom Left)
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.03f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.03f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.20f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.37f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.37f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.03f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);

	// Black triangle (Bottom Right)
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.63f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.03f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.80f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.37f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.97f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.03f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);

	// White/Black rectangle (Center)
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.33f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.32f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.67f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.58f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.67f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.32f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);

	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.33f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.32f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.33f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.58f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.67f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.58f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);

	// Black triangle (Top Left)
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.03f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.63f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.20f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.97f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.37f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.63f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(0,0,0,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);

	// White triangle (Top Right)
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.63f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.63f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.80f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.97f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((D3DVALUE)(int)(0.97f * (float)dwWidth + (float)m_vpTest.X), (D3DVALUE)(int)((float)dwHeight - (0.63f * (float)dwHeight)) + (float)m_vpTest.Y, 0.5f), (1.0f / 0.5f), RGBA_MAKE(255,255,255,nAlphaValue[(nTest % 6)-1]),0, 0.0f,0.0f);
}

//************************************************************************
// Internal API:    DrawCircle
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CAlphaBldTest::DrawCircle(D3DVECTOR* pCenter, float Radius, int nDivisions, D3DCOLOR Color)
{
    int             i, NumVertices;
    float           Theta, Rads;

  	dwVertexCount = 0;
	NumVertices = nDivisions + 2;
    Theta = 360.0f / nDivisions;

    // Initialize the center point
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(pCenter->x, pCenter->y, pCenter->z), (1.0f / pCenter->z), Color, 0, 0.0f,0.0f);

    for (i=1; i<NumVertices; i++)
    {
        // cos takes a radian value, so convert.
        Rads = (Theta * (i-1) * pi)/180.0f;

		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((pCenter->x + Radius * (float)cos(Rads)), (pCenter->y + Radius * (float)sin(Rads)), pCenter->z), (1.0f / pCenter->z), Color, 0, 0.0f,0.0f);
    }

	RenderPrimitive(D3DPT_TRIANGLEFAN,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);
}
