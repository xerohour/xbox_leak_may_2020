/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WireBall.cpp

Abstract:

	A 3d ball

Author:

	Robert Heitkamp (robheit) 5-Feb-2001

Revision History:

	5-Feb-2001	robheit
		Initial Version

--*/

#ifndef __WIREBALL_H__
#define __WIREBALL_H__

//------------------------------------------------------------------------------
//	Includes
//------------------------------------------------------------------------------
#include "d3dlocus.h"

//------------------------------------------------------------------------------
//	CWireBall
//------------------------------------------------------------------------------
class CWireBall
{
public:

	struct Vertex
	{
		D3DXVECTOR3	position;
		D3DXVECTOR3	normal;
	};

public:

	CWireBall(void);
	virtual ~CWireBall(void);
	
	void Create(IDirect3DDevice8*, const D3DXVECTOR3&, float, int, int);
	void Render(void);

	const D3DXVECTOR3& GetMin(void) const;
	const D3DXVECTOR3& GetMax(void) const;
	int GetNumVerts(void) const;
	int GetNumLines(void) const;
	const D3DXVECTOR3& GetLocation(void) const;

private:

	void Release(void);

private:

	IDirect3DDevice8*		m_pDevice;
	D3DMATERIAL8			m_material;
	IDirect3DVertexBuffer8*	m_pVB;
	int						m_numVertices;
	IDirect3DIndexBuffer8*	m_pLinesIB;
	int						m_numLines;
	int						m_numTriangles;
	D3DXVECTOR3				m_min;
	D3DXVECTOR3				m_max;
	BOOL					m_dontDraw;
	D3DXMATRIX				m_worldMatrix;
	float					m_radius;
	D3DXVECTOR3				m_location;
	IDirect3DTexture8*		m_pTexture;

};

#endif
