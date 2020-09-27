#include "pchmesh.h"


typedef struct _D3DXBEZIERTRI
{
    UINT16 rgiControlPoints[10];
    DWORD rgiVertices[3];
} D3DXBEZIERTRI, *LPD3DXBEZIERTRI;

#if 0
void
EvalBezierTri
(
    LPD3DXVECTOR2 pPosition, 
    LPD3DXBEZIERTRI pbtTri, 
    LPD3DXVECTOR3 rgvControl, 
    LPD3DXVECTOR3 rgvControlNormal, 
    BOOL bLinearNormal,
    PVOID pvVertices,
    DXCrackFVF &cfvf,
    PVOID pvOut
    )
{
	float *pfTexCur0;
	float *pfTexCur1;
	float *pfTexCur2;
	float *pfTexCurOut;
	DWORD iTex;
	DWORD cTexFloats;

	float *pfWeightCur0;
	float *pfWeightCur1;
	float *pfWeightCur2;
	float *pfWeightCurOut;
	DWORD iWeight;

	D3DXCOLOR colorOut;

    D3DXVec3BaryCentric(cfvf.PvGetPosition(pvOut), 
                                    &rgvControl[pbtTri->rgiControlPoints[0]],
                                    &rgvControl[pbtTri->rgiControlPoints[3]],
                                    &rgvControl[pbtTri->rgiControlPoints[6]],
                                    pPosition->x, pPosition->y);

    if (cfvf.CTexCoords() > 0)
    {
		cTexFloats = (cfvf.m_cBytesPerVertex - cfvf.m_oTex1) / sizeof(float);

		pfTexCur0 = (float*)cfvf.PuvGetTex1(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[0]));
		pfTexCur1 = (float*)cfvf.PuvGetTex1(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[1]));
		pfTexCur2 = (float*)cfvf.PuvGetTex1(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[2]));
		pfTexCurOut = (float*)cfvf.PuvGetTex1(pvOut);

		for (iTex = 0; iTex < cTexFloats; iTex++)
		{
		    *pfTexCurOut = *pfTexCur0
									+ pPosition->x * (*pfTexCur1 - *pfTexCur0) 
									+ pPosition->y * (*pfTexCur2 - *pfTexCur0);

			pfTexCurOut += 1;
			pfTexCur0 += 1;
			pfTexCur1 += 1;
			pfTexCur2 += 1;
		}
    }

	if (cfvf.CWeights() > 0)
	{
		pfWeightCur0 = cfvf.PfGetWeights(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[0]));
		pfWeightCur1 = cfvf.PfGetWeights(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[1]));
		pfWeightCur2 = cfvf.PfGetWeights(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[2]));
		pfWeightCurOut = cfvf.PfGetWeights(pvOut);


		for (iWeight = 0; iWeight < cfvf.CWeights(); iWeight++)
		{
		    *pfWeightCurOut = *pfWeightCur0
									+ pPosition->x * (*pfWeightCur1 - *pfWeightCur0) 
									+ pPosition->y * (*pfWeightCur2 - *pfWeightCur0);


			pfWeightCur0 += 1;
			pfWeightCur1 += 1;
			pfWeightCur2 += 1;
			pfWeightCurOut += 1;
		}
	}

	if (cfvf.BDiffuse())
	{
		D3DXVec4BaryCentric((D3DXVECTOR4*)&colorOut,
								(D3DXVECTOR4*)&D3DXCOLOR(cfvf.ColorGetDiffuse(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[0]))),
								(D3DXVECTOR4*)&D3DXCOLOR(cfvf.ColorGetDiffuse(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[1]))),
								(D3DXVECTOR4*)&D3DXCOLOR(cfvf.ColorGetDiffuse(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[2]))),
								pPosition->x, pPosition->y);

		cfvf.SetDiffuse(pvOut, (DWORD)colorOut);
	}

	if (cfvf.BSpecular())
	{
		D3DXVec4BaryCentric((D3DXVECTOR4*)&colorOut,
								(D3DXVECTOR4*)&D3DXCOLOR(cfvf.ColorGetSpecular(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[0]))),
								(D3DXVECTOR4*)&D3DXCOLOR(cfvf.ColorGetSpecular(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[1]))),
								(D3DXVECTOR4*)&D3DXCOLOR(cfvf.ColorGetSpecular(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[2]))),
								pPosition->x, pPosition->y);

		cfvf.SetSpecular(pvOut, (DWORD)colorOut);
	}

    if (cfvf.BNormal())
    {
        D3DXVec3BaryCentric(cfvf.PvGetNormal(pvOut), 
                                        cfvf.PvGetNormal(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[0])),
                                        cfvf.PvGetNormal(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[1])),
                                        cfvf.PvGetNormal(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[2])),
                                        pPosition->x, pPosition->y);
    }
}

