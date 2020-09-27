//-----------------------------------------------------------------------------
// FILE: Polyhedron.cpp
//
// Desc: Collision primitive for all objects.
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Common.h"
#include <memory>
#include <assert.h>
#include "Polyhedron.h"
#include "PerfTimer.h"
#include "Globals.h"




Polyhedron::Polyhedron()
{
	m_iNumFaces = 0;
	m_iNumVerts = 0;
	m_iNumVertPtrsUsed = 0;
}


Polyhedron::Polyhedron(float w, float h, float d)
{
	float hh = 0.5f * h;
	float hw = 0.5f * w;
	float hd = 0.5f * d;

	new (this) Polyhedron( Vector3(-hw, -hh, -hd), Vector3(hw, hh, hd) );
}


Polyhedron::Polyhedron(const Vector3& min, const Vector3& max)
{
	m_iNumFaces = 6;
	m_iNumVerts = 8;

	for (int i = 0; i < 6; i++)
		m_Faces[i].pVerts = m_VertPtrPool + i*4;

	m_iNumVertPtrsUsed = 6*4;

	for (int i = 0; i < 8; i++)
	{
		m_v3Verts[i].x = (i & 4) ? max.x : min.x;
		m_v3Verts[i].y = (i & 2) ? max.y : min.y;
		m_v3Verts[i].z = (i & 1) ? max.z : min.z;
	}

	// Bottom.
	m_Faces[0].plPlane = Plane3(0.0f, -1.0f, 0.0f, min.y);
	m_Faces[0].iNumVerts = 4;
	m_Faces[0].pVerts[0] = 0; // 000
	m_Faces[0].pVerts[1] = 4; // 100
	m_Faces[0].pVerts[2] = 5; // 101
	m_Faces[0].pVerts[3] = 1; // 001

	// Top.
	m_Faces[1].plPlane = Plane3(0.0f, 1.0f, 0.0f, -max.y);
	m_Faces[1].iNumVerts = 4;
	m_Faces[1].pVerts[0] = 2; // 010
	m_Faces[1].pVerts[1] = 3; // 011
	m_Faces[1].pVerts[2] = 7; // 111
	m_Faces[1].pVerts[3] = 6; // 110

	// Left.
	m_Faces[2].plPlane = Plane3(-1.0f, 0.0f, 0.0f, min.x);
	m_Faces[2].iNumVerts = 4;
	m_Faces[2].pVerts[0] = 0; // 000
	m_Faces[2].pVerts[1] = 1; // 001
	m_Faces[2].pVerts[2] = 3; // 011
	m_Faces[2].pVerts[3] = 2; // 010

	// Right.
	m_Faces[3].plPlane = Plane3(1.0f, 0.0f, 0.0f, -max.x);
	m_Faces[3].iNumVerts = 4;
	m_Faces[3].pVerts[0] = 4; // 100
	m_Faces[3].pVerts[1] = 6; // 110
	m_Faces[3].pVerts[2] = 7; // 111
	m_Faces[3].pVerts[3] = 5; // 101

	// Front.
	m_Faces[4].plPlane = Plane3(0.0f, 0.0f, -1.0f, min.z);
	m_Faces[4].iNumVerts = 4;
	m_Faces[4].pVerts[0] = 0; // 000
	m_Faces[4].pVerts[1] = 2; // 010
	m_Faces[4].pVerts[2] = 6; // 110
	m_Faces[4].pVerts[3] = 4; // 100

	// Back.
	m_Faces[5].plPlane = Plane3(0.0f, 0.0f, 1.0f, -max.z);
	m_Faces[5].iNumVerts = 4;
	m_Faces[5].pVerts[0] = 1; // 001
	m_Faces[5].pVerts[1] = 5; // 101
	m_Faces[5].pVerts[2] = 7; // 111
	m_Faces[5].pVerts[3] = 3; // 011
}


