//-----------------------------------------------------------------------------
// File: xobj.h
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __XOBJ_H
#define __XOBJ_H

#ifdef _XBOX
#include <xtl.h>
#include <XBMesh.h>
#else
#include <d3dx8.h>
#endif
#include "xmodel.h"

// object flags
#define OBJ_SYSFLAGSM	0xffff0000			// system flags mask
#define OBJ_USRFLAGSM	0x0000ffff			// user flags mask

#define OBJ_NOMCALCS	0x00010000			// dont do matrix calcs in render

//-----------------------------------------------------------------------------
// Object class
//-----------------------------------------------------------------------------
class CXObject
{
public:

	CXModel				*m_Model;			// model to render
#ifdef _XBOX
	CXBMesh				*m_pXBMesh;			// xbg mesh
#endif

	D3DXVECTOR3			m_vPosition;		// position
	D3DXVECTOR3			m_vRotation;		// rotation
	D3DXMATRIX			m_matOrientation;	// orientation matrix
	D3DMATERIAL8		*m_Material;		// lighting material

	DWORD				m_dwFlags;			// object flags
	DWORD				m_dwType;			// object type
	VOID				*m_pvInfo;			// info pointer

    CXObject();
    ~CXObject();

	void SetModel(CXModel *model);
	void SetXBMesh(CXBMesh *xbm);

	void SetPosition(float x, float y, float z)
	{
		m_vPosition.x = x;
		m_vPosition.y = y;
		m_vPosition.z = z;
	};

	void SetRotation(float x, float y, float z)
	{
		m_vRotation.x = x;
		m_vRotation.y = y;
		m_vRotation.z = z;
	};

	float GetRadius2();
	float GetRadius();

	void CrunchMatrix();
	void Render(int flags);
};

#endif