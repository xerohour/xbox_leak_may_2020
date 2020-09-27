///////////////////////////////////////////////////////////////////////////////
// File: VBlob.h
//
// Copyright 2001 Pipeworks Software
///////////////////////////////////////////////////////////////////////////////
#ifndef VBLOB_H
#define VBLOB_H


#include "qrand.h"




// The vertex list holding the unit sphere coordinates remain on the card.
// Each frame another list is streamed.

struct VBlobConstantVertex
{
	D3DVECTOR unit_sphere_normal;
};


struct VBlobChangingVertex
{
    D3DVECTOR4 normal;	// not normalized (do it in the GPU); w is the displacement due to bumps
};




// Bloblets need to be able to be copied.
class VBloblet
{
public:
	void Init()  { fWobble = 1.0f; fWobbleDirection = 0.0f; }
	void UnInit() {}

	void set(float rad, float x, float y, float z)
	{
		fRadius = rad;
		Set(&vPosition, x, y, z);
	}

	// Perturbation is normal * fMagnitude * f( (r*r)/(fRadius*fRadius) ).
	// f(r/R) will start at a peak of 1 (at r=0) and fall to 0 as r goes to R. The
	// slope at 0 and 1 will be 0.
	// The perturbation needs to be applied in code so the normals can be accumulated.
	float fRadius;
	D3DVECTOR vPosition;
	bool bFarSide;

	D3DVECTOR vDirection;
	float fStartTime;
	float fTimeMultiple;
	float fMaxDist;	// in world units

	float fCurDist; // fMaxDist * sin(...)

	float fWobble;
	float fWobbleDirection;





	bool update(float elapsed_time, float dt);	// returns false if it consents to be deleted
};





class VBlobBump
{
public:
	void Init();
	void UnInit();

	void set(float rad, float mag, float x, float y, float z)
	{
		fRadius = rad;
		fRadius2 = fRadius*fRadius;
		fOORadius2 = 1.0f / fRadius2;
		fMagnitude = mag;
		Set(&vPosition, x, y, z);
	}

	// Perturbation is normal * fMagnitude * f( (r*r)/(fRadius*fRadius) ).
	// f(r/R) will start at a peak of 1 (at r=0) and fall to 0 as r goes to R. The
	// slope at 0 and 1 will be 0.
	// The perturbation needs to be applied in code so the normals can be accumulated.
	float fRadius, fRadius2, fOORadius2;
	float fMagnitude;	// wrt unit sphere
	D3DVECTOR vPosition;

	int   facesOfInterest;		// bitfield indicating the three faces (0,1,2,4,5) <-> (-x,-y,-z,+x,+y,+z) the blob points most toward


	// returns true if it initialized a new Bloblet
	bool create(float cur_time, VBloblet* p_bloblet);	// randomly creates a blob bump
	bool update(float elapsed_time, float dt, VBloblet* p_bloblet);

protected:

	// Positioning data.
	D3DVECTOR vDirection;	// normalized
	
	float fStartTime;
	float fTimeMul;

	float fMaxMagnitude;

	VBloblet* pMyBloblet;
	bool bStillAttachedToBloblet;

	void recalculateFacesOfInterest();
};




   

class VBlob
{
protected:
    LPDIRECT3DVERTEXBUFFER8 m_pBlobletVB;		// VBlobConstantVertex, for the emerging blobs, lower tesselation
    LPDIRECT3DINDEXBUFFER8  m_pBlobletIB;
	
	LPDIRECT3DVERTEXBUFFER8 pHaloQuadVB;

    LPDIRECT3DVERTEXBUFFER8 m_pBlobVBConst;		// VBlobConstantVertex
    LPDIRECT3DVERTEXBUFFER8 m_pBlobVBChangingR;	// VBlobChangingVertex (rendering version) (swapped in render)
    LPDIRECT3DVERTEXBUFFER8 m_pBlobVBChangingU;	// VBlobChangingVertex (updating version)
    LPDIRECT3DINDEXBUFFER8  m_pBlobIB;
	D3DVECTOR*				m_pUnitSphereNormals;
    DWORD					m_dwNumVertices, m_dwNumBlobletVertices;
    DWORD					m_dwNumIndices,  m_dwNumBlobletIndices;
	int						m_NumVertsPerFace;

    DWORD                   m_dwVShaderBlob, m_dwVShaderBloblet;
    DWORD					m_dwPShaderBlob, m_dwPShaderBloblet;



	D3DVECTOR4				m_BlobColor;


	D3DVECTOR				m_Pos;		// position of center
	D3DVECTOR				m_Scale;
	FLOAT					m_fRadius;


	// BlobBumps work in unit-sphere space.
	enum { MAX_BLOBBUMPS = 32 };
	VBlobBump				m_BlobBumps[MAX_BLOBBUMPS];
	int						m_NumBlobBumps;

	enum { MAX_BLOBLETS = 8 };
	VBloblet				m_Bloblets[MAX_BLOBLETS];
	int						m_NumBloblets;



	static QRand			m_QRand;

	// Creates a VBlobConstantVertex vertex buffer. Creates WORD index buffer.
	static bool				generateUnitSphere(
											int resolution,
											LPDIRECT3DVERTEXBUFFER8* pp_vb,
											LPDIRECT3DINDEXBUFFER8* pp_ib,
											D3DVECTOR** pp_us,
											DWORD* num_verts, DWORD* num_indices);

	void					prepareChangingVertices();
	void					zeroChangingVertices();	// zeros bumps in m_pBlobVBChangingU
	inline void				swapChangingVertices()
	{
		LPDIRECT3DVERTEXBUFFER8 temp = m_pBlobVBChangingR;
		m_pBlobVBChangingR = m_pBlobVBChangingU;
		m_pBlobVBChangingU = temp;
	}



public:
	void Init();
	void UnInit()
    {
        int i;

        for (i = 0; i < MAX_BLOBBUMPS; i++)
        {
            m_BlobBumps[i].UnInit();
        }

        for (i = 0; i < MAX_BLOBLETS; i++)
        {
            m_Bloblets[i].UnInit();
        }

        destroy();
    }

	void create();
	void destroy();

	void render();
	void advanceTime(float fElapsedTime, float fDt);

	void restart();


	float getRadius() const { return m_fRadius; }
	const D3DVECTOR& getCenter() const { return m_Pos; }


	void getLightForPosition(D3DVECTOR* p_light_pos, float* p_intensity, D3DVECTOR position);



	static inline float fRand01();
	static inline float fRand11();
};

extern VBlob* gpVBlob;



#define LLI_RAND_MAX  0x00010000
#define LLI_RAND_MASK 0x0000FFFF

float VBlob::fRand01()
{
	static float mul = 1.0f / ((float)LLI_RAND_MAX);
	return ((float)(m_QRand.Rand()&LLI_RAND_MASK)) * mul;
}

float VBlob::fRand11()
{
	static float mul = 2.0f / ((float)LLI_RAND_MAX);
	return (((float)(m_QRand.Rand()&LLI_RAND_MASK)) * mul) - 1.0f;
}



#endif // VBLOB_H

