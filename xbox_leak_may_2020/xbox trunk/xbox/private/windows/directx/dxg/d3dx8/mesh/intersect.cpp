/*//////////////////////////////////////////////////////////////////////////////
//
// File: intersect.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
// @@BEGIN_MSINTERNAL
//
// History:
// -@-          (craigp)    - created
//
// @@END_MSINTERNAL
//
//////////////////////////////////////////////////////////////////////////////*/

#include "pchmesh.h"

/*****************************************************************************
This function returns true if the ray intersects the volume of the bounding
sphere (not just the surface of the sphere).

The basis for this algorithm is found in "Real-Time Rendering", Moller/Haines,
sec. 10.3.
*****************************************************************************/

BOOL WINAPI D3DXSphereBoundProbe 
    (
    CONST D3DXVECTOR3 *pvCenter,
    float fRadius,
    D3DXVECTOR3 *pvRayPosition,
    D3DXVECTOR3 *pvRayDirection
    )
{
    D3DXVECTOR3 vLength;
    float fLengthSq;
    float fDot;
    float fDotSq;
    float fProjLength;
    float fRayLengthSq;
    const float fRadiusSq = fRadius * fRadius;

    // If the sphere radius is negative, the sphere is nil; return false.
    if (fRadius <= 0.0f) 
        return FALSE;


    // Length <- sphere.center - ray.origin
    vLength = *pvCenter;
    vLength -= *pvRayPosition;

    // L2 <- L dot L (squared length of vector L)

    fLengthSq = D3DXVec3LengthSq(&vLength);

    // If the ray origin is inside the sphere, then return true.

    if (fLengthSq < fRadiusSq) 
        return TRUE;

    // d <- L dot ray.direction
    fDot = D3DXVec3Dot(&vLength, pvRayDirection);

    // Due to the (L2 < r2) test above, we know that the ray origin is outside
    // the sphere.  Given this, the ray cannot intersect the sphere if the
    // sphere center is behind the ray.

    if (fDot < 0)
        return FALSE;

    // At this point, we need the ray direction to be normalized.

    fRayLengthSq = D3DXVec3LengthSq(pvRayDirection);
    fDotSq = (fDot * fDot) / fRayLengthSq;
    fProjLength = fLengthSq - fDotSq;     // m^2 <- L^2 - d^2

    // If the distance from the sphere center to its projection on the ray is
    // greater than the radius of the sphere, then the ray does not intersect.

    if (fProjLength > fRadiusSq) 
        return FALSE;
    else 
        return TRUE;
}

/*****************************************************************************
This function returns true if the probe ray intersects the bbox volume (not
just the surface of the box).

Based on Woo's method presented in Gems I, p. 395.  See also "Real-Time
Rendering", Haines, sec 10.4.2.
*****************************************************************************/

BOOL WINAPI D3DXBoxBoundProbe 
    (
    CONST D3DXVECTOR3 *pvMin, 
    CONST D3DXVECTOR3 *pvMax,
    D3DXVECTOR3 *pvRayPosition,
    D3DXVECTOR3 *pvRayDirection
    )
{
    // Reject empty bounding boxes.

    if (*pvMin == *pvMax)
        return FALSE;

    BOOL   inside = TRUE;
    BOOL   middle[3];        // True if ray origin in middle for coord i.
    DOUBLE plane[3];         // Candidate BBox Planes
    int    i;                // General Loop Counter

    // Find all candidate planes; select the plane nearest to the ray origin
    // for each coordinate.

    float *rgfRayPos = (float*)pvRayPosition;
    float *rgfMin = (float*)pvMin;
    float *rgfMax = (float*)pvMax;
    float *rgfRayDir = (float*)pvRayDirection;

    for (i=0;  i < 3;  ++i)
    {
        if (rgfRayPos[i] < rgfMin[i])
        {
            middle[i] = FALSE;
            plane[i] = rgfMin[i];
            inside = FALSE;
        }
        else if (rgfRayPos[i] > rgfMax[i])
        {
            middle[i] = FALSE;
            plane[i] = rgfMax[i];
            inside = FALSE;
        }
        else
        {
            middle[i] = TRUE;
        }
    }

    // If the ray origin is inside the box, then it must intersect the volume
    // of the bounding box.

    if (inside) 
        return TRUE;

    DOUBLE rayt = -1;
    int maxPlane = 0;

    for (i=0;  i < 3;  ++i)
    {
        if (!middle[i] && (rgfRayDir[i] != 0))
        {
            const DOUBLE t = (plane[i] - rgfRayPos[i]) / rgfRayDir[i];

            if (t > rayt)
            {   rayt = t;
                maxPlane = i;
            }
        }
    }

    // If the box is behind the ray, or if the box is beyond the extent of the
    // ray, then return no-intersect.

    if (rayt < 0)
        return FALSE;

    // The intersection candidate point is within acceptible range; test each
    // coordinate here to ensure that it actually hits the box.

    for (i=0;  i < 3;  ++i)
    {
        if (i != maxPlane)
        {
            DOUBLE c = rgfRayPos[i] + (rayt * rgfRayDir[i]);
            if ((c < rgfMin[i]) || (rgfMax[i] < c))
                return FALSE;
        }
    }

    return TRUE;
}