Polyhedron::Polyhedron(float radius, int numMajor, int numMinor)
{
	float majorStep = (fPI / numMajor);
	float minorStep = (2.0f * fPI / numMinor);

	m_iNumFaces = numMajor * numMinor;
	m_iNumVerts = 2 + (numMajor - 1) * numMinor;
	m_iNumVertPtrsUsed = 0;

	int k = 0;

    // Build verts
    for( int i = 0; i <= numMajor; ++i ) 
    {
        FLOAT a = i * majorStep;
        FLOAT r0 = radius * sinf(a);
        FLOAT z0 = radius * cosf(a);

		if (i == 0)
		{
			// Top vert.
			m_v3Verts[k].x = 0.0f;
			m_v3Verts[k].y = 0.0f;
			m_v3Verts[k].z = z0;

			++k;
		}
		else if (i == numMajor)
		{
			// Bottom vert.
			m_v3Verts[k].x = 0.0f;
			m_v3Verts[k].y = 0.0f;
			m_v3Verts[k].z = z0;

			++k;
		}
		else
		{
			for( int j = 0; j < numMinor; ++j )
			{
				FLOAT c = j * minorStep;
				FLOAT x = cosf(c);
				FLOAT y = sinf(c);

				m_v3Verts[k].x = x * r0;
				m_v3Verts[k].y = y * r0;
				m_v3Verts[k].z = z0;

				++k;
			}
		}
    }

	assert(k == m_iNumVerts);

	k = 0;

	// Build faces.
    for( int i = 0; i < numMajor; ++i )
    {
        for( int j = 0; j < numMinor; ++j )
        {
			if (i == 0)
			{
				// Top tri.
				m_Faces[k].iNumVerts = 3;
				m_Faces[k].pVerts = m_VertPtrPool + m_iNumVertPtrsUsed;
				m_iNumVertPtrsUsed += 3;

				int base = 1;

				m_Faces[k].pVerts[0] = base + j;
				m_Faces[k].pVerts[1] = 0;
				m_Faces[k].pVerts[2] = base + (j+1) % numMinor;

				m_Faces[k].plPlane = Plane3(m_v3Verts[m_Faces[k].pVerts[0]], 
										    m_v3Verts[m_Faces[k].pVerts[1]], 
										    m_v3Verts[m_Faces[k].pVerts[2]]);
			}
			else if (i == numMajor-1)
			{
				// Bottom tri.
				m_Faces[k].iNumVerts = 3;
				m_Faces[k].pVerts = m_VertPtrPool + m_iNumVertPtrsUsed;
				m_iNumVertPtrsUsed += 3;

				int base = 1 + (i - 1) * numMinor;

				m_Faces[k].pVerts[0] = base + j;
				m_Faces[k].pVerts[1] = base + (j+1) % numMinor;
				m_Faces[k].pVerts[2] = m_iNumVerts-1;

				m_Faces[k].plPlane = Plane3(m_v3Verts[m_Faces[k].pVerts[0]], 
										    m_v3Verts[m_Faces[k].pVerts[1]], 
										    m_v3Verts[m_Faces[k].pVerts[2]]);
			}
			else
			{
				m_Faces[k].iNumVerts = 4;
				m_Faces[k].pVerts = m_VertPtrPool + m_iNumVertPtrsUsed;
				m_iNumVertPtrsUsed += 4;

				int base = 1 + (i - 1) * numMinor;

				m_Faces[k].pVerts[0] = base + j;
				m_Faces[k].pVerts[1] = base + (j+1) % numMinor;
				m_Faces[k].pVerts[2] = base + numMinor + (j+1) % numMinor;
				m_Faces[k].pVerts[3] = base + numMinor + j;

				m_Faces[k].plPlane = Plane3(m_v3Verts[m_Faces[k].pVerts[0]], 
										    m_v3Verts[m_Faces[k].pVerts[1]], 
										    m_v3Verts[m_Faces[k].pVerts[2]]);
			}

			k++;
        }
    }

	assert(k == m_iNumFaces);
}


