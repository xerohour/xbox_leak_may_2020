//-----------------------------------------------------------------------------
//  
//  File: octosphere.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//-----------------------------------------------------------------------------
#include "octosphere.h"

//-----------------------------------------------------------------------------
// Name: OctoSphereIndex
// Desc: The octohedron is mapped to a square and layed-out in memory as follows:
//
//             DOMAIN                        MEMORY LAYOUT
//               -Y
//   -Z  +a0a1a2a3+a3a2a2a0+ -Z           Z0a0a1a2a3Y0b3b2b1b0
//      c0       /|\       d0		      c0 . . . . . . . . .
//      c1     /  |  \     d1		      c1 . . . . . . . . .
//      c2   /    |    \   d2		      c2 . . . . . . . . .
//      c3 /   +Z |      \ d3		      c3 . . . . . . . . .
//    -X +--------+--------+ +X		      X0 . . . .Z1 . . . .
//      c3 \      |      / d3		      d3 . . . . . . . . .
//      c2   \    |    /   d2		      d2 . . . . . . . . .
//      c1     \  |  /     d1		      d1 . . . . . . . . .
//      c0       \|/       d0		      d0 . . . . . . . . .
//       +b0b1b2b3+b3b2b1b0+ -Z		      X1 Y1               
//      -Z       +Y
//
// Where the a0=a0, b2=b2, etc. points are identical.
// The -Z axis is at all four corners.
// For the memory layout, the right column and bottom row are "chopped" off.
//   'b' values are packed next to 'a' values in the top row
//   'd' values are packed below 'c' values in the left column
//   X1 = +X and Y1=+Y are put at the end
//-----------------------------------------------------------------------------
inline int OctoSphereStandardIndex(int i, int j, int M)
{
	return (i + M) * M * 2 + j + M; // default indexing
}
inline int OctoSphereIndex(int i, int j, int M)
{
	if (i == -M)
	{
		if (j == -M || j == M)
			return 0; // -Z
		else if (j > 0)
			return OctoSphereStandardIndex(i, -j, M); // flipped 'a' value
		else
			return OctoSphereStandardIndex(i, j, M);
	}
	else if (i == M)
	{
		if (j == -M || j == M)
			return 0; // -Z
		else if (j == 0)
			return 4*M*M + 1; // +Y
		else if (j < 0)
			return OctoSphereStandardIndex(-M, -j, M); // flipped packed 'b' value
		else // if (j > 0)
			return OctoSphereStandardIndex(-M, j, M);   // packed 'b' value
	}
	else if (j == -M)
	{
		if (i == -M || i == M)
			return 0; // -Z
		else if (i > 0)
			return OctoSphereStandardIndex(-i, j, M); // flipped 'c' value
		else
			return OctoSphereStandardIndex(i, j, M);
	}
	else if (j == M)
	{
		if (i == -M || i == M)
			return 0; // -Z
		else if (i == 0)
			return 4*M*M; // +X
		else if (i < 0)
			return OctoSphereStandardIndex(-i, -M, M); // flipped packed 'd' value
		else // if (i > 0)
			return OctoSphereStandardIndex(i, -M, M); // packed 'd' value
	}
	else
		return OctoSphereStandardIndex(i, j, M);
}

//-----------------------------------------------------------------------------
// Name: FillOctoSphere
// Desc: Create a sphere as a subdivision of an octohedron.
//       Pass in NULL pointers to rVertex or rIndex to return needed sizes of buffers.
//-----------------------------------------------------------------------------
HRESULT FillOctoSphere(UINT nSplit, // number of splits on each edge, 0 = octohedron
					  UINT *pnVertex, D3DXVECTOR3 *rVertex, // vertices
					  UINT *pnIndex, WORD *rIndex) // triangle indices
{
	int M = nSplit + 1;
	int N = M * 2; // number of edges along one side of our domain
	if (pnVertex != NULL)
		*pnVertex = N * N + 2;
	if (pnIndex != NULL)
		*pnIndex = 3 * 2 * N * N;

	// Fill in vertices based on faces of octohedron
	if (rVertex)
	{
		// TODO: Add chord -> angle correction to get better shaped triangles
		D3DXVECTOR3 *pVertex = rVertex;
		for (int i = -M; i < M; i++)
		{
			for (int j = -M; j < M; j++)
			{
				int i0, j0;
				// handle special packing of boundaries
				if (i == -M && j > 0)
				{
					i0 = M;	 // flip i to handle 'b' boundary
					j0 = j;
				}
				else if (j == -M && i > 0)
				{
					i0 = i;
					j0 = M;	// flip j to handle 'd' boundary
				}
				else
				{
					// standard case
					i0 = i;
					j0 = j;
				}
				int i1 = (i0 < 0) ? -i0 : i0;	// flip y to positive quadrant
				int j1 = (j0 < 0) ? -j0 : j0;	// flip x to positive quadrant
				int ix, iy, iz;
				if (i1 <= M - j1)				// test against x + y = M line
				{
					ix = j0;
					iy = i0;
					iz = M - i1 - j1;
				}
				else // flip z
				{
					// reflect x and y across x + y = M line
					ix = M - i1; // swap x and y
					iy = M - j1;
					if (j0 < 0) ix = -ix; // restore quadrant
					if (i0 < 0) iy = -iy;
					iz = M - i1 - j1;  // -M + (M - i1) + (M - j1);
				}
				pVertex->x = (float)ix;
				pVertex->y = (float)iy;
				pVertex->z = (float)iz;
				D3DXVec3Normalize(pVertex, pVertex);
				pVertex++;
			}
		}
		// last two vertices are special cases
		pVertex->x = 1.f; pVertex->y = 0.f; pVertex->z = 0.f; pVertex++; // +X
		pVertex->x = 0.f; pVertex->y = 1.f; pVertex->z = 0.f; pVertex++; // +Y
	}

	// Fill in triangles, orienting triangle flips by quadrant
	if (rIndex)
	{
		WORD *pIndex = rIndex;
		for (int i = -M; i < M; i++)
		{
			for (int j = -M; j < M; j++)
			{
#define INDEX(i,j) 	((WORD)(OctoSphereIndex(i, j, M)))
				if ((i < 0 && j < 0) || (i >= 0 && j >= 0))
				{
					pIndex[0] = INDEX(i,j);
					pIndex[1] = INDEX(i+1,j);
					pIndex[2] = INDEX(i,j+1);
					pIndex += 3;
					pIndex[0] = INDEX(i,j+1);
					pIndex[1] = INDEX(i+1,j);
					pIndex[2] = INDEX(i+1,j+1);
					pIndex += 3;
				}
				else
				{
					pIndex[0] = INDEX(i,j);
					pIndex[1] = INDEX(i+1,j);
					pIndex[2] = INDEX(i+1,j+1);
					pIndex += 3;
					pIndex[0] = INDEX(i,j);
					pIndex[1] = INDEX(i+1,j+1);
					pIndex[2] = INDEX(i,j+1);
					pIndex += 3;
				}
#undef INDEX				
			}
		}
	}
	return S_OK;
}
