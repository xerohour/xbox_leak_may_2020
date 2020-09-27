/*//////////////////////////////////////////////////////////////////////////////
//
// File: createmesh.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
// @@BEGIN_MSINTERNAL
//
// History:
// -@-          (craigp)    - created 
// -@- 08/19/99 (mikemarr)  - prepend GX to gxfmath functions
//                          - started comment history
//                          - replace references to gxbasetype.h with gxmathcore.h
// -@- 09/23/99 (mikemarr)  - changed <> to "" on #includes
//
// @@END_MSINTERNAL
//
//////////////////////////////////////////////////////////////////////////////*/

#include "pchmesh.h"
#include "orbitvertex.h"

template<class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT ValidateIndices
    (
	UINT_IDX *rgwIndices,
	CONST DWORD *rgdwAdjacency,
	DWORD cFaces,
	DWORD cVertices
	);


// break bowties... two fans of triangles using the same vertex, just add a new vertex
HRESULT D3DXBreakBowTies
    (
    LPD3DXMESH pMeshIn,
    CONST DWORD *rgdwAdjacency,
    LPD3DXMESH *ppMeshOut
    )
{
    HRESULT hr = S_OK;
    BYTE *rgbFaceSeen = NULL;
    DWORD *rgdwFaceIds = NULL;
    DWORD *rgdwNewVertices = NULL;
    DWORD cNewVerticesMax;
    DWORD cNewVertices;
    DWORD cVertices;
    DWORD cFaces;
    COrbitVertexIter ovi(NULL, NULL, 2);
    BOOL b16BitIndex;
    DWORD wPoint;
    DWORD wCurPoint;
    DWORD iPoint;
    DWORD iCurPoint;
    DWORD iCurFace;
    WORD *rgwIndices = NULL;
    DWORD *rgdwIndices;
    WORD *rgwIndicesNew = NULL;
    DWORD *rgdwIndicesNew = NULL;
    WORD *rgwIndicesOut = NULL;
    PBYTE pbPointsOut = NULL;
    PBYTE pbPointsIn = NULL;
    PBYTE pbCurPoint;
    PBYTE pbSplitPoint;
    DWORD iVertex;
    DWORD iFace;
    DWORD iReplaceVertex;
    DWORD iReplaceValue;
    DWORD *rgdwTemp;
    DWORD iCurNewVertex;
    DXCrackFVF cfvf(D3DFVF_XYZ);
    LPD3DXMESH pMeshOut = NULL;
    LPDIRECT3DDEVICE8 pDevice;

    if ((pMeshIn == NULL) || (rgdwAdjacency == NULL) || (ppMeshOut == NULL))
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    cVertices = pMeshIn->GetNumVertices();
    cFaces = pMeshIn->GetNumFaces();
    b16BitIndex = !(pMeshIn->GetOptions() & D3DXMESH_32BIT);
    cfvf = DXCrackFVF(pMeshIn->GetFVF());

    hr = pMeshIn->LockIndexBuffer(0, (PBYTE*)&rgwIndices);
    if (FAILED(hr))
        goto e_Exit;
    rgdwIndices = (DWORD*)rgwIndices;

	if (b16BitIndex)
	{
		hr = ValidateIndices<unsigned short, TRUE, UNUSED16>(rgwIndices, rgdwAdjacency, cFaces, cVertices);
		if (FAILED(hr))
			goto e_Exit;
	}
	else
	{
		hr = ValidateIndices<unsigned int, FALSE, UNUSED32>((unsigned int*)rgdwIndices, rgdwAdjacency, cFaces, cVertices);
		if (FAILED(hr))
			goto e_Exit;
	}

    ovi = COrbitVertexIter(rgdwAdjacency, (PVOID)rgwIndices, b16BitIndex ? 2 : 4);

    cNewVertices = 0; 
    cNewVerticesMax = 10;
    iCurNewVertex = cVertices;

    rgbFaceSeen = new BYTE[cFaces * 3];
    rgdwFaceIds = new DWORD[cVertices];
    rgdwNewVertices = new DWORD[cNewVerticesMax];
    if ((rgbFaceSeen == NULL) 
        || (rgdwFaceIds == NULL)
        || (rgdwNewVertices == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    if (b16BitIndex)
    {
        rgwIndicesNew = new WORD[cFaces * 3];
        if (rgwIndicesNew == NULL) 
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        memcpy(rgwIndicesNew, rgwIndices, sizeof(WORD) * 3 * cFaces);
    }
    else
    {
        rgdwIndicesNew = new DWORD[cFaces * 3];
        if (rgdwIndicesNew == NULL) 
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        memcpy(rgdwIndicesNew, rgdwIndices, sizeof(DWORD) * 3 * cFaces);
    }


    memset(rgbFaceSeen, 0, sizeof(BYTE) * 3 * cFaces);
    memset(rgdwFaceIds, 0xff, sizeof(DWORD) * cVertices);

    for (iFace = 0; iFace < cFaces; iFace++)
    {
        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            if (!rgbFaceSeen[iFace * 3 + iPoint])
            {
                rgbFaceSeen[iFace*3 + iPoint] = TRUE;

                if (b16BitIndex)
                {
                    wPoint = rgwIndices[iFace * 3 + iPoint];
                }
                else
                {
                    wPoint = rgdwIndices[iFace * 3 + iPoint];
                }

                ovi.Init(iFace, wPoint, x_iAllFaces);
                ovi.MoveToCCWFace();

                iReplaceVertex = UNUSED32;
                iReplaceValue = UNUSED32;

                while (!ovi.BEndOfList())
                {
                    iCurFace = ovi.GetNextFace();

                    iCurPoint = ovi.IGetPointIndex();
                    rgbFaceSeen[iCurFace * 3 + iCurPoint] = TRUE;

                    if (b16BitIndex)
                    {
                        wCurPoint = rgwIndices[iCurFace * 3 + iCurPoint];
                    }
                    else
                    {
                        wCurPoint = rgdwIndices[iCurFace * 3 + iCurPoint];
                    }

                    // if the point is in the process of being split, then remap to new value
                    if (wCurPoint == iReplaceVertex)
                    {
                        if (b16BitIndex)
                            rgwIndicesNew[iCurFace * 3 + iCurPoint] = (WORD)iReplaceValue;
                        else
                            rgdwIndicesNew[iCurFace * 3 + iCurPoint] = (WORD)iReplaceValue;
                    }
                    // if unclaimed, then claim it
                    else if (rgdwFaceIds[wCurPoint] == UNUSED32)
                    {
                        rgdwFaceIds[wCurPoint] = iFace;
                    }
                    // if it is claimed, but not by a face in this orbit, then split (bow tie found)
                    else if (rgdwFaceIds[wCurPoint] != iFace)
                    {
                        // realloc arry if neccessary
                        if (cNewVertices == cNewVerticesMax)
                        {
                            cNewVerticesMax *= 2;
                            rgdwTemp = new DWORD[cNewVerticesMax];
                            if (rgdwTemp == NULL)
                            {
                                hr = E_OUTOFMEMORY;
                                goto e_Exit;
                            }

                            memcpy(rgdwTemp, rgdwNewVertices, sizeof(DWORD) * cNewVertices);
                            
                            rgdwNewVertices = rgdwTemp;
                        }

                        // come up with a new vertex to split out to
                        iReplaceVertex = wCurPoint;
                        iReplaceValue = iCurNewVertex;
                        iCurNewVertex += 1;

                        // add a new point to be replicated onto the end of the original vertex buffer
                        rgdwNewVertices[cNewVertices] = iReplaceVertex;
                        cNewVertices += 1;

                        // update the current face to point at the next value
                        if (b16BitIndex)
                            rgwIndicesNew[iCurFace * 3 + iCurPoint] = (WORD)iReplaceValue;
                        else
                            rgdwIndicesNew[iCurFace * 3 + iCurPoint] = (WORD)iReplaceValue;
                    }
                }
            }
        }
    }

    GXASSERT(iCurNewVertex == cVertices + cNewVertices);

    // if any vertices were split, create a new mesh
    if (cNewVertices > 0)
    {
        pMeshIn->GetDevice(&pDevice);
        pDevice->Release(); // pMeshIn still maintains a ref to it, create mesh will add another

        hr = D3DXCreateMeshFVF(cFaces, iCurNewVertex, pMeshIn->GetOptions(), pMeshIn->GetFVF(), pDevice, &pMeshOut);
        if (FAILED(hr))
            goto e_Exit;

        hr = pMeshOut->LockIndexBuffer(0, (PBYTE*)&rgwIndicesOut);
        if (FAILED(hr))
            goto e_Exit;

        hr = pMeshIn->LockVertexBuffer(0, &pbPointsIn);
        if (FAILED(hr))
            goto e_Exit;

        hr = pMeshOut->LockVertexBuffer(0, &pbPointsOut);
        if (FAILED(hr))
            goto e_Exit;

        if (b16BitIndex)
            memcpy(rgwIndicesOut, rgwIndicesNew, sizeof(WORD) * 3 * cFaces);
        else
            memcpy(rgwIndicesOut, rgdwIndicesNew, sizeof(DWORD) * 3 * cFaces);

        memcpy(pbPointsOut, pbPointsIn, cfvf.m_cBytesPerVertex * cVertices);

        pbCurPoint = pbPointsOut + cfvf.m_cBytesPerVertex * cVertices;
        for (iVertex = 0; iVertex < cNewVertices; iVertex++)
        {
            pbSplitPoint = cfvf.GetArrayElem(pbPointsIn, rgdwNewVertices[iVertex]);
            memcpy(pbCurPoint, pbSplitPoint, cfvf.m_cBytesPerVertex);

            pbCurPoint += cfvf.m_cBytesPerVertex;
        }

        *ppMeshOut = pMeshOut;
        pMeshOut->AddRef();
    }
    else // no bow ties found return the same mesh
    {
        *ppMeshOut = pMeshIn;
        pMeshIn->AddRef();
    }


e_Exit:
    delete []rgdwFaceIds;
    delete []rgbFaceSeen;
    delete []rgdwNewVertices;
    delete []rgwIndicesNew;
    delete []rgdwIndicesNew;

    if (rgwIndices != NULL)
    {
        pMeshIn->UnlockIndexBuffer();
    }

    if (rgwIndicesOut != NULL)
    {
        pMeshOut->UnlockIndexBuffer();
    }

    if (pbPointsIn != NULL)
    {
        pMeshIn->UnlockVertexBuffer();
    }

    if (pbPointsOut != NULL)
    {
        pMeshOut->UnlockVertexBuffer();
    }

    GXRELEASE(pMeshOut);

    return hr;
}

// clean a mesh up for simplification (try to make manifold) currently only breaks bowties
HRESULT WINAPI D3DXCleanMesh
    (
    LPD3DXMESH pMeshIn,
    CONST DWORD *rgdwAdjacency,
    LPD3DXMESH *ppMeshOut
    )
{
	HRESULT hr = S_OK;
    hr = D3DXBreakBowTies(pMeshIn, rgdwAdjacency,ppMeshOut);

#ifdef _DEBUG
	// if debug, do a valid mesh to spit other consistencies out to 
	//  the debug window, but DON'T return the return code
	//    CleanMesh succeeds if it cleans up what is is supposed to
    if (!FAILED(hr))
	    D3DXValidMesh(*ppMeshOut, rgdwAdjacency);
#endif

	return hr;
}

template<class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT ValidateIndices
    (
	UINT_IDX *rgwIndices,
	CONST DWORD *rgdwAdjacency,
	DWORD cFaces,
	DWORD cVertices
	)
{
	HRESULT hr = S_OK;
	DWORD iFace;
	DWORD iPoint;
    DWORD iPointBad;
    DWORD iFaceBad;

	for (iFace = 0; iFace < cFaces; iFace++)
	{
		// make sure that the index values are valid
		for (iPoint = 0; iPoint < 3; iPoint++)
		{
			if ((rgwIndices[iFace*3 + iPoint] >= cVertices) 
				|| (rgwIndices[iFace*3 + iPoint] == UNUSED))
			{
				DPF(0, "D3DXValidIndices: An invalid index value(%d) was found on face %d", rgwIndices[iFace*3 + iPoint], iFace);
				hr = D3DXERR_INVALIDMESH;
			}

			if ((rgdwAdjacency[iFace*3 + iPoint] >= cFaces) && (rgdwAdjacency[iFace*3 + iPoint] != UNUSED32)) 
			{
				DPF(0, "D3DXValidIndices: An invalid neighbor index value(%d) was found on face %d", rgdwAdjacency[iFace*3 + iPoint], iFace);
				hr = D3DXERR_INVALIDMESH;
			}
		}

		// first check to make sure that there are no degenerate triangles with
		//   two or more identical vertex indices per face
		if ((rgwIndices[iFace*3 + 0] == rgwIndices[iFace*3 + 1])
			|| (rgwIndices[iFace*3 + 0] == rgwIndices[iFace*3 + 2])
			|| (rgwIndices[iFace*3 + 1] == rgwIndices[iFace*3 + 2]))
		{
            if (rgwIndices[iFace*3 + 0] == rgwIndices[iFace*3 + 1])
                iPointBad = rgwIndices[iFace*3 + 0];
            else if (rgwIndices[iFace*3 + 1] == rgwIndices[iFace*3 + 2])
                iPointBad = rgwIndices[iFace*3 + 2];
            else //if (rgwIndices[iFace*3 + 0] == rgwIndices[iFace*3 + 2])
                iPointBad = rgwIndices[iFace*3 + 0];

			DPF(0, "D3DXValidIndices: A point(%d) was found more than once in triangle %d", iPointBad, iFace);
			hr = D3DXERR_INVALIDMESH;
		}

		// next make sure that each triangle points
		//   two or more identical vertex indices per face
		if (((rgdwAdjacency[iFace*3 + 0] == rgdwAdjacency[iFace*3 + 1]) && (rgdwAdjacency[iFace*3 + 0] != UNUSED32))
			|| ((rgdwAdjacency[iFace*3 + 0] == rgdwAdjacency[iFace*3 + 2]) && (rgdwAdjacency[iFace*3 + 0] != UNUSED32))
			|| ((rgdwAdjacency[iFace*3 + 1] == rgdwAdjacency[iFace*3 + 2]) && (rgdwAdjacency[iFace*3 + 1] != UNUSED32)))
		{
		    if ((rgdwAdjacency[iFace*3 + 0] == rgdwAdjacency[iFace*3 + 1]) && (rgdwAdjacency[iFace*3 + 0] != UNUSED32))
                iFaceBad = rgdwAdjacency[iFace*3 + 0];
            else if ((rgdwAdjacency[iFace*3 + 0] == rgdwAdjacency[iFace*3 + 2]) && (rgdwAdjacency[iFace*3 + 0] != UNUSED32))
                iFaceBad = rgdwAdjacency[iFace*3 + 0];
			else // if  ((rgdwAdjacency[iFace*3 + 1] == rgdwAdjacency[iFace*3 + 2]) && (rgdwAdjacency[iFace*3 + 1] != UNUSED32)))
                iFaceBad = rgdwAdjacency[iFace*3 + 1];

			DPF(0, "D3DXValidIndices: A neighbor triangle index(%d) was found more than once on triangle %d", iFaceBad, iFace);
			DPF(0, "D3DXValidIndices:   (Likely problem is that two triangles share same points with opposite orientations)");
			hr = D3DXERR_INVALIDMESH;
		}
	}

	return hr;
}
 


template<class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT D3DXValidMeshEx
    (
    LPD3DXMESH pMeshIn,
    CONST DWORD *rgdwAdjacency,
	UINT_IDX Bogus
    )
{
	HRESULT hr = S_OK;
	UINT_IDX *rgwIndices = NULL;
	DWORD cVertices;
	DWORD cFaces;
	DWORD iFace;
	DWORD iPoint;
	UINT_IDX wPoint;
	DWORD iCurPoint;
	DWORD iCurFace;
	UINT_IDX wCurPoint;
	DXCrackFVF cfvf(D3DFVF_XYZ);
	BYTE *rgbFaceSeen = NULL;
	UINT_IDX *rgwFaceIds = NULL;
    COrbitVertexIter ovi(NULL, NULL, 2);

    if ((pMeshIn == NULL) || (rgdwAdjacency == NULL))
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    cVertices = pMeshIn->GetNumVertices();
    cFaces = pMeshIn->GetNumFaces();
    cfvf = DXCrackFVF(pMeshIn->GetFVF());

    hr = pMeshIn->LockIndexBuffer(0, (PBYTE*)&rgwIndices);
    if (FAILED(hr))
        goto e_Exit;

	hr = ValidateIndices<UINT_IDX,b16BitIndex,UNUSED>(rgwIndices, rgdwAdjacency, cFaces, cVertices);
	if (FAILED(hr))
		goto e_Exit;

	// allocate buffers to check for bow ties
    rgbFaceSeen = new BYTE[cFaces * 3];
    rgwFaceIds = new UINT_IDX[cVertices];
    if ((rgbFaceSeen == NULL) 
        || (rgwFaceIds == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    memset(rgbFaceSeen, 0, sizeof(BYTE) * 3 * cFaces);
    memset(rgwFaceIds, 0xff, sizeof(UINT_IDX) * cVertices);

    ovi = COrbitVertexIter(rgdwAdjacency, (PVOID)rgwIndices, b16BitIndex ? 2 : 4);

	// actually check all the faces for bowties
	for (iFace = 0; iFace < cFaces; iFace++)
	{
        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            if (!rgbFaceSeen[iFace*3 + iPoint])
            {
                rgbFaceSeen[iFace*3 + iPoint] = TRUE;

                wPoint = rgwIndices[iFace*3 + iPoint];

				// walk all the faces around the point looking for bowties
                ovi.Init(iFace, wPoint, x_iAllFaces);
                ovi.MoveToCCWFace();
                while (!ovi.BEndOfList())
                {
                    iCurFace = ovi.GetNextFace();

                    iCurPoint = ovi.IGetPointIndex();
                    rgbFaceSeen[iCurFace*3 + iCurPoint] = TRUE;
                    wCurPoint = rgwIndices[iCurFace*3 + iCurPoint];

					// if the point hasn't been claimed yet, then mark it
                    if (rgwFaceIds[wCurPoint] == UNUSED)
                    {
                        rgwFaceIds[wCurPoint] = (UINT_IDX)iFace;
                    }
					// else if it wasn't claimed by another face in this orbit, then
					//   we have found a bowtie
					else if (rgwFaceIds[wCurPoint] != iFace)
					{
						DPF(0, "D3DXValidMesh: Bowtie found");
						hr = D3DXERR_INVALIDMESH;
						goto e_Exit;
					}
				}
			}
		}
	}

e_Exit:
    if (rgwIndices != NULL)
    {
        pMeshIn->UnlockIndexBuffer();
    }

	delete []rgbFaceSeen;
	delete []rgwFaceIds;
	return hr;
}

HRESULT WINAPI D3DXValidMesh
    (
    LPD3DXMESH pMeshIn,
    CONST DWORD *rgdwAdjacency
    )
{
    unsigned int Bogus1 = 0;
    unsigned short Bogus2 = 0;

	if (pMeshIn == NULL)
    {
        DPF(0, "pMeshIn pointer is invalid");
		return D3DERR_INVALIDCALL;
    }

    if (pMeshIn->GetOptions() & D3DXMESH_32BIT)
        return D3DXValidMeshEx<unsigned int, FALSE, UNUSED32>(pMeshIn, rgdwAdjacency, Bogus1);
    else
        return D3DXValidMeshEx<unsigned short, TRUE, UNUSED16>(pMeshIn, rgdwAdjacency, Bogus2);
}