Polyhedron::Polyhedron(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& sweep)
{
	m_iNumFaces = 5;
	m_iNumVerts = 6;
	m_iNumVertPtrsUsed = 0;

	m_v3Verts[0] = v1;
	m_v3Verts[1] = v2;
	m_v3Verts[2] = v3;
	m_v3Verts[3] = v1 + sweep;
	m_v3Verts[4] = v2 + sweep;
	m_v3Verts[5] = v3 + sweep;

	// Top.
	m_Faces[0].plPlane = Plane3(v1, v2, v3);
	m_Faces[0].iNumVerts = 3;
	m_Faces[0].pVerts = m_VertPtrPool + m_iNumVertPtrsUsed;
	m_iNumVertPtrsUsed += 3;
	m_Faces[0].pVerts[0] = 0;
	m_Faces[0].pVerts[1] = 1;
	m_Faces[0].pVerts[2] = 2;

	// Sides.
	m_Faces[1].plPlane = Plane3(v2, v1, v1+sweep);
	m_Faces[1].iNumVerts = 4;
	m_Faces[1].pVerts = m_VertPtrPool + m_iNumVertPtrsUsed;
	m_iNumVertPtrsUsed += 4;
	m_Faces[1].pVerts[0] = 1;
	m_Faces[1].pVerts[1] = 0;
	m_Faces[1].pVerts[2] = 0 + 3;
	m_Faces[1].pVerts[3] = 1 + 3;

	m_Faces[2].plPlane = Plane3(v3, v2, v2+sweep);
	m_Faces[2].iNumVerts = 4;
	m_Faces[2].pVerts = m_VertPtrPool + m_iNumVertPtrsUsed;
	m_iNumVertPtrsUsed += 4;
	m_Faces[2].pVerts[0] = 2;
	m_Faces[2].pVerts[1] = 1;
	m_Faces[2].pVerts[2] = 1 + 3;
	m_Faces[2].pVerts[3] = 2 + 3;

	m_Faces[3].plPlane = Plane3(v1, v3, v3+sweep);
	m_Faces[3].iNumVerts = 4;
	m_Faces[3].pVerts = m_VertPtrPool + m_iNumVertPtrsUsed;
	m_iNumVertPtrsUsed += 4;
	m_Faces[3].pVerts[0] = 0;
	m_Faces[3].pVerts[1] = 2;
	m_Faces[3].pVerts[2] = 2 + 3;
	m_Faces[3].pVerts[3] = 0 + 3;

	// Bottom.
	m_Faces[4].plPlane = Plane3(v3+sweep, v2+sweep, v1+sweep);
	m_Faces[4].iNumVerts = 3;
	m_Faces[4].pVerts = m_VertPtrPool + m_iNumVertPtrsUsed;
	m_iNumVertPtrsUsed += 3;
	m_Faces[4].pVerts[0] = 2 + 3;
	m_Faces[4].pVerts[1] = 1 + 3;
	m_Faces[4].pVerts[2] = 0 + 3;
}


Polyhedron::Polyhedron(const Polyhedron& ph)
{
	m_iNumFaces = ph.m_iNumFaces;
	m_iNumVerts = ph.m_iNumVerts;
	m_iNumVertPtrsUsed = 0;

	for (int i = 0; i < m_iNumVerts; i++)
	{
		m_v3Verts[i] = ph.m_v3Verts[i];
	}

	for (int i = 0; i < m_iNumFaces; i++)
	{
		m_Faces[i].plPlane = ph.m_Faces[i].plPlane;
		m_Faces[i].iNumVerts = ph.m_Faces[i].iNumVerts;
		m_Faces[i].pVerts = m_VertPtrPool + m_iNumVertPtrsUsed;
		m_iNumVertPtrsUsed += m_Faces[i].iNumVerts;

		for (int j = 0; j < m_Faces[i].iNumVerts; j++)
			m_Faces[i].pVerts[j] = ph.m_Faces[i].pVerts[j];
	}
}


Polyhedron& Polyhedron::operator= (const Polyhedron& ph)
{
	new (this) Polyhedron(ph);

	return *this;
}


