/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	InvertedBall.cpp

Abstract:

	A 3d ball

Author:

	Robert Heitkamp (robheit) 5-Feb-2001

Revision History:

	5-Feb-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "InvertedBall.h"
#include "texture.h"

//------------------------------------------------------------------------------
//	Defines:
//------------------------------------------------------------------------------
#define XYZ_NORMAL_TEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#ifndef PI 
#	define PI 3.14159265359f
#endif

//------------------------------------------------------------------------------
//	Globals
//------------------------------------------------------------------------------
D3DMATERIAL8 g_material = {
	{1.0f, 1.0f, 1.0f, 1.0f},
	{0.2f, 0.3f, 0.2f, 1.0f},
	{0.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 0.0f},
	0.0f
};
//------------------------------------------------------------------------------
//	CInvertedBall::CInvertedBall
//------------------------------------------------------------------------------
CInvertedBall::CInvertedBall(void)
/*++

Routine Description:

	Constructor

Arguments:

	None

Return Value:

	None

--*/
{
	m_dontDraw	= TRUE;
	m_pVB		= NULL;
	m_pIB		= NULL;
}

//------------------------------------------------------------------------------
//	CInvertedBall::~CInvertedBall
//------------------------------------------------------------------------------
CInvertedBall::~CInvertedBall(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
	Release();
}

//------------------------------------------------------------------------------
//	CInvertedBall::Create
//------------------------------------------------------------------------------
void
CInvertedBall::Create(
					  IN IDirect3DDevice8*	pDevice,
					  IN float				radius,
					  IN int				numLong,
					  IN int				numLat
					  )