#else
void
EvalBezierTri
(
    LPD3DXVECTOR2 pPosition, 
    LPD3DXBEZIERTRI pbtTri, 
    LPD3DXVECTOR3 rgvControl, 
    LPD3DXVECTOR3 rgvControlNormal, 
    BOOL bLinearNormal,
    PVOID pvVertices,
    DXCrackFVF &cfvf,
    PVOID pvOut
    )
{
    D3DXVECTOR3 p002;
    D3DXVECTOR3 p020;
    D3DXVECTOR3 p200;
    D3DXVECTOR3 p110;
    D3DXVECTOR3 p101;
    D3DXVECTOR3 p011;
    D3DXVECTOR3 p100;
    D3DXVECTOR3 p010;
    D3DXVECTOR3 p001;

	float *pfTexCur0;
	float *pfTexCur1;
	float *pfTexCur2;
	float *pfTexCurOut;
	DWORD iTex;
	DWORD cTexFloats;

	float *pfWeightCur0;
	float *pfWeightCur1;
	float *pfWeightCur2;
	float *pfWeightCurOut;
	DWORD iWeight;

	D3DXCOLOR colorOut;


    D3DXVec3BaryCentric(&p002, &rgvControl[pbtTri->rgiControlPoints[0]],
                                    &rgvControl[pbtTri->rgiControlPoints[1]],
                                    &rgvControl[pbtTri->rgiControlPoints[8]],
                                    pPosition->x, pPosition->y);

    D3DXVec3BaryCentric(&p011, &rgvControl[pbtTri->rgiControlPoints[1]],
                                    &rgvControl[pbtTri->rgiControlPoints[2]],
                                    &rgvControl[pbtTri->rgiControlPoints[9]],
                                    pPosition->x, pPosition->y);

    D3DXVec3BaryCentric(&p020, &rgvControl[pbtTri->rgiControlPoints[2]],
                                    &rgvControl[pbtTri->rgiControlPoints[3]],
                                    &rgvControl[pbtTri->rgiControlPoints[4]],
                                    pPosition->x, pPosition->y);

    D3DXVec3BaryCentric(&p110, &rgvControl[pbtTri->rgiControlPoints[9]],
                                    &rgvControl[pbtTri->rgiControlPoints[4]],
                                    &rgvControl[pbtTri->rgiControlPoints[5]],
                                    pPosition->x, pPosition->y);

    D3DXVec3BaryCentric(&p101, &rgvControl[pbtTri->rgiControlPoints[8]],
                                    &rgvControl[pbtTri->rgiControlPoints[9]],
                                    &rgvControl[pbtTri->rgiControlPoints[7]],
                                    pPosition->x, pPosition->y);

    D3DXVec3BaryCentric(&p200, &rgvControl[pbtTri->rgiControlPoints[7]],
                                    &rgvControl[pbtTri->rgiControlPoints[5]],
                                    &rgvControl[pbtTri->rgiControlPoints[6]],
                                    pPosition->x, pPosition->y);

    D3DXVec3BaryCentric(&p001, &p002, &p011, &p101, pPosition->x, pPosition->y);
    D3DXVec3BaryCentric(&p010, &p011, &p020, &p110, pPosition->x, pPosition->y);
    D3DXVec3BaryCentric(&p100, &p101, &p110, &p200, pPosition->x, pPosition->y);

    D3DXVec3BaryCentric(cfvf.PvGetPosition(pvOut), &p001, &p010, &p100, pPosition->x, pPosition->y);

    if (cfvf.CTexCoords() > 0)
    {
		cTexFloats = (cfvf.m_cBytesPerVertex - cfvf.m_oTex1) / sizeof(float);

		pfTexCur0 = (float*)cfvf.PuvGetTex1(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[0]));
		pfTexCur1 = (float*)cfvf.PuvGetTex1(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[1]));
		pfTexCur2 = (float*)cfvf.PuvGetTex1(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[2]));
		pfTexCurOut = (float*)cfvf.PuvGetTex1(pvOut);

		for (iTex = 0; iTex < cTexFloats; iTex++)
		{
		    *pfTexCurOut = *pfTexCur0
									+ pPosition->x * (*pfTexCur1 - *pfTexCur0) 
									+ pPosition->y * (*pfTexCur2 - *pfTexCur0);

			pfTexCurOut += 1;
			pfTexCur0 += 1;
			pfTexCur1 += 1;
			pfTexCur2 += 1;
		}
    }

	if (cfvf.CWeights() > 0)
	{
		pfWeightCur0 = cfvf.PfGetWeights(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[0]));
		pfWeightCur1 = cfvf.PfGetWeights(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[1]));
		pfWeightCur2 = cfvf.PfGetWeights(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[2]));
		pfWeightCurOut = cfvf.PfGetWeights(pvOut);


		for (iWeight = 0; iWeight < cfvf.CWeights(); iWeight++)
		{
		    *pfWeightCurOut = *pfWeightCur0
									+ pPosition->x * (*pfWeightCur1 - *pfWeightCur0) 
									+ pPosition->y * (*pfWeightCur2 - *pfWeightCur0);


			pfWeightCur0 += 1;
			pfWeightCur1 += 1;
			pfWeightCur2 += 1;
			pfWeightCurOut += 1;
		}
	}

	if (cfvf.BDiffuse())
	{
		D3DXVec4BaryCentric((D3DXVECTOR4*)&colorOut,
								(D3DXVECTOR4*)&D3DXCOLOR(cfvf.ColorGetDiffuse(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[0]))),
								(D3DXVECTOR4*)&D3DXCOLOR(cfvf.ColorGetDiffuse(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[1]))),
								(D3DXVECTOR4*)&D3DXCOLOR(cfvf.ColorGetDiffuse(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[2]))),
								pPosition->x, pPosition->y);

		cfvf.SetDiffuse(pvOut, (DWORD)colorOut);
	}

	if (cfvf.BSpecular())
	{
		D3DXVec4BaryCentric((D3DXVECTOR4*)&colorOut,
								(D3DXVECTOR4*)&D3DXCOLOR(cfvf.ColorGetSpecular(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[0]))),
								(D3DXVECTOR4*)&D3DXCOLOR(cfvf.ColorGetSpecular(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[1]))),
								(D3DXVECTOR4*)&D3DXCOLOR(cfvf.ColorGetSpecular(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[2]))),
								pPosition->x, pPosition->y);

		cfvf.SetSpecular(pvOut, (DWORD)colorOut);
	}

    if (cfvf.BNormal())
    {
        if (bLinearNormal)
        {
            D3DXVec3BaryCentric(cfvf.PvGetNormal(pvOut), 
                                            cfvf.PvGetNormal(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[0])),
                                            cfvf.PvGetNormal(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[1])),
                                            cfvf.PvGetNormal(cfvf.GetArrayElem(pvVertices, pbtTri->rgiVertices[2])),
                                            pPosition->x, pPosition->y);
        }
        else
        {
            D3DXVec3BaryCentric(&p001, &rgvControlNormal[0], &rgvControlNormal[1], &rgvControlNormal[5], pPosition->x, pPosition->y);
            D3DXVec3BaryCentric(&p010, &rgvControlNormal[1], &rgvControlNormal[2], &rgvControlNormal[3], pPosition->x, pPosition->y);
            D3DXVec3BaryCentric(&p100, &rgvControlNormal[5], &rgvControlNormal[3], &rgvControlNormal[4], pPosition->x, pPosition->y);

            D3DXVec3BaryCentric(cfvf.PvGetNormal(pvOut), &p001, &p010, &p100, pPosition->x, pPosition->y);
        }
    }
}
#endif

template <class UINT_IDX>
struct SFaceTempStruct
{
    UINT_IDX rgwIndices[3];
};

template <class UINT_IDX>
struct SVertOffsetStruct
{
    UINT_IDX rgdwOffsets[3];
};


// -------------------------------------------------------------------------------
//  method    CalculateCounts
//
//  devnote     Calculates the number of vertices and faces for a bezier triangle
//                  that is tesselated with the given level
//
//  returns     S_OK if suceeded, else error code
//
void 
CalculateCounts(DWORD cLevels, PDWORD pcFaces, PDWORD pcVertices)
{
    DWORD iLevel;
    DWORD cFacesPrev;
    DWORD cFacesOut;


    // UNDONE - should be a convenient formula to figure this one out
    cFacesOut = 1;
    cFacesPrev = 1;
    for (iLevel = 0; iLevel < cLevels; iLevel++)
    {
        cFacesPrev += 2;
        cFacesOut += cFacesPrev;
    }

    *pcVertices = ((cLevels+2) * (cLevels + 3)) / 2;
    *pcFaces = cFacesOut;
}

