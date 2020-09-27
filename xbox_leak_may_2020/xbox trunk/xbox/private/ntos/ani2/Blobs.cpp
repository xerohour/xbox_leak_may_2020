///////////////////////////////////////////////////////////////////////////////
// File: Blobs.cpp
//
// Copyright 2001 Pipeworks Software
///////////////////////////////////////////////////////////////////////////////
#include <xtl.h>
#include "xbs_math.h"
#include "xbs_app.h"
#include "qrand.h"
#include "Blobs.h"


const LavaLampInterior* LLBlob::spLL = NULL;
QRand LavaLampInterior::m_QRand;


void LavaLampInterior::destroy()
{
//MTS	m_BlobRenderer.destroy();

	for (int i=0; i<NUM_LLBLOBS; i++)
	{
		m_Blobs[i].destroy();
	}


    if( m_dwPixelShader )
    {
        gpd3dDev->DeletePixelShader( m_dwPixelShader );
        m_dwPixelShader = 0;
    }

	if ( m_dwVertexShader )
	{
		gpd3dDev->DeleteVertexShader( m_dwVertexShader );
		m_dwVertexShader = 0;
	}
}



LLBlob::LLBlob()
{
	m_pBlobVB = NULL;
	m_pBlobIB = NULL;
	m_Temperature = 0.5f;
	Set(&m_Accel, 0.0f, 0.0f, 0.0f);
	Set(&m_Velocity, 0.0f, 0.0f, 0.0f);
	m_DeformationInertia = 0.3f;
}



void LLBlob::destroy()
{
	if (m_pBlobVB)
	{
		m_pBlobVB->Release();
		m_pBlobVB = NULL;
	}

	if (m_pBlobIB)
	{
		m_pBlobIB->Release();
		m_pBlobIB = NULL;
	}
}


void LavaLampInterior::InitPixelShader()
{
    if( m_dwPixelShader )
    {
        gpd3dDev->DeletePixelShader( m_dwPixelShader );
        m_dwPixelShader = 0;
    }

	m_dwPixelShader = gApp.loadPixelShader("D:\\Shaders\\blob.xpu");
}







void LavaLampInterior::create()
{
	LLBlob::spLL = this;


	m_ConicSectionCenterX = +0.04f;
	m_ConicSectionCenterY = -0.082f;

	m_NumConicSections = 2;
	m_ConicSectionBotZ[0] = -0.47f;
	m_ConicSectionBotZ[1] = -0.25f;
	m_ConicSectionBotZ[2] = +0.35f;

	m_ConicSectionRadius[0] = 0.11f;
	m_ConicSectionRadius[1] = 0.25f;
	m_ConicSectionRadius[2] = 0.12f;


	int i;
	for (i=0; i<m_NumConicSections; i++)
	{
		m_ConicSectionSlope[i] =	(m_ConicSectionRadius[i+1] - m_ConicSectionRadius[i+0]) /
									(m_ConicSectionBotZ[i+1]   - m_ConicSectionBotZ[i+0]);
		float norm = 1.0f / sqrtf(1 + m_ConicSectionSlope[i]*m_ConicSectionSlope[i]);
		m_ConicSectionNormalR[i] = norm * -1.0f;
		m_ConicSectionNormalZ[i] = norm * m_ConicSectionSlope[i];
	}


    // Initialize pixel shader
    InitPixelShader();

    DWORD dwShaderVertexDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),    // Position
        D3DVSD_END()
    };


	m_dwVertexShader = gApp.loadVertexShader("D:\\Shaders\\blob.xvu", dwShaderVertexDecl);



	float bot = m_ConicSectionBotZ[0];
	float sx = m_ConicSectionCenterX - 0.1f;
	float sy = m_ConicSectionCenterY - 0.1f;
	for (i=0; i<NUM_LLBLOBS; i++)
	{
		D3DVECTOR pos;
		D3DVECTOR4 color, base_color;
		Set(&pos, m_ConicSectionCenterX, m_ConicSectionCenterY, m_ConicSectionBotZ[m_NumConicSections>>1] );
		Set(&color, (i&0x04) ? 0.0f : 1.0f, (i&0x02) ? 0.0f : 1.0f, (i&0x01) ? 0.0f : 1.0f, 1.0f);
		Set(&base_color, 0.724f, 0.732f, 0.556f, 1.0f);
		
//MTS		m_Blobs[i].create(pos, color*0.1f + base_color*0.9f);
		m_Blobs[i].create(pos, base_color);
		m_Blobs[i].setSpecies(i);
	}


