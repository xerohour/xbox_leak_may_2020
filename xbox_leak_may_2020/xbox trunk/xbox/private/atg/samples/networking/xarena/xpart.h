//-----------------------------------------------------------------------------
// File: xpart.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>


//-----------------------------------------------------------------------------
// particle vertex type
//-----------------------------------------------------------------------------
#define PS_FVF D3DFVF_XYZ | D3DFVF_DIFFUSE
struct PS_FVFT
{
	D3DXVECTOR3 vPos;
	DWORD dwDiffuse;
};


//-----------------------------------------------------------------------------
// particle structure
//-----------------------------------------------------------------------------
struct XParticle
{
	DWORD dwFlags;					// particle flags
	float fLife;					// # of seconds to live
	D3DXVECTOR3 vPos;				// particle position
	DWORD dwCurColor;				// current color
	DWORD dwDestColor;				// destination color
	D3DXVECTOR3 vVel;				// velocity
	D3DXVECTOR3 vAcc;				// acceleration
};


//-----------------------------------------------------------------------------
// particle system class
//-----------------------------------------------------------------------------
class CXParticleSystem
{
	XParticle *m_Particles;			// array of particles
    LPDIRECT3DVERTEXBUFFER8 m_pVB;	// vertex buffer for particles
    LPDIRECT3DTEXTURE8 m_pTexture;	// guess what this is...

	DWORD m_dwNumParticles;			// # of active particles
	DWORD m_dwMaxParticles;			// max # of particles

public:
	CXParticleSystem(DWORD dwMax, char *texname);
	~CXParticleSystem();

	XParticle *AddParticle();
	void Update(float fElapsedTime);
	void Render();

	void Burst(D3DXVECTOR3 *pos, DWORD dwNumParticles);
};