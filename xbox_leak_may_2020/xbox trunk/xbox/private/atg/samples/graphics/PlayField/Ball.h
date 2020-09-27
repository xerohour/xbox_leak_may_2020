#pragma once
//-----------------------------------------------------------------------------
//  File: Ball.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//-----------------------------------------------------------------------------
#include <XBMesh.h>

extern D3DXVECTOR3 g_vGravity;

class CBall {
public:
	CXBMesh			m_mesh;
	D3DXVECTOR3		m_vPosition;
	D3DXVECTOR3		m_vVelocity;
	D3DXMATRIX		m_matRoll;
	D3DXMATRIX		m_matWorld;
	float m_fGrassRestFraction;	// distance within grass that force of grass resistance equals gravity
	float m_fBallRadius;
	float m_fBallMass;
	bool m_bShadowed;
	D3DCOLOR	m_rcolorSave[4];
	D3DXVECTOR3	m_vPositionShadowed;
public:
	HRESULT Initialize();
	HRESULT Render();
	HRESULT LaunchBall(const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTargetPosition);
	HRESULT MoveBall(float dt);
};

