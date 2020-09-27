//-----------------------------------------------------------------------------
// FILE: HeightField.cpp
//
// Desc: Height field collision primitive.
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "HeightField.h"
#include "Math3d.h"




// Hack to fix non-standard C++ for scoping in old compiler
#if !defined(_XBOX)
    #define for if( FALSE ) ; else for
#endif




//-----------------------------------------------------------------------------
// Name: HeightField()
// Desc: Construct an empty heightfield
//-----------------------------------------------------------------------------
HeightField::HeightField()
:
    m_v3Min         ( FLT_MAX, FLT_MAX, FLT_MAX ),
    m_v3Max         ( -FLT_MAX, -FLT_MAX, -FLT_MAX ),
    m_iNumVerts     ( 0 ),
    m_pVerts        ( NULL ),
    m_iNumTris      ( 0 ),
    m_pTris         ( NULL ),
	m_fQuadXSubdiv  ( 0.0f ),
    m_fQuadZSubdiv  ( 0.0f ),
	m_iQuadXDim     ( 0 ),
    m_iQuadZDim     ( 0 ),
	m_pQuadTree     ( NULL ),
	m_pQuadTreeStorage( NULL )
{
}




//-----------------------------------------------------------------------------
// Name: ~HeightField()
// Desc: Dtor
//-----------------------------------------------------------------------------
HeightField::~HeightField()
{
	delete [] m_pVerts;
	delete [] m_pTris;
	delete [] m_pQuadTree;
	delete [] m_pQuadTreeStorage;
}




