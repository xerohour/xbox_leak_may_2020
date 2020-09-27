///////////////////////////////////////////////////////////////////////////////
// File: BlobRenderer.h
//
// Copyright 2001 Pipeworks Software
//
// WORK HALTED: Patent issues
//
///////////////////////////////////////////////////////////////////////////////
#ifndef BLOBRENDERER_H
#define BLOBRENDERER_H

#include "render_object.h"


// Adapted from: http://www.gamasutra.com/features/20000523/lander_01.htm
// Possibly in violation of US Patent #4,710,876
// http://164.195.100.11/netacgi/nph-Parser?Sect1=PTO2&Sect2=HITOFF&p=1&u=/netahtml/search-bool.html&r=1&f=G&l=50&co1=AND&d=pall&s1='4710876'.WKU.&OS=PN/4710876&RS=PN/4710876



struct BlobVertex
{
	D3DVECTOR pos;
    D3DVECTOR normal;	// not normalized, do it in the GPU
	// Diffuse color is set as a constant.
};








class BlobSource
{
public:
	D3DVECTOR		ptPosition;
	float			fStrength;
	float			fConsiderationRadius;
	float			fConsiderationRadius2;
	int				dAffiliation;	// affiliation of -1 is never rendered, but does repulse

	inline float	calculate(int affiliation, const D3DVECTOR& pos) const
	{
		float dist2 = Distance2(ptPosition, pos);
		if (dist2 > fConsiderationRadius2) return 0.0f;
		float falloff = 1.0f - (dist2/fConsiderationRadius2);
		return ((affiliation==dAffiliation) ? 1.0f : -1.0f) * fStrength * falloff*falloff;
	}
};




class BlobRenderer : public RenderObject
{
protected:
	const BlobSource*		m_pSources;
	int m_NumBlobs;

	float					m_Threshhold;

    DWORD					m_dwNumVertices;
    DWORD					m_dwNumIndices;

    LPDIRECT3DVERTEXBUFFER8 m_pBlobVB;
    LPDIRECT3DINDEXBUFFER8  m_pBlobIB;
	


	float*		m_pField;
	int*		m_pVertexIndices;	// circular fifo which stores one layer of vertex indices
	int			m_FieldX, m_FieldY, m_FieldXY, m_FieldZ;		// number of entries in each field coord

	D3DVECTOR	m_LowerLeftCorner;
	D3DVECTOR	m_FieldToWorld;
	D3DVECTOR	m_WorldToField;

	int			getCoords(const D3DVECTOR& pos, int* p_x, int* p_y, int* p_z, D3DVECTOR* p_remainder=NULL);
	void		getWorldPos(D3DVECTOR* pos, int x, int y, int z);

public:
	BlobRenderer();
	~BlobRenderer();

	virtual bool isVisible() { return true; }
	virtual void destroy();

	virtual void render();
	virtual void advanceTime(float fElapsedTime, float fDt) {}

	
	void init(	const BlobSource* p_blob_sources, int num_blobs,
				float xy_spacing, float z_spacing,
				const D3DVECTOR& center, const D3DVECTOR& half_dim);

	void	getThreshhold() const	{ return m_Threshhold; }
	void	setThreshhold(float th)	{ m_Threshhold = th; }
};


class TestBlobRenderer : public BlobRenderer
{
protected:
	enum { NUM_BLOBS = 6 };
	BlobSource m_Sources[NUM_BLOBS];

public:
	TestBlobRenderer();
	~TestBlobRenderer();

	virtual void create();
	virtual void advanceTime(float fElapsedTime, float fDt);
};



#endif // BLOBRENDERER_H
