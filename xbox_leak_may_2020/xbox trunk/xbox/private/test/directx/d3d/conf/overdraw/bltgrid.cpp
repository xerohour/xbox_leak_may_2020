//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
//#include "types.h"
#include "Overdraw.h"

//************************************************************************
// Internal API:    OverdrawGrid
//
// Purpose:         Draws a non-textured Grid using D3DTLVERTEX.      
//
// Return:          E_FAIL if malloc fails, ERROR_SUCCESS otherwise.
//************************************************************************

// The grid is composed of nHDiv rectangular regions made of two strips,
// a "Top Strip" and a "Bottom Strip".  

//            Top Strip
//    o-----o-----o-----o-----o
//    |  o  |     | o   |   o |
//    |     |  o  |     |     |
//    o-----o-----o-----o-----o
//          Bottom Strip

// The left edge is included with the top strip, and the right edge is
// part of the bottom strip.

// We'll be drawing each triangle seperately, alternating between red 
// and green triangles, then blitting each one.

bool COverdrawTest::BltGrid(int nTest, float fGridXPos, float fGridYPos, int nXDivisions, int nYDivisions)
{
	float          fRowHeight, fColWidth;
	float          fColLeft,   fColRight;   // left and right X positions of the current column.
	float          fRowTop,    fRowBottom;  // top and bottom Y positions of the current row.
	int            nCurMid,    nCurGrid;    // incrementors
	D3DTLVERTEX *  pMidPoints;
	D3DTLVERTEX *  pGridPoints;
	D3DTLVERTEX *  pTriangleVertices;
	float *        pGridXPositions;
	D3DCOLOR       dcTriColor;
	BOOL           bGreenTriangle=0;
	BOOL           bBottom;
	int            nRow;
	CSurface8 *    pTempSurface=NULL;
	CSurface8 *    pPrevSurface=NULL;
    POINT          Pt;
	DWORD		   dwSurface;

    m_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pPrevSurface);

	// Initialize some temporary variables
	pMidPoints = (D3DTLVERTEX *)malloc(sizeof(D3DTLVERTEX) * nXDivisions);
	pGridPoints = (D3DTLVERTEX *)malloc(sizeof(D3DTLVERTEX) * (nXDivisions+1));
	pGridXPositions = (float *)malloc(sizeof(float) * (nXDivisions+1));
	pTriangleVertices = (D3DTLVERTEX *)malloc(sizeof(D3DTLVERTEX) * 3);
	 
	if ((!pMidPoints) || (!pGridPoints) || (!pGridXPositions) || (!pTriangleVertices))
	{
		WriteToLog("Not enough memory (malloc failed)\n");
		TestFinish(D3DTESTFINISH_ABORT);
		return false;
	}

	memset(pMidPoints, 0, sizeof(D3DTLVERTEX) * nXDivisions);
	memset(pGridPoints, 0, sizeof(D3DTLVERTEX) * (nXDivisions+1));
	memset(pTriangleVertices, 0, sizeof(D3DTLVERTEX) * 3);
	memset(pGridXPositions, 0, sizeof(float) * (nXDivisions+1));

    // Create a temp surface for rendering
/*
    pTempSurface = new CDirectDrawSurface(this);

    if (NULL == pTempSurface)
	{
		WriteToLog("Initialize failed\n");
		RELEASE(pTempSurface);
		TestFinish(D3DTESTFINISH_ABORT);
		return false;
	}

    if (m_dwVersion <= 0x0700)
    {
	    if (m_pAdapter->Devices[m_pMode->nSrcDevice].fHardware)
		    dwSurface = CDDS_SURFACE_VIDEO;
	    else
		    dwSurface = CDDS_SURFACE_SYSTEM;

	    if (!pTempSurface->CreateSurface(m_pSrcTarget->m_dwWidth,m_pSrcTarget->m_dwHeight,dwSurface))
	    {
		    WriteToLog("CreateSurface failed\n");
		    RELEASE(pTempSurface);
		    TestFinish(D3DTESTFINISH_ABORT);
		    return false;
	    }
    }
    else // m_dwVersion >= 0x0800
*/
    {
        HRESULT hr;
        D3DSURFACE_DESC Desc;
        pPrevSurface->GetDesc(&Desc);
//        m_pSrcTarget->GetSurfaceDescription(&Desc);

        hr = m_pSrcDevice8->CreateRenderTarget(Desc.Width, 
                                               Desc.Height,
                                               Desc.Format,
                                               D3DMULTISAMPLE_NONE,
                                               TRUE,
                                               &pTempSurface); 
        if (FAILED(hr))
        {
	        WriteToLog("CreateRenderTarget() failed with HResult = %X.\n",hr);
//		    RELEASE(pTempSurface);
		    TestFinish(D3DTESTFINISH_ABORT);
		    return false;
	    }

//        pTempSurface->ProcessSurfaceDescription(&Desc);
    }

	// Save a pointer to the previous surface