//-----------------------------------------------------------------------------
// Name: BuildFromMesh()
// Desc: Build the heightfield from the given mesh
//-----------------------------------------------------------------------------
void HeightField::BuildFromMesh( const Mesh* pMesh )
{
	INT iMaxVerts = 0;
	INT iMaxTris = 0;

	// Count triangles, and maximum number of verts
	for( INT elem = 0; elem < pMesh->m_iNumMeshElements; ++elem )
	{
		iMaxVerts += pMesh->m_Elements[elem].m_uNumVertices;

		if( pMesh->m_Elements[elem].m_PrimType == D3DPT_TRIANGLELIST )
			iMaxTris += pMesh->m_Elements[elem].m_uIndexBufferCount / 3;
		else if( pMesh->m_Elements[elem].m_PrimType == D3DPT_TRIANGLESTRIP )
			iMaxTris += pMesh->m_Elements[elem].m_uIndexBufferCount-2;
	}

	// Pre-allocate verts/faces
	m_iNumVerts = 0;
	m_pVerts = new Vector3 [iMaxVerts];

	m_iNumTris = 0;
	m_pTris = new Triangle [iMaxTris];

	// Suck vertices and triangles out of the mesh
	for( INT elem = 0; elem < pMesh->m_iNumMeshElements; ++elem )
	{
		MeshElement* pMeshElem = &pMesh->m_Elements[elem];
		INT* pVertexMap = new INT [pMeshElem->m_uNumVertices] ;

		// Suck vertices out of mesh (merge duplicated verts)
		const BYTE* pVertices;
		pMeshElem->m_pVertexBuffer->Lock( 
                    0, pMeshElem->m_uVertexSize * pMeshElem->m_uNumVertices, 
					(BYTE**)&pVertices, D3DLOCK_READONLY );

		for( UINT i = 0; i < pMeshElem->m_uNumVertices; ++i )
		{
			const Vector3& vert = *(Vector3*)pVertices;

			// See if we already have this vertex
			INT j;
			for( j = 0; j < m_iNumVerts; ++j )
			{
				// Do fuzzy comparison
				if( fabs(m_pVerts[j].x - vert.x) < 0.0001f && 
					fabs(m_pVerts[j].y - vert.y) < 0.0001f &&
					fabs(m_pVerts[j].z - vert.z) < 0.0001f )
				{
					// Found a match
					break;
				}
			}

			// Add this vertex
			if( j == m_iNumVerts )
			{
				m_pVerts[m_iNumVerts] = vert;
				m_iNumVerts++;
			}

			pVertexMap[i] = j;

			pVertices += pMeshElem->m_uVertexSize;
		}

		pMeshElem->m_pVertexBuffer->Unlock();

		// Suck triangles out of mesh
		const WORD* pIndices;
		pMeshElem->m_pIndexBuffer->Lock(
                    0, sizeof(WORD) * pMeshElem->m_uIndexBufferCount, 
					(BYTE**)&pIndices, D3DLOCK_READONLY );

		if( pMeshElem->m_PrimType == D3DPT_TRIANGLELIST )
		{
			for( UINT i = 0; i < pMeshElem->m_uIndexBufferCount; i += 3 )
			{
				m_pTris[m_iNumTris].Verts[0] = WORD(pVertexMap[pIndices[i+0]]);
				m_pTris[m_iNumTris].Verts[1] = WORD(pVertexMap[pIndices[i+1]]);
				m_pTris[m_iNumTris].Verts[2] = WORD(pVertexMap[pIndices[i+2]]);

				m_iNumTris++;
			}
		}
		else if( pMeshElem->m_PrimType == D3DPT_TRIANGLESTRIP )
		{
			INT v0 = pIndices[0];
			INT v1 = pIndices[1];

			for( UINT i = 2; i < pMeshElem->m_uIndexBufferCount; ++i )
			{
				INT v2 = pIndices[i];
				
				if( v0 != v1 && v1 != v2 && v2 != v0 )
				{
					if( i & 1 )
					{
						m_pTris[m_iNumTris].Verts[0] = WORD(pVertexMap[v2]);
						m_pTris[m_iNumTris].Verts[1] = WORD(pVertexMap[v1]);
						m_pTris[m_iNumTris].Verts[2] = WORD(pVertexMap[v0]);
					}
					else
					{
						m_pTris[m_iNumTris].Verts[0] = WORD(pVertexMap[v0]);
						m_pTris[m_iNumTris].Verts[1] = WORD(pVertexMap[v1]);
						m_pTris[m_iNumTris].Verts[2] = WORD(pVertexMap[v2]);
					}

					m_iNumTris++;
				}

				v0 = v1;
				v1 = v2;
			}
		}

		delete [] pVertexMap;

		pMeshElem->m_pIndexBuffer->Unlock();
	}

	// TODO: Shrink vertex and triangle allocation

	// Compute triangle data, min, and max
	for( INT i = 0; i < m_iNumTris; ++i )
	{
		// Find triangle min/max
		m_pTris[i].fMinX = m_pVerts[m_pTris[i].Verts[0]].x;
		m_pTris[i].fMaxX = m_pVerts[m_pTris[i].Verts[0]].x;
		m_pTris[i].fMinZ = m_pVerts[m_pTris[i].Verts[0]].z;
		m_pTris[i].fMaxZ = m_pVerts[m_pTris[i].Verts[0]].z;

		FLOAT fMinY = m_pVerts[m_pTris[i].Verts[0]].y;
		m_pTris[i].fMaxY = m_pVerts[m_pTris[i].Verts[0]].y;

		for( INT j = 1; j <= 2; ++j )
		{
			if( m_pTris[i].fMinX > m_pVerts[m_pTris[i].Verts[j]].x )
				m_pTris[i].fMinX = m_pVerts[m_pTris[i].Verts[j]].x;

			if( m_pTris[i].fMaxX < m_pVerts[m_pTris[i].Verts[j]].x )
				m_pTris[i].fMaxX = m_pVerts[m_pTris[i].Verts[j]].x;

			if( m_pTris[i].fMinZ > m_pVerts[m_pTris[i].Verts[j]].z )
				m_pTris[i].fMinZ = m_pVerts[m_pTris[i].Verts[j]].z;

			if( m_pTris[i].fMaxZ < m_pVerts[m_pTris[i].Verts[j]].z )
				m_pTris[i].fMaxZ = m_pVerts[m_pTris[i].Verts[j]].z;

			if( fMinY > m_pVerts[m_pTris[i].Verts[j]].y )
				fMinY = m_pVerts[m_pTris[i].Verts[j]].y;

			if( m_pTris[i].fMaxY < m_pVerts[m_pTris[i].Verts[j]].y )
				m_pTris[i].fMaxY = m_pVerts[m_pTris[i].Verts[j]].y;
		}

		// Update min/max for whole heightfield
		if( m_v3Min.x > m_pTris[i].fMinX )
			m_v3Min.x = m_pTris[i].fMinX;

		if( m_v3Min.y > fMinY )
			m_v3Min.y = fMinY;

		if( m_v3Min.z > m_pTris[i].fMinZ )
			m_v3Min.z = m_pTris[i].fMinZ;

		if( m_v3Max.x < m_pTris[i].fMaxX )
			m_v3Max.x = m_pTris[i].fMaxX;

		if( m_v3Max.y < m_pTris[i].fMaxY )
			m_v3Max.y = m_pTris[i].fMaxY;

		if( m_v3Max.z < m_pTris[i].fMaxZ )
			m_v3Max.z = m_pTris[i].fMaxZ;
	}

	// Determine subdivision size
	const FLOAT fSubdivisionSize = 10.0f;

	m_iQuadXDim = INT((m_v3Max.x - m_v3Min.x) / fSubdivisionSize);
	m_iQuadZDim = INT((m_v3Max.z - m_v3Min.z) / fSubdivisionSize);

	if( m_iQuadXDim < 1 )
        m_iQuadXDim = 1;
	if( m_iQuadZDim < 1 )
        m_iQuadZDim = 1;

	m_fQuadXSubdiv = (m_v3Max.x - m_v3Min.x) / m_iQuadXDim;
	m_fQuadZSubdiv = (m_v3Max.z - m_v3Min.z) / m_iQuadZDim;

	m_pQuadTree = new QuadTreeEntry [m_iQuadXDim*m_iQuadZDim] ;
	memset( m_pQuadTree, 0, sizeof(QuadTreeEntry) * m_iQuadXDim * m_iQuadZDim );

	INT iTotalCount = 0;

	// Count number of entries in each quadtree node
	for( INT i = 0; i < m_iNumTris; ++i )
	{
		INT iMinX = INT((m_pTris[i].fMinX - m_v3Min.x) / m_fQuadXSubdiv);
		INT iMinZ = INT((m_pTris[i].fMinZ - m_v3Min.z) / m_fQuadZSubdiv);
		INT iMaxX = INT((m_pTris[i].fMaxX - m_v3Min.x) / m_fQuadXSubdiv);
		INT iMaxZ = INT((m_pTris[i].fMaxZ - m_v3Min.z) / m_fQuadZSubdiv);

		iMinX = Clamp(0, iMinX, m_iQuadXDim-1);
		iMinZ = Clamp(0, iMinZ, m_iQuadZDim-1);
		iMaxX = Clamp(0, iMaxX, m_iQuadXDim-1);
		iMaxZ = Clamp(0, iMaxZ, m_iQuadZDim-1);

		for( INT ix = iMinX; ix <= iMaxX; ++ix )
		{
			for( INT iz = iMinZ; iz <= iMaxZ; ++iz )
			{
				QuadTreeEntry& quad = m_pQuadTree[iz * m_iQuadXDim + ix];

				// TODO: Add more precise triangle/quad intersection test

				quad.iNumTris++;
				iTotalCount++;
			}
		}
	}

	// Allocate storage for triangles pointers
	m_pQuadTreeStorage = new Triangle * [iTotalCount];

	INT iOffset = 0;
	for( INT i = 0; i < m_iQuadXDim*m_iQuadZDim; ++i )
	{
		m_pQuadTree[i].pTris = m_pQuadTreeStorage + iOffset;
		iOffset += m_pQuadTree[i].iNumTris;
		m_pQuadTree[i].iNumTris = 0;
		m_pQuadTree[i].fMaxY = -FLT_MAX;
	}

	// Put triangles into quadtree
	for( INT i = 0; i < m_iNumTris; ++i )
	{
		INT iMinX = INT((m_pTris[i].fMinX - m_v3Min.x) / m_fQuadXSubdiv);
		INT iMinZ = INT((m_pTris[i].fMinZ - m_v3Min.z) / m_fQuadZSubdiv);
		INT iMaxX = INT((m_pTris[i].fMaxX - m_v3Min.x) / m_fQuadXSubdiv);
		INT iMaxZ = INT((m_pTris[i].fMaxZ - m_v3Min.z) / m_fQuadZSubdiv);

		iMinX = Clamp(0, iMinX, m_iQuadXDim-1);
		iMinZ = Clamp(0, iMinZ, m_iQuadZDim-1);
		iMaxX = Clamp(0, iMaxX, m_iQuadXDim-1);
		iMaxZ = Clamp(0, iMaxZ, m_iQuadZDim-1);

		for( INT ix = iMinX; ix <= iMaxX; ++ix )
		{
			for( INT iz = iMinZ; iz <= iMaxZ; ++iz )
			{
				QuadTreeEntry& quad = m_pQuadTree[iz * m_iQuadXDim + ix];

				// TODO: Add more precise triangle/quad intersection test

				quad.pTris[quad.iNumTris++] = &m_pTris[i];

				if( m_pTris[i].fMaxY > quad.fMaxY )
					quad.fMaxY = m_pTris[i].fMaxY;
			}
		}
	}
}




