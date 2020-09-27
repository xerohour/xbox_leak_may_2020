//
//  Sphere.cpp
//
//  CShapes class routines for generating and drawing Spheres.
//

#include "d3dlocus.h"
#include "..\..\..\inc\3DMath.h"
#include "..\..\..\inc\CShapes.h"

// ----------------------------------------------------------------------------

bool CShapes::GenerateSphere(const int nDivisions)  // called by NewShape
{
	bool bInvertFaces = FALSE;
	bool bInvertNormal = FALSE;
	const float	pi = 3.1415926535f;
	const int   nVertices = (2 * nDivisions * nDivisions) + 2;
	const int   nIndices = 12 * nDivisions * nDivisions;
	const float fRadius = 0.5f;
	D3DVERTEX * pVertices;
	WORD *      pIndices;


	int     i, j;
	float   dj = (float)(pi/(nDivisions+1.0f));
	float	di = (float)(pi/nDivisions);
	int	    v;		// vertex offset
	int     ind;	// indices offset

	// allocate the needed memory
	pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * nVertices);
	pIndices = (WORD *)malloc(sizeof(WORD) * nIndices);

	if(pVertices && pIndices)
	{
		// generate the sphere data, note the random texture coords
		// vertices 0 and 1 are the north and south poles
		pVertices[0] = cD3DVERTEX(cD3DVECTOR(0.0f, fRadius, 0.0f), Normalize(cD3DVECTOR(0.0f, 1.0f, 0.0f)), 0.0f, 0.0f);
		pVertices[1] = cD3DVERTEX(cD3DVECTOR(0.0f, -fRadius, 0.0f), Normalize(cD3DVECTOR(0.0f, -1.0f, 0.0f)), 1.0f, 1.0f);

		for (j=0; j<nDivisions; j++) 
		{
			for (i=0; i<nDivisions*2; i++) 
			{
				D3DVECTOR	p;
				D3DVECTOR	n;
				float		u, v;

				// Calculate points and normal vectors
				n.y=p.y = (float) cos((j+1) * dj);
				n.x=p.x = (float) sin(i * di) * (float) sin((j+1) * dj);
				n.z=p.z = (float) cos(i * di) * (float) sin((j+1) * dj);

				// apply radius
				p.y *= fRadius;
				p.x *= fRadius;
				p.z *= fRadius;

				// Determine normal direction
				if(bInvertNormal)
				{
					n.x *= -1.0f;
					n.y *= -1.0f;
					n.z *= -1.0f;
				}

				u = ((float)i/nDivisions) / 2.0f;

				if (u>1.0f) 
					u -= 1.0f;

				u = 1.0f - u;	// flip so texture is not mirrored
				v = (float)j/nDivisions;

				pVertices[2+i+j*nDivisions*2] = cD3DVERTEX(p, n, u, v);
			}
		}

		// now generate the triangle indices
		// strip around north pole first
		for (i=0; i<nDivisions*2; i++) 
		{
			if (bInvertFaces)
			{
				pIndices[3*i] = (WORD)(i+3);
				pIndices[3*i+1] = (WORD)(i+2);
				pIndices[3*i+2] = (WORD)0;
			}
			else
			{
				pIndices[3*i] = (WORD)0;
				pIndices[3*i+1] = (WORD)(i+2);
				pIndices[3*i+2] = (WORD)(i+3);
			}

			if (i==nDivisions*2-1)
				pIndices[3*i+2] = 2;
		}

		// now all the middle strips
		for (j=0; j<nDivisions-1; j++) 
		{
			v = 2+j*nDivisions*2;
			ind = 3*nDivisions*2 + j*6*nDivisions*2;

			for (i=0; i<nDivisions*2; i++) 
			{
				if (bInvertFaces)
				{
					pIndices[6*i+ind]   = (WORD)(v + i + nDivisions * 2);
					pIndices[6*i+2+ind] = (WORD)(v + i + 1);
					pIndices[6*i+1+ind] = (WORD)(v + i);

					pIndices[6*i+ind+3]   = (WORD)(v + i + 1);
					pIndices[6*i+2+ind+3] = (WORD)(v + i + nDivisions * 2);
					pIndices[6*i+1+ind+3] = (WORD)(v + i + nDivisions * 2 + 1);

					if (i==nDivisions*2-1) 
					{
						pIndices[6*i+2+ind] = (WORD)(v + i + 1 - 2 * nDivisions);
						pIndices[6*i+2+ind+3] = (WORD)(v + i + 1 - 2 * nDivisions);
						pIndices[6*i+1+ind+3] = (WORD)(v + i + nDivisions * 2 + 1 - 2 * nDivisions);
					}
				}
				else
				{
					pIndices[6*i+ind] = (WORD)(v + i);
					pIndices[6*i+2+ind] = (WORD)(v + i + 1);
					pIndices[6*i+1+ind] = (WORD)(v + i + nDivisions * 2);

					pIndices[6*i+ind+3] = (WORD)(v + i + nDivisions * 2);
					pIndices[6*i+2+ind+3] = (WORD)(v + i + 1);
					pIndices[6*i+1+ind+3] = (WORD)(v + i + nDivisions * 2 + 1);

					if (i==nDivisions*2-1) 
					{
						pIndices[6*i+2+ind] = (WORD)(v + i + 1 - 2 * nDivisions);
						pIndices[6*i+2+ind+3] = (WORD)(v + i + 1 - 2 * nDivisions);
						pIndices[6*i+1+ind+3] = (WORD)(v + i + nDivisions * 2 + 1 - 2 * nDivisions);
					}
				}
			}
		}

		// finally strip around south pole
		v = nVertices - nDivisions * 2;
		ind = nIndices - 3 * nDivisions * 2;

		for (i=0; i<nDivisions*2; i++) 
		{
			if (bInvertFaces)
			{
				pIndices[3*i+ind] = (WORD)(v + i);
				pIndices[3*i+1+ind] = (WORD)(v + i + 1);
				pIndices[3*i+2+ind] = (WORD)1;
			}
			else
			{
				pIndices[3*i+ind] = (WORD)1;
				pIndices[3*i+1+ind] = (WORD)(v + i + 1);
				pIndices[3*i+2+ind] = (WORD)(v + i);
			}

			if (i==nDivisions*2-1)
				pIndices[3*i+1+ind] = (WORD)v;
		}

		m_pVertices = pVertices;
		m_nVertices = nVertices;
		m_pIndices = pIndices;
		m_nIndices = nIndices;
		m_bWrapU = true;
		m_bWrapV = true;

		return true;
	}
	else {
	    if(pVertices)
		    free(pVertices);
	    if(pIndices)
		    free(pIndices);
		return false;
    }
}

// ----------------------------------------------------------------------------