//MTS	m_BlobRenderer.create();
//MTS	m_BlobRenderer.init(this, &m_Blobs[0], NUM_LLBLOBS, 0.01f, 0.01f, m_ConicSectionCenterX, m_ConicSectionCenterY);

    return;
};


void LLBlob::calcFacePoint(D3DVECTOR* ppos, int face, int u, int v)
{
	float fu = (u==m_Subdivisions) ? +1.0f : -1.0f + m_fDivisionStep * ((float)u);
	float fv = (v==m_Subdivisions) ? +1.0f : -1.0f + m_fDivisionStep * ((float)v);
	switch(face)
	{
		case 0: Set(ppos, -1.0f, -fu, +fv); break;
		case 1: Set(ppos, +fv, -1.0f, -fu); break;
		case 2: Set(ppos, -fu, +fv, -1.0f); break;
		case 3: Set(ppos, +1.0f, +fu, +fv); break;
		case 4: Set(ppos, +fv, +1.0f, +fu); break;
		case 5: Set(ppos, +fu, +fv, +1.0f); break;
	}
}


void LLBlob::create(D3DVECTOR pos, D3DVECTOR4 color)
{
	// A blob is modeled as a subdivided cube.

	m_BlobColor = color;
	m_Pos = pos;
	Set(&m_Scale, 0.9f, 0.9f, 0.9f);
	m_DeformationInertia = spLL->fRand11() * 0.1f;


	m_fRadius = LavaLampInterior::fRand01();
	m_fRadius = 0.5f * (m_fRadius*m_fRadius + LavaLampInterior::fRand01());
	m_fRadius = 0.03f + 0.05f * m_fRadius;
	m_TemperatureAbsorbance = 0.05f / m_fRadius;	// is just representational

	m_Temperature = 0.5f + 0.2f * LavaLampInterior::fRand11();


	m_Subdivisions = 4;	// face is a grid of sd+1 quads square
	m_fDivisionStep = 2.0f / m_Subdivisions;

//MTS	m_dwNumVertices = 2*(subdivisions+2)*(subdivisions+2) + 4*subdivisions*(subdivisions+1);	// number of unique vertices
	m_dwNumVertices = 6*(m_Subdivisions+1)*(m_Subdivisions+1);
	m_dwNumIndices = 6*(m_Subdivisions)*(m_Subdivisions)*2*3;


	gpd3dDev->CreateVertexBuffer( m_dwNumVertices * sizeof(BlobVertex), 0, 0, 0, &m_pBlobVB);
	gpd3dDev->CreateIndexBuffer(  m_dwNumIndices  * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pBlobIB);


	BlobVertex* p_verts;
	WORD* p_indices;

	m_pBlobVB->Lock(0, 0, (BYTE**)&p_verts, 0);
	m_pBlobIB->Lock(0, 0, (BYTE**)&p_indices, 0);



	// Create vertices.
	BlobVertex* p_vert = &p_verts[0];
	for (int k=0; k<6; k++)
	{
		for (int j=0; j<=m_Subdivisions; j++)
		{
			for (int i=0; i<=m_Subdivisions; i++)
			{
				calcFacePoint(&p_vert->pos, k, i, j);
				float oo_dist = 1.0f / sqrtf(p_vert->pos.x*p_vert->pos.x + p_vert->pos.y*p_vert->pos.y + p_vert->pos.z*p_vert->pos.z);
				p_vert->pos.x *= oo_dist;
				p_vert->pos.y *= oo_dist;
				p_vert->pos.z *= oo_dist;
				p_vert++;
			}
		}
	}


	// Create indices.
	WORD* p_index = &p_indices[0];
	for (int k=0; k<6; k++)
	{
		int face_start = k*(m_Subdivisions+1)*(m_Subdivisions+1);
		for (int j=0; j<m_Subdivisions; j++)
		{
			for (int i=0; i<m_Subdivisions; i++)
			{
				*(p_index++) = face_start + (j+0)*(m_Subdivisions+1) + (i+0);
				*(p_index++) = face_start + (j+0)*(m_Subdivisions+1) + (i+1);
				*(p_index++) = face_start + (j+1)*(m_Subdivisions+1) + (i+0);

				*(p_index++) = face_start + (j+1)*(m_Subdivisions+1) + (i+1);
				*(p_index++) = face_start + (j+1)*(m_Subdivisions+1) + (i+0);
				*(p_index++) = face_start + (j+0)*(m_Subdivisions+1) + (i+1);
			}
		}
	}



	m_pBlobVB->Unlock();
	m_pBlobIB->Unlock();
}




