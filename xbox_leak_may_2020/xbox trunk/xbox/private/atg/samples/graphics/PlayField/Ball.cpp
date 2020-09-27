//-----------------------------------------------------------------------------
//  File: Ball.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xgraphics.h>
#include <XbApp.h>
#include "Ball.h"

D3DXVECTOR3	g_vGravity = D3DXVECTOR3(0.f, -9.8f, 0.f);
extern HRESULT GetTerrainColors(const D3DXVECTOR3 &vPosition, D3DCOLOR *rColor);
extern HRESULT SetTerrainColors(const D3DXVECTOR3 &vPosition, D3DCOLOR *rColor);
extern HRESULT GetTerrainPoint(const D3DXVECTOR3 &vPosition, float *pfGrassThickness, float *pfHeight);
extern HRESULT MarkTerrain(const D3DXVECTOR3 &vPosition, 
						   const D3DXCOLOR &colorScale, 
						   const D3DXCOLOR &colorOffset, 
						   const D3DXCOLOR &colorMin, 
						   D3DCOLOR *rColorPrevious);	// 2x2 array of previous colors

HRESULT CBall::Initialize()
{
	// Set simulation parameters
	m_fGrassRestFraction = 1.f/2.f;
	m_fBallRadius = 0.05f/2.f; // meters, current art. Actual golf ball should be 0.042672/2 radius (1.68 inches in diameter)
	m_fBallMass = 0.0460227f; // 1.62 ounces * 1 pound / 16 ounces * 1 kg / 2.2 pound
	D3DXMatrixIdentity(&m_matRoll);
	m_bShadowed = false;
	
	// Load the ball mesh
	if( FAILED( m_mesh.Create( g_pd3dDevice, "Models\\GBall.xbg" ) ) )
		return XBAPPERR_MEDIANOTFOUND;
	
	D3DXVECTOR3 vFrom(100.f, 10.f, 0.f);
	D3DXVECTOR3 vTarget(0.f, 0.f, 0.f);
	return LaunchBall(vFrom, vTarget);
}

//-----------------------------------------------------------------------------
// Name: Render
// Desc: Draw the ball
//-----------------------------------------------------------------------------
HRESULT CBall::Render()
{
    g_pd3dDevice->SetPixelShader( 0 );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); 
    g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE ); 
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetTexture( 0, NULL );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	g_pd3dDevice->SetTexture( 1, NULL );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	g_pd3dDevice->SetTexture( 2, NULL );
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	g_pd3dDevice->SetTexture( 3, NULL );
    g_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
	m_mesh.Render( g_pd3dDevice, XBMESH_NOTEXTURES );
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: LaunchBall
// Desc: Fire new ball in a random direction
//-----------------------------------------------------------------------------
HRESULT CBall::LaunchBall(const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTargetPosition)
{
	// Pick a random velocity and position
#define fUnitRand(fmin, fmax) ((float)rand()/32768.0f*((fmax) - (fmin)) + (fmin))
	float fTheta = 0.7f * D3DX_PI * fUnitRand(-0.5f, 0.5f);
	float fRadius = 1.f;
	float fX = cosf(fTheta);
	float fZ = sinf(fTheta);
	m_vVelocity = D3DXVECTOR3(-fX*fUnitRand(2.f, 10.f), fUnitRand(0.5f, 2.5f), fZ*fUnitRand(2.f, 10.f));
	m_vPosition = vFrom + fRadius * D3DXVECTOR3(fX * fUnitRand(-0.5f, 0.5f), fUnitRand(0.f, 1.f), fZ * fUnitRand(-0.5f, 0.5f));
	
	// Solve for time when ball is going to hit z=0 plane
	float a = 0.5f * g_vGravity.y;
	float b = m_vVelocity.y;
	float c = m_vPosition.y;
	// Solve quadratic a t^2 + b t + c = 0
	float q = b*b - 4.0f * a * c;
	if (q < 0.0)
		return 0; // no real solution
	if (b < 0.0) // q = -(1/2)*(b + sgn(b)*sqrt(b^2 - 4ac))
		q = -0.5f * (b - sqrtf(q));
	else
		q = -0.5f * (b + sqrtf(q));
	// Roots are q/a and c/q.
	float t = q / a;
	float t1 = c / q;
	if (t < 0.f || (t1 > 0.f && t1 < t)) // use smaller positive root
		t = t1;

	// Get position of collision
	D3DXVECTOR3 vEstimatedHit = vFrom + t * m_vVelocity + 0.5f * t * t * g_vGravity;
	static float fRollTime = 0.0f;
	vEstimatedHit.x += fRollTime * m_vVelocity.x;
	vEstimatedHit.z += fRollTime * m_vVelocity.z;

	// Translate starting point to match target
	m_vPosition.x += vTargetPosition.x - vEstimatedHit.x;
	m_vPosition.z += vTargetPosition.z - vEstimatedHit.z;
	
	return MoveBall(0.f);
}

