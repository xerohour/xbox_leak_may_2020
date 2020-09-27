//
//  Pyramid.cpp
//
//  CShapes class routines for generating and drawing Pyramids.
//

#include "d3dlocus.h"
#include "..\..\..\inc\3DMath.h"
#include "..\..\..\inc\CShapes.h"

// ----------------------------------------------------------------------------

bool CShapes::GeneratePyramid()  // called by NewShape
{
	D3DVERTEX * pVertices;
	WORD *      pIndices;
	const int   nVertices = 16;
	const int   nIndices = 18;

	// define a basic pyramid, centered at the origin.
	D3DVERTEX VertexList[nVertices] = {
		//  x      y      z      xN        yN        zN       u     v
		D3DVERTEX(cD3DVECTOR(  0.5f, -0.5f, -0.5f), cD3DVECTOR( 0.0000f,  0.4472f, -0.8944f), 1.0f, 1.0f ), // face 1 (front)
		D3DVERTEX(cD3DVECTOR( -0.5f, -0.5f, -0.5f), cD3DVECTOR( 0.0000f,  0.4472f, -0.8944f), 0.0f, 1.0f ),
		D3DVERTEX(cD3DVECTOR(  0.0f,  0.5f,  0.0f), cD3DVECTOR( 0.0000f,  0.4472f, -0.8944f), 0.5f, 0.5f ),

		D3DVERTEX(cD3DVECTOR(  0.5f, -0.5f,  0.5f), cD3DVECTOR( 0.8944f,  0.4472f,  0000.0f), 1.0f, 0.0f ),  // face 2 (right)
		D3DVERTEX(cD3DVECTOR(  0.5f, -0.5f, -0.5f), cD3DVECTOR( 0.8944f,  0.4472f,  0000.0f), 1.0f, 1.0f ),
		D3DVERTEX(cD3DVECTOR(  0.0f,  0.5f,  0.0f), cD3DVECTOR( 0.8944f,  0.4472f,  0000.0f), 0.5f, 0.5f ),

		D3DVERTEX(cD3DVECTOR( -0.5f, -0.5f,  0.5f), cD3DVECTOR( 0.0000f,  0.4472f,  0.8944f), 0.0f, 0.0f ),  // face 3 (back)
		D3DVERTEX(cD3DVECTOR(  0.5f, -0.5f,  0.5f), cD3DVECTOR( 0.0000f,  0.4472f,  0.8944f), 1.0f, 0.0f ),
		D3DVERTEX(cD3DVECTOR(  0.0f,  0.5f,  0.0f), cD3DVECTOR( 0.0000f,  0.4472f,  0.8944f), 0.5f, 0.5f ),

		D3DVERTEX(cD3DVECTOR( -0.5f, -0.5f, -0.5f), cD3DVECTOR(-0.8944f,  0.4472f,  0000.0f), 0.0f, 1.0f ), // face 4 (left)
		D3DVERTEX(cD3DVECTOR( -0.5f, -0.5f,  0.5f), cD3DVECTOR(-0.8944f,  0.4472f,  0000.0f), 0.0f, 0.0f ),
		D3DVERTEX(cD3DVECTOR(  0.0f,  0.5f,  0.0f), cD3DVECTOR(-0.8944f,  0.4472f,  0000.0f), 0.5f, 0.5f ),
		
		D3DVERTEX(cD3DVECTOR(  0.5f, -0.5f,  0.5f), cD3DVECTOR( 0.0000f, -1.0000f,  0.0000f), 1.0f, 0.0f ),  // face 5 (bottom)
		D3DVERTEX(cD3DVECTOR( -0.5f, -0.5f,  0.5f), cD3DVECTOR( 0.0000f, -1.0000f,  0.0000f), 0.0f, 0.0f ),
		D3DVERTEX(cD3DVECTOR( -0.5f, -0.5f, -0.5f), cD3DVECTOR( 0.0000f, -1.0000f,  0.0000f), 0.0f, 1.0f ),
		D3DVERTEX(cD3DVECTOR(  0.5f, -0.5f, -0.5f), cD3DVECTOR( 0.0000f, -1.0000f,  0.0000f), 1.0f, 1.0f ),
	};

	WORD IndexList[nIndices] = {
		 0,  1,  2,  // face 1
		 3,  4,  5,  // face 2
		 6,  7,  8,  // face 3
		 9, 10, 11,  // face 4
		12, 13, 14,  // face 5
		12, 14, 15,  // face 6
	};

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

	int i;
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
