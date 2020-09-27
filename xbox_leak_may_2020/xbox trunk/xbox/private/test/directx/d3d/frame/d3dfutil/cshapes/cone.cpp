// Cone.cpp
//
//  CShapes class routine for generating a triangle-list cone.
//

#include "d3dlocus.h"
#include "..\..\..\inc\3DMath.h"
#include "..\..\..\inc\CShapes.h"

bool CShapes::GenerateCone(const int nDivisions)
{
	const float	pi = 3.1415926535f;
	const float fRadius = 0.5f;
	const float fTheta = 360.0f / nDivisions;

	const int   nVertices = (3 * nDivisions + 1);
	const int   nIndices = (6 * nDivisions); // 3*nDivisions (top) + 3*nDivisions (bottom) 
	D3DVERTEX * pVertices = NULL;
	WORD *      pIndices = NULL;
	
	float       fRads;

	int i, n;
	float fX, fZ, fU;
	D3DVECTOR Normal;

	// allocate memory for the vertices and indices
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

	// 
	// Set Vertex Data
	//
	
	// Initialize the bottom center point
	pVertices[nVertices - 1].dvX = 0.0f;
	pVertices[nVertices - 1].dvY = -0.5f;
	pVertices[nVertices - 1].dvZ = 0.0f;
	pVertices[nVertices - 1].dvNX = 0.0f;
	pVertices[nVertices - 1].dvNY = -1.0f;
	pVertices[nVertices - 1].dvNZ = 0.0f;
	pVertices[nVertices - 1].dvTU = 0.5f;
	pVertices[nVertices - 1].dvTV = 1.0f;

	// generate the cone vertices
	for (i=0; i < (nDivisions); i++)
	{
		// cos takes a radian value, so convert.
		fRads = (fTheta * (float)i * pi) / 180.0f;
		fX = (float)(fRadius * cos(fRads));
		fZ = (float)(fRadius * sin(fRads));
		fU = (float)(i+1) / (float)nDivisions;

		Normal = Normalize(cD3DVECTOR(fX, 0.5f, fZ));

		// top -- these are the same physical point, but
		// with normals facing outward.
		pVertices[i].dvX = 0.0f;
		pVertices[i].dvY = 0.5f;
		pVertices[i].dvZ = 0.0f;
		pVertices[i].dvNX = Normal.x;
		pVertices[i].dvNY = Normal.y;
		pVertices[i].dvNZ = Normal.z;
		pVertices[i].dvTU = fU;
		pVertices[i].dvTV = 0.0f;

		// bottom side
		pVertices[nDivisions + i].dvX = fX;
		pVertices[nDivisions + i].dvY = -0.5f;
		pVertices[nDivisions + i].dvZ = fZ;
		pVertices[nDivisions + i].dvNX = Normal.x;
		pVertices[nDivisions + i].dvNY = Normal.y;
		pVertices[nDivisions + i].dvNZ = Normal.z;
		pVertices[nDivisions + i].dvTU = fU;
		pVertices[nDivisions + i].dvTV = 0.75f;

		// bottom
		pVertices[(nDivisions * 2) + i].dvX = fX;
		pVertices[(nDivisions * 2) + i].dvY = -0.5f;
		pVertices[(nDivisions * 2) + i].dvZ = fZ;
		pVertices[(nDivisions * 2) + i].dvNX = 0.0f;
		pVertices[(nDivisions * 2) + i].dvNY = -1.0f;
		pVertices[(nDivisions * 2) + i].dvNZ = 0.0f;
		pVertices[(nDivisions * 2) + i].dvTU = fU;
		pVertices[(nDivisions * 2) + i].dvTV = 0.75f;
	}

	//
	// Set Index Data
	//
	n = 0;
	// top
	for(i = 0; i < nDivisions; i++)
	{
		pIndices[n] = (WORD)(nDivisions + i);
		pIndices[n+1] = (WORD)i;
		if(i < (nDivisions - 1))
			pIndices[n+2] = (WORD)(nDivisions + i + 1);
		else
			pIndices[n+2] = (WORD)nDivisions;  // the last index on the top.
		n += 3;
	}

	// bottom
	for(i = 0; i < nDivisions; i++)
	{
		if(i < (nDivisions - 1))
			pIndices[n] = (WORD)((nDivisions * 2) + i + 1);
		else
			pIndices[n] = (WORD)(nDivisions * 2);
		pIndices[n+1] = (WORD)(nVertices - 1);
		pIndices[n+2] = (WORD)((nDivisions * 2) + i);
		n += 3;
	}

	//
	// set class data
	//
	m_pVertices = pVertices;
	m_nVertices = nVertices;
	m_pIndices = pIndices;
	m_nIndices = nIndices;
	m_bWrapU = true;
	m_bWrapV = false;

	return true;
}