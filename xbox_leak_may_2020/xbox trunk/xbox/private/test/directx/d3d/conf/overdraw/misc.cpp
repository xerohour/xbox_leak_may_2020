//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "Overdraw.h"

// ReverseVertices
//
// Takes a pointer to a contiguous block of memory containing vertices
// (as in an array) and reverses the order of the vertices, copying
// all the vertex information in the process.

void COverdrawTest::ReverseVertices(D3DTLVERTEX * pVertices, int nVertices)
{
	int nCurVertex = 0;
	int nLastVertex = (nVertices - 1);
	D3DTLVERTEX tempVertex;
	
	while(nCurVertex < nLastVertex)
	{
		tempVertex = pVertices[nCurVertex];
		pVertices[nCurVertex] = pVertices[nLastVertex];
		pVertices[nLastVertex] = tempVertex;

		nCurVertex++;
		nLastVertex--;
	}
}

void COverdrawTest::OrBlt(CSurface8 * pDest, CSurface8 * pSrc)
{
	int		 i, j;
	RECT	 rSurf;
	char	 *pSrcBuf, *pDestBuf;
    D3DLOCKED_RECT d3dlrSrc, d3dlrDst;
    D3DSURFACE_DESC d3dsd;
    DWORD dwDepth;

    pDest->GetDesc(&d3dsd);
    dwDepth = FormatToBitDepth(d3dsd.Format) / 8;

	// Build the subrect we want
	rSurf.top = 0;
	rSurf.left = 0;
	rSurf.bottom = rSurf.top + d3dsd.Height;
	rSurf.right = rSurf.left + d3dsd.Width;

	// Get the Surface data pointer for our subrect
//    pSrcBuf = (char*)pSrc->Lock(&rSurf);
    pSrc->LockRect(&d3dlrSrc, &rSurf, 0);
    pSrcBuf = (char*)d3dlrSrc.pBits;
//    pDestBuf = (char*)pDest->Lock(&rSurf);
    pDest->LockRect(&d3dlrDst, &rSurf, 0);
    pDestBuf = (char*)d3dlrDst.pBits;

	// OR the surfaces together
	for (j=0; j < (int)d3dsd.Height; j++)		// Height loop
	{
		char *pSrcColor = pSrcBuf + (d3dlrSrc.Pitch * j);
		char *pDestColor = pDestBuf + (d3dlrDst.Pitch * j);

		for (i=0; i < (int)(d3dsd.Width * dwDepth); i++)	// Width loop
		{
            *pDestColor = *pSrcColor++ | *pDestColor++;
        }
	}

	pSrc->UnlockRect();
	pDest->UnlockRect();
}
