///////////////////////////////////////////////////////////////////////////////
// File: BlobRenderer.cpp
//
// Copyright 2001 Pipeworks Software
//
// WORK HALTED: Patent issues
//
///////////////////////////////////////////////////////////////////////////////
#include <xtl.h>
#include "xbs_math.h"
#include "xbs_app.h"
#include "Blobs.h"
#include "BlobRenderer.h"


///////////////////////////////////////////////////////////////////////////////
#define LLI_RAND_MAX  0x00010000
#define LLI_RAND_MASK 0x0000FFFF
///////////////////////////////////////////////////////////////////////////////
float FRand01()
{
	static QRand qrand;
	static float mul = 1.0f / ((float)LLI_RAND_MAX);
	return ((float)(qrand.Rand()&LLI_RAND_MASK)) * mul;
}
///////////////////////////////////////////////////////////////////////////////
float FRand11()
{
	static QRand qrand;
	static float mul = 2.0f / ((float)LLI_RAND_MAX);
	return (((float)(qrand.Rand()&LLI_RAND_MASK)) * mul) - 1.0f;
}
///////////////////////////////////////////////////////////////////////////////
BlobRenderer::BlobRenderer()
{
	m_pLLI = NULL;
	m_pBlobArray = NULL;
	m_NumBlobs = 0;

	m_Threshhold = 1.0f;

    m_dwNumVertices = 0;
    m_dwNumIndices = 0;
    m_pBlobVB = NULL;
    m_pBlobIB = NULL;

	m_pField = NULL;
	m_pVertexIndices = NULL;
}
///////////////////////////////////////////////////////////////////////////////
BlobRenderer::~BlobRenderer()
{
	destroy();
}
///////////////////////////////////////////////////////////////////////////////
void BlobRenderer::destroy()
{
	if (m_pBlobVB) m_pBlobVB->Release();
	if (m_pBlobIB) m_pBlobIB->Release();
	delete m_pField;
	delete m_pVertexIndices;

	m_pBlobArray = NULL;

	m_pLLI = NULL;
	m_pBlobArray = NULL;
	m_NumBlobs = 0;

    m_dwNumVertices = 0;
    m_dwNumIndices = 0;
    m_pBlobVB = NULL;
    m_pBlobIB = NULL;

	m_pField = NULL;
	m_pVertexIndices = NULL;
}
///////////////////////////////////////////////////////////////////////////////
void BlobRenderer::render()
{
	int next_affiliation = m_pSources[0]->dAffiliation;
	int i;
	for (i=1; i<m_NumBlobs; i++)
	{
		next_affiliation = min(next_affiliation, m_pSources[i]->dAffiliation);
	}

	for (int affiliation = next_affiliation; affiliation < m_NumBlobs; affiliation = next_affiliation)
	{
		next_affiliation = m_NumBlobs;
		D3DVECTOR pt_min, pt_max;
		pt_min = pt_max = bs.ptPosition;
		bool b_unset = true;

		// For this affiliation, iterate through all the sources, adjusting
		// the min/max for any blobs with the correct affiliation.
		for (int i=0; i<m_NumBlobs; i++)
		{
			BlobSource& bs = m_pSources[i];

			// Keep track of what the smallest affiliation larger than the current affiliation is.
			int aff = bs.dAffiliation;
			if (aff > affiliation) next_affiliation = min(aff, next_affiliation);

			if (aff == affiliation)
			{
				if (b_unset)
				{
					b_unset = false;
					pt_min = pt_max = bs.ptPosition;
					pt_min.x -= bs.fConsiderationRadius;
					pt_min.y -= bs.fConsiderationRadius;
					pt_min.z -= bs.fConsiderationRadius;
					pt_max.x += bs.fConsiderationRadius;
					pt_max.y += bs.fConsiderationRadius;
					pt_max.z += bs.fConsiderationRadius;
				}
				else
				{
					pt_min.x = min(pt_min.x, bs.ptPosition.x - bs.fConsiderationRadius);
					pt_min.y = min(pt_min.y, bs.ptPosition.y - bs.fConsiderationRadius);
					pt_min.z = min(pt_min.z, bs.ptPosition.z - bs.fConsiderationRadius);

					pt_max.x = min(pt_max.x, bs.ptPosition.x + bs.fConsiderationRadius);
					pt_max.y = min(pt_max.y, bs.ptPosition.y + bs.fConsiderationRadius);
					pt_max.z = min(pt_max.z, bs.ptPosition.z + bs.fConsiderationRadius);
				}
			}
		}

		// Now have the boundaries for the rendering region. Blobs outside the area may
		// affect the energy threshold, but no polygons will be drawn outside it.
		int sx, sy, sz, ex, ey, ez;
		getCoords(pt_min, &sx, &sy, &sz);
		getCoords(pt_max, &ex, &ey, &ez);
		ex++; ey++; ez++;

		int len_x = ex - sx + 1;
		int len_y = ey - sy + 1;
		int len_z = ez - sz + 1;
		int len_xy = len_x*len_y;
		int len_xyz = len_xy * len_z;

		if (len_x*len_y*len_z > m_FieldX*m_FieldY*m_FieldZ)
		{
			continue;	// can't render it, it is too big
		}


		// Clear the part of the field that will be used. Note that this does not
		// share the rows/columns/levels that would be used if the entire field
		// was used. Instead, the minimum amount of memory is used in a contiguous
		// fashion.
		for (i=0; i<len_xyz; i++) m_pField[i] = 0.0f;


		// For each blob, isolate the box within influence and iterate for each point.
		// The box of influence can (and must) be clipped to the previously determined
		// region.
		for (i=0; i<m_NumBlobs; i++)
		{
			BlobSource& bs = m_pSources[i];
			D3DVECTOR pt_start, pt_end;
			Set(&pt_start,
				max(pt_min.x, bs.ptPosition.x - bs.fConsiderationRadius),
				max(pt_min.y, bs.ptPosition.y - bs.fConsiderationRadius),
				max(pt_min.z, bs.ptPosition.z - bs.fConsiderationRadius)
				);
			Set(&pt_end,
				min(pt_max.x, bs.ptPosition.x + bs.fConsiderationRadius),
				min(pt_max.y, bs.ptPosition.y + bs.fConsiderationRadius),
				min(pt_max.z, bs.ptPosition.z + bs.fConsiderationRadius)
				);

			int bsx, bsy, bsz, bex, bey, bez;
			getCoords(pt_start, &bsx, &bsy, &bsz);
			getCoords(pt_end  , &bex, &bey, &bez);
			bex++; bey++; bez++;
			// The coordinates have already been capped to the rendering region.

			if ( (bsx<=bex) && (bsy<=bey) && (bsz<=bez) )
			{
				D3DVECTOR pos, posll;
				getWorldPos(&posll, bsx, bsy, bsz);
				pos.z = posll.z;
				for (int w=bsz; w<=bez; w++, pos.z += m_FieldToWorld.z)
				{
					pos.y = posll.y;
					for (int v=bsy; v<=bey; v++, pos.y += m_FieldToWorld.y)
					{
						pos.x = posll.x;
						float* p_field = &m_pField[w*len_xy + v*len_x + bsx];
						for (int u=bsx; u<=bex; u++, pos.x += m_FieldToWorld.x)
						{
							*(p_field++) += bs.calculate(affiliation, pos);
						}
					}
				}
			}
		}
		// It is legitimate for the outermost points to have a negative energy accumulation,
		// but nothing should be giving it a positive value. It would be nice to enforce this,
		// but I'm not sure it's worth the trouble.

		// Woohoo! The field is complete for this affiliation. Start rendering.

		// Lock the buffers.
		int max_tris = len_xyz * 5 * 2;		// for a Sierpinski cube relative, maybe...
		BlobVertex* p_verts;
		u16* p_indices;
		if (D3D_OK != m_pBlobVB->Lock(0, len_xyz*6, (BYTE**) &pbv_buf, D3DLOCK_DISCARD))
		{
			continue;
		}
		if (D3D_OK != m_pBlobIB->Lock(0, max_tris*3, (BYTE**) &pbv_buf, D3DLOCK_DISCARD))
		{
			m_pBlobVB->Unlock();
			continue;
		}

		int num_tris = 0;
		int num_verts = 0;
		BlobVertex* p_vert = p_verts;
		u16* p_index = p_indices;


		// This buffer stores the vertex indices (or -1) for the vertex on each segment
		// of a voxel. The offsets march through the buffer, returning to zero when they
		// reach the voxel_wrap value. m_pVertexIndices[voxel_offset] is for the voxel
		// with the current point in the max_pt position.
		int voxel_offset = 0;
		int voxel_offset_x = 6*1;
		int voxel_offset_y = 6*len_x;
		int voxel_offset_z = 6*len_xy;
		int voxel_wrap = 6*(len_xy+1);
		memset(m_pVertexIndices, -1, sizeof(int) * voxel_wrap);



		D3DVECTOR pos;
		pos.z = pt_min.z;
		for (int w=sz; w<ez; w++, pos.z += m_FieldToWorld.z)
		{
			pos.y = pt_min.y;
			for (int v=sy; v<ey; v++, pos.y += m_FieldToWorld.y)
			{
				pos.x = pt_min.x;
				p_field = &m_pField[w*len_xy + v*len_x];
				for (int u=sx; u<ex; u++, pos.x += m_FieldToWorld.x)
				{
					u32 in_or_out;
					if (p_field[             0] > m_Threshhold) in_or_out |= 0x0001;
					if (p_field[             1] > m_Threshhold) in_or_out |= 0x0002;
					if (p_field[       len_x  ] > m_Threshhold) in_or_out |= 0x0004;
					if (p_field[       len_x+1] > m_Threshhold) in_or_out |= 0x0008;
					if (p_field[len_xy        ] > m_Threshhold) in_or_out |= 0x0010;
					if (p_field[len_xy+      1] > m_Threshhold) in_or_out |= 0x0020;
					if (p_field[len_xy+len_x  ] > m_Threshhold) in_or_out |= 0x0040;
					if (p_field[len_xy+len_x+1] > m_Threshhold) in_or_out |= 0x0080;

					if ((in_or_out!=0) && (in_or_out!=0x00FF))
					{
						// Not all corners are the same. Analyze the five tetrahedrons.
						// The five tetrahedrons are defined as corners:
						const int  odd_tetrahedrons[20] = { 0,1,2,4,  5,1,4,7,  3,1,2,7,  6,2,7,4,  1,2,4,7 };
						const int even_tetrahedrons[20] = { 1,0,5,3,  7,3,5,6,  2,0,6,3,  4,0,6,5,  0,6,5,3 };
						const int* tetrahedrons = ((u+v+w)&0x01) ? odd_tetrahedrons : even_tetrahedrons;
						
						for (int tet=0; tet<5; tet++)
						{
							int mask = 0;
							mask |= (1<<(tetrahedrons[4*tet+0]));
							mask |= (1<<(tetrahedrons[4*tet+1]));
							mask |= (1<<(tetrahedrons[4*tet+2]));
							mask |= (1<<(tetrahedrons[4*tet+3]));

							int masked = in_or_out & mask;
							if ((masked!=0) && (masked!=mask))
							{
								// Mixed corners for this tetrahedron. Triangles are needed.
								int ins[4];
								int outs[4];
								int num_in = 0;
								int num_out = 0;
								if (mask & 0x0001) ins[num_in++] = 0;
								if (mask & 0x0002) ins[num_in++] = 1;
								if (mask & 0x0004) ins[num_in++] = 2;
								if (mask & 0x0008) ins[num_in++] = 3;
								if (mask & 0x0010) ins[num_in++] = 4;
								if (mask & 0x0020) ins[num_in++] = 5;
								if (mask & 0x0040) ins[num_in++] = 6;
								if (mask & 0x0080) ins[num_in++] = 7;

								if (num_in==1)
								{
									// One is in, three are out.
									D3DVECTOR pos_in = pos;
									if (ins[0] & 0x01) pos_in.x += m_FieldToWorld.x * 0.5f;
									if (ins[0] & 0x02) pos_in.y += m_FieldToWorld.y * 0.5f;
									if (ins[0] & 0x04) pos_in.z += m_FieldToWorld.z * 0.5f;
								}
							}
						}
					}
				}
			}
		}


		// Render.


		m_pBlobIB->Unlock();
		m_pBlobVB->Unlock();
	}	
}
///////////////////////////////////////////////////////////////////////////////
void BlobRenderer::init(	const BlobSource* p_blob_sources, int num_blobs,
							float xy_spacing, float z_spacing,
							const D3DVECTOR& center, const D3DVECTOR& half_dim)
{
	m_FieldX = (int)((half_dim.x * 2.0f / xy_spacing) + 2.0f);
	m_FieldY = (int)((half_dim.y * 2.0f / xy_spacing) + 2.0f);
	m_FieldZ = (int)((half_dim.z * 2.0f /  z_spacing) + 2.0f);
	m_FieldXY = m_FieldX * m_FieldY;
	m_pField = new float[m_FieldXY * m_FieldZ];
	m_pVertexIndices = new int[(m_FieldXY+1) * 6];

	Sub(&m_LowerLeftCorner, center, half_dim);
	m_FieldToWorld.x = xy_spacing;
	m_FieldToWorld.y = xy_spacing;
	m_FieldToWorld.z =  z_spacing;
	m_WorldToField.x = 1.0f / xy_spacing;
	m_WorldToField.y = 1.0f / xy_spacing;
	m_WorldToField.z = 1.0f /  z_spacing;

	// Create a vertex buffer, too.
}
///////////////////////////////////////////////////////////////////////////////
void BlobRenderer::getCoords(const D3DVECTOR& pos, int* p_x, int* p_y, int* p_z, D3DVECTOR* p_remainder)
{
	float fx = (pos.x - m_LowerLeftCorner.x) * m_WorldToField.x;
	float fy = (pos.y - m_LowerLeftCorner.y) * m_WorldToField.y;
	float fz = (pos.z - m_LowerLeftCorner.z) * m_WorldToField.z;
	*p_x = (int) (fx);
	*p_y = (int) (fy);
	*p_z = (int) (fz);

	if (p_remainder)
	{
		p_remainder->x = fx - ((float)*p_x);
		p_remainder->y = fy - ((float)*p_y);
		p_remainder->z = fz - ((float)*p_z);
	}
	return (*p_z * m_FieldXY) + (*p_y * m_FieldX) + (*p_x);
}
///////////////////////////////////////////////////////////////////////////////
void BlobRenderer::getWorldPos(D3DVECTOR* pos, int x, int y, int z)
{
	pos->x = m_LowerLeftCorner.x + ((float)x) * m_FieldToWorld.x;
	pos->y = m_LowerLeftCorner.y + ((float)y) * m_FieldToWorld.y;
	pos->z = m_LowerLeftCorner.z + ((float)z) * m_FieldToWorld.z;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TestBlobRenderer::TestBlobRenderer()
{
}
///////////////////////////////////////////////////////////////////////////////
TestBlobRenderer::~TestBlobRenderer()
{
	destroy();
}
///////////////////////////////////////////////////////////////////////////////
void TestBlobRenderer::create()
{
	for (int i=0; i<NUM_BLOBS; i++)
	{
		Set(&m_Sources[i].ptPosition, FRand01() * 0.2f, FRand01() * 0.2f, FRand01() * 0.2f);
		m_Sources[i].fStrength = 1.0f;
		m_Sources[i].fConsiderationRadius = 0.09f;
		m_Sources[i].fConsiderationRadius2 = m_Sources[i].fConsiderationRadius * m_Sources[i].fConsiderationRadius;
		m_Sources[i].dAffiliation = 0;
	}

	D3DVECTOR center, hd;
	Set(&center, 0.0f, 0.0f, 0.0f);
	Set(&hd, 1.0f, 1.0f, 1.0f);
	init(&m_Sources[i], NUM_BLOBS, 0.005f, 0.005f, center, hd);
}
///////////////////////////////////////////////////////////////////////////////
void TestBlobRenderer::advanceTime(float fElapsedTime, float fDt)
{
}
///////////////////////////////////////////////////////////////////////////////
