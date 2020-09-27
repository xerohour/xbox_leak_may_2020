//-----------------------------------------------------------------------------
// FILE: Polyhedron.h
//
// Desc: Collision primitive for all objects.
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef TECH_CERT_GAME_POLYHEDRON_H
#define TECH_CERT_GAME_POLYHEDRON_H

#include "Common.h"
#include "XMath.h"
#include "PhysicsShape.h"


const int AVG_VERTS_PER_FACE = 4;
const int MAX_FACES_PER_POLYHEDRON = 64;
const int MAX_VERTS_PER_POLYHEDRON = 64;


//
// Convex polyhedron.
//
class Polyhedron
{
public:
	Polyhedron();

	// Construct a box centered at zero.
	Polyhedron(float w, float h, float d);

	// Construct a box with an arbitrary center.
	Polyhedron(const Vector3& min, const Vector3& max);

	// Construct a sphere.
	Polyhedron(float radius, int num_major, int num_minor);

	// Construct a volume from a triangle swept along a direction.
	Polyhedron(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& sweep);

	// Copy constructor.
	Polyhedron(const Polyhedron& ph);

	// Assignment operator.
	Polyhedron& operator= (const Polyhedron& ph);

	// Construct a copy transformed by the given coordinate frame.
	Polyhedron(const Polyhedron& ph, const Transformation& tf);

	// Destructor.
	~Polyhedron();

	// Return true if a point is inside a polyhedron.
	bool PointInPolyhedron(const Vector3& pnt, float fEpsilon = 0.0f) const;

	// Compute world space extents of the polyhedron (with a transform).
	void ComputeExtents(const Transformation& tf, Vector3* pMin, Vector3* pMax);

	// Compute world space extents of the polyhedron.
	void ComputeExtents(Vector3* pMin, Vector3* pMax);

	// Return true if the two polyhedrons intersect.
	static bool CheckForIntersection(const Transformation& ta, const Polyhedron& a, 
									 const Transformation& tb, const Polyhedron& b);

	// Find intersections.
	static bool ComputeIntersectionInfo(const Transformation& ta, const Polyhedron& a, 
										const Transformation& tb, const Polyhedron& b,
										int* pNumInfos, IntersectionInfo* pInfo);

	// Compute the center of mass of a polyhedron.
	void ComputeCenterOfMassAndVolume(Vector3* pCoM, float* pVolume);

	// Compute the mass, center of mass, and intertia tensor given the density.
	void ComputeMassProperties(float fDensity, float* pMass, Vector3* pCoM, Matrix3* pIT);

private:
	// A Face of the polyhedron.
	struct Face
	{
		Plane3 plPlane;
		int iNumVerts;
		int* pVerts;
	};

	// Intermediate data used when computing mass properties.
	struct VolumeIntegrals
	{
		int A;   // alpha
		int B;   // beta
		int C;   // gamma

		// projection integrals
		float P1, Pa, Pb, Paa, Pab, Pbb, Paaa, Paab, Pabb, Pbbb;

		// face integrals
		float Fa, Fb, Fc, Faa, Fbb, Fcc, Faaa, Fbbb, Fccc, Faab, Fbbc, Fcca;

		// volume integrals
		float T0[3], T1[3], T2[3], TP[3];
	};

	// Compute various integrations over a projection of a face.
	void ComputeProjectionIntegrals(const Face& face, VolumeIntegrals& vi);
	void ComputeFaceIntegrals(const Face& face, VolumeIntegrals& vi);
	void ComputeVolumeIntegrals(VolumeIntegrals& vi);

	int m_iNumFaces;
	Face m_Faces[MAX_FACES_PER_POLYHEDRON];

	int m_iNumVerts;
	Vector3 m_v3Verts[MAX_VERTS_PER_POLYHEDRON];

	int m_iNumVertPtrsUsed;
	int m_VertPtrPool[AVG_VERTS_PER_FACE*MAX_VERTS_PER_POLYHEDRON];
};


#endif // TECH_CERT_GAME_POLYHEDRON_H
