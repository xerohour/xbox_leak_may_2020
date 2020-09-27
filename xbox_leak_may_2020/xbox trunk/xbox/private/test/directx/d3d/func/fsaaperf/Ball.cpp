/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Ball.cpp

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
#include "Ball.h"

//------------------------------------------------------------------------------
//	Defines:
//------------------------------------------------------------------------------
#define XYZ_NORMAL (D3DFVF_XYZ | D3DFVF_NORMAL)
#ifndef PI 
#	define PI 3.14159265359f
#endif

//------------------------------------------------------------------------------
//	CBall::CBall
//------------------------------------------------------------------------------
CBall::CBall(void)
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
//	CBall::~CBall
//------------------------------------------------------------------------------
CBall::~CBall(void)
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
//	CBall::Create
//------------------------------------------------------------------------------
void
CBall::Create(
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

	// Num verts and tris
	m_numVertices	= latitude1 * numLong + 2;
	m_numTriangles	= (latitude1 * 2) * numLong;
	
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

	// Build the indices
	indices = new WORD [m_numTriangles * 3];
	for(index = 0, p=0; p<numLong; ++p) {

		// Shortcuts
		pl = p * latitude1 + 1;
		if(p != longitude1)
			p1l = (p+1) * latitude1 + 1;
		else
			p1l = 1;

		// Top triangle
		indices[index++] = 0;
		indices[index++] = (WORD)pl;
		indices[index++] = (WORD)p1l;

		// Middle triangles
		for(r=0; r<latitude2; ++r) {
			indices[index++] = (WORD)(pl + r);
			indices[index++] = (WORD)(pl + r + 1);
			indices[index++] = (WORD)(p1l + r);

			indices[index++] = (WORD)(pl + r + 1);
			indices[index++] = (WORD)(p1l + r + 1);
			indices[index++] = (WORD)(p1l + r);
		}

		// Bottom triangle
		indices[index++] = (WORD)(last);
		indices[index++] = (WORD)(p1l + r);
		indices[index++] = (WORD)(pl + r);
	}

	// Setup the vertex buffer for the Ball
	if(FAILED(m_pDevice->CreateVertexBuffer(m_numVertices * sizeof(Vertex), 0, 
											XYZ_NORMAL, D3DPOOL_DEFAULT, &m_pVB))) 
	{
		delete [] vertices;
		delete [] indices;
		return;
	}

	if(FAILED(m_pVB->Lock(0, m_numVertices * sizeof(Vertex), (BYTE**)&pVoid, 0)))
	{
		Release();
		delete [] vertices;
		delete [] indices;
		return;
	}

	CopyMemory(pVoid, vertices, m_numVertices * sizeof(Vertex));
	m_pVB->Unlock();
	delete [] vertices;

	// Setup the index buffer for the Ball
	if(FAILED(m_pDevice->CreateIndexBuffer(m_numTriangles * 3 * sizeof(WORD),
										   D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
										   D3DPOOL_DEFAULT, &m_pIB))) 
	{
		Release();
		delete [] indices;
		return;
	}

	if(FAILED(m_pIB->Lock(0, m_numTriangles * 3 * sizeof(WORD), (BYTE**)&pVoid, 0))) 
	{
		Release();
		delete [] indices;
		return;
	}

	CopyMemory(pVoid, indices, m_numTriangles * 3 * sizeof(WORD));
	m_pIB->Unlock();
	delete [] indices;
	m_dontDraw = FALSE;
}

//------------------------------------------------------------------------------
//	CBall::SetPosition
//------------------------------------------------------------------------------
void 
CBall::SetPosition(
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
//	CBall::SetDirection
//------------------------------------------------------------------------------
void 
CBall::SetDirection(
					IN const D3DXVECTOR3& direction
					)
/*++

Routine Description:

	Sets the direction of the ball

Arguments:

	IN direction -	Direction

Return Value:

	None

--*/
{
	m_direction = direction;
}

//------------------------------------------------------------------------------
//	CBall::SetSpeed
//------------------------------------------------------------------------------
void 
CBall::SetSpeed(
				IN float speed
				)
				/*++

Routine Description:

	Sets the speed of the ball

Arguments:

	IN speed -	Speed

Return Value:

	None

--*/
{
	m_speed = speed;
}

//------------------------------------------------------------------------------
//	CBall::GetSpeed
//------------------------------------------------------------------------------
float 
CBall::GetSpeed(void) const
/*++

Routine Description:

	Returns the speed of the ball

Arguments:

	None

Return Value:

	The speed of the ball

--*/
{
	return m_speed;
}

//------------------------------------------------------------------------------
//	CBall::GetDirection
//------------------------------------------------------------------------------
const D3DXVECTOR3& 
CBall::GetDirection(void) const
/*++

Routine Description:

	Returns the direction of the ball

Arguments:

	None

Return Value:

	Direction of the ball

--*/
{
	return m_direction;
}

//------------------------------------------------------------------------------
//	CBall::Move
//------------------------------------------------------------------------------
void
CBall::Move(
			IN float amount
			)
/*++

Routine Description:

	Updates the position of the ball by moving it amount

Arguments:

	IN amount -	Amount to move

Return Value:

	None

--*/
{
	D3DXVECTOR3	da = m_direction * amount;

	m_location += da;
	m_worldMatrix(3,0) += da.x;
	m_worldMatrix(3,1) += da.y;
	m_worldMatrix(3,2) += da.z;
	m_min = D3DXVECTOR3(m_location.x - m_radius, m_location.y - m_radius, m_location.z - m_radius);
	m_max = D3DXVECTOR3(m_location.x + m_radius, m_location.y + m_radius, m_location.z + m_radius);
}

//------------------------------------------------------------------------------
//	CBall::GetRadius
//------------------------------------------------------------------------------
float 
CBall::GetRadius(void) const
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
//	CBall::Render
//------------------------------------------------------------------------------
void
CBall::Render(void)
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
	m_pDevice->SetVertexShader(XYZ_NORMAL);

	// Set the translation
	m_pDevice->SetTransform(D3DTS_WORLD, &m_worldMatrix);

	// Set the material
	m_pDevice->SetMaterial(&m_material);

	// Set the vertex buffer stream source
	m_pDevice->SetStreamSource(0, m_pVB, sizeof(Vertex));


	// Draw the object
	m_pDevice->SetIndices(m_pIB, 0);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_numVertices, 0, 
									m_numTriangles);
}

//------------------------------------------------------------------------------
//	CBall::GetMin
//------------------------------------------------------------------------------
const D3DXVECTOR3& 
CBall::GetMin(void) const
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
//	CBall::GetMax
//------------------------------------------------------------------------------
const D3DXVECTOR3& 
CBall::GetMax(void) const
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
//	CBall::GetNumVerts
//------------------------------------------------------------------------------
int 
CBall::GetNumVerts(void) const
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
//	CBall::GetNumTris
//------------------------------------------------------------------------------
int 
CBall::GetNumTris(void) const
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
//	CBall::GetLocation
//------------------------------------------------------------------------------
const D3DXVECTOR3&
CBall::GetLocation(void) const
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
//	CBall::Release
//------------------------------------------------------------------------------
void
CBall::Release(void)
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
