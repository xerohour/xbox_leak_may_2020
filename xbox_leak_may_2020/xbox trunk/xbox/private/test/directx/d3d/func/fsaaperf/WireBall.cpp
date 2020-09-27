/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WireBall.cpp

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
#include "WireBall.h"

//------------------------------------------------------------------------------
//	Defines:
//------------------------------------------------------------------------------
#define XYZ_NORMAL (D3DFVF_XYZ | D3DFVF_NORMAL)
#ifndef PI 
#	define PI 3.14159265359f
#endif

//------------------------------------------------------------------------------
//	CWireBall::CWireBall
//------------------------------------------------------------------------------
CWireBall::CWireBall(void)
/*++

Routine Description:

	Constructor

Arguments:

	None

Return Value:

	None

--*/
{
	m_dontDraw		= TRUE;
	m_pVB			= NULL;
	m_pLinesIB		= NULL;
}

//------------------------------------------------------------------------------
//	CWireBall::~CWireBall
//------------------------------------------------------------------------------
CWireBall::~CWireBall(void)
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
//	CWireBall::Create
//------------------------------------------------------------------------------
void
CWireBall::Create(
				  IN IDirect3DDevice8*	pDevice,
				  IN const D3DXVECTOR3&	location,
				  IN float				radius,
				  IN int				numLong,
				  IN int				numLat
				  )
/*++

Routine Description:

	Creats a sphere whose center is at the origin with a given radius

Arguments:

	IN pDevice -	D3D Device
	IN location -	Location of ball
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
	WORD*		indices		= NULL;
	int			latitude1	= numLat - 1;
	int			latitude2	= numLat - 2;
	int			longitude1	= numLong - 1;
	int			p1;
	int			p2;
	int			pl;
	int			p1l;
	int			last;

	// Release previous buffers
	Release();

	// Location
	m_location = location;
	D3DXMatrixTranslation(&m_worldMatrix, m_location[0], m_location[1], 
						  m_location[2]);

	// Radius
	m_radius = radius;

	// Initialize bounds
	m_min	= D3DXVECTOR3(m_radius, m_radius, m_radius) + location;
	m_max	= D3DXVECTOR3(-m_radius, -m_radius, -m_radius) - location;
	
	// Create a random color for the material
	do {
		m_material.Diffuse.r	= (float)rand() / (float)RAND_MAX;
		m_material.Diffuse.g	= (float)rand() / (float)RAND_MAX;
		m_material.Diffuse.b	= (float)rand() / (float)RAND_MAX;
	} while ((m_material.Diffuse.r + m_material.Diffuse.g + m_material.Diffuse.b) < 1.0f);
	m_material.Diffuse.a	= 1.0f;
	m_material.Ambient.r	= m_material.Diffuse.r * 0.4f;
	m_material.Ambient.g	= m_material.Diffuse.g * 0.4f;
	m_material.Ambient.b	= m_material.Diffuse.b * 0.4f;
	m_material.Ambient.a	= 1.0f;
	m_material.Specular.r	= 1.0f;
	m_material.Specular.g	= 1.0f;
	m_material.Specular.b	= 1.0f;
	m_material.Specular.a	= 1.0f;
	m_material.Emissive.r	= 0.0f;
	m_material.Emissive.g	= 0.0f;
	m_material.Emissive.b	= 0.0f;
	m_material.Emissive.a	= 0.0f;
	m_material.Power		= 100.0f;

	// Num verts and lines
	m_numVertices	= latitude1 * numLong + 2;
	m_numLines		= numLat * numLong + (latitude1 * (numLong + 1));
	
	// Allocate vertices
	vertices = new Vertex [m_numVertices];

	// Increments
	phiInc	= (2.0f * PI) / (float)numLong;
	rhoInc	= PI / (float)numLat;

	// Calculate vertices
	vertices[0].normal		= D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	vertices[0].position	= D3DXVECTOR3(0.0f, m_radius, 0.0f);
	for(index=1, phi=0.0f, p=0; p<numLong; ++p, phi += phiInc) {
		for(rho=PI/2.0f - rhoInc, r=0; r<latitude1; ++r, rho-=rhoInc, ++index) {
			vertices[index].normal		= D3DXVECTOR3((float)(cos(phi) * fabs(cos(rho))), 
													  (float)sin(rho), 
													  -(float)(sin(phi) * fabs(cos(rho))));
			vertices[index].position	= D3DXVECTOR3(vertices[index].normal.x * m_radius, 
													  vertices[index].normal.y * m_radius, 
													  vertices[index].normal.z * m_radius);
		}
	}
	vertices[index].normal		= D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	vertices[index].position	= D3DXVECTOR3(0.0f, -m_radius, 0.0f);
	last = index;

	// Setup the vertex buffer for the Ball
	if(FAILED(m_pDevice->CreateVertexBuffer(m_numVertices * sizeof(Vertex), 0, 
											XYZ_NORMAL, D3DPOOL_DEFAULT, &m_pVB))) 
	{
		delete [] vertices;
		return;
	}

	if(FAILED(m_pVB->Lock(0, m_numVertices * sizeof(Vertex), (BYTE**)&pVoid, 0)))
	{
		Release();
		delete [] vertices;
		return;
	}

	CopyMemory(pVoid, vertices, m_numVertices * sizeof(Vertex));
	m_pVB->Unlock();
	delete [] vertices;

	// Build the indices
	indices = new WORD [m_numLines + 1];
	for(index = 0, p=0; p<numLong/2; ++p) {

		// Shortcuts
		p1 = p * latitude1 + 1;
		p2 = ((p + numLong/2) * latitude1) + latitude1;

		indices[index++] = 0;
		for(r=0; r<latitude1; ++r) 
			indices[index++] = (WORD)(p1++);
		indices[index++] = (WORD)last;
		for(r=0; r<latitude1; ++r)
			indices[index++] = (WORD)(p2--);
	}
	indices[index++] = 0;

	for(p=0; p<latitude1; ++p) {
		for(r=0; r<numLong; ++r) 
			indices[index++] = r * (latitude1) + p + 1;
		indices[index++] = p + 1;
	}

	// Setup the index buffer for the Ball
	if(FAILED(m_pDevice->CreateIndexBuffer((m_numLines + 1) * sizeof(WORD),
										   D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
										   D3DPOOL_DEFAULT, &m_pLinesIB))) 
	{
		Release();
		delete [] indices;
		return;
	}

	if(FAILED(m_pLinesIB->Lock(0, (m_numLines + 1) * sizeof(WORD), 
			  (BYTE**)&pVoid, 0))) 
	{
		Release();
		delete [] indices;
		return;
	}

	CopyMemory(pVoid, indices, (m_numLines + 1) * sizeof(WORD));
	m_pLinesIB->Unlock();
	delete [] indices;

	m_dontDraw = FALSE;
}

//------------------------------------------------------------------------------
//	CWireBall::Render
//------------------------------------------------------------------------------
void
CWireBall::Render(void)
/*++

Routine Description:

	Renders the Ball

Arguments:

	IN wireframe -	TRUE to render wireframe

Return Value:

	None

--*/
{
	if(m_dontDraw)
		return;

	// Vertex shader type
	m_pDevice->SetVertexShader(XYZ_NORMAL);

	// Set the translation
	m_pDevice->SetTransform(D3DTS_WORLD, &m_worldMatrix);

	// Set the vertex buffer stream source
	m_pDevice->SetStreamSource(0, m_pVB, sizeof(Vertex));

	// Draw the lines
	m_pDevice->SetMaterial(&m_material);
	m_pDevice->SetIndices(m_pLinesIB, 0);
	m_pDevice->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, m_numVertices, 0, 
									m_numLines);
}