Polyhedron::Polyhedron(const Polyhedron& ph, const Transformation& tf)
{
	m_iNumFaces = ph.m_iNumFaces;
	m_iNumVerts = ph.m_iNumVerts;
	m_iNumVertPtrsUsed = 0;

	for (int i = 0; i < m_iNumVerts; i++)
	{
		m_v3Verts[i] = ph.m_v3Verts[i] * tf;
	}

	for (int i = 0; i < m_iNumFaces; i++)
	{
		m_Faces[i].plPlane = ph.m_Faces[i].plPlane * tf;
		m_Faces[i].iNumVerts = ph.m_Faces[i].iNumVerts;
		m_Faces[i].pVerts = m_VertPtrPool + m_iNumVertPtrsUsed;
		m_iNumVertPtrsUsed += m_Faces[i].iNumVerts;

		for (int j = 0; j < m_Faces[i].iNumVerts; j++)
			m_Faces[i].pVerts[j] = ph.m_Faces[i].pVerts[j];
	}
}


Polyhedron::~Polyhedron()
{
}


bool Polyhedron::PointInPolyhedron(const Vector3& pnt, float fEpsilon) const
{
	for (int i = 0; i < m_iNumFaces; i++)
	{
		float dist = m_Faces[i].plPlane.GetDistance(pnt);
		if (dist > fEpsilon)
			return false;
	}

	return true;
}


void Polyhedron::ComputeExtents(const Transformation& tf, Vector3* pMin, Vector3* pMax)
{
	pMin->x = FLT_MAX;
	pMin->y = FLT_MAX;
	pMin->z = FLT_MAX;

	pMax->x = -FLT_MAX;
	pMax->y = -FLT_MAX;
	pMax->z = -FLT_MAX;

	for (int i = 0; i < m_iNumVerts; i++)
	{
		Vector3 vert = m_v3Verts[i] * tf;

		if (vert.x < pMin->x)
			pMin->x = vert.x;

		if (vert.y < pMin->y)
			pMin->y = vert.y;

		if (vert.z < pMin->z)
			pMin->z = vert.z;

		if (vert.x > pMax->x)
			pMax->x = vert.x;

		if (vert.y > pMax->y)
			pMax->y = vert.y;

		if (vert.z > pMax->z)
			pMax->z = vert.z;
	}
}


void Polyhedron::ComputeExtents(Vector3* pMin, Vector3* pMax)
{
	pMin->x = FLT_MAX;
	pMin->y = FLT_MAX;
	pMin->z = FLT_MAX;

	pMax->x = -FLT_MAX;
	pMax->y = -FLT_MAX;
	pMax->z = -FLT_MAX;

	for (int i = 0; i < m_iNumFaces; i++)
	{
		Vector3& vert = m_v3Verts[i];

		if (vert.x < pMin->x)
			pMin->x = vert.x;

		if (vert.y < pMin->y)
			pMin->y = vert.y;

		if (vert.z < pMin->z)
			pMin->z = vert.z;

		if (vert.x > pMax->x)
			pMax->x = vert.x;

		if (vert.y > pMax->y)
			pMax->y = vert.y;

		if (vert.z > pMax->z)
			pMax->z = vert.z;
	}
}


bool Polyhedron::CheckForIntersection(const Transformation& ta, const Polyhedron& a, 
									  const Transformation& tb, const Polyhedron& b)
{
	const Transformation a_to_b = ta * ~tb;
	const Transformation b_to_a = ~a_to_b;

	// Are all points of b outside any plane of a?
	for (int i = 0; i < a.m_iNumFaces; i++)
	{
		bool bAllOut = true;

		for (int j = 0; j < b.m_iNumVerts; j++)
		{
			float d = a.m_Faces[i].plPlane.GetDistance(b.m_v3Verts[j] * b_to_a);

			if (d < 0.0f)
			{
				bAllOut = false;
				break;
			}
		}

		if (bAllOut)
			return false;
	}
				
	// Are all points of a outside any plane of b?
	for (int i = 0; i < b.m_iNumFaces; i++)
	{
		bool bAllOut = true;

		for (int j = 0; j < a.m_iNumVerts; j++)
		{
			float d = b.m_Faces[i].plPlane.GetDistance(a.m_v3Verts[j] * a_to_b);

			if (d < 0.0f)
			{
				bAllOut = false;
				break;
			}
		}

		if (bAllOut)
			return false;
	}

	// The polyhedrons may also be seperated by and edge/edge plane.

	return true;
}