/*++

Routine Description:

	Creats a sphere whose center is at the origin with a given radius

Arguments:

	IN pDevice -	D3D Device
	IN radius -		Radius of sphere
	IN numLong -	Number of longitutinal lines
	IN numLat -		Num latitutinal lines

Return Value:

	None

--*/
{
	float		phi;
	float		rho;
	float		phiInc;
	float		rhoInc;
	int			p;
	int			r;
	int			index;
	VOID*		pVoid		= NULL;
	Vertex*		vertices	= NULL;
	Vertex*		newVerts	= NULL;
	WORD*		indices		= NULL;
	int			latitude1	= numLat - 1;
	int			latitude2	= numLat - 2;
	int			longitude1	= numLong - 1;
	int			p1l;
	int			pl;
	int			last;

	// Release previous buffers
	Release();

	// Setup
	m_radius = radius;

	// Num verts and tris
	m_numVertices	= latitude1 * numLong + 2;
	m_numTriangles	= (latitude1 * 2) * numLong;
	
	// Allocate vertices
	vertices = new Vertex [m_numVertices];

	// Increments
	phiInc	= (2.0f * PI) / (float)numLong;
	rhoInc	= PI / (float)numLat;

	// Calculate vertices
	vertices[0].normal		= D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	vertices[0].position	= D3DXVECTOR3(0.0f, m_radius, 0.0f);
	for(index=1, phi=0.0f, p=0; p<numLong; ++p, phi += phiInc) {
		for(rho=PI/2.0f - rhoInc, r=0; r<latitude1; ++r, rho-=rhoInc, ++index) {
			vertices[index].normal		= D3DXVECTOR3((float)(cos(phi) * fabs(cos(rho))), 
													  (float)sin(rho), 
													  -(float)(sin(phi) * fabs(cos(rho))));
			vertices[index].position	= D3DXVECTOR3(vertices[index].normal.x * m_radius, 
													  vertices[index].normal.y * m_radius, 
													  vertices[index].normal.z * m_radius);
			vertices[index].normal		= -vertices[index].normal;
		}
	}
	vertices[index].normal		= D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	vertices[index].position	= D3DXVECTOR3(0.0f, -m_radius, 0.0f);
	last = index;

	// Build the indices & real vertices
	m_numVertices	= m_numTriangles * 3;
	indices			= new WORD [m_numVertices];
	newVerts		= new Vertex [m_numVertices];
	for(index = 0, p=0; p<numLong; ++p) {

		// Shortcuts
		pl = p * latitude1 + 1;
		if(p != longitude1)
			p1l = (p+1) * latitude1 + 1;
		else
			p1l = 1;

		// Top triangle
		newVerts[index].tu			= 0.5f;
		newVerts[index].tv			= 0.0f;
		newVerts[index].normal		= vertices[0].normal;
		newVerts[index++].position	= vertices[0].position;
		newVerts[index].tu			= 0.0f;
		newVerts[index].tv			= 1.0f;
		newVerts[index].normal		= vertices[p1l].normal;
		newVerts[index++].position	= vertices[p1l].position;
		newVerts[index].tu			= 1.0f;
		newVerts[index].tv			= 1.0f;
		newVerts[index].normal		= vertices[pl].normal;
		newVerts[index++].position	= vertices[pl].position;

		// Middle triangles
		for(r=0; r<latitude2; ++r) {
			newVerts[index].tu			= 0.0f;
			newVerts[index].tv			= 0.0f;
			newVerts[index].normal		= vertices[pl + r].normal;
			newVerts[index++].position	= vertices[pl + r].position;
			newVerts[index].tu			= 1.0f;
			newVerts[index].tv			= 0.0f;
			newVerts[index].normal		= vertices[p1l + r].normal;
			newVerts[index++].position	= vertices[p1l + r].position;
			newVerts[index].tu			= 0.0f;
			newVerts[index].tv			= 1.0f;
			newVerts[index].normal		= vertices[pl + r + 1].normal;
			newVerts[index++].position	= vertices[pl + r + 1].position;

			newVerts[index].tu			= 0.0f;
			newVerts[index].tv			= 1.0f;
			newVerts[index].normal		= vertices[pl + r + 1].normal;
			newVerts[index++].position	= vertices[pl + r + 1].position;
			newVerts[index].tu			= 1.0f;
			newVerts[index].tv			= 0.0f;
			newVerts[index].normal		= vertices[p1l + r].normal;
			newVerts[index++].position	= vertices[p1l + r].position;
			newVerts[index].tu			= 1.0f;
			newVerts[index].tv			= 1.0f;
			newVerts[index].normal		= vertices[p1l + r + 1].normal;
			newVerts[index++].position	= vertices[p1l + r + 1].position;
		}

		// Bottom triangle
		newVerts[index].tu			= 0.0f;
		newVerts[index].tv			= 1.0f;
		newVerts[index].normal		= vertices[last].normal;
		newVerts[index++].position	= vertices[last].position;
		newVerts[index].tu			= 1.0f;
		newVerts[index].tv			= 1.0f;
		newVerts[index].normal		= vertices[pl + r].normal;
		newVerts[index++].position	= vertices[pl + r].position;
		newVerts[index].tu			= 0.5f;
		newVerts[index].tv			= 0.0f;
		newVerts[index].normal		= vertices[p1l + r].normal;
		newVerts[index++].position	= vertices[p1l + r].position;
	}
	delete [] vertices;
	for(p=0; p<m_numVertices; ++p)
		indices[p] = (WORD)p;

	// Setup the vertex buffer for the Ball
	if(FAILED(m_pDevice->CreateVertexBuffer(m_numVertices * sizeof(Vertex), 0, 
											XYZ_NORMAL_TEX, D3DPOOL_DEFAULT, 
											&m_pVB))) 
	{
		delete [] newVerts;
		delete [] indices;
		return;
	}

	if(FAILED(m_pVB->Lock(0, m_numVertices * sizeof(Vertex), (BYTE**)&pVoid, 0)))
	{
		Release();
		delete [] newVerts;
		delete [] indices;
		return;
	}

	CopyMemory(pVoid, newVerts, m_numVertices * sizeof(Vertex));
	m_pVB->Unlock();
	delete [] newVerts;

	// Setup the index buffer for the Ball
	if(FAILED(m_pDevice->CreateIndexBuffer(m_numVertices * sizeof(WORD),
										   D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
										   D3DPOOL_DEFAULT, &m_pIB))) 
	{
		Release();
		delete [] indices;
		return;
	}

	if(FAILED(m_pIB->Lock(0, m_numVertices * sizeof(WORD), (BYTE**)&pVoid, 0))) 
	{
		Release();
		delete [] indices;
		return;
	}

	CopyMemory(pVoid, indices, m_numVertices * sizeof(WORD));
	m_pIB->Unlock();
	delete [] indices;

	// Create the texture
	D3DXCreateTextureFromFileInMemory(m_pDevice, (LPCVOID)g_texture, 
									  g_sizeofTexture, &m_pTexture);
    if(!m_pTexture)
        return;

	m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	m_dontDraw = FALSE;
}