/*****************************************************************************
This is the main routine that determines if the ray intersects the triangle.
*****************************************************************************/

BOOL IntersectTri 
(
    CONST D3DXVECTOR3 *pv0,
    CONST D3DXVECTOR3 *pv1,
    CONST D3DXVECTOR3 *pv2,
    CONST D3DXVECTOR3 *pfRayPos,            // ray origin
    CONST D3DXVECTOR3 *pfRayDir,            // ray direction
    float *pfU,                       // Barycentric Hit Coordinates
    float *pfV,                       // Barycentric Hit Coordinates
    float *pfDist)                    // Ray-Intersection Parameter Distance
{
    D3DXVECTOR3 e1 (pv1->x - pv0->x, pv1->y - pv0->y, pv1->z - pv0->z);
    D3DXVECTOR3 e2 (pv2->x - pv0->x, pv2->y - pv0->y, pv2->z - pv0->z);
    D3DXVECTOR3 r;
    double a;
    double f;
    D3DXVECTOR3 s;
    D3DXVECTOR3 q;
    double u;
    double v;
    double t;

    D3DXVec3Cross(&r, pfRayDir, &e2);

    a = D3DXVec3Dot (&e1, &r);


    if (a > 0)
    {
        s = D3DXVECTOR3(pfRayPos->x - pv0->x, pfRayPos->y - pv0->y, pfRayPos->z - pv0->z);
    }
    else if (a < 0)
    {
        s = D3DXVECTOR3(pv0->x - pfRayPos->x, pv0->y - pfRayPos->y, pv0->z - pfRayPos->z);
        a = -a;
    }
    else
    {
        return FALSE;
    }

    f = 1 / a;

    u = D3DXVec3Dot(&s, &r);
    if ((u < 0) || (a < u)) 
        return FALSE;

    D3DXVec3Cross(&q, &s, &e1);

    v = D3DXVec3Dot(pfRayDir, &q);
    if ((v < 0) || (a < (u+v))) 
        return FALSE;

    t = D3DXVec3Dot(&e2, &q);
    if (t < 0)
        return FALSE;

    t = t * f;
    u = u * f;
    v = v * f;

    *pfU = (float)u;
    *pfV = (float)v;
    *pfDist = (float)t;

    return TRUE;
}

/*****************************************************************************
*****************************************************************************/