//-----------------------------------------------------------------------------
// Name: PointInTriangle()
// Desc: TRUE if the given point is within the triangle
//-----------------------------------------------------------------------------
BOOL PointInTriangle( const Vector3& p, const Vector3& p1, 
                      const Vector3& p2, const Vector3& p3 )
{
	Vector3 n = (p3 - p1) ^ (p2 - p1);
	Vector3 v1 = (p - p1) ^ (p2 - p1);
	Vector3 v2 = (p - p2) ^ (p3 - p2);
	Vector3 v3 = (p - p3) ^ (p1 - p3);

	return( v1 * n >= 0.0f && v2 * n >= 0.0f && v3 * n >= 0.0f );
}




//-----------------------------------------------------------------------------
// Name: CheckLineSegment()
// Desc: Check a line segment from p1 to p2 for intersection.
//-----------------------------------------------------------------------------
BOOL HeightField::CheckLineSegment( const Vector3& p1, const Vector3& p2, 
                                    FLOAT* pT, Vector3* pQ )
{
	Vector3 v3Min;
	Vector3 v3Max;

	// Find min,max of line segment
	if( p1.x < p2.x )
	{
		v3Min.x = p1.x;
		v3Max.x = p2.x;
	}
	else
	{
		v3Min.x = p2.x;
		v3Max.x = p1.x;
	}

	if( p1.y < p2.y )
	{
		v3Min.y = p1.y;
		v3Max.y = p2.y;
	}
	else
	{
		v3Min.y = p2.y;
		v3Max.y = p1.y;
	}

	if( p1.z < p2.z )
	{
		v3Min.z = p1.z;
		v3Max.z = p2.z;
	}
	else
	{
		v3Min.z = p2.z;
		v3Max.z = p1.z;
	}

	INT iMinX = INT((v3Min.x - m_v3Min.x) / m_fQuadXSubdiv);
	INT iMinZ = INT((v3Min.z - m_v3Min.z) / m_fQuadZSubdiv);
	INT iMaxX = INT((v3Max.x - m_v3Min.x) / m_fQuadXSubdiv);
	INT iMaxZ = INT((v3Max.z - m_v3Min.z) / m_fQuadZSubdiv);

	iMinX = Clamp(0, iMinX, m_iQuadXDim-1);
	iMinZ = Clamp(0, iMinZ, m_iQuadZDim-1);
	iMaxX = Clamp(0, iMaxX, m_iQuadXDim-1);
	iMaxZ = Clamp(0, iMaxZ, m_iQuadZDim-1);

	FLOAT fSmallestT = FLT_MAX;

	for( INT ix = iMinX; ix <= iMaxX; ++ix )
	{
		for( INT iz = iMinZ; iz <= iMaxZ; ++iz )
		{
			QuadTreeEntry& quad = m_pQuadTree[iz * m_iQuadXDim + ix];

			// Do quick height test
			if( v3Min.y > quad.fMaxY )
				continue;

			for( INT i = 0; i < quad.iNumTris; ++i )
			{
				Plane3 plnFace( m_pVerts[quad.pTris[i]->Verts[2]],
								m_pVerts[quad.pTris[i]->Verts[1]],
								m_pVerts[quad.pTris[i]->Verts[0]] );

				FLOAT d1 = plnFace.GetDistance(p1);
				FLOAT d2 = plnFace.GetDistance(p2);

				BOOL s1 = (d1 < 0.0f);
				BOOL s2 = (d2 < 0.0f);

				if( s1 != s2 )
				{
					// Compute intersection
					FLOAT t = d1 / (d1 - d2);

					if( t < fSmallestT )
					{
						Vector3 p = (1.0f - t) * p1 + t * p2;

						// Check if intersection point is inside triangle
						if( PointInTriangle( p, m_pVerts[quad.pTris[i]->Verts[0]], 
											    m_pVerts[quad.pTris[i]->Verts[1]],
											    m_pVerts[quad.pTris[i]->Verts[2]]) )
						{
							// Best point so far
							fSmallestT = t;
							*pT = t;
							*pQ = p;
						}
					}
				}
			}
		}
	}
		
	return (fSmallestT < FLT_MAX);
}




