//-----------------------------------------------------------------------------
// File: xpart.cpp
//
// Desc: mini particle system for xarena
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "xpath.h"
#include "xpart.h"

//-----------------------------------------------------------------------------
// misc
//-----------------------------------------------------------------------------
extern LPDIRECT3DDEVICE8 g_pd3dDevice;

#define frand(a) ((float)rand()*(a)/32768.0f)
inline DWORD ftodw(FLOAT f) {return *((DWORD*)&f);}


//-----------------------------------------------------------------------------
// Name: CXParticleSystem
// Desc: Particle system constructor
//-----------------------------------------------------------------------------
CXParticleSystem::CXParticleSystem(DWORD dwMax, char *texname)
{
	m_Particles = new XParticle[dwMax];
	m_dwMaxParticles = dwMax;
	m_dwNumParticles = 0;

	// create vertex buffer
	g_pd3dDevice->CreateVertexBuffer(dwMax*sizeof(PS_FVFT), D3DUSAGE_WRITEONLY,
            PS_FVF, D3DPOOL_DEFAULT, &m_pVB);

    D3DXCreateTextureFromFileA(g_pd3dDevice, _FNA(texname), &m_pTexture);
}


//-----------------------------------------------------------------------------
// Name: ~CXParticleSystem
// Desc: Particle system destructor
//-----------------------------------------------------------------------------
CXParticleSystem::~CXParticleSystem()
{
	delete m_Particles;
	m_pVB->Release();
}


//-----------------------------------------------------------------------------
// Name: Update
// Desc: Update all particles
//-----------------------------------------------------------------------------
void CXParticleSystem::Update(float fElapsedTime)
{
	PS_FVFT *vert;
	DWORD i;
	XParticle *part;

	if(!m_dwNumParticles)				// no particles to update
		return;

	// lock the vertex buffer
	m_pVB->Lock(0, 0, (BYTE **)&vert, 0);

	// update the particles and stuff em into the VB
	for(i=0; i<m_dwNumParticles; i++)
	{
next:
		part = &m_Particles[i];

		part->vVel += part->vAcc;
		part->vPos += part->vVel*fElapsedTime;

		part->fLife -= fElapsedTime;
		if(part->fLife<0.0f || part->vPos.y<0.0f)
		{
			m_dwNumParticles--;
			m_Particles[i] = m_Particles[m_dwNumParticles];
			if(m_dwNumParticles>i)
				goto next;
		}

		vert[i].vPos = part->vPos;
		vert[i].dwDiffuse = part->dwCurColor;
	}

	m_pVB->Unlock();
}


//-----------------------------------------------------------------------------
// Name: AddParticle
// Desc: Yeah... Just adds a particle.
//-----------------------------------------------------------------------------
XParticle *CXParticleSystem::AddParticle()
{
	XParticle *part;

	if(m_dwNumParticles==m_dwMaxParticles)
		return NULL;

	part = &m_Particles[m_dwNumParticles++];

	return part;
}


//-----------------------------------------------------------------------------
// Name: Burst
// Desc: Do a particle system burst
//-----------------------------------------------------------------------------
void CXParticleSystem::Burst(D3DXVECTOR3 *pos, DWORD dwNumParticles)
{
	DWORD i;
	XParticle *part;

	for(i=0; i<dwNumParticles; i++)
	{
		part = AddParticle();
		if(!part)
			return;

		part->dwFlags = 0;

		// init position and movement parameters
		part->vPos = *pos;
		part->vVel.x = frand(1.0f)-0.5f;
		part->vVel.y = frand(1.0f)-0.5f;
		part->vVel.z = frand(1.0f)-0.5f;
		D3DXVec3Normalize(&part->vVel, &part->vVel);
		part->vVel.x *= 6.0f;
		part->vVel.y *= 6.0f;
		part->vVel.z *= 6.0f;
		part->vAcc = D3DXVECTOR3(0.0f, -0.3f, 0.0f);

		// init color params
		part->dwCurColor = 0xffff7f00 + ((rand()&0x7f)<<8);
		part->dwDestColor = 0;

		// init life
		part->fLife = frand(0.75f) + 0.4f;
	}
}


//-----------------------------------------------------------------------------
// Name: Render
// Desc: Render the particles
//-----------------------------------------------------------------------------
void CXParticleSystem::Render()
{
	D3DXMATRIX m;

	if(!m_dwNumParticles)
		return;

	D3DXMatrixIdentity(&m);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &m);

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

    g_pd3dDevice->SetTexture(3, m_pTexture);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);

    g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_MODULATE);
    g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	g_pd3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSIZE, ftodw(0.1f));
	g_pd3dDevice->SetRenderState(D3DRS_POINTSIZE_MIN, ftodw(0.02f));
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALE_A, ftodw(0.0f));
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALE_B, ftodw(3.0f));
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALE_C, ftodw(0.0f));

    g_pd3dDevice->SetStreamSource(0, m_pVB, sizeof(PS_FVFT));
    g_pd3dDevice->SetVertexShader(D3DFVF_XYZ|D3DFVF_DIFFUSE);
    g_pd3dDevice->DrawPrimitive(D3DPT_POINTLIST, 0, m_dwNumParticles);

    g_pd3dDevice->SetTexture(3, NULL);
    g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_DISABLE);
    g_pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    g_pd3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}