bool Polyhedron::ComputeIntersectionInfo(const Transformation& ta, const Polyhedron& a, 
										 const Transformation& tb, const Polyhedron& b, 
										 int* pNumInfos, IntersectionInfo* pInfo)
{
	TimeStamp start, end;

	*pNumInfos = 0;

	GetTimeStamp(&start);

	// Transform both polyhedrons into world space.
	Polyhedron world_a(a, ta);
	Polyhedron world_b(b, tb);

	GetTimeStamp(&end);
	g_TransformTime += SecondsElapsed(start, end);

	GetTimeStamp(&start);

	// Find any vertices of a that are in b.
	for (int i = 0; i < world_a.m_iNumVerts; i++)
	{
		bool bInside = true;
		float nearest_dist = -FLT_MAX;
		int nearest_face = 0;

		for (int j = 0; j < world_b.m_iNumFaces; j++)
		{
			float d = world_b.m_Faces[j].plPlane.GetDistance(world_a.m_v3Verts[i]);

			if (d >= 0.0f)
			{
				bInside = false;
				break;
			}
			else if (d > nearest_dist)
			{
				nearest_dist = d;
				nearest_face = j;
			}
		}

		if (bInside)
		{
			// Add spring.
			pInfo[*pNumInfos].bAtowardsB = true;
			pInfo[*pNumInfos].v3ExtractionDirection = world_b.m_Faces[nearest_face].plPlane.GetNormal();
			pInfo[*pNumInfos].v3CenterOfIntersection = world_a.m_v3Verts[i];
			pInfo[*pNumInfos].fPenetrationDepth = -nearest_dist;
			(*pNumInfos)++;
		}
	}

	// Find any vertices of b that are in a.
	for (int i = 0; i < world_b.m_iNumVerts; i++)
	{
		bool bInside = true;
		float nearest_dist = -FLT_MAX;
		int nearest_face = 0;

		for (int j = 0; j < world_a.m_iNumFaces; j++)
		{
			float d = world_a.m_Faces[j].plPlane.GetDistance(world_b.m_v3Verts[i]);

			if (d >= 0.0f)
			{
				bInside = false;
				break;
			}
			else if (j == 0 /*d > nearest_dist*/)
			{
				nearest_dist = d;
				nearest_face = j;
			}
		}

		if (bInside)
		{
			// Add spring.
			pInfo[*pNumInfos].bAtowardsB = false;
			pInfo[*pNumInfos].v3ExtractionDirection = world_a.m_Faces[nearest_face].plPlane.GetNormal();
			pInfo[*pNumInfos].v3CenterOfIntersection = world_b.m_v3Verts[i];
			pInfo[*pNumInfos].fPenetrationDepth = -nearest_dist;
			(*pNumInfos)++;
		}
	}

	GetTimeStamp(&end);
	g_ClipTime += SecondsElapsed(start, end);

	return( *pNumInfos != 0 ? TRUE : FALSE );
}


// Some defines used by the following code.
#define CUBE(x) ((x) * (x) * (x))
#define SQR(x) ((x) * (x))
enum { X = 0, Y = 1, Z = 2 };


