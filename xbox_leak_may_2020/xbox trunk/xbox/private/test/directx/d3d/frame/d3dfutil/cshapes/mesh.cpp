//
//  Mesh.cpp
//
//  CShapes class routine for generating a triangle-list mesh.
//

#include "d3dlocus.h"
#include "..\..\..\inc\3DMath.h"
#include "..\..\..\inc\CShapes.h"

// ----------------------------------------------------------------------------

bool CShapes::GenerateMesh(const int xDivisions, const int yDivisions)  // called by NewShape
{
	const int   nVertices = (xDivisions + 1) * (yDivisions + 1);
	const int   nIndices =  xDivisions * yDivisions * 6;
	D3DVERTEX * pVertices = NULL;
	WORD *      pIndices = NULL;
	int         nCurVtx = 0;
	int         nCurIdx = 0;
	int i, j;
	float xPos, yPos;

	if((xDivisions < 1) || (yDivisions < 1))
		return false;

	pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * nVertices);
    if (!pVertices) {
        return false;
    }
	pIndices = (WORD *)malloc(sizeof(WORD) * nIndices);
    if (!pIndices) {
        free(pVertices);
        return false;
    }

	memset(pVertices, 0, sizeof(D3DVERTEX) * nVertices);
	memset(pIndices, 0, sizeof(WORD) * nIndices);

	// create vertices
	for(i = yDivisions; i >= 0; i--)
	{
		for(j = 0; j <= xDivisions; j++)
		{
			xPos = (1.0f / xDivisions) * float(j);
			yPos = (1.0f / yDivisions) * float(i);
			pVertices[nCurVtx].x = xPos - 0.5f;
			pVertices[nCurVtx].y = yPos - 0.5f;
			pVertices[nCurVtx].z = 0.0f;
			pVertices[nCurVtx].nx = 0.0f;
			pVertices[nCurVtx].ny = 0.0f;
			pVertices[nCurVtx].nz = -1.0f;
			pVertices[nCurVtx].tu = xPos;
			pVertices[nCurVtx].tv = 1.0f - yPos;
			nCurVtx++;
		}
	}

	// create indices
	for(i = (xDivisions + 1); i < nVertices; i += (xDivisions + 1))
	{
		for(j = 0; j < (xDivisions); j++)
		{
			// tri 1
			pIndices[nCurIdx    ] = (WORD)(i + j + 1);
			pIndices[nCurIdx + 1] = (WORD)(i + j);
			pIndices[nCurIdx + 2] = (WORD)(i + j - (xDivisions + 1));
			// tri 2
			pIndices[nCurIdx + 3] = (WORD)(i + j - (xDivisions + 1));
			pIndices[nCurIdx + 4] = (WORD)(i + j - (xDivisions + 1) + 1);
			pIndices[nCurIdx + 5] = (WORD)(i + j + 1);
			nCurIdx += 6;
		}
	}

	//
	// set class data
	//
	m_pVertices = pVertices;
	m_nVertices = nVertices;
	m_pIndices = pIndices;
	m_nIndices = nIndices;
	m_bWrapU = false;
	m_bWrapV = false;

	return true;
}