void LavaLampInterior::advanceTime(float fElapsedTime, float fDt)
{
	if (fDt < 0.0f) fDt = 0.0f;
	if (fDt > 0.1f) fDt = 0.1f;

//MTS	m_BlobRenderer.advanceTime(fElapsedTime, fDt);

	for (int i=0; i<NUM_LLBLOBS; i++)
	{
		m_Blobs[i].advanceTime(fElapsedTime, fDt);
	}

	recomputeSpecie();
}


void LLBlob::advanceTime(float fElapsedTime, float fDt)
{
	float ambient_temp = spLL->getTemperature(m_Pos.z);
	float scale = 0.002f * m_TemperatureAbsorbance*m_TemperatureAbsorbance * fDt;
	m_Temperature += scale * (ambient_temp - m_Temperature);

	// For purposes of the simulation, water stays at a constant density.
	// A blob's density is equal to water at temperature 0.5f. For each unit
	// of temperature difference, an acceleration of 1.0 m/s/s is applied.
	m_Velocity.z += fDt * 1.0f * (m_Temperature-0.5f);

	m_Accel.x += LavaLampInterior::fRand11() * fDt;
	m_Accel.y += LavaLampInterior::fRand11() * fDt;
	m_Accel.z += LavaLampInterior::fRand11() * fDt;
//MTS	m_Velocity.x += LavaLampInterior::fRand11() * fDt * fDt;
//MTS	m_Velocity.y += LavaLampInterior::fRand11() * fDt * fDt;
//MTS	m_Velocity.z += LavaLampInterior::fRand11() * fDt * fDt;

	m_Accel.x = min(+0.05f, max(-0.05f, m_Accel.x));
	m_Accel.y = min(+0.05f, max(-0.05f, m_Accel.y));
	m_Accel.z = min(+0.05f, max(-0.05f, m_Accel.z));

	if (Length2(m_Accel) > 1.0f) Scale(&m_Accel, 0.96f);

	// Friction moving through water.
	AddScaled(&m_Velocity, m_Accel, fDt);
	float vel2 = Length2(m_Velocity);
	Scale(&m_Velocity, 1.0f - fDt * 120.0f * vel2);

	AddScaled(&m_Pos, m_Velocity, fDt);


	// Do collisions.
	spLL->collide(this, m_Pos.x, m_Pos.y, m_Pos.z, m_fRadius, fDt);


	// Adjust render wobble.
	m_Scale.x += m_DeformationInertia * fDt * m_TemperatureAbsorbance;
	m_Scale.y += m_DeformationInertia * fDt * m_TemperatureAbsorbance;
	m_Scale.z = 0.9f - (m_Scale.x - 0.9f) * (0.9f+0.9f)*0.9f / (0.9f*0.9f);

	float accel;
	if (m_DeformationInertia > 0.0f)
	{
		accel = 0.91f - m_Scale.x;
	}
	else
	{
		accel = 0.89f - m_Scale.x;
	}

	m_DeformationInertia += 20.0f * accel * fDt;
	m_DeformationInertia = max(-0.3f, min(+0.3f, m_DeformationInertia));
}


