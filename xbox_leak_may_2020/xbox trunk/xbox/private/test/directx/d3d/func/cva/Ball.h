/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Ball.cpp

Abstract:

	A 3d ball

Author:

	Robert Heitkamp (robheit) 5-Feb-2001

Revision History:

	5-Feb-2001	robheit
		Initial Version

--*/

#ifndef __BALL_H__
#define __BALL_H__

//------------------------------------------------------------------------------
//	Includes
//------------------------------------------------------------------------------
#include "d3dlocus.h"

//------------------------------------------------------------------------------
//	CBall
//------------------------------------------------------------------------------
class CBall
{
public:

	CBall(void);
	virtual ~CBall(void);
	
	void Create(IDirect3DDevice8*, int, int, float, int, int);
	void SetPosition(const D3DXVECTOR3&);
	void SetDirection(const D3DXVECTOR3&);
	void SetSpeed(float);
	float GetSpeed(void) const;
	const D3DXVECTOR3& GetDirection(void) const;
	void Move(float);
	void Render(void);

	const D3DXVECTOR3& GetMin(void) const;
	const D3DXVECTOR3& GetMax(void) const;
	int GetNumVerts(void) const;
	int GetNumTris(void) const;
	float GetRadius(void) const;
	const D3DXVECTOR3& GetLocation(void) const;
	void SetColor(float, float, float);

private:

	void Release(void);
	short ToShort(float, float);
	BYTE ToOffsetBYTE(float, float);
	
private:

	IDirect3DDevice8*		m_pDevice;
	D3DMATERIAL8			m_material;
	IDirect3DVertexBuffer8*	m_pVB;
	int						m_numVertices;
	IDirect3DIndexBuffer8*	m_pIB;
	int						m_numTriangles;
	D3DXVECTOR3				m_min;
	D3DXVECTOR3				m_max;
	BOOL					m_dontDraw;
	D3DXMATRIX				m_worldMatrix;
	D3DXVECTOR3				m_direction;
	float					m_speed;
	float					m_radius;
	D3DXVECTOR3				m_location;
	DWORD					m_fvf;
	D3DXMATRIX*				m_matrices;
	BOOL					m_drawTris;
	DWORD					m_vertexShader;
	UINT					m_vertexSize;

};

#endif
