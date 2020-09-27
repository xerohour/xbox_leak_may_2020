///////////////////////////////////////////////////////////////////////////////
// File: Blobs.h
//
// Copyright 2001 Pipeworks Software
///////////////////////////////////////////////////////////////////////////////
#ifndef BLOBS_H
#define BLOBS_H


#include "render_object.h"
#include "qrand.h"



struct BlobVertex
{
	D3DVECTOR pos;
    // D3DVECTOR normal;		// sphere with radius 1, normal is the pos!
	// Diffuse color is set as a constant.
};

   
class LavaLampInterior;


class LLBlob : public RenderObject
{
protected:
    DWORD					m_dwNumVertices;
    DWORD					m_dwNumIndices;

    LPDIRECT3DVERTEXBUFFER8 m_pBlobVB;
    LPDIRECT3DINDEXBUFFER8  m_pBlobIB;

	D3DVECTOR4				m_BlobColor;


	D3DVECTOR				m_Pos;
	D3DVECTOR				m_Scale;
	float					m_DeformationInertia;
	FLOAT					m_fRadius;


	float					m_Temperature;
	float					m_TemperatureAbsorbance;
	D3DVECTOR				m_Velocity;
	D3DVECTOR				m_Accel;


	int						m_Subdivisions;		// number of quads in a direction for each face
	float					m_fDivisionStep;	// distance on cube face that a division spans

	int						m_Species;

	// Cube has dimensions from -1 to +1.
	void					calcFacePoint(D3DVECTOR* ppos, int face, int u, int v);


public:
	LLBlob();

	void					create(D3DVECTOR pos, D3DVECTOR4 color);

	virtual void create() {}
	virtual void destroy();

	virtual void render();
	virtual void advanceTime(float fElapsedTime, float fDt);


	void			collided(D3DVECTOR pos, D3DVECTOR normal);
	void			interactWithBlob(const LLBlob* pllb, float dt);

	D3DVECTOR		getPos() const { return m_Pos; }
	float			getRadius() const { return m_fRadius; }
	float			getRadius2() const { return m_fRadius*m_fRadius; }

	int				getSpecies() const { return m_Species; }
	void			setSpecies(int s) { m_Species = s; }

	void			setColor(D3DVECTOR4 color) { m_BlobColor = color; }
	const D3DVECTOR4& getColor() const { return m_BlobColor; }


	static const LavaLampInterior* spLL;
};


class LavaLampInterior : public RenderObject
{
protected:
	enum { NUM_LLBLOBS = 64 };
	LLBlob m_Blobs[NUM_LLBLOBS];

    DWORD                   m_dwVertexShader;   // Vertex shader handle
    DWORD					m_dwPixelShader;	// Handle for pixel shader

	LPDIRECT3DCUBETEXTURE8	m_pNormCubemap;
	LPDIRECT3DDEVICE8		m_pd3dDevice;

	void					InitPixelShader();

	enum { MAX_CONIC_SECTIONS = 32 };
	int						m_NumConicSections;
	float					m_ConicSectionCenterX, m_ConicSectionCenterY;
	float					m_ConicSectionBotZ[MAX_CONIC_SECTIONS+1];
	float					m_ConicSectionRadius[MAX_CONIC_SECTIONS+1];
	float					m_ConicSectionSlope[MAX_CONIC_SECTIONS];	// dr/dz
	float					m_ConicSectionNormalR[MAX_CONIC_SECTIONS];	// -1, normalized
	float					m_ConicSectionNormalZ[MAX_CONIC_SECTIONS];	// slope, normalized

	static QRand			m_QRand;



public:

	virtual bool isVisible() { return true; }
	virtual void create();
	virtual void destroy();

	virtual void render();
	virtual void advanceTime(float fElapsedTime, float fDt);


	float	getBottom() const { return m_ConicSectionBotZ[0]; }
	float	getTop() const { return m_ConicSectionBotZ[m_NumConicSections+1]; }

	float	getRadius(float z) const
	{
		if (z < m_ConicSectionBotZ[0]) return 0.0f;
		for (int i=1; i<=m_NumConicSections; i++)
		{
			if (m_ConicSectionBotZ[i+1] < z) continue;
			float diff = m_ConicSectionBotZ[i+1] - m_ConicSectionBotZ[i];
			float s = (z - m_ConicSectionBotZ[i]) / diff;
			return m_ConicSectionRadius[i] + s*(m_ConicSectionRadius[i+1] - m_ConicSectionRadius[i]);
		}
		return 0.0f;
	}


	// Temperature is 1.0f at bottom, ramps to 0.0f at top.
	float getTemperature(float z) const;

	void collide(LLBlob* pllb, float x, float y, float z, float radius, float dt) const;
	bool collideWithCaps(LLBlob* pllb, float x, float y, float z, float radius) const;


	void recomputeSpecie();

	static inline float fRand01();
	static inline float fRand11();
};


#define LLI_RAND_MAX  0x00010000
#define LLI_RAND_MASK 0x0000FFFF

float LavaLampInterior::fRand01()
{
	static float mul = 1.0f / ((float)LLI_RAND_MAX);
	return ((float)(m_QRand.Rand()&LLI_RAND_MASK)) * mul;
}

float LavaLampInterior::fRand11()
{
	static float mul = 2.0f / ((float)LLI_RAND_MAX);
	return (((float)(m_QRand.Rand()&LLI_RAND_MASK)) * mul) - 1.0f;
}



#endif // BLOBS_H