float LavaLampInterior::getTemperature(float z) const
{
	float dz = -0.5f + (z - m_ConicSectionBotZ[0]) / (m_ConicSectionBotZ[m_NumConicSections] - m_ConicSectionBotZ[0]);
	dz *= 2.6f;
	dz *= dz*dz;
	return max(0.0f, min(1.0f, 0.5f - dz));
}


bool LavaLampInterior::collideWithCaps(LLBlob* pllb, float x, float y, float z, float radius) const
{
	if (z - radius < m_ConicSectionBotZ[0])
	{
		D3DVECTOR pos, norm;
		Set(&pos, x, y, m_ConicSectionBotZ[0] + radius + 0.001f);
		Set(&norm, 0.0f, 0.0f, +1.0f);
		pllb->collided(pos, norm);
		return true;
	}

	if (z + radius > m_ConicSectionBotZ[m_NumConicSections])
	{
		D3DVECTOR pos, norm;
		Set(&pos, x, y, m_ConicSectionBotZ[m_NumConicSections] - radius - 0.001f);
		Set(&norm, 0.0f, 0.0f, -1.0f);
		pllb->collided(pos, norm);
		return true;
	}
	return false;
}


void LavaLampInterior::collide(LLBlob* pllb, float x, float y, float z, float radius, float dt) const
{
	// Bounce off the caps.
	if (collideWithCaps(pllb, x, y, z, radius))
	{
		z = pllb->getPos().z;
	}

	x -= m_ConicSectionCenterX;
	y -= m_ConicSectionCenterY;
	float r = sqrtf( x*x + y*y );

	// Check the conic sections. Check the surfaces first, the corners will be checked afterwards.
	int i;
	bool b_hit_wall = false;
	for (i=0; i<m_NumConicSections; i++)
	{
		if (m_ConicSectionBotZ[i+1] < z - radius) continue;
		if (m_ConicSectionBotZ[i+0] > z + radius) break;

		// Sphere overlaps the section at least somewhat. Find the nearest
		// point on the cone surface and see if that is within the range.
		float dz = z - m_ConicSectionBotZ[i];
		float dr = r - m_ConicSectionRadius[i];

		float overlap = radius - (dr*m_ConicSectionNormalR[i] + dz*m_ConicSectionNormalZ[i]);
		if (overlap < 0.0f) continue;

		// Calculate distance along surface to the nearest point of collision.
		float s = dr*m_ConicSectionNormalZ[i] - dz*m_ConicSectionNormalR[i];
		if (s<0.0f) continue;
		float height = m_ConicSectionBotZ[i+1]-m_ConicSectionBotZ[i];
		if (s*s > height*height * (1.0f + m_ConicSectionSlope[i]*m_ConicSectionSlope[i])) continue;

		// Collision.
		float nz = m_ConicSectionNormalZ[i];
		float oo_r = 1.0f / max(0.001f, r);
		float nx = x * oo_r * m_ConicSectionNormalR[i];
		float ny = y * oo_r * m_ConicSectionNormalR[i];

		D3DVECTOR pos, norm;
		x += nx*overlap;
		y += ny*overlap;
		z += nz*overlap;
		r = sqrtf( x*x + y*y );	// recompute radius
		Set(&pos, m_ConicSectionCenterX + x, m_ConicSectionCenterY + y, z);
		Set(&norm, nx, ny, nz);
		pllb->collided(pos, norm);
		b_hit_wall = true;
	}


	// Check the corners.
	if (!b_hit_wall) for (i=0; i<m_NumConicSections; i++)
	{
		if (m_ConicSectionBotZ[i+1] < z - radius) continue;
		if (m_ConicSectionBotZ[i+0] > z + radius) break;

		float dz = z - m_ConicSectionBotZ[i];
		float dr = r - m_ConicSectionRadius[i];

		if (dz*dz + dr*dr > radius*radius) continue;
		float dist = sqrtf(dz*dz + dr*dr);
		float overlap =  radius - dist;

		float f_norm = 1.0f / max(0.001f, dist);
		float nz = -dz * f_norm;
		float nr = -dr * f_norm;

		float oo_r = 1.0f / max(0.001f, r);
		float nx = x * oo_r * nr;
		float ny = y * oo_r * nr;

		D3DVECTOR pos, norm;
		x += nx*overlap;
		y += ny*overlap;
		z += nz*overlap;
		r = sqrtf( x*x + y*y );	// recompute radius
		Set(&pos, m_ConicSectionCenterX + x, m_ConicSectionCenterY + y, z);
		Set(&norm, nx, ny, nz);
		pllb->collided(pos, norm);
		b_hit_wall = true;
	}


	// Check other blobs.
	for (i=0; i<NUM_LLBLOBS; i++)
	{
		if (&m_Blobs[i] == pllb) continue;
		D3DVECTOR delta;
		Sub(m_Blobs[i].getPos(), pllb->getPos(), &delta);
		float rad = (m_Blobs[i].getRadius() + pllb->getRadius());

		if (Length2(delta) > rad*rad) continue;
		pllb->interactWithBlob(&m_Blobs[i], dt);
	}

	if (collideWithCaps(pllb, x + m_ConicSectionCenterX, y + m_ConicSectionCenterY, z, radius))
	{
		// Should never happen with convex hull.
		z = pllb->getPos().z;
	}
}