void Polyhedron::ComputeCenterOfMassAndVolume(Vector3* pCoM, float* pVolume)
{
	float total_volume = 0.0f;
	Vector3 center_of_mass(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < m_iNumFaces; i++)
	{
		assert(m_Faces[i].iNumVerts >= 3);

		// Area of face times normal.
		for (int j = 2; j < m_Faces[i].iNumVerts; j++)
		{
			// Area of triangle.
			Vector3 v1 = m_v3Verts[m_Faces[i].pVerts[j-1]] - m_v3Verts[m_Faces[i].pVerts[0]];
			Vector3 v2 = m_v3Verts[m_Faces[i].pVerts[j]] - m_v3Verts[m_Faces[i].pVerts[0]];
			Vector3 cross = (v1 ^ v2);
			float area = 0.5f * cross.GetLength();

			float volume = -m_Faces[i].plPlane.d * area;

			// Add the volume of this tetrahedron.
			total_volume += volume;

			// Centroid of triangle (and center of mass of this tetrahedron).
			Vector3 cent = m_v3Verts[m_Faces[i].pVerts[0]] + 
						   m_v3Verts[m_Faces[i].pVerts[j-1]] + 
						   m_v3Verts[m_Faces[i].pVerts[j]];

			center_of_mass += volume * cent;
		}
	}

	center_of_mass /= 4.0f * total_volume;
	total_volume *= (1.0f/3.0f);

	*pVolume = total_volume;
	*pCoM = center_of_mass;
}


//
// The following code is based on:
//
// Brian Mirtich, ``Fast and Accurate Computation of Polyhedral Mass Properties,'' 
// journal of graphics tools, volume 1, number 2, 1996
//
void Polyhedron::ComputeMassProperties(float fDensity, float* pMass, Vector3* pCoM, Matrix3* pIT)
{
	VolumeIntegrals vi;

	ComputeVolumeIntegrals(vi);

	float fMass = fDensity * vi.T0[X];
	float r[3];
	float J[3][3];

	// Compute center of mass.
	r[X] = vi.T1[X] / vi.T0[X];
	r[Y] = vi.T1[Y] / vi.T0[X];
	r[Z] = vi.T1[Z] / vi.T0[X];

	// Compute inertia tensor.
	J[X][X] = fDensity * (vi.T2[Y] + vi.T2[Z]);
	J[Y][Y] = fDensity * (vi.T2[Z] + vi.T2[X]);
	J[Z][Z] = fDensity * (vi.T2[X] + vi.T2[Y]);
	J[X][Y] = J[Y][X] = -fDensity * vi.TP[X];
	J[Y][Z] = J[Z][Y] = -fDensity * vi.TP[Y];
	J[Z][X] = J[X][Z] = -fDensity * vi.TP[Z];

	// Translate inertia tensor to center of mass.
	J[X][X] -= fMass * (r[Y]*r[Y] + r[Z]*r[Z]);
	J[Y][Y] -= fMass * (r[Z]*r[Z] + r[X]*r[X]);
	J[Z][Z] -= fMass * (r[X]*r[X] + r[Y]*r[Y]);
	J[X][Y] = J[Y][X] += fMass * r[X] * r[Y];
	J[Y][Z] = J[Z][Y] += fMass * r[Y] * r[Z];
	J[Z][X] = J[X][Z] += fMass * r[Z] * r[X];

	// Copy to output.
	*pMass = fMass;

	pCoM->x = r[X];
	pCoM->y = r[Y];
	pCoM->z = r[Z];

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			pIT->m[i][j] = J[i][j];
}