//------------------------------------------------------------------------------
//	CInvertedBall::SetPosition
//------------------------------------------------------------------------------
void 
CInvertedBall::SetPosition(
						   IN const D3DXVECTOR3& position
						   )
/*++

Routine Description:

	Sets the position of the ball

Arguments:

	IN position -	position

Return Value:

	None

--*/
{
	m_location = position;
	D3DXMatrixTranslation(&m_worldMatrix, m_location[0], m_location[1], 
						  m_location[2]);
	m_min	= D3DXVECTOR3(m_radius, m_radius, m_radius) + m_location;
	m_max	= D3DXVECTOR3(-m_radius, -m_radius, -m_radius) - m_location;
}

//------------------------------------------------------------------------------
//	CInvertedBall::GetRadius
//------------------------------------------------------------------------------
float 
CInvertedBall::GetRadius(void) const
/*++

Routine Description:

	Returns the radius of the ball

Arguments:

	None

Return Value:

	The radius

--*/
{
	return m_radius;
}

//------------------------------------------------------------------------------
//	CInvertedBall::GetMin
//------------------------------------------------------------------------------
const D3DXVECTOR3& 
CInvertedBall::GetMin(void) const
/*++

Routine Description:

	Returns the min coordinate of the Ball

Arguments:

	None

Return Value:

	The min coordinate of the Ball

--*/
{
	return m_min;
}

//------------------------------------------------------------------------------
//	CInvertedBall::GetMax
//------------------------------------------------------------------------------
const D3DXVECTOR3& 
CInvertedBall::GetMax(void) const
/*++

Routine Description:

	Returns the max coordinate of the Ball

Arguments:

	None

Return Value:

	The max coordinate of the Ball

--*/
{
	return m_max;
}

//------------------------------------------------------------------------------
//	CInvertedBall::Render
//------------------------------------------------------------------------------
void
CInvertedBall::Render(void)
/*++

Routine Description:

	Renders the Ball

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_dontDraw)
		return;

	// Vertex shader type
	m_pDevice->SetVertexShader(XYZ_NORMAL_TEX);

	// Set the translation
	m_pDevice->SetTransform(D3DTS_WORLD, &m_worldMatrix);

	// Enable texture
    m_pDevice->SetTexture(0, m_pTexture);

	// Set the material
	m_pDevice->SetMaterial(&g_material);

	// Set the vertex buffer stream source
	m_pDevice->SetStreamSource(0, m_pVB, sizeof(Vertex));

	// Draw the object
	m_pDevice->SetIndices(m_pIB, 0);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_numVertices, 0, 
									m_numTriangles);

	// Disable texture
    m_pDevice->SetTexture(0, NULL);
}

//------------------------------------------------------------------------------
//	CInvertedBall::GetNumVerts
//------------------------------------------------------------------------------
int 
CInvertedBall::GetNumVerts(void) const
/*++

Routine Description:

	Returns the number of vertices in the Ball

Arguments:

	None

Return Value:

	The number of vertices

--*/
{
	return m_numVertices;
}

//------------------------------------------------------------------------------
//	CInvertedBall::GetNumTris
//------------------------------------------------------------------------------
int 
CInvertedBall::GetNumTris(void) const
/*++

Routine Description:

	Returns the number of triangles in the Ball

Arguments:

	None

Return Value:

	number of triangles

--*/
{
	return m_numTriangles;
}

//------------------------------------------------------------------------------
//	CInvertedBall::Release
//------------------------------------------------------------------------------
void
CInvertedBall::Release(void)
/*++

Routine Description:

	Releases the Ball

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_pVB) {
		m_pVB->Release();
		m_pVB = NULL;
	}
	if(m_pIB) {
		m_pIB->Release();
		m_pIB = NULL;
	}
}