void LLBlob::collided(D3DVECTOR pos, D3DVECTOR normal)
{
	D3DVECTOR diff;
	Sub(pos, m_Pos, &diff);
	if (Length2(diff) > 0.5f * m_fRadius * m_fRadius)
	{
		int a = 0;
	}

	float dot;
	dot = Dot(normal, m_Accel);
	if (dot<0.0f) AddScaled(&m_Accel, normal, -dot);

	dot = Dot(normal, m_Velocity);
	if (dot<0.0f) AddScaled(&m_Velocity, normal, -dot);

	m_Pos = pos;
}


void LLBlob::interactWithBlob(const LLBlob* pllb, float dt)
{
	// Attract at range, repel when close.

	float mass_b = pllb->m_fRadius;
	mass_b *= mass_b*mass_b;

	D3DVECTOR delta;
	Sub(pllb->getPos(), getPos(), &delta);
	float dist2 = Length2(delta);
	if (dist2 < 0.000001f) return;

	D3DVECTOR delta_v;
	Sub(pllb->m_Velocity, m_Velocity, &delta_v);
	float f_part_mag = Dot(delta_v, delta);


	if (getSpecies() == pllb->getSpecies())
	{
		// Same species.
		float extreme_rad = (m_fRadius + pllb->m_fRadius);
		float attract = mass_b * dt * 10000.0f * ((f_part_mag > 0.0f) ? 1.0f : 0.5f);
		AddScaled(&m_Velocity, delta, attract);
//MTS		float attract = ((1.0f / dist2) - (1.0f / (extreme_rad*extreme_rad))) * mass_b * dt * 5.0f;
//MTS		AddScaled(&m_Accel, delta, attract);

		// Should try adding to the velocity directly...

		extreme_rad *= 0.6f;
		float extreme_rad2 = extreme_rad*extreme_rad;
		float repel = ((1.0f / min(extreme_rad2*0.04f, dist2)) - (1.0f / extreme_rad2)) * mass_b * dt * 5.0f;
		repel *=  ((f_part_mag > 0.0f) ? 0.3f : 1.0f);
		if (repel > 0.0f) AddScaled(&m_Velocity, delta, -repel);
//MTS		float repel = ((1.0f / min(extreme_rad2*0.04f, dist2)) - (1.0f / extreme_rad2)) * mass_b * dt * 500.0f;
//MTS		if (repel > 0.0f) AddScaled(&m_Accel, delta, -repel);
	}
	else
	{
		// Different specie.
		float extreme_rad2 = (m_fRadius + pllb->m_fRadius);
		extreme_rad2 *= extreme_rad2;
		float repel = ((1.0f / min(extreme_rad2*0.04f, dist2)) - (1.0f / extreme_rad2)) * mass_b * dt * 1.0f;
		repel *=  ((f_part_mag > 0.0f) ? 0.3f : 1.0f);
		AddScaled(&m_Velocity, delta, -repel);
//MTS		float repel = ((1.0f / min(extreme_rad2*0.04f, dist2)) - (1.0f / extreme_rad2)) * mass_b * dt * 50.0f;
//MTS		AddScaled(&m_Accel, delta, -repel);
	}
}



