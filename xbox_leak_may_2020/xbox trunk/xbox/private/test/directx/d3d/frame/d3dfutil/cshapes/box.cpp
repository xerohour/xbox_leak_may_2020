//
//  Box.cpp
//
//  CShapes class routine for generating a triangle-list box.
//

#include "d3dlocus.h"
#include "..\..\..\inc\3DMath.h"
#include "..\..\..\inc\CShapes.h"

// ----------------------------------------------------------------------------

bool CShapes::GenerateBox()  // called by NewShape
{
	D3DVERTEX * pVertices;
	WORD *      pIndices;
	const int   nVertices = 24;
	const int   nIndices = 36;

	// define a basic cube, centered at the origin.
	D3DVERTEX VertexList[nVertices] = {
		//  x      y      z      Nx     Ny     Nz    u     v
		D3DVERTEX(cD3DVECTOR(  0.5f, -0.5f, -0.5f), cD3DVECTOR( 0.0f,  0.0f, -1.0f), 1.0f, 1.0f ), // face 1 (front)
		D3DVERTEX(cD3DVECTOR( -0.5f, -0.5f, -0.5f), cD3DVECTOR( 0.0f,  0.0f, -1.0f), 0.0f, 1.0f ),
		D3DVERTEX(cD3DVECTOR( -0.5f,  0.5f, -0.5f), cD3DVECTOR( 0.0f,  0.0f, -1.0f), 0.0f, 0.0f ),
		D3DVERTEX(cD3DVECTOR(  0.5f,  0.5f, -0.5f), cD3DVECTOR( 0.0f,  0.0f, -1.0f), 1.0f, 0.0f ),

		D3DVERTEX(cD3DVECTOR(  0.5f, -0.5f,  0.5f), cD3DVECTOR( 1.0f,  0.0f,  0.0f), 1.0f, 1.0f ), // face 2 (right)
		D3DVERTEX(cD3DVECTOR(  0.5f, -0.5f, -0.5f), cD3DVECTOR( 1.0f,  0.0f,  0.0f), 0.0f, 1.0f ),
		D3DVERTEX(cD3DVECTOR(  0.5f,  0.5f, -0.5f), cD3DVECTOR( 1.0f,  0.0f,  0.0f), 0.0f, 0.0f ),
		D3DVERTEX(cD3DVECTOR(  0.5f,  0.5f,  0.5f), cD3DVECTOR( 1.0f,  0.0f,  0.0f), 1.0f, 0.0f ),

		D3DVERTEX(cD3DVECTOR( -0.5f, -0.5f,  0.5f), cD3DVECTOR( 0.0f,  0.0f,  1.0f), 1.0f, 1.0f ), // face 3 (back)
		D3DVERTEX(cD3DVECTOR(  0.5f, -0.5f,  0.5f), cD3DVECTOR( 0.0f,  0.0f,  1.0f), 0.0f, 1.0f ),
		D3DVERTEX(cD3DVECTOR(  0.5f,  0.5f,  0.5f), cD3DVECTOR( 0.0f,  0.0f,  1.0f), 0.0f, 0.0f ),
		D3DVERTEX(cD3DVECTOR( -0.5f,  0.5f,  0.5f), cD3DVECTOR( 0.0f,  0.0f,  1.0f), 1.0f, 0.0f ),

		D3DVERTEX(cD3DVECTOR( -0.5f, -0.5f, -0.5f), cD3DVECTOR(-1.0f,  0.0f,  0.0f), 1.0f, 1.0f ), // face 4 (left)
		D3DVERTEX(cD3DVECTOR( -0.5f, -0.5f,  0.5f), cD3DVECTOR(-1.0f,  0.0f,  0.0f), 0.0f, 1.0f ),
		D3DVERTEX(cD3DVECTOR( -0.5f,  0.5f,  0.5f), cD3DVECTOR(-1.0f,  0.0f,  0.0f), 0.0f, 0.0f ),
		D3DVERTEX(cD3DVECTOR( -0.5f,  0.5f, -0.5f), cD3DVECTOR(-1.0f,  0.0f,  0.0f), 1.0f, 0.0f ),

		D3DVERTEX(cD3DVECTOR(  0.5f,  0.5f, -0.5f), cD3DVECTOR( 0.0f,  1.0f,  0.0f), 1.0f, 1.0f ), // face 5 (top)
		D3DVERTEX(cD3DVECTOR( -0.5f,  0.5f, -0.5f), cD3DVECTOR( 0.0f,  1.0f,  0.0f), 0.0f, 1.0f ),
		D3DVERTEX(cD3DVECTOR( -0.5f,  0.5f,  0.5f), cD3DVECTOR( 0.0f,  1.0f,  0.0f), 0.0f, 0.0f ),
		D3DVERTEX(cD3DVECTOR(  0.5f,  0.5f,  0.5f), cD3DVECTOR( 0.0f,  1.0f,  0.0f), 1.0f, 0.0f ),
		
		D3DVERTEX(cD3DVECTOR(  0.5f, -0.5f,  0.5f), cD3DVECTOR( 0.0f, -1.0f,  0.0f), 1.0f, 1.0f ), // face 6 (bottom)
		D3DVERTEX(cD3DVECTOR( -0.5f, -0.5f,  0.5f), cD3DVECTOR( 0.0f, -1.0f,  0.0f), 0.0f, 1.0f ),
		D3DVERTEX(cD3DVECTOR( -0.5f, -0.5f, -0.5f), cD3DVECTOR( 0.0f, -1.0f,  0.0f), 0.0f, 0.0f ),
		D3DVERTEX(cD3DVECTOR(  0.5f, -0.5f, -0.5f), cD3DVECTOR( 0.0f, -1.0f,  0.0f), 1.0f, 0.0f ),
	};

	WORD IndexList[nIndices] = {
		 0,  1,  2,  0,  2,  3,  // face 1
		 4,  5,  6,  4,  6,  7,  // face 2
		 8,  9, 10,  8, 10, 11,  // face 3
		12, 13, 14, 12, 14, 15,  // face 4
		16, 17, 18, 16, 18, 19,  // face 5
		20, 21, 22, 20, 22, 23,  // face 6
	};

	int i;

	// allocate the needed memory
	pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * nVertices);
    if (!pVertices) {
        return false;
    }
	pIndices = (WORD *)malloc(sizeof(WORD) * nIndices);
    if (!pIndices) {
        free(pVertices);
        return false;
    }

	//
	// set vertex data
	//
	for(i = 0; i < nVertices; i++)
	{
		pVertices[i].dvX = VertexList[i].dvX;
		pVertices[i].dvY = VertexList[i].dvY;
		pVertices[i].dvZ = VertexList[i].dvZ;
		pVertices[i].dvNX = VertexList[i].dvNX;
		pVertices[i].dvNY = VertexList[i].dvNY;
		pVertices[i].dvNZ = VertexList[i].dvNZ;
		pVertices[i].dvTU = VertexList[i].dvTU;
		pVertices[i].dvTV = VertexList[i].dvTV;
	}
	
	//
	// set index data
	//
	for(i = 0; i < nIndices; i++)
		pIndices[i] = IndexList[i];
	
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

// ----------------------------------------------------------------------------
