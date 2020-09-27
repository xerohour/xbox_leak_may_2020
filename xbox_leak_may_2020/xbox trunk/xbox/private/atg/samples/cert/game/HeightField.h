//-----------------------------------------------------------------------------
// FILE: HeightField.h
//
// Desc: Height field collision primitive.
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_HEIGHTFIELD_H
#define TECH_CERT_GAME_HEIGHTFIELD_H

#include "Common.h"
#include "Mesh.h"
#include "PolyHedron.h"




//-------------------------------------------------------------------------
// Name: HeightField
// Desc: height field
//-------------------------------------------------------------------------
class HeightField
{

	// Bounding box for heightfield
	Vector3 m_v3Min;
    Vector3 m_v3Max;

	// Vertex data
	INT      m_iNumVerts;
	Vector3* m_pVerts;

	// Triangle data
	struct Triangle
	{
		FLOAT fMinX;
        FLOAT fMaxX;
		FLOAT fMinZ;
        FLOAT fMaxZ;
		FLOAT fMaxY;
		WORD  Verts[3];
	};

	INT       m_iNumTris;
	Triangle* m_pTris;

	// Quadtree of triangles
	struct QuadTreeEntry
	{
		INT        iNumTris;
		FLOAT      fMaxY;
		Triangle** pTris;
	};

	FLOAT m_fQuadXSubdiv;
    FLOAT m_fQuadZSubdiv;
	INT   m_iQuadXDim;
    INT   m_iQuadZDim;

	QuadTreeEntry* m_pQuadTree;
	Triangle**     m_pQuadTreeStorage;

public:

	HeightField();
	~HeightField();

	VOID BuildFromMesh( const Mesh* pMesh );
	BOOL CheckLineSegment( const Vector3& p1, const Vector3& p2, 
                           FLOAT* pT, Vector3* pQ );
	BOOL CheckForIntersection( const Transformation& ta,
							   const Transformation& tb, const Polyhedron& b );
	BOOL ComputeIntersectionInfo( const Transformation& ta, 
								  const Transformation& tb, const Polyhedron& b,
								  INT* pNumInfos, IntersectionInfo* pInfo );

};




#endif // TECH_CERT_GAME_HEIGHTFIELD_H