void Polyhedron::ComputeProjectionIntegrals(const Face& face, VolumeIntegrals& vi)
{
	vi.P1 = 0.0f;
	vi.Pa = 0.0f;
	vi.Pb = 0.0f;
	vi.Paa = 0.0f;
	vi.Pab = 0.0f;
	vi.Pbb = 0.0f;
	vi.Paaa = 0.0f;
	vi.Paab = 0.0f;
	vi.Pabb = 0.0f;
	vi.Pbbb = 0.0f;

	int v1 = face.iNumVerts-1;
	for (int v2 = 0; v2 < face.iNumVerts; v1 = v2, v2++) 
	{
		float a0 = m_v3Verts[face.pVerts[v1]][vi.A];
		float b0 = m_v3Verts[face.pVerts[v1]][vi.B];
		float a1 = m_v3Verts[face.pVerts[v2]][vi.A];
		float b1 = m_v3Verts[face.pVerts[v2]][vi.B];
		float da = a1 - a0;
		float db = b1 - b0;
		float a0_2 = a0 * a0; 
		float a0_3 = a0_2 * a0; 
		float a0_4 = a0_3 * a0;
		float b0_2 = b0 * b0; 
		float b0_3 = b0_2 * b0; 
		float b0_4 = b0_3 * b0;
		float a1_2 = a1 * a1; 
		float a1_3 = a1_2 * a1; 
		float b1_2 = b1 * b1; 
		float b1_3 = b1_2 * b1;

		float C1 = a1 + a0;
		float Ca = a1*C1 + a0_2; 
		float Caa = a1*Ca + a0_3; 
		float Caaa = a1*Caa + a0_4;
		float Cb = b1*(b1 + b0) + b0_2; 
		float Cbb = b1*Cb + b0_3; 
		float Cbbb = b1*Cbb + b0_4;
		float Cab = 3*a1_2 + 2*a1*a0 + a0_2; 
		float Kab = a1_2 + 2*a1*a0 + 3*a0_2;
		float Caab = a0*Cab + 4*a1_3; 
		float Kaab = a1*Kab + 4*a0_3;
		float Cabb = 4*b1_3 + 3*b1_2*b0 + 2*b1*b0_2 + b0_3;
		float Kabb = b1_3 + 2*b1_2*b0 + 3*b1*b0_2 + 4*b0_3;

		vi.P1 += db*C1;
		vi.Pa += db*Ca;
		vi.Paa += db*Caa;
		vi.Paaa += db*Caaa;
		vi.Pb += da*Cb;
		vi.Pbb += da*Cbb;
		vi.Pbbb += da*Cbbb;
		vi.Pab += db*(b1*Cab + b0*Kab);
		vi.Paab += db*(b1*Caab + b0*Kaab);
		vi.Pabb += da*(a1*Cabb + a0*Kabb);
	}

	vi.P1 *= (1.0f / 2.0f);
	vi.Pa *= (1.0f / 6.0f);
	vi.Paa *= (1.0f / 12.0f);
	vi.Paaa *= (1.0f / 20.0f);
	vi.Pb *= (1.0f / -6.0f);
	vi.Pbb *= (1.0f / -12.0f);
	vi.Pbbb *= (1.0f / -20.0f);
	vi.Pab *= (1.0f / 24.0f);
	vi.Paab *= (1.0f / 60.0f);
	vi.Pabb *= (1.0f / -60.0f);
}


void Polyhedron::ComputeFaceIntegrals(const Face& face, VolumeIntegrals& vi)
{
	ComputeProjectionIntegrals(face, vi);

	const float* n = face.plPlane;
	float w = face.plPlane.d;

	float k1 = 1.0f / n[vi.C]; 
	float k2 = k1 * k1; 
	float k3 = k2 * k1; 
	float k4 = k3 * k1;

	vi.Fa = k1 * vi.Pa;
	vi.Fb = k1 * vi.Pb;
	vi.Fc = -k2 * (n[vi.A]*vi.Pa + n[vi.B]*vi.Pb + w*vi.P1);

	vi.Faa = k1 * vi.Paa;
	vi.Fbb = k1 * vi.Pbb;
	vi.Fcc = k3 * (SQR(n[vi.A])*vi.Paa + 2*n[vi.A]*n[vi.B]*vi.Pab + SQR(n[vi.B])*vi.Pbb
		   + w*(2*(n[vi.A]*vi.Pa + n[vi.B]*vi.Pb) + w*vi.P1));

	vi.Faaa = k1 * vi.Paaa;
	vi.Fbbb = k1 * vi.Pbbb;
	vi.Fccc = -k4 * (CUBE(n[vi.A])*vi.Paaa + 3*SQR(n[vi.A])*n[vi.B]*vi.Paab 
			+ 3*n[vi.A]*SQR(n[vi.B])*vi.Pabb + CUBE(n[vi.B])*vi.Pbbb
			+ 3*w*(SQR(n[vi.A])*vi.Paa + 2*n[vi.A]*n[vi.B]*vi.Pab + SQR(n[vi.B])*vi.Pbb)
			+ w*w*(3*(n[vi.A]*vi.Pa + n[vi.B]*vi.Pb) + w*vi.P1));

	vi.Faab = k1 * vi.Paab;
	vi.Fbbc = -k2 * (n[vi.A]*vi.Pabb + n[vi.B]*vi.Pbbb + w*vi.Pbb);
	vi.Fcca = k3 * (SQR(n[vi.A])*vi.Paaa + 2*n[vi.A]*n[vi.B]*vi.Paab + SQR(n[vi.B])*vi.Pabb
			+ w*(2*(n[vi.A]*vi.Paa + n[vi.B]*vi.Pab) + w*vi.Pa));
}