void LavaLampInterior::recomputeSpecie()
{
	int prev_species[NUM_LLBLOBS];
	int i;
	for (i=0; i<NUM_LLBLOBS; i++)
	{
		prev_species[i] = m_Blobs[i].getSpecies();
		m_Blobs[i].setSpecies(i);
	}

	for (i=0; i<NUM_LLBLOBS-1; i++)
	{
		for (int j=i+1; j<NUM_LLBLOBS; j++)
		{
			D3DVECTOR diff;
			Sub(m_Blobs[i].getPos(), m_Blobs[j].getPos(), &diff);
			float dist2 = Length2(diff);
			if (prev_species[i] == prev_species[j])
			{
				// Were connected.
				float radii2 = m_Blobs[i].getRadius() + m_Blobs[j].getRadius();
				radii2 *= radii2;
				if (dist2 < 0.9f*0.9f * radii2)
				{
					// Are still touching. Maintain the same specie.
					m_Blobs[j].setSpecies( m_Blobs[i].getSpecies() );
				}
			}
			else
			{
				// Were not connected.
				float check_rad = max(m_Blobs[i].getRadius(), m_Blobs[j].getRadius());
				check_rad += 0.7f * min(m_Blobs[i].getRadius(), m_Blobs[j].getRadius());
				if (dist2 < check_rad*check_rad)
				{
					// Connect them only if they are in the top or bottom.
					bool b_close_to_end = false;
					float z = m_Blobs[i].getPos().z;
					float r = m_Blobs[i].getRadius();
					b_close_to_end = b_close_to_end || (z - m_ConicSectionBotZ[0] < 1.5f * r);
					b_close_to_end = b_close_to_end || (m_ConicSectionBotZ[m_NumConicSections] - z < 1.5f * r);
					z = m_Blobs[j].getPos().z;
					r = m_Blobs[j].getRadius();
					b_close_to_end = b_close_to_end || (z - m_ConicSectionBotZ[0] < 1.5f * r);
					b_close_to_end = b_close_to_end || (m_ConicSectionBotZ[m_NumConicSections] - z < 1.5f * r);

					if (b_close_to_end)
					{
						m_Blobs[j].setSpecies( m_Blobs[i].getSpecies() );
					}
				}
			}
		}
	}

	for (i=0; i<NUM_LLBLOBS; i++)
	{
		m_Blobs[i].setSpecies( m_Blobs[m_Blobs[i].getSpecies()].getSpecies() );
	}


	// For debug purposes, color code the blobs based on specie.
	for (i=0; i<NUM_LLBLOBS; i++)
	{
		D3DVECTOR4 color, base_color;
		int s = m_Blobs[i].getSpecies();
		Set(&color, (s&0x04) ? 0.0f : 1.0f, (s&0x02) ? 0.0f : 1.0f, (s&0x01) ? 0.0f : 1.0f, 1.0f);
		Set(&base_color, 0.724f, 0.732f, 0.556f, 1.0f);
		color.x = color.x*0.05f + base_color.x*0.95f;
		color.y = color.y*0.05f + base_color.y*0.95f;
		color.z = color.z*0.05f + base_color.z*0.95f;
		color.w = color.w*0.05f + base_color.w*0.95f;
//MTS		m_Blobs[i].setColor(color);
	}
}




