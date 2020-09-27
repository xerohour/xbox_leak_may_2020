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
#include <xtl.h>
#include <xdbg.h>
#include "InvertedBall.h"
#include "helpers.h"

using namespace WFVO;

namespace WFVO {

//------------------------------------------------------------------------------
//	Defines:
//------------------------------------------------------------------------------
#define XYZ_NORMAL_TEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#ifndef PI 
#	define PI 3.14159265359f
#endif

//------------------------------------------------------------------------------
//	Static member initialization
//------------------------------------------------------------------------------

UINT CInvertedBall::m_uRef = 0;
IDirect3DTexture8* CInvertedBall::m_pd3dtBase = NULL;

//------------------------------------------------------------------------------
//	Globals
//------------------------------------------------------------------------------
D3DMATERIAL8 g_material = {
	{0.5f, 0.5f, 0.5f, 1.0f},
	{0.25f, 0.25f, 0.25f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	{0.0f, 0.0f, 0.0f, 0.0f},
	0.0f
};

D3DMATERIAL8 g_material2 = {
	{0.5f, 0.5f, 0.5f, 1.0f},
	{0.0f, 0.0f, 0.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
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
    m_uRef++;
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

    if (--m_uRef == 0) {
        if (m_pd3dtBase) {
            m_pd3dtBase->Release();
            m_pd3dtBase = NULL;
        }
    }
}

//------------------------------------------------------------------------------
//	CInvertedBall::Create
//------------------------------------------------------------------------------
HRESULT
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

	S_OK on success, any other value on failure

--*/
{
	HRESULT		hr;
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
	ASSERT(vertices != NULL);

	// Increments
	phiInc	= (2.0f * PI) / (float)numLong;
	rhoInc	= PI / (float)numLat;

	// Calculate vertices
	vertices[0].normal		= XGVECTOR3(0.0f, -1.0f, 0.0f);
	vertices[0].position	= XGVECTOR3(0.0f, m_radius, 0.0f);
    vertices[0].tu          = 0.0f;
    vertices[0].tv          = 0.0f;
	for(index=1, phi=0.0f, p=0; p<numLong; ++p, phi += phiInc) 
	{
		for(rho=PI/2.0f - rhoInc, r=0; r<latitude1; ++r, rho-=rhoInc, ++index) 
		{
			vertices[index].normal		= XGVECTOR3((float)(cos(phi) * fabs(cos(rho))), 
													  (float)sin(rho), 
													  -(float)(sin(phi) * fabs(cos(rho))));
			vertices[index].position	= XGVECTOR3(vertices[index].normal.x * m_radius, 
													  vertices[index].normal.y * m_radius, 
													  vertices[index].normal.z * m_radius);
			vertices[index].normal		= -vertices[index].normal;
            vertices[index].tu          = (float)p / (float)(numLong - 1) * 4.0f;
            vertices[index].tv          = (float)r / (float)(latitude1 - 1) * 3.0f;
		}
	}
	vertices[index].normal		= XGVECTOR3(0.0f, 1.0f, 0.0f);
	vertices[index].position	= XGVECTOR3(0.0f, -m_radius, 0.0f);
    vertices[index].tu          = 0.0f;
    vertices[index].tv          = 3.0f;
	last = index;

	// Build the indices & real vertices
	m_numVertices	= m_numTriangles * 3;

	indices = new WORD [m_numVertices];
	ASSERT(indices != NULL);

	newVerts = new Vertex [m_numVertices];
	ASSERT(newVerts != NULL);

	for(index = 0, p=0; p<numLong; ++p) 
	{
		// Shortcuts
		pl = p * latitude1 + 1;
		if(p != longitude1)
			p1l = (p+1) * latitude1 + 1;
		else
			p1l = 1;

		// Top triangle
		newVerts[index].tu			= vertices[0].tu;//0.5f;
		newVerts[index].tv			= vertices[0].tv;//0.0f;
		newVerts[index].normal		= vertices[0].normal;
		newVerts[index++].position	= vertices[0].position;
		newVerts[index].tu			= vertices[p1l].tu;//0.0f;
		newVerts[index].tv			= vertices[p1l].tv;//1.0f;
		newVerts[index].normal		= vertices[p1l].normal;
		newVerts[index++].position	= vertices[p1l].position;
		newVerts[index].tu			= vertices[pl].tu;//1.0f;
		newVerts[index].tv			= vertices[pl].tv;//1.0f;
		newVerts[index].normal		= vertices[pl].normal;
		newVerts[index++].position	= vertices[pl].position;

		// Middle triangles
		for(r=0; r<latitude2; ++r) 
		{
			newVerts[index].tu			= vertices[pl + r].tu;//0.0f;
			newVerts[index].tv			= vertices[pl + r].tv;//0.0f;
			newVerts[index].normal		= vertices[pl + r].normal;
			newVerts[index++].position	= vertices[pl + r].position;
			newVerts[index].tu			= vertices[p1l + r].tu;//1.0f;
			newVerts[index].tv			= vertices[p1l + r].tv;//0.0f;
			newVerts[index].normal		= vertices[p1l + r].normal;
			newVerts[index++].position	= vertices[p1l + r].position;
			newVerts[index].tu			= vertices[pl + r + 1].tu;//0.0f;
			newVerts[index].tv			= vertices[pl + r + 1].tv;//1.0f;
			newVerts[index].normal		= vertices[pl + r + 1].normal;
			newVerts[index++].position	= vertices[pl + r + 1].position;

			newVerts[index].tu			= vertices[pl + r + 1].tu;//0.0f;
			newVerts[index].tv			= vertices[pl + r + 1].tv;//1.0f;
			newVerts[index].normal		= vertices[pl + r + 1].normal;
			newVerts[index++].position	= vertices[pl + r + 1].position;
			newVerts[index].tu			= vertices[p1l + r].tu;//1.0f;
			newVerts[index].tv			= vertices[p1l + r].tv;//0.0f;
			newVerts[index].normal		= vertices[p1l + r].normal;
			newVerts[index++].position	= vertices[p1l + r].position;
			newVerts[index].tu			= vertices[p1l + r + 1].tu;//1.0f;
			newVerts[index].tv			= vertices[p1l + r + 1].tv;//1.0f;
			newVerts[index].normal		= vertices[p1l + r + 1].normal;
			newVerts[index++].position	= vertices[p1l + r + 1].position;
		}

		// Bottom triangle
		newVerts[index].tu			= vertices[last].tu;//0.0f;
		newVerts[index].tv			= vertices[last].tv;//1.0f;
		newVerts[index].normal		= vertices[last].normal;
		newVerts[index++].position	= vertices[last].position;
		newVerts[index].tu			= vertices[pl + r].tu;//1.0f;
		newVerts[index].tv			= vertices[pl + r].tv;//1.0f;
		newVerts[index].normal		= vertices[pl + r].normal;
		newVerts[index++].position	= vertices[pl + r].position;
		newVerts[index].tu			= vertices[p1l + r].tu;//0.5f;
		newVerts[index].tv			= vertices[p1l + r].tv;//0.0f;
		newVerts[index].normal		= vertices[p1l + r].normal;
		newVerts[index++].position	= vertices[p1l + r].position;
	}
	delete [] vertices;
	for(p=0; p<m_numVertices; ++p)
		indices[p] = (WORD)p;

	// Setup the vertex buffer for the Ball
	hr = pDevice->CreateVertexBuffer(m_numVertices * sizeof(Vertex), 0, 
									 XYZ_NORMAL_TEX, D3DPOOL_DEFAULT, 
									 &m_pVB);
	if(FAILED(hr)) 
	{
		delete [] newVerts;
		delete [] indices;
		return hr;
	}

	hr = m_pVB->Lock(0, m_numVertices * sizeof(Vertex), (BYTE**)&pVoid, 0);
	if(FAILED(hr))
	{
		Release();
		delete [] newVerts;
		delete [] indices;
		return hr;
	}

	CopyMemory(pVoid, newVerts, m_numVertices * sizeof(Vertex));
	m_pVB->Unlock();
	delete [] newVerts;

	// Setup the index buffer for the Ball
	hr = pDevice->CreateIndexBuffer(m_numVertices * sizeof(WORD),
									D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
									D3DPOOL_DEFAULT, &m_pIB);
	if(FAILED(hr)) 
	{
		Release();
		delete [] indices;
		return hr;
	}

	hr = m_pIB->Lock(0, m_numVertices * sizeof(WORD), (BYTE**)&pVoid, 0);
	if(FAILED(hr)) 
	{
		Release();
		delete [] indices;
		return hr;
	}

	CopyMemory(pVoid, indices, m_numVertices * sizeof(WORD));
	m_pIB->Unlock();
	delete [] indices;

	// Create the texture
    if (!m_pd3dtBase) {
#if 1
        hr = D3DXCreateTextureFromFileEx(pDevice, g_TestParams.szSphereBaseTex, D3DX_DEFAULT, D3DX_DEFAULT, 
                                    D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, 0, D3DX_FILTER_TRIANGLE,
                                    D3DX_FILTER_TRIANGLE, 0, NULL, NULL, &m_pd3dtBase);
#else
        HANDLE hFile;
        DWORD dwSize, dwRead;
        LPBYTE pData;

        hFile = CreateFile(g_TestParams.szSphereBaseTex, GENERIC_READ, 
                                    FILE_SHARE_READ, NULL, OPEN_EXISTING, 
                                    FILE_ATTRIBUTE_NORMAL, NULL);

        if (hFile == INVALID_HANDLE_VALUE) {
            return E_FAIL;
        }

        // Get the size of the file
        dwSize = GetFileSize(hFile, NULL);
        if (dwSize == 0xFFFFFFFF) {
            CloseHandle(hFile);
            return E_FAIL;
        }

        // Allocate a buffer for the file data
        pData = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, dwSize);
        if (!pData) {
            CloseHandle(hFile);
            return E_OUTOFMEMORY;
        }                

        // Read the file into the buffer
        if (!ReadFile(hFile, pData, dwSize, &dwRead, NULL)
            || dwSize != dwRead)
        {
            HeapFree(GetProcessHeap(), 0, pData);
            CloseHandle(hFile);
            return E_FAIL;
        }

        // Close the file
        CloseHandle(hFile);

        hr = D3DXCreateTextureFromFileInMemory(pDevice, pData, dwSize, &m_pd3dtBase);

        HeapFree(GetProcessHeap(), 0, pData);
#endif

        if (FAILED(hr)) {
            return hr;
        }
    }

	pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	m_dontDraw = FALSE;
	return S_OK;
}

//------------------------------------------------------------------------------
//	CInvertedBall::SetPosition
//------------------------------------------------------------------------------
void 
CInvertedBall::SetPosition(
						   IN const XGVECTOR3& position
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
	XGMatrixTranslation(&m_worldMatrix, m_location[0], m_location[1], 
						  m_location[2]);
	m_min	= XGVECTOR3(m_radius, m_radius, m_radius) + m_location;
	m_max	= XGVECTOR3(-m_radius, -m_radius, -m_radius) - m_location;
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
const XGVECTOR3& 
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
const XGVECTOR3& 
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
CInvertedBall::Render(
					  IN IDirect3DDevice8*	pDevice,
                      IN UINT               uPass,
					  IN BOOL				bDrawWireframe
					  )
/*++

Routine Description:

	Renders the Ball

Arguments:

	IN pDevice -		IDirect3DDevice8
	IN uPass -			Render Pass
	IN bDrawWireframe -	TRUE to render a wireframe shell

Return Value:

	None

--*/
{
	if(m_dontDraw)
		return;

	// Vertex shader type
	pDevice->SetVertexShader(XYZ_NORMAL_TEX);

	// Set the translation
	pDevice->SetTransform(D3DTS_WORLD, &m_worldMatrix);

	// Enable texture
    pDevice->SetTexture(0, m_pd3dtBase);

	// Set the material
    if (uPass) {
    	pDevice->SetMaterial(&g_material2);
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    }
    else {
    	pDevice->SetMaterial(&g_material);
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    }

    pDevice->SetRenderState(D3DRS_WRAP0, D3DWRAP_U);

	// Set the vertex buffer stream source
	pDevice->SetStreamSource(0, m_pVB, sizeof(Vertex));

	// Make sure it draws solid
	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	// Draw the object
	pDevice->SetIndices(m_pIB, 0);
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_numVertices, 0, 
								  m_numTriangles);

    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_WRAP0, 0);

	// Disable texture
    pDevice->SetTexture(0, NULL);

	if(bDrawWireframe)
	{
		// Draw backfacing wireframe
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

		// Draw the object
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_numVertices, 0, 
									  m_numTriangles);

		// Restore the modes
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
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
}