void Polyhedron::ComputeVolumeIntegrals(VolumeIntegrals& vi)
{
	vi.T0[X] = 0.0f;
	vi.T0[Y] = 0.0f;
	vi.T0[Z] = 0.0f;
	vi.T1[X] = 0.0f;
	vi.T1[Y] = 0.0f;
	vi.T1[Z] = 0.0f;
	vi.T2[X] = 0.0f;
	vi.T2[Y] = 0.0f;
	vi.T2[Z] = 0.0f;
	vi.TP[X] = 0.0f;
	vi.TP[Y] = 0.0f;
	vi.TP[Z] = 0.0f;

	for (int i = 0; i < m_iNumFaces; i++) 
	{
		float nx = fabsf(m_Faces[i].plPlane[X]);
		float ny = fabsf(m_Faces[i].plPlane[Y]);
		float nz = fabsf(m_Faces[i].plPlane[Z]);

		if (nx > ny && nx > nz)
			vi.C = X;
		else
			vi.C = (ny > nz) ? Y : Z;

		vi.A = (vi.C + 1) % 3;
		vi.B = (vi.A + 1) % 3;

		ComputeFaceIntegrals(m_Faces[i], vi);

		vi.T0[vi.A] += m_Faces[i].plPlane[vi.A] * vi.Fa;
		vi.T0[vi.B] += m_Faces[i].plPlane[vi.B] * vi.Fb;
		vi.T0[vi.C] += m_Faces[i].plPlane[vi.C] * vi.Fc;

		vi.T1[vi.A] += m_Faces[i].plPlane[vi.A] * vi.Faa;
		vi.T1[vi.B] += m_Faces[i].plPlane[vi.B] * vi.Fbb;
		vi.T1[vi.C] += m_Faces[i].plPlane[vi.C] * vi.Fcc;
		vi.T2[vi.A] += m_Faces[i].plPlane[vi.A] * vi.Faaa;
		vi.T2[vi.B] += m_Faces[i].plPlane[vi.B] * vi.Fbbb;
		vi.T2[vi.C] += m_Faces[i].plPlane[vi.C] * vi.Fccc;
		vi.TP[vi.A] += m_Faces[i].plPlane[vi.A] * vi.Faab;
		vi.TP[vi.B] += m_Faces[i].plPlane[vi.B] * vi.Fbbc;
		vi.TP[vi.C] += m_Faces[i].plPlane[vi.C] * vi.Fcca;
	}

	vi.T1[X] *= (1.0f / 2.0f); 
	vi.T1[Y] *= (1.0f / 2.0f); 
	vi.T1[Z] *= (1.0f / 2.0f);
	vi.T2[X] *= (1.0f / 3.0f); 
	vi.T2[Y] *= (1.0f / 3.0f); 
	vi.T2[Z] *= (1.0f / 3.0f);
	vi.TP[X] *= (1.0f / 2.0f); 
	vi.TP[Y] *= (1.0f / 2.0f); 
	vi.TP[Z] *= (1.0f / 2.0f);
}