HRESULT WINAPI D3DXIntersect 
    (
    LPD3DXBASEMESH pMesh,
    CONST D3DXVECTOR3 *pfRayPos,
    CONST D3DXVECTOR3 *pfRayDir,
    BOOL    *pbHit,
    DWORD   *piFaceIndex,
    float   *pfA,
    float   *pfB,
    float   *pfDist
    )
{
    HRESULT hr = S_OK;
    DWORD cVertices;
    DWORD cFaces;
    BYTE *rgbFlags = NULL;
    float fA;
    float fB;
    float fDist;
    DWORD iFaceIndex;
    D3DXPLANE plane;
    float fBa,fBb;   // Barycentric Hit Coordinates
    float fDistCur;    // Current Hit Distance (Ray Parameter)
    float fD;
    WORD *rgwFaces = NULL;
    PBYTE pbPoints = NULL;
    DWORD iVertex;
    D3DXVECTOR3 *pvPos;
    DXCrackFVF cfvf(D3DFVF_XYZ);
    DWORD iFace;
    DWORD iFaceEnd;
    D3DXATTRIBUTERANGE *rgAttribTable = NULL;
    DWORD iAttrib;
    DWORD cAttribs;
    DWORD cMaxVertices;

    if ((pMesh == NULL) 
        || (pfRayPos == NULL) 
        || (pfRayDir == NULL)
        || (pbHit == NULL))
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    // UNDONE UNDONE - templatize to fix 32 bit mesh issues
    if (pMesh->GetOptions() & D3DXMESH_32BIT)
    {
        hr = E_NOTIMPL;
        goto e_Exit;
    }

    cfvf = DXCrackFVF(pMesh->GetFVF());
    cVertices = pMesh->GetNumVertices();
    cFaces = pMesh->GetNumFaces();

    hr = pMesh->GetAttributeTable(NULL, &cAttribs);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh->LockIndexBuffer(D3DLOCK_READONLY, (PBYTE*)&rgwFaces);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh->LockVertexBuffer(D3DLOCK_READONLY, &pbPoints);
    if (FAILED(hr))
        goto e_Exit;

    *pbHit = FALSE;
    fDist = 1.0e35f;

    if (cVertices == 0) 
        goto e_Exit;

    rgAttribTable = new D3DXATTRIBUTERANGE[max(1, cAttribs)];
    if (rgAttribTable == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // get the attribute table, if zero
    if (cAttribs > 0)
    {
        hr = pMesh->GetAttributeTable(rgAttribTable, NULL);
        if (FAILED(hr))
            goto e_Exit;
    }
    else
    {
        cAttribs = 1;  // allocated to be at least 1 in size above
                            // set to one, the "faked" up one
        rgAttribTable[0].FaceStart = 0;
        rgAttribTable[0].FaceCount = cFaces;
        rgAttribTable[0].VertexStart = 0;
        rgAttribTable[0].VertexCount = cVertices;
    }

    // find the max vertex index
    cMaxVertices = 0;
    for (iAttrib = 0; iAttrib < cAttribs; iAttrib++)
    {
        cMaxVertices = max(cMaxVertices, (rgAttribTable[iAttrib].VertexStart + rgAttribTable[iAttrib].VertexCount));
    }

    // allocate an array of vertex flags to help narrow the search
    rgbFlags = new BYTE[cMaxVertices];
    if (rgbFlags == NULL) 
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    memset(rgbFlags, 0, sizeof(BYTE) * cMaxVertices);

    // generate a plane containing the ray, which is parallel to the z-axis
    if (pfRayDir->x == 0)
    {
        plane.a = 1.0f;
        plane.b = 0.0f;
        plane.c = 0.0f;        
        plane.d = -pfRayPos->x;
    }
    else
    {
        plane.a = pfRayDir->y;
        plane.b = -pfRayDir->x;
        plane.c = 0.0f;        
        plane.d = (pfRayPos->y * pfRayDir->x) - (pfRayPos->x * pfRayDir->y);
    }

    fD = -plane.d;  // This speeds up the following loop.
    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        pvPos = cfvf.PvGetPosition(cfvf.GetArrayElem(pbPoints, iVertex));

        if (((plane.a * pvPos->x) + (plane.b * pvPos->y)) > fD)
            rgbFlags[iVertex] = 0x1;
    }

    // use the attribute table in case there are any faces that are invalid
    //   i.e. a PM with multiple attributes not at the highest LOD
    for (iAttrib = 0; iAttrib < cAttribs; iAttrib++)
    {
        iFaceEnd = rgAttribTable[iAttrib].FaceStart + rgAttribTable[iAttrib].FaceCount;
        for (iFace = rgAttribTable[iAttrib].FaceStart; iFace < iFaceEnd; iFace++)
        {
            // first check the flags to see if all the points of the triangles lay
            //   on the same side of the plane, if they do, they can't intersect
            if ( (   rgbFlags[rgwFaces[3*iFace+0]]
                  == rgbFlags[rgwFaces[3*iFace+2]])
               &&(   rgbFlags[rgwFaces[3*iFace+1]]
                  == rgbFlags[rgwFaces[3*iFace+2]]))
            {
                continue;
            }

            if (IntersectTri(cfvf.PvGetPosition(cfvf.GetArrayElem(pbPoints, rgwFaces[3*iFace+0])),
                            cfvf.PvGetPosition(cfvf.GetArrayElem(pbPoints, rgwFaces[3*iFace+1])),
                            cfvf.PvGetPosition(cfvf.GetArrayElem(pbPoints, rgwFaces[3*iFace+2])),
                            pfRayPos, pfRayDir, &fBa, &fBb, &fDistCur))
            {
                // If the current hit is closer to the ray origin, then choose this
                // one as the winner.

                if (fDistCur < fDist)
                {
                    iFaceIndex = iFace;
                    fA = fBa;
                    fB = fBb;
                    fDist = fDistCur;
                    *pbHit = TRUE;
                }
            }
        }
    }

    if (*pbHit == TRUE)
    {
        if (piFaceIndex)
        {
            *piFaceIndex = iFaceIndex;
        }
        if (pfA)
        {
            *pfA = fA;
        }
        if (pfB)
        {
            *pfB = fB;
        }
        if (pfDist)
        {
            *pfDist = fDist;
        }
    }
e_Exit:
    delete []rgbFlags;
    delete []rgAttribTable;

	if (rgwFaces != NULL)
	{
		pMesh->UnlockIndexBuffer();
	}
	if (pbPoints != NULL)
	{
		pMesh->UnlockVertexBuffer();
	}

    return hr;
}