//	pPrevSurface = m_pSrcTarget;
//    pPrevSurface->AddRef();

	// Set the render target to render to our new temporary surface.
	if (FAILED(m_pDevice->SetRenderTarget(pTempSurface, FALSE)))
	{
		WriteToLog("SetRenderTarget() to TempSurface failed\n");
		RELEASE(pTempSurface);
		TestFinish(D3DTESTFINISH_ABORT);
		return false;
	}

//	RELEASE(pTempSurface);

	// Set the random number seed so we can reproduce this case if necessary.
	srand(RAND_SEED * nTest);

	// Get the row height and width
	fRowHeight = 256.0f / (float)nYDivisions;
	fColWidth =  256.0f / (float)nXDivisions;

	// The grid X Positions will be used for every row, so we'll generate them here.
	for (nCurGrid = 0; nCurGrid < (nXDivisions+1); nCurGrid++)
		pGridXPositions[nCurGrid] = fGridXPos + (nCurGrid * fColWidth);

	// Now we're going to step through each row of the grid.
	for (nRow = 0; nRow < nYDivisions; nRow++)
	{
		// Set the row top and bottom positions
		if (nRow == 0)
			fRowTop = fGridYPos;   // If row zero, the top will be the grid Y position.
		else                       //     else
			fRowTop = fRowBottom;  // set the row top equal to the *previous* bottom.

		fRowBottom = fRowTop + fRowHeight;

		// Generate the list of semi-random internal points for this row.
		for (nCurMid = 0; nCurMid < nXDivisions; nCurMid++)
		{
			fColLeft = pGridXPositions[nCurMid];
			fColRight = pGridXPositions[nCurMid+1];

			switch (nTest)
			{
				case 1: case 2:	case 3:
					SetVertex(
						&(pMidPoints[nCurMid]),
						fColLeft + ((fColWidth * nTest) / 4),
						fRowTop + ((fRowHeight * nTest) / 4),
						0.9f);
					break;
				case 4:
					SetVertex(
						&(pMidPoints[nCurMid]),
						fColLeft + (fColWidth / 4),
						fRowTop + ((fRowHeight * 3) / 4),
						0.9f);
					break;
				case 5:
					SetVertex(
						&(pMidPoints[nCurMid]),
						fColLeft + ((fColWidth * 3) / 4),
						fRowTop + (fRowHeight / 4),
						0.9f);
					break;
				default:
					SetVertex(
						&(pMidPoints[nCurMid]),
						fColLeft + (fColWidth * ((float)rand() / (float)RAND_MAX)),
						fRowTop + (fRowHeight * ((float)rand() / (float)RAND_MAX)),
						0.9f);
					break;
			}
		}

		// Repeat this for the top and bottom strips.
		for (bBottom = 0; bBottom < 2; bBottom++)
		{
			int bForward = TRUE;
			D3DTLVERTEX FirstVertex;

			memset(&FirstVertex, 0, sizeof(D3DTLVERTEX));

			// Generate the list of grid points for the row.
			// if bBottom, then we're going to fill the pGridPoints with
			// the values from the pGridXPositions in reverse.
			for (nCurGrid = 0; nCurGrid < (nXDivisions + 1); nCurGrid++)
			{
				SetVertex(
					&(pGridPoints[nCurGrid]),
					pGridXPositions[(bBottom ? (nXDivisions - nCurGrid) : nCurGrid)],
					(bBottom ? fRowBottom : fRowTop),
					0.9f);
			}

			// if bBottom, reverse the midpoint array.
			if (bBottom)
				ReverseVertices(pMidPoints, nXDivisions);

			// set the first point
			SetVertex(
				&FirstVertex,
				pGridPoints[0].dvSX,
				(bBottom ? fRowTop : fRowBottom),
				0.9f);

			// Reset our counters
			nCurGrid = 0;
			nCurMid = -1; // the first midpoint is actually the opposite gridpoint.

			// Walk across the row, making triangles as we go!
			for (int i = 0; i < (2 * nXDivisions); i++)
			{
				// pick the three triangle vertices
				if (bForward)
				{
					if (nCurMid == -1)
						pTriangleVertices[0] = FirstVertex;
					else
						pTriangleVertices[0] = pMidPoints[nCurMid];
					pTriangleVertices[1] = pGridPoints[nCurGrid];
					pTriangleVertices[2] = pMidPoints[++nCurMid];						
				}
				else
				{
					pTriangleVertices[0] = pMidPoints[nCurMid];
					pTriangleVertices[1] = pGridPoints[nCurGrid];
					pTriangleVertices[2] = pGridPoints[++nCurGrid];
				}

				// Set the Color of the Triangle.  We're alternating red
				// and green here.
				bGreenTriangle = !bGreenTriangle;
				if (bGreenTriangle)
					dcTriColor = RGBA_MAKE( 0, 255, 0, 255 );
				else
					dcTriColor = RGBA_MAKE( 255, 0, 0, 255 );
				pTriangleVertices[0].dcColor = dcTriColor;
				pTriangleVertices[1].dcColor = dcTriColor;
				pTriangleVertices[2].dcColor = dcTriColor;

				bForward = !bForward;

				// Clear the previous geometry and then blt to black. 
				Clear(RGB_MAKE(0,0,0), 1.0f, 0, D3DCLEAR_TARGET);

				// now call RenderPrimitive to render the triangle
				BeginScene();

				RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,pTriangleVertices,3,NULL,0,CD3D_SRC);

				EndScene();

				// Blt the image from the temp surface to the main surface.
/*
                if (m_dwVersion <= 0x0700)
				{
					HDC srcDC, dstDC;

					srcDC = m_pSrcTarget->GetDC();
					dstDC = pPrevSurface->GetDC();

					BitBlt( dstDC, 0, 0,
							pPrevSurface->m_dwWidth, pPrevSurface->m_dwHeight,
							srcDC, 0, 0, SRCPAINT);

					m_pSrcTarget->ReleaseDC(srcDC);
					pPrevSurface->ReleaseDC(dstDC);
                
					// Blt the surface to the primary (triangle by triangle)
					Pt.x = Pt.y = 0;
					Blt(pPrevSurface,Pt);
				}
                else // m_dwVersion >= 0x0800
                {
*/
                    OrBlt(pPrevSurface, pTempSurface);

				    // Blt the surface to the primary (triangle by triangle)
                    Pt.x = 0;
                    Pt.y = 20;
//                    Present8(Pt);
//                }
			}
		}
	}

	RELEASE(pTempSurface);

	// clear the memory we allocated
	if (pGridPoints)
		free(pGridPoints);
	if (pMidPoints)
		free(pMidPoints);
	if (pGridXPositions)
		free(pGridXPositions);
	if (pTriangleVertices)
		free(pTriangleVertices);

	// Set the render target to our old surface and release the temporary one.
	if (FAILED(m_pDevice->SetRenderTarget(pPrevSurface, false)))
	{
		WriteToLog("SetRenderTarget() back to PrevSurface failed\n");
		TestFinish(D3DTESTFINISH_ABORT);
		return false;
	}

    RELEASE(pPrevSurface);

	return true;
}