//------------------------------------------------------------------------------
//	CWireBall::GetMin
//------------------------------------------------------------------------------
const D3DXVECTOR3& 
CWireBall::GetMin(void) const
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
//	CWireBall::GetMax
//------------------------------------------------------------------------------
const D3DXVECTOR3& 
CWireBall::GetMax(void) const
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
//	CWireBall::GetNumVerts
//------------------------------------------------------------------------------
int 
CWireBall::GetNumVerts(void) const
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
//	CWireBall::GetNumLines
//------------------------------------------------------------------------------
int 
CWireBall::GetNumLines(void) const
/*++

Routine Description:

	Returns the number of line segments in the Ball

Arguments:

	None

Return Value:

	number of line segments

--*/
{
	return m_numLines;
}

//------------------------------------------------------------------------------
//	CWireBall::GetLocation
//------------------------------------------------------------------------------
const D3DXVECTOR3&
CWireBall::GetLocation(void) const
/*++

Routine Description:

	Returns the location of the ball

Arguments:

	None

Return Value:

	The location of the ball

--*/
{
	return m_location;
}

//------------------------------------------------------------------------------
//	CWireBall::Release
//------------------------------------------------------------------------------
void
CWireBall::Release(void)
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
	if(m_pLinesIB) {
		m_pLinesIB->Release();
		m_pLinesIB = NULL;
	}
}