//-----------------------------------------------------------------------------
// Name: CheckForIntersection()
// Desc: Do a quick test for intersection
//-----------------------------------------------------------------------------
BOOL HeightField::CheckForIntersection( const Transformation& ta,
									    const Transformation& tb, 
                                        const Polyhedron& b )
{
    USED( ta );
    USED( tb );
    USED( b );
	return TRUE;
}




//-----------------------------------------------------------------------------
// Name: ComputeIntersectionInfo()
// Desc: Compute information about intersection with a convex polyhedron
//-----------------------------------------------------------------------------
BOOL HeightField::ComputeIntersectionInfo( const Transformation& ta,
										   const Transformation& tb,
                                           const Polyhedron& b,
										   INT* pNumInfos, 
                                           IntersectionInfo* pInfo )
{
	const Transformation tBtoA = tb * ~ta;

	*pNumInfos = 0;

	// Move the polyhedron into the space of heightfield
	Polyhedron tLocalB( b, tBtoA );

	// Find what triangles of the mesh the polyhedron may overlap
	Vector3 v3MinB;
    Vector3 v3MaxB;
	tLocalB.ComputeExtents( &v3MinB, &v3MaxB );

	INT iMinX = INT((v3MinB.x - m_v3Min.x) / m_fQuadXSubdiv);
	INT iMinZ = INT((v3MinB.z - m_v3Min.z) / m_fQuadZSubdiv);
	INT iMaxX = INT((v3MaxB.x - m_v3Min.x) / m_fQuadXSubdiv);
	INT iMaxZ = INT((v3MaxB.z - m_v3Min.z) / m_fQuadZSubdiv);

	iMinX = Clamp(0, iMinX, m_iQuadXDim-1);
	iMinZ = Clamp(0, iMinZ, m_iQuadZDim-1);
	iMaxX = Clamp(0, iMaxX, m_iQuadXDim-1);
	iMaxZ = Clamp(0, iMaxZ, m_iQuadZDim-1);

	for( INT ix = iMinX; ix <= iMaxX; ++ix )
	{
		for( INT iz = iMinZ; iz <= iMaxZ; ++iz )
		{
			QuadTreeEntry& quad = m_pQuadTree[iz * m_iQuadXDim + ix];

			// Do quick height test
			if( v3MinB.y > quad.fMaxY )
				continue;

			for( INT i = 0; i < quad.iNumTris; ++i )
			{
				// Do a quick test against the triangle
				quad.pTris[i];

				// Build a polyhedron for the triangle
				Polyhedron polyTri( m_pVerts[quad.pTris[i]->Verts[2]],
								    m_pVerts[quad.pTris[i]->Verts[1]],
								    m_pVerts[quad.pTris[i]->Verts[0]],
								    Vector3(0.0f, -1000.0f, 0.0f) );

				// Compute intersection of the polyhedrons
				if( Polyhedron::CheckForIntersection(ta, polyTri, tb, b) )
				{
					int iNumSubInfos = 0;

					if (Polyhedron::ComputeIntersectionInfo(ta, polyTri, 
															tb, b, 
															&iNumSubInfos,
															pInfo + *pNumInfos))
					{
						*pNumInfos += iNumSubInfos;
					}
				}
			}
		}
	}

    return( *pNumInfos ? TRUE : FALSE );
}
