/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	InvertedBall.cpp

Abstract:

	A 3d ball

Author:

	Robert Heitkamp (robheit) 5-Feb-2001

Revision History:

	5-Feb-2001	robheit
		Initial Version

--*/

#ifndef __INVERTEDBALL_H__
#define __INVERTEDBALL_H__

//------------------------------------------------------------------------------
//	Includes
//------------------------------------------------------------------------------
#include "xtl.h"
#include "xgmath.h"

namespace Media4 {

//------------------------------------------------------------------------------
//	CInvertedBall
//------------------------------------------------------------------------------
class CInvertedBall
{
public:

	struct Vertex
	{
		XGVECTOR3	position;
		XGVECTOR3	normal;
		float		tu, tv;
	};

public:

	CInvertedBall(void);
	virtual ~CInvertedBall(void);
	
	HRESULT Create(IDirect3DDevice8*, float, int, int);
	void SetPosition(const XGVECTOR3&);
	const XGVECTOR3& GetMin(void) const;
	const XGVECTOR3& GetMax(void) const;
	void Render(IDirect3DDevice8*, UINT, BOOL);

	int GetNumVerts(void) const;
	int GetNumTris(void) const;
	float GetRadius(void) const;

private:

	void Release(void);

private:

	IDirect3DVertexBuffer8*	m_pVB;
	int						m_numVertices;
	IDirect3DIndexBuffer8*	m_pIB;
	int						m_numTriangles;
	BOOL					m_dontDraw;
	float					m_radius;
	XGMATRIX				m_worldMatrix;
	XGVECTOR3				m_location;
	XGVECTOR3				m_min;
	XGVECTOR3				m_max;

    static UINT             m_uRef;
    static IDirect3DTexture8* m_pd3dtBase;
};
}
#endif