void LavaLampInterior::render()
{
    // Set default states
    gpd3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );
    gpd3dDev->SetRenderState( D3DRS_ZENABLE,  TRUE );
    gpd3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    gpd3dDev->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    gpd3dDev->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

    gpd3dDev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
    gpd3dDev->SetRenderState( D3DRS_ALPHAFUNC,       D3DCMP_GREATEREQUAL );
    gpd3dDev->SetRenderState( D3DRS_ALPHAREF,        0x00000001 );
    gpd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    gpd3dDev->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
    gpd3dDev->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );
//MTS    gpd3dDev->SetRenderState( D3DRS_FILLMODE,         D3DFILL_WIREFRAME );

    gpd3dDev->SetTexture( 0, gApp.pNormalCubeMap );


    gpd3dDev->SetVertexShader( m_dwVertexShader );
    gpd3dDev->SetPixelShader( m_dwPixelShader );



    // Note: when passing matrices to a vertex shader, we transpose them, since
    // matrix multiplies are done with dot product operations on the matrix rows.

	D3DMATRIX matFinal,matTmp;
	MulMats(gApp.theCamera.matWTC,gApp.theCamera.matProj,&matTmp);
	SetTranspose(matTmp,&matFinal);
	gpd3dDev->SetVertexShaderConstant(4,(CONST void *)&matFinal,4);


	//; Expected vertex shaders constants
	//;    c0-c3    = Transpose of world matrix
	//;    c4-c7    = Transpose of view*projection matrix
	//;    c8       = some constants, x=0, y=1, z=2, w=0.5
	D3DVECTOR4 val;

	// Constants
	Set(&val, 0.0f, 1.0f, 2.0f, 0.5f);
	gpd3dDev->SetVertexShaderConstant( 8, &val, 1 );



//MTS	m_BlobRenderer.render();

    
	for (int i=0; i<NUM_LLBLOBS; i++)
	{
		m_Blobs[i].render();
	}


    // Restore the state
    gpd3dDev->SetPixelShader( NULL );
    gpd3dDev->SetVertexShader( NULL );

	gpd3dDev->SetTexture(0, NULL);
}



void LLBlob::render()
{
	// Make the object to World transform. Transpose it.
    D3DMATRIX matT, mat;

	SetIdentity( &mat );
	mat.m[0][0] = m_Scale.x * m_fRadius;
	mat.m[1][1] = m_Scale.y * m_fRadius;
	mat.m[2][2] = m_Scale.z * m_fRadius;
	mat.m[3][0] = m_Pos.x;
	mat.m[3][1] = m_Pos.y;
	mat.m[3][2] = m_Pos.z;

	SetTranspose( mat, &matT );
    gpd3dDev->SetVertexShaderConstant( 0, &matT, 4 );


	// Expected pixel shader constants
	//; c0          = light 1 direction
	//; c1          = light 2 direction
	//; c2          = base blob color
	//; c3          = ambient color
	D3DVECTOR4 val;
	Set(&val, 0.5f, 0.6f, 0.5f, 1.0f );
	gpd3dDev->SetPixelShaderConstant( 0, &val, 1 );

	Set(&val, 0.5f, 0.4f, 0.5f, 1.0f );
	gpd3dDev->SetPixelShaderConstant( 1, &val, 1 );

	// Blob color
	gpd3dDev->SetPixelShaderConstant( 2, &m_BlobColor, 1 );

	// Ambient light
	val = m_BlobColor;
	val.x *= 0.6f;
	val.y *= 0.6f;
	val.z *= 0.6f;
	gpd3dDev->SetPixelShaderConstant( 3, &val, 1 );


    // This could be set in the lava lamp, but we might have blobs with different numbers of vertices.
	gpd3dDev->SetIndices( m_pBlobIB, 0 );
    gpd3dDev->SetStreamSource( 0, m_pBlobVB, sizeof(BlobVertex) );

    gpd3dDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumVertices, 
                                        0, m_dwNumIndices/3 );
}