//-----------------------------------------------------------------------------
// Name: MoveBall
// Desc: Called once per frame (if not paused) to update ball position.
//
// The physics implemented here is extremely simple-minded. It's just
// to show how the ball interacts with the grass.
//-----------------------------------------------------------------------------
HRESULT CBall::MoveBall(float dt)
{
	// Restore current shadowed point
	if (m_bShadowed)
	{
		SetTerrainColors(m_vPositionShadowed, m_rcolorSave);
		m_bShadowed = false;
	}

	// Get current terrain values
	D3DXVECTOR3 vPosition0 = m_vPosition; // save current ball position for update call
	float fGrassThickness0;
	float fHeight0;
	GetTerrainPoint(m_vPosition, &fGrassThickness0, &fHeight0);
	
	// Apply grass force if the ball is penetrating the grass
	float fDepth = fHeight0 + fGrassThickness0 - (m_vPosition.y - m_fBallRadius);	// depth of penetration of bottom of ball
	D3DXVECTOR3 vAcceleration = g_vGravity;
	if (fDepth > 0.f && fGrassThickness0 > 0.f)
	{
		// TODO: use correct normal and better friction model that separates
		// normal and tangential components. Add rolling to the model, too.
		
		float fGrassSpring = 9.8f / (fGrassThickness0 * (1.f - m_fGrassRestFraction)); // cancel gravity when ball is at grass resting point
		if (fDepth > fGrassThickness0) fDepth = fGrassThickness0; // Terrain collision is modeled below as impulse.
		vAcceleration.y += fGrassSpring * fDepth;

		// Dampen velocity
		static D3DXVECTOR3 vGrassDamping(0.9f, 0.85f, 0.9f);
		m_vVelocity.x *= vGrassDamping.x;
		m_vVelocity.y *= vGrassDamping.y;
		m_vVelocity.z *= vGrassDamping.z;

		// Mark grass
		static D3DXCOLOR colorScale(0.9f, 0.98f, 0.9f, 0.9f);
		static D3DXCOLOR colorOffset(0.f, 0.f, 0.f, -0.03f);
		static D3DXCOLOR colorMin(0.3f, 0.4f, 0.1f, 0.3f);
		MarkTerrain(m_vPosition, colorScale, colorOffset, colorMin, NULL);
	}
	
	// Update the position of the ball
	static float dtMax = 1.f / 10.f;
	if (dt > dtMax) dt = dtMax;
	m_vPosition += dt * m_vVelocity + 0.5f * dt * dt * vAcceleration;
	m_vVelocity += dt * vAcceleration;

	// Check for collisions with terrain
	float fGrassThickness;
	float fHeight;
	GetTerrainPoint(m_vPosition, &fGrassThickness, &fHeight);
	float fBallCenterHeight = fHeight + m_fBallRadius;
	if (m_vPosition.y < fBallCenterHeight && m_vVelocity.y < 0.f)
	{
		m_vPosition.y = fBallCenterHeight + fGrassThickness * m_fGrassRestFraction; // slide ball out of intersection
		m_vVelocity.y = -m_vVelocity.y; // reverse velocity

		// Dampen velocity
		static D3DXVECTOR3 vTerrainDamping(0.9f, 0.8f, 0.9f);
		m_vVelocity.x *= vTerrainDamping.x;
		m_vVelocity.y *= vTerrainDamping.y;
		m_vVelocity.z *= vTerrainDamping.z;

		// Mark the grass where we hit
		static float fYMin = 6.f;
		static float fYMax = 12.f;
		if (m_vVelocity.y >= fYMin)
		{
			float fLerp = (m_vVelocity.y - fYMin) / (fYMax - fYMin);
			if (fLerp < 0.f) fLerp = 0.f;
			else if (fLerp > 1.f) fLerp = 1.f;
			static float fScaleMin = 0.9f;
			static float fScaleMax = 0.1f;
			float fScale = fScaleMin + fLerp * (fScaleMax - fScaleMin);
			D3DXCOLOR colorScale(fScale, fScale, fScale, fScale);
			static D3DXCOLOR colorOffset(0.4f * 0.1f, 0.3f * 0.1f, 0.1f * 0.1f, 0.1f);
			static D3DXCOLOR colorMin(0.4f, 0.3f, 0.1f, 0.0f);
			MarkTerrain(m_vPosition, colorScale, colorOffset, colorMin, NULL);
		}
	}

	// Darken the shadow point
	// To make this more realistic, we should project from the sun's position.  This gives the
	// basic idea, though.
	static float fMinHeight = 0.f;
	static float fMaxHeight = 3.f;
	float fBallHeight = m_vPosition.y - fHeight;
	if (fBallHeight < fMaxHeight)
	{
		float fLerp = (fBallHeight - fMinHeight) / (fMaxHeight - fMinHeight);
		if (fLerp < 0.f) fLerp = 0.f;
		static D3DXCOLOR shadowScaleDarkest(0.3f, 0.3f, 0.3f, 1.f);
		static D3DXCOLOR shadowScaleLightest(1.f, 1.f, 1.f, 1.f);
		static D3DXCOLOR shadowOffset(0.f, 0.f, 0.f, 0.f);
		static D3DXCOLOR shadowMin(0.f, 0.f, 0.f, 0.f);
		D3DXCOLOR shadowScale = shadowScaleDarkest + fLerp * (shadowScaleLightest - shadowScaleDarkest);
		if (MarkTerrain(m_vPosition, shadowScale, shadowOffset, shadowMin, m_rcolorSave) == S_OK)
		{
			m_bShadowed = true;
			m_vPositionShadowed = m_vPosition;
		}
	}

	// Update matrices
	D3DXMATRIX matScale;
	static float fBallScale = 2.f;	// extra scaling to make the ball look right
	D3DXMatrixScaling(&matScale, fBallScale, fBallScale, fBallScale);
	D3DXMATRIX matTranslate;
	D3DXMatrixTranslation(&matTranslate, m_vPosition.x, m_vPosition.y, m_vPosition.z);
	D3DXVECTOR3 vFlatVelocity(m_vVelocity.x, 0.f, m_vVelocity.z);
	float fRollSpeed = D3DXVec3Length(&vFlatVelocity);
	static float fRollSpeedThreshold = 1e-3f;
	if (fRollSpeed > fRollSpeedThreshold)
	{
		// Make the ball roll in the direction of travel.  A better physics model would produce the rolling directly.
		static float fRollFactor = -10.f;
		float fRollAngle = fRollFactor * D3DXVec3Dot(&vFlatVelocity, &m_vPosition) / fRollSpeed;
		D3DXVECTOR3 vUp(0.f, 1.f, 0.f);
		D3DXVECTOR3 vRollAxis;
		D3DXVec3Cross(&vRollAxis, &vFlatVelocity, &vUp);
		D3DXVec3Normalize(&vRollAxis, &vRollAxis);
		D3DXMatrixRotationAxis(&m_matRoll, &vRollAxis, fRollAngle);
	}
	m_matWorld = matScale * m_matRoll * matTranslate;
	return S_OK;
}