// -------------------------------------------------------------------------------
//  method    TesselateBTri
//
//  devnote     Tesselates the bezier triangle into the provided buffer
//
//  returns     S_OK if suceeded, else error code
//
template<class UINT_IDX, unsigned int UNUSED>
HRESULT TesselateBTri
(
    LPD3DXBEZIERTRI pbtTri,             // bezier triangle to tesselate
    LPD3DXVECTOR3 rgvControl,           // control point buffer (n control points, indexed by pbtTri)
    LPD3DXVECTOR3 rgvControlNormal,     // normal control point buffer (n control points)
    BOOL bLinearNormals,                // should the normals be tesselated linearly or quadraticly
    PVOID pvPointsInt,                  // input vertices
    UINT_IDX *rgiVerts,                   // input indices, indices that are != UNUSED have already been computed
    DXCrackFVF &cfvf,                   // fvf of input and output vertices
    DWORD cLevels,                       // number of levels of tesselation
    DWORD cVertices,                     // number of vertices to tesselate to (derived from cLevels)
    DWORD &iCurVertex,                    // relative index of pvPointsOut to the start of the vertex buffer
    PBYTE &pvPointsOut,                  // generated vertices
    SFaceTempStruct<UINT_IDX> *rgwFaceOut, // generated face indices
    UINT_IDX *rgiVertOffsets               // at what offset do each of the side vertices start on
)
{
    DWORD iLevel;
    //DWORD iVertex;
    DWORD iCurTriangle;
    DWORD iPrevLevelPos;
    DWORD iCurLevelPos;
    float fIncrement;
    float fCurLevelA;
    //DWORD iattr;
    DWORD iTriangle;
    PBYTE pbCurPoint;
    float fPos;
    float fIntIncrement;
    D3DXVECTOR2 vPos;
    D3DXVECTOR2 vPos1;
    D3DXVECTOR2 vPos2;
    DWORD cNewVertices;
    DWORD iPosIncrement;

    pbCurPoint = (PBYTE)pvPointsOut;

    if (cLevels > 0)
    {
        fIncrement = 1.0f / (float)(cLevels + 1);
    }
    else
    {
        fIncrement = 1.0f;
    }

    cNewVertices = 0;

    if (rgiVerts[0] == UNUSED)
    {
        vPos = D3DXVECTOR2(0.0, 0.0);
        EvalBezierTri(&vPos, pbtTri, rgvControl, rgvControlNormal, bLinearNormals, pvPointsInt, cfvf, pbCurPoint);
        pbCurPoint += cfvf.m_cBytesPerVertex;
        rgiVerts[0] = (UINT_IDX)iCurVertex + 0;
        iCurVertex += 1;
        cNewVertices += 1;
    }

    // NOTE: the cLevels+1 is not necessary if the three original points are always specified!

    // first do left side points
    fCurLevelA = fIncrement;
    iPosIncrement = 2;
    iCurLevelPos = 1;
    rgiVertOffsets[0] = (UINT_IDX)iCurVertex;
    for (iLevel = 0; iLevel < cLevels+1; iLevel++)
    {
        vPos1 = D3DXVECTOR2(fCurLevelA, 0.0f);

        if (rgiVerts[iCurLevelPos] == UNUSED)
        {
            EvalBezierTri(&vPos1, pbtTri, rgvControl, rgvControlNormal, bLinearNormals, pvPointsInt, cfvf, pbCurPoint);
            pbCurPoint += cfvf.m_cBytesPerVertex;
            rgiVerts[iCurLevelPos] = (UINT_IDX)iCurVertex;
            iCurVertex += 1;
            cNewVertices += 1;
        }

        iCurLevelPos += iPosIncrement;
        iPosIncrement += 1;
        fCurLevelA += fIncrement;
    }

    // next do right side points
    fCurLevelA = 1.0f - fIncrement;
    iPosIncrement = cLevels + 1;
    iCurLevelPos = cVertices - 1 - (cLevels + 2);
    rgiVertOffsets[2] = (UINT_IDX)iCurVertex;
    for (iLevel = 0; iLevel < cLevels+1; iLevel++)
    {
        vPos2 = D3DXVECTOR2(0, fCurLevelA);

        if (rgiVerts[iCurLevelPos] == UNUSED)
        {
            EvalBezierTri(&vPos2, pbtTri, rgvControl, rgvControlNormal, bLinearNormals, pvPointsInt, cfvf, pbCurPoint);
            pbCurPoint += cfvf.m_cBytesPerVertex;
            rgiVerts[iCurLevelPos] = (UINT_IDX)iCurVertex;
            iCurVertex += 1;
            cNewVertices += 1;
        }

        iCurLevelPos -= iPosIncrement;
        iPosIncrement -= 1;
        fCurLevelA -= fIncrement;
    }

    // next do bottom side points
    fCurLevelA = fIncrement;
    iCurLevelPos = cVertices - (cLevels + 2);
    fPos = 0.0f;
    fIntIncrement = fIncrement;
    rgiVertOffsets[1] = (UINT_IDX)iCurVertex;
    for (iLevel = 0; iLevel < cLevels+2; iLevel++)
    {
        vPos1 = D3DXVECTOR2(1.0f, 0.0f);
        vPos2 = D3DXVECTOR2(0.0f, 1.0f);

        if (rgiVerts[iCurLevelPos] == UNUSED)
        {
            D3DXVec2Lerp(&vPos, &vPos1, &vPos2, fPos);
            EvalBezierTri(&vPos, pbtTri, rgvControl, rgvControlNormal, bLinearNormals, pvPointsInt, cfvf, pbCurPoint);
            pbCurPoint += cfvf.m_cBytesPerVertex;
            rgiVerts[iCurLevelPos] = (UINT_IDX)iCurVertex;
            iCurVertex += 1;
            cNewVertices += 1;
        }

        iCurLevelPos += 1;
        fPos += fIntIncrement;
    }

    // process the inner vertices only if there are any
    if (cLevels > 0)
    {
        iCurLevelPos = 3;
        iPrevLevelPos = 1;
        fCurLevelA = fIncrement + fIncrement;
        iCurTriangle = 1;
        for (iLevel = 0; iLevel < cLevels - 1; iLevel++)
        {
            vPos1 = D3DXVECTOR2(fCurLevelA, 0.0f);
            vPos2 = D3DXVECTOR2(0.0f, fCurLevelA);

            fIntIncrement = 1.0f / (iLevel + 2.0f);
            fPos = fIntIncrement;

            for (iTriangle = 0; iTriangle < iLevel + 1; iTriangle++)
            {
                if (rgiVerts[iCurLevelPos + 1] == UNUSED)
                {
                    D3DXVec2Lerp(&vPos, &vPos1, &vPos2, fPos);
                    EvalBezierTri(&vPos, pbtTri, rgvControl, rgvControlNormal, bLinearNormals, pvPointsInt, cfvf, pbCurPoint);
                    pbCurPoint += cfvf.m_cBytesPerVertex;
                    rgiVerts[iCurLevelPos + 1] = (UINT_IDX)iCurVertex;
                    iCurVertex += 1;
                    cNewVertices += 1;
                }

                iCurLevelPos++;
                iPrevLevelPos++;
                fPos += fIntIncrement;
            }
            fCurLevelA += fIncrement;

            iPrevLevelPos += 1;
            iCurLevelPos += 2;
        }
    }

    // make certain that we haven't added too many vertices
    if (iCurVertex >= UNUSED)
    {
        return E_FAIL;
    }

    rgwFaceOut[0].rgwIndices[0] = rgiVerts[0];
    rgwFaceOut[0].rgwIndices[1] = rgiVerts[1];
    rgwFaceOut[0].rgwIndices[2] = rgiVerts[2];

    iCurLevelPos = 3;
    iPrevLevelPos = 1;
    iCurTriangle = 1;
    for (iLevel = 0; iLevel < cLevels; iLevel++)
    {
        for (iTriangle = 0; iTriangle < iLevel + 1; iTriangle++)
        {
            // add two triangles
            rgwFaceOut[iCurTriangle].rgwIndices[0] = rgiVerts[iPrevLevelPos];
            rgwFaceOut[iCurTriangle].rgwIndices[1] = rgiVerts[iCurLevelPos];
            rgwFaceOut[iCurTriangle].rgwIndices[2] = rgiVerts[iCurLevelPos + 1];
            iCurTriangle++;

            rgwFaceOut[iCurTriangle].rgwIndices[0] = rgiVerts[iPrevLevelPos];
            rgwFaceOut[iCurTriangle].rgwIndices[1] = rgiVerts[iCurLevelPos + 1];
            rgwFaceOut[iCurTriangle].rgwIndices[2] = rgiVerts[iPrevLevelPos + 1];
            iCurTriangle++;

            iCurLevelPos++;
            iPrevLevelPos++;
        }

        // add one triangle
        rgwFaceOut[iCurTriangle].rgwIndices[0] = rgiVerts[iPrevLevelPos];
        rgwFaceOut[iCurTriangle].rgwIndices[1] = rgiVerts[iCurLevelPos];
        rgwFaceOut[iCurTriangle].rgwIndices[2] = rgiVerts[iCurLevelPos + 1];
        iCurTriangle++;

        iPrevLevelPos += 1;
        iCurLevelPos += 2;
    }

    // update the current vertex pointer
    //iCurVertex += cNewVertices;

    pvPointsOut = pbCurPoint;

    return S_OK;
}

#if 0
// -------------------------------------------------------------------------------
//  method    D3DXTesselateBezierTri
//
//  devnote     Tesselates the given bezier triangle into a mesh
//
//  returns     S_OK if suceeded, else error code
//
HRESULT
D3DXTesselateBezierTri
(
    LPD3DXBEZIERTRI pbtTri,             // bezier triangle to tesselate
    LPD3DXVECTOR3 rgvControl,           // control point buffer (n control points, indexed by pbtTri)
    PVOID pvVertices,                   // vertices (indexed by pbtTri)
    DWORD cLevels,                       // number of levels of tesselation
    DWORD dwFVF,                        // FVF both for the mesh and the pvVertices vertex array
    DWORD dwOptions,                    // options to specify to the mesh
    LPDIRECT3DDEVICE8 pD3DDevice,       // device for mesh
    LPD3DXMESH *pptmMesh                  // output mesh for the tesselation
)
{
    HRESULT hr = S_OK;
    LPD3DXMESH ptmMesh = NULL;
    LPDIRECT3DVERTEXBUFFER8 pVertexBuffer = NULL;
    PVOID pvPoints = NULL;
    DXCrackFVF cfvf(dwFVF);

    DWORD cFacesOut;
    DWORD cFacesPrev;
    DWORD cVerticesOut;


    // temporary data
    SFaceTempStruct *rgwFaceTemp = NULL;
    DWORD *rgattrTemp = NULL;


    if ((rgvControl == NULL) || (pvVertices == NULL) || (dwOptions & D3DXMESH_32BIT))
        return D3DERR_INVALIDCALL;

    // get the number of faces and vertices generated
    CalculateCounts(cLevels, &cFacesOut, &cVerticesOut);

    rgwFaceTemp = new SFaceTempStruct[cFacesOut];
    rgattrTemp = new DWORD[cFacesOut];
    if ((rgwFaceTemp == NULL) || (rgattrTemp == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    memset(rgattrTemp, 0, sizeof(DWORD) * cFacesOut);

    hr = D3DXCreateMeshFVF(cFacesOut, cVerticesOut, dwOptions, dwFVF, pD3DDevice, &ptmMesh);
    if (FAILED(hr))
        goto e_Exit;


    hr = ptmMesh->GetVertexBuffer(&pVertexBuffer);
    if (FAILED(hr))
        goto e_Exit;

    hr = pVertexBuffer->Lock(0,0, (PBYTE*)&pvPoints, 0);
    if (FAILED(hr))
        goto e_Exit;

    TesselateBTri(pbtTri, rgvControl, NULL, TRUE, pvVertices, cfvf, cLevels, cVerticesOut, 0, pvPoints, rgwFaceTemp);

    ptmMesh->SetMesh(rgwFaceTemp, rgattrTemp);

    *pptmMesh = ptmMesh;
    ptmMesh = NULL;

e_Exit:
    GXRELEASE(ptmMesh);

    delete []rgwFaceTemp;
    delete []rgattrTemp;

    if (pvPoints != NULL)
    {
        pVertexBuffer->Unlock();
    }
    GXRELEASE(pVertexBuffer);
    return hr;
}
#endif

// -------------------------------------------------------------------------------
//  method    CalculateEdgeControlPoint
//
//  devnote     Generate an interior control point given the position and normal
//
//  returns     S_OK if suceeded, else error code
//
void
CalculateEdgeControlPoint
(
    D3DXVECTOR3 *pvPos1, 
    D3DXVECTOR3 *pvPos0, 
    D3DXVECTOR3 *pvNormal, 
    float fTension, 
    D3DXVECTOR3 *pvControl
)
{
    //float fEdgeLen;
    //float fTanLen;
    D3DXVECTOR3 vEdge;
    D3DXVECTOR3 vTangent;
  	//edge = v1 - v0
	//Cv01 = v0 + (edge - (edge dot N0)*N0)*Tension

    *pvControl = *pvPos1;
    *pvControl -= *pvPos0;
    vEdge = *pvControl;
    *pvControl -= (D3DXVec3Dot(pvControl, pvNormal) * *pvNormal);
    vTangent = *pvControl;

#if 0
    fEdgeLen = D3DXVec3Length(&vEdge);
    fTanLen = D3DXVec3Length(&vTangent);

    fTension = ((2.0f / 3.0f) * (fEdgeLen / fTanLen)) / (1.0f + ( D3DXVec3Dot(&vEdge, &vTangent) / (fEdgeLen * fTanLen) ) );
#endif

    *pvControl *= fTension;
	*pvControl += *pvPos0;
}

// -------------------------------------------------------------------------------
//  method    CalculateEdgeControlPoint
//
//  devnote     Generate an interior control point given the position and normal
//
//  returns     S_OK if suceeded, else error code
//
void
CalculateEdgeControlPoint2
(
    D3DXVECTOR3 *pvPos1, 
    D3DXVECTOR3 *pvMid, 
    D3DXVECTOR3 *pvPos0, 
    D3DXVECTOR3 *pvNormal, 
    float fTension, 
    D3DXVECTOR3 *pvControl
)
{
  	//edge = v1 - mid
	//Cv01 = v0 + (edge - (edge dot N0)*N0)*Tension

    *pvControl = *pvPos1;
    *pvControl -= *pvMid;
    *pvControl -= (D3DXVec3Dot(pvControl, pvNormal) * *pvNormal);
    *pvControl *= fTension;
	*pvControl += *pvPos0;
}

void
EvalBezier( D3DXVECTOR3 *pvPos0, D3DXVECTOR3 *pvPos1, D3DXVECTOR3 *pvPos2, D3DXVECTOR3 *pvPos3, 
           float t, D3DXVECTOR3 *pvOut)
{
    D3DXVECTOR3 p[4];

    p[0] = *pvPos0;
    p[1] = *pvPos1;
    p[2] = *pvPos2;
    p[3] = *pvPos3;

    for (int r = 1; r <= 3; r++)
    {
        for (int i = 0; i <= 3-r; i++)
        {
            D3DXVec3Lerp(&p[i], &p[i], &p[i+1], t );
        }
    }

    *pvOut = p[0];
}

// -------------------------------------------------------------------------------
//  method    CalcEdge
//
//  devnote     Calculates the normalized edge vector between two points
//
void
CalcEdge
(
    D3DXVECTOR3 *pvNormEdge,
    D3DXVECTOR3 *pvPoint1,
    D3DXVECTOR3 *pvPoint2
)
{
    *pvNormEdge = *pvPoint1;
    *pvNormEdge -= *pvPoint2;
    D3DXVec3Normalize(pvNormEdge, pvNormEdge);
}

// -------------------------------------------------------------------------------
//  method    GenerateNormalControlPoints
//
//  devnote     Generate control points for normals from the normals and position provided
//
//  returns     S_OK if suceeded, else error code
//
void 
GenerateNormalControlPoints
(
    PVOID pvPoint0,
    PVOID pvPoint1,
    PVOID pvPoint2,
    DXCrackFVF &cfvf,
    LPD3DXVECTOR3 rgvControl
)
{
    D3DXVECTOR3 vNorm;
    D3DXVECTOR3 vProj;
    float fProj;

    rgvControl[0] = *cfvf.PvGetNormal(pvPoint0);
    rgvControl[2] = *cfvf.PvGetNormal(pvPoint1);
    rgvControl[4] = *cfvf.PvGetNormal(pvPoint2);

	// Navg = (N0 + N1)*0.5;		// average the normals at both ends
	// enorm = Normalize( v1 - v0 );		// normalize edge vector (wah)
	// Nproj = (enorm dot Navg)*Navg;	// project onto edge vector
	// Nmid = Navg - 2*Nproj;			// reflect it

    D3DXVec3Lerp( &rgvControl[1], &rgvControl[0], &rgvControl[2], 0.5);
    D3DXVec3Lerp( &rgvControl[3], &rgvControl[2], &rgvControl[4], 0.5);
    D3DXVec3Lerp( &rgvControl[5], &rgvControl[0], &rgvControl[4], 0.5);

    CalcEdge(&vNorm, cfvf.PvGetPosition(pvPoint1), cfvf.PvGetPosition(pvPoint0));
    fProj = D3DXVec3Dot(&vNorm, &rgvControl[1]);
    vProj = vNorm;
    vProj *= 2.0f * fProj;
    rgvControl[1] -= vProj;

    CalcEdge(&vNorm, cfvf.PvGetPosition(pvPoint2), cfvf.PvGetPosition(pvPoint1));
    fProj = D3DXVec3Dot(&vNorm, &rgvControl[3]);
    vProj = vNorm;
    vProj *= 2.0f * fProj;
    rgvControl[3] -= vProj;

    CalcEdge(&vNorm, cfvf.PvGetPosition(pvPoint0), cfvf.PvGetPosition(pvPoint2));
    fProj = D3DXVec3Dot(&vNorm, &rgvControl[5]);
    vProj = vNorm;
    vProj *= 2.0f * fProj;
    rgvControl[5] -= vProj;
}

// -------------------------------------------------------------------------------
//  method    GenerateControlPoints
//
//  devnote     Generate control points from the normals and position provided
//
//  returns     S_OK if suceeded, else error code
//
void 
GenerateControlPoints
(
    PVOID pvPoint0,
    PVOID pvPoint1,
    PVOID pvPoint2,
    DXCrackFVF &cfvf,
    float fExtSlack,
    float fIntSlack,
    LPD3DXVECTOR3 rgvControl
)
{
    D3DXVECTOR3 *pvPos0;
    D3DXVECTOR3 *pvPos1;
    D3DXVECTOR3 *pvPos2;
    D3DXVECTOR3 *pvNormal0;
    D3DXVECTOR3 *pvNormal1;
    D3DXVECTOR3 *pvNormal2;
    //float fTension = 1.0f / 3.0f; 		// for now.
    float fTension = fExtSlack; 		// for now.
    //float fTension = 0.55f; 		// for now.
    //float fTension = 0.45f; 		// for now.
    D3DXVECTOR3 vTemp;

    pvPos0 = cfvf.PvGetPosition(pvPoint0);
    pvPos1 = cfvf.PvGetPosition(pvPoint1);
    pvPos2 = cfvf.PvGetPosition(pvPoint2);
    pvNormal0 = cfvf.PvGetNormal(pvPoint0);
    pvNormal1 = cfvf.PvGetNormal(pvPoint1);
    pvNormal2 = cfvf.PvGetNormal(pvPoint2);

    rgvControl[0] = *pvPos0;
    rgvControl[3] = *pvPos1;
    rgvControl[6] = *pvPos2;

#if 0
    if (cfvf.CTexCoords() <= 1)
    {
#endif
        CalculateEdgeControlPoint(pvPos1, pvPos0, pvNormal0, fTension, &rgvControl[1]);

        CalculateEdgeControlPoint(pvPos0, pvPos1, pvNormal1, fTension, &rgvControl[2]);

        CalculateEdgeControlPoint(pvPos2, pvPos1, pvNormal1, fTension, &rgvControl[4]);

        CalculateEdgeControlPoint(pvPos1, pvPos2, pvNormal2, fTension, &rgvControl[5]);

        CalculateEdgeControlPoint(pvPos0, pvPos2, pvNormal2, fTension, &rgvControl[7]);

        CalculateEdgeControlPoint(pvPos2, pvPos0, pvNormal0, fTension, &rgvControl[8]);
#if 0  
    }  // crease prototype
    else
    {
        D3DXVECTOR3 *pvEdgeNormal0;
        D3DXVECTOR3 *pvEdgeNormal1;
        D3DXVECTOR3 *pvEdgeNormal2;

        pvEdgeNormal0 = (D3DXVECTOR3*)cfvf.PuvGetTex1(pvPoint0);
        pvEdgeNormal1 = (D3DXVECTOR3*)cfvf.PuvGetTex1(pvPoint1);
        pvEdgeNormal2 = (D3DXVECTOR3*)cfvf.PuvGetTex1(pvPoint2);

        if ((*pvEdgeNormal0 == *pvNormal0) || (*pvEdgeNormal1 == *pvNormal1))
        {
            CalculateEdgeControlPoint(pvPos1, pvPos0, pvNormal0, fTension, &rgvControl[1]);

            CalculateEdgeControlPoint(pvPos0, pvPos1, pvNormal1, fTension, &rgvControl[2]);
        }
        else
        {
            CalculateEdgeControlPoint(pvPos1, pvPos0, pvEdgeNormal0, fTension, &rgvControl[1]);

            CalculateEdgeControlPoint(pvPos0, pvPos1, pvEdgeNormal1, fTension, &rgvControl[2]);
        }


        if ((*pvEdgeNormal1 == *pvNormal1) || (*pvEdgeNormal2 == *pvNormal2))
        {
            CalculateEdgeControlPoint(pvPos2, pvPos1, pvNormal1, fTension, &rgvControl[4]);

            CalculateEdgeControlPoint(pvPos1, pvPos2, pvNormal2, fTension, &rgvControl[5]);
        }
        else
        {
            CalculateEdgeControlPoint(pvPos2, pvPos1, pvEdgeNormal1, fTension, &rgvControl[4]);

            CalculateEdgeControlPoint(pvPos1, pvPos2, pvEdgeNormal2, fTension, &rgvControl[5]);
        }

        if ((*pvEdgeNormal0 == *pvNormal0) || (*pvEdgeNormal2 == *pvNormal2))
        {
            CalculateEdgeControlPoint(pvPos0, pvPos2, pvNormal2, fTension, &rgvControl[7]);

            CalculateEdgeControlPoint(pvPos2, pvPos0, pvNormal0, fTension, &rgvControl[8]);
        }
        else
        {
            CalculateEdgeControlPoint(pvPos0, pvPos2, pvEdgeNormal2, fTension, &rgvControl[7]);

            CalculateEdgeControlPoint(pvPos2, pvPos0, pvEdgeNormal0, fTension, &rgvControl[8]);
        }
    }
#endif

	//Cv = (Cv01 + Cv02 + Cv12 + Cv10 + Cv21 + Cv20)/4.0;	// edge control pts
	//Cv -= (v0 + v1 + v2 )/6.0;					// corner pts

    vTemp = rgvControl[1];
    vTemp += rgvControl[2];
    vTemp += rgvControl[4];
    vTemp += rgvControl[5];
    vTemp += rgvControl[7];
    vTemp += rgvControl[8];
    vTemp /= 4;
    //vTemp /= 3;
    //vTemp /= 2.4f;
    //vTemp /= 1.5f;
    rgvControl[9] = vTemp;

    vTemp = rgvControl[0];
    vTemp += rgvControl[3];
    vTemp += rgvControl[6];
    //vTemp /= 3;
    vTemp /= 6;
    //vTemp /= 2.0f;
    //vTemp /= 1.0f;
    rgvControl[9] -= vTemp;
}

DWORD FindNeighbor
    (
    CONST DWORD *pdwAdjacency, 
    DWORD iFace
    )
{
    if (pdwAdjacency[0] == iFace)
        return 0;
    else if (pdwAdjacency[1] == iFace)
        return 1;
    else 
        return 2;
}

template<class UINT_IDX, unsigned int UNUSED>
void InitVertIndices
    (
    UINT_IDX *rgiVerts, 
    DWORD cLevels, 
    DWORD cVertices, 
    DWORD iFace, 
    SFaceTempStruct<UINT_IDX> *rgwFaces, 
    CONST DWORD *rgdwAdjacency, 
    SVertOffsetStruct<UINT_IDX> *rgdwVertOffsets
    )
{
    CONST DWORD *pdwAdjacency;
    UINT_IDX rgiVertOffsets[3];
    DWORD iOffset;
    DWORD iEdge;
    DWORD iNeighbor;
    DWORD iLeftPos;
    DWORD iRightPos;
    DWORD iPoint;
    DWORD iInc;
    SFaceTempStruct<UINT_IDX> *pwFace = &rgwFaces[iFace];


    memset(rgiVerts, 0xff, cVertices * sizeof(UINT_IDX));

    if ((rgdwAdjacency != NULL) && (cLevels > 0))
    {
        pdwAdjacency = &rgdwAdjacency[iFace * 3];

        for (iOffset = 0; iOffset < 3; iOffset++)
        {
            iNeighbor = pdwAdjacency[iOffset];
            if (iNeighbor != UNUSED32)
            {
                iEdge = FindNeighbor(&rgdwAdjacency[iNeighbor * 3], iFace);

                // make certain that there is no crease between this triangle and the neighbor
                if ((rgwFaces[iNeighbor].rgwIndices[iEdge] == pwFace->rgwIndices[(iOffset+1)%3])
                    && (rgwFaces[iNeighbor].rgwIndices[(iEdge+1)%3] == pwFace->rgwIndices[iOffset]))
                {
                    rgiVertOffsets[iOffset] = rgdwVertOffsets[iNeighbor].rgdwOffsets[iEdge];
                }
                else
                {
                    rgiVertOffsets[iOffset] = UNUSED;
                }
            }
            else
            {
                rgiVertOffsets[iOffset] = UNUSED;
            }
        }

        if ((rgiVertOffsets[2] != UNUSED) || (rgiVertOffsets[0] != UNUSED))
        {
            if (rgiVertOffsets[0] != UNUSED)
                rgiVertOffsets[0] += (UINT_IDX)(cLevels - 1);

            iLeftPos = 1;
            iRightPos = 2;
            iInc = 2;
            for (iPoint = 0; iPoint < cLevels; iPoint++)
            {
                if (rgiVertOffsets[0] != UNUSED)
                {
                    rgiVerts[iLeftPos] = (UINT_IDX)rgiVertOffsets[0];
                    rgiVertOffsets[0] -= 1;
                }

                if (rgiVertOffsets[2] != UNUSED)
                {
                    rgiVerts[iRightPos] = (UINT_IDX)rgiVertOffsets[2];
                    rgiVertOffsets[2] += 1;
                }

                iLeftPos += iInc;
                iInc += 1;
                iRightPos += iInc;
            }
        }

        if (rgiVertOffsets[1] != UNUSED)
        {
            iLeftPos = cVertices - 2;
            for (iPoint = 0; iPoint < cLevels; iPoint++)
            {
                rgiVerts[iLeftPos] = (UINT_IDX)rgiVertOffsets[1];
                rgiVertOffsets[1] += 1;

                iLeftPos -= 1;
            }
           
        }
    }

    rgiVerts[0] = pwFace->rgwIndices[0];
    rgiVerts[cVertices - (cLevels + 2)] = pwFace->rgwIndices[1];
    rgiVerts[cVertices-1] = pwFace->rgwIndices[2];
}

// -------------------------------------------------------------------------------
//  method    D3DXTesselateMesh
//
//  devnote     Tesselates the given mesh, treating each triangle as a bezier triangle
//
//  returns     S_OK if suceeded, else error code
//
template<class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
inline
HRESULT 
D3DXTesselateMeshEx
(
    LPD3DXMESH ptmMeshIn,               // mesh to tesselate
    CONST DWORD *rgdwAdjacency,               // adjacency info
    FLOAT fNumSegs,                      // number of segments per edge to tesselate to
    BOOL bQuadraticNormals,             // if true use quadratic interp for normals, if false use linear
    LPD3DXMESH *pptmMeshOut,             // tesselated mesh
    UINT_IDX Bogus
)
{
    DWORD cNumSegs = (DWORD)fNumSegs;
    DWORD cLevels = (cNumSegs == 0) ? 0 : cNumSegs - 1;
    DWORD iNormalInterpDegree = bQuadraticNormals ? 2 : 1;
    float fExtSlack = 1.0f/3.0f;
    float fIntSlack = 2.0f/3.0f;

    HRESULT hr = S_OK;
    DWORD cNewFacesPerTriangle;
    DWORD cNewVerticesPerTriangle;
    DWORD cFacesOrig;
    DWORD cFacesNew;
    DWORD cVerticesOrig;
    DWORD cVerticesNew;
    DWORD dwOptions;
    DWORD dwFVF;
    LPDIRECT3DDEVICE8 pD3DDevice;
    LPD3DXMESH ptmMesh = NULL;
    SFaceTempStruct<UINT_IDX> *rgwFaceOrig = NULL;
    SFaceTempStruct<UINT_IDX> *pwFaceCur;
    SVertOffsetStruct<UINT_IDX> *rgdwVertOffsets = NULL;
    D3DXBEZIERTRI btri;
    D3DXVECTOR3 rgvControl[10];
    D3DXVECTOR3 rgvControlNormal[6];
    DWORD iCurVertex;
    DWORD iFace;
    DWORD iFaceNew;
    DXCrackFVF cfvf(D3DFVF_XYZ);
    DWORD iPoint;

    PBYTE pvPointsNew = NULL;
    PBYTE pvPointsNewMesh = NULL;
    PBYTE pvPointsOrig = NULL;
    PBYTE pvPointsCur = NULL;
    DWORD *rgattrIn = NULL;
    BOOL bLinearNormals;
    PBYTE rgbVertBuf = NULL;
    PBYTE rgbFaceBuf = NULL;
    UINT_IDX *rgwFacesNewMesh = NULL;
    DWORD *rgattrNew = NULL;

//    DWORD rgiVertOffsets[3];

    // temporary data
    SFaceTempStruct<UINT_IDX> *rgwFaceTemp = NULL;
    DWORD *rgattrTemp = NULL;
    UINT_IDX *rgiVerts;

    if ((ptmMeshIn == NULL) || (pptmMeshOut == NULL) 
        || ((iNormalInterpDegree != 1) && (iNormalInterpDegree != 2)))
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    if (b16BitIndex)
    {
        GXASSERT(!(ptmMeshIn->GetOptions() & D3DXMESH_32BIT));
    }
    else
    {
        GXASSERT(ptmMeshIn->GetOptions() & D3DXMESH_32BIT);
    }

    bLinearNormals = iNormalInterpDegree == 1;

    // first calculate the new faces/vertex counts from the original and tesselation level
    cFacesOrig = ptmMeshIn->GetNumFaces();
    cVerticesOrig = ptmMeshIn->GetNumVertices();
    dwFVF = ptmMeshIn->GetFVF();
    dwOptions = ptmMeshIn->GetOptions();
    ptmMeshIn->GetDevice(&pD3DDevice);
    pD3DDevice->Release(); // ptmMeshIn still maintains a ref to it, create mesh will add another
    cfvf = DXCrackFVF(dwFVF);

    CalculateCounts(cLevels, &cNewFacesPerTriangle, &cNewVerticesPerTriangle);

    cVerticesNew = cNewVerticesPerTriangle * cFacesOrig;
    cFacesNew = cNewFacesPerTriangle * cFacesOrig;

    rgiVerts = (UINT_IDX*)_alloca(cNewVerticesPerTriangle * sizeof(UINT_IDX));

    // if too many for a 16 bit mesh, bail
    if ((cFacesNew >= UNUSED) || (cVerticesNew >= UNUSED))
    {
        if (b16BitIndex)
            DPF(0, "D3DXTesselateMesh: Too many faces/vertices for a 16bit mesh, convert input mesh to 32 bit and try again\n");
        else
            DPF(0, "D3DXTesselateMesh: Too many faces/vertices for a 32bit mesh\n");

        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    // work around issue with delete's not working on arrays of templated structures
    rgbFaceBuf = new BYTE[sizeof(SFaceTempStruct<UINT_IDX>) * cFacesNew];
    rgbVertBuf = new BYTE[sizeof(SVertOffsetStruct<UINT_IDX>) * cFacesNew];

    // need to create these wonderful arrays to temporarily fill
    rgwFaceTemp = (SFaceTempStruct<UINT_IDX>*)rgbFaceBuf;
    rgdwVertOffsets = (SVertOffsetStruct<UINT_IDX>*)rgbVertBuf;
    rgattrTemp = new DWORD[cFacesNew];
    pvPointsNew = new BYTE[cfvf.m_cBytesPerVertex * cVerticesNew];
    if ((rgwFaceTemp == NULL) || (rgattrTemp == NULL) || (rgdwVertOffsets == NULL) || (pvPointsNew == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // initialize all vert offsets to unused
    memset(rgdwVertOffsets, 0xff, sizeof(SVertOffsetStruct<UINT_IDX>) * cFacesNew);

    hr = ptmMeshIn->LockAttributeBuffer(D3DLOCK_READONLY ,&rgattrIn);
    if (FAILED(hr))
        goto e_Exit;

    for (iFace = 0; iFace < cFacesOrig; iFace++)
    {
        for (iFaceNew = 0; iFaceNew < cNewFacesPerTriangle; iFaceNew++)
        {
            rgattrTemp[iFace * cNewFacesPerTriangle + iFaceNew] = rgattrIn[iFace];
        }
    }

    hr = ptmMeshIn->LockIndexBuffer(0, (PBYTE*)&rgwFaceOrig);
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmMeshIn->LockVertexBuffer(0, &pvPointsOrig);
    if (FAILED(hr))
        goto e_Exit;

    memcpy(pvPointsNew, pvPointsOrig, cVerticesOrig * cfvf.m_cBytesPerVertex);

    // probably should remove this extra level of indirection
    for (iPoint = 0; iPoint < 10; iPoint++)
    {
        btri.rgiControlPoints[iPoint] = static_cast<UINT16>(iPoint);
    }

    pvPointsCur = (PBYTE)pvPointsNew + cVerticesOrig * cfvf.m_cBytesPerVertex;
    pwFaceCur = rgwFaceTemp;
    iCurVertex = cVerticesOrig;
    for (iFace = 0; iFace < cFacesOrig; iFace++)
    {
        GenerateControlPoints(cfvf.GetArrayElem(pvPointsOrig, rgwFaceOrig[iFace].rgwIndices[0]),
                              cfvf.GetArrayElem(pvPointsOrig, rgwFaceOrig[iFace].rgwIndices[1]),
                              cfvf.GetArrayElem(pvPointsOrig, rgwFaceOrig[iFace].rgwIndices[2]), 
                              cfvf, fExtSlack, fIntSlack,
                              rgvControl);

        if (!bLinearNormals)
        {
            GenerateNormalControlPoints(cfvf.GetArrayElem(pvPointsOrig, rgwFaceOrig[iFace].rgwIndices[0]),
                                        cfvf.GetArrayElem(pvPointsOrig, rgwFaceOrig[iFace].rgwIndices[1]),
                                        cfvf.GetArrayElem(pvPointsOrig, rgwFaceOrig[iFace].rgwIndices[2]), 
                                        cfvf,
                                        rgvControlNormal);
        }

        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            btri.rgiVertices[iPoint] = rgwFaceOrig[iFace].rgwIndices[iPoint];
        }

        InitVertIndices<UINT_IDX,UNUSED>(rgiVerts, cLevels, cNewVerticesPerTriangle, iFace, rgwFaceOrig, rgdwAdjacency, rgdwVertOffsets);

        hr = TesselateBTri<UINT_IDX,UNUSED>(&btri, rgvControl, rgvControlNormal, bLinearNormals, pvPointsOrig, rgiVerts, cfvf, cLevels, cNewVerticesPerTriangle, iCurVertex, pvPointsCur, pwFaceCur, rgdwVertOffsets[iFace].rgdwOffsets);
        if (FAILED(hr))
            goto e_Exit;

        pwFaceCur += cNewFacesPerTriangle;
    }

    // create mesh after tesselation when number of vertices is known
    hr = D3DXCreateMeshFVF(cFacesNew, iCurVertex, dwOptions, dwFVF, pD3DDevice, &ptmMesh);
    if (FAILED(hr))
        goto e_Exit;

    // latch in the face neighborhood
    hr = ptmMesh->LockVertexBuffer(0, (PBYTE*)&pvPointsNewMesh);
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmMesh->LockIndexBuffer(0, (PBYTE*)&rgwFacesNewMesh);
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmMesh->LockAttributeBuffer(0, &rgattrNew);
    if (FAILED(hr))
        goto e_Exit;

    memcpy(pvPointsNewMesh, pvPointsNew, cfvf.m_cBytesPerVertex * iCurVertex);
    memcpy(rgwFacesNewMesh, rgwFaceTemp, cFacesNew * 3 * sizeof(UINT_IDX));
    memcpy(rgattrNew, rgattrTemp, sizeof(DWORD) * cFacesNew);

    *pptmMeshOut = ptmMesh;
    ptmMesh->AddRef();

e_Exit:
    delete []rgattrTemp;
    delete []rgbFaceBuf;
    delete []rgbVertBuf;
    delete []pvPointsNew;

    if (rgattrIn != NULL)
    {
        ptmMeshIn->UnlockAttributeBuffer();
    }

    if (rgwFaceOrig != NULL)
    {
        ptmMeshIn->UnlockIndexBuffer();
    }

    if (rgwFacesNewMesh != NULL)
    {
        ptmMesh->UnlockIndexBuffer();
    }
    
    if (rgattrNew != NULL)
    {
        ptmMesh->UnlockAttributeBuffer();
    }

    if (pvPointsNewMesh != NULL)
    {
        ptmMesh->UnlockVertexBuffer();
    }
 
    if (pvPointsOrig != NULL)
    {
        ptmMeshIn->UnlockVertexBuffer();
    } 

    GXRELEASE(ptmMesh);

    return hr;
}


// -------------------------------------------------------------------------------
//  method    D3DXTesselateMesh
//
//  devnote     Tesselates the given mesh, treating each triangle as a bezier triangle
//
//  returns     S_OK if suceeded, else error code
//
HRESULT WINAPI
D3DXTesselateMesh
(
    LPD3DXMESH ptmMeshIn,               // mesh to tesselate
    CONST DWORD *rgdwAdjacency,               // adjacency info
    FLOAT fNumSegs,                      // number of segments per edge to tesselate to
    BOOL bQuadraticNormals,             // if true use quadratic interp for normals, if false use linear
    LPD3DXMESH *pptmMeshOut             // tesselated mesh
)
{
    unsigned int Bogus1 = 0;
    unsigned short Bogus2 = 0;
	if (ptmMeshIn == NULL)
    {
        DPF(0, "D3DXTesselateMesh: Failed due to input mesh being NULL\n");
		return D3DERR_INVALIDCALL;
    }

    if (ptmMeshIn->GetOptions() & D3DXMESH_32BIT)
        return D3DXTesselateMeshEx<unsigned int, FALSE, UNUSED32>(ptmMeshIn, rgdwAdjacency, fNumSegs, bQuadraticNormals, pptmMeshOut, Bogus1);
    else
        return D3DXTesselateMeshEx<unsigned short, TRUE, UNUSED16>(ptmMeshIn, rgdwAdjacency, fNumSegs, bQuadraticNormals, pptmMeshOut, Bogus2);
}
