/*//////////////////////////////////////////////////////////////////////////////
//
// File: loadutil.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
// @@BEGIN_MSINTERNAL
//
// History:
// -@-          (craigp)    - created 
// -@- 08/06/99 (mikemarr)  - prepend GX to all Luciform functions
//                          - started comment history
// -@- 08/26/99 (mikemarr)  - replace gxbasetype.h with gxmathcore.h
// -@- 09/23/99 (mikemarr)  - GXCross return D3DXVECTOR
//                          - added newline to the end of the file
//
// @@END_MSINTERNAL
//
//////////////////////////////////////////////////////////////////////////////*/

#include "pchmesh.h"

#include "loadutil.h"


/*
 * Template routine to handle dynamic allocation, based on Heap* Win32 APIs.
 *
 *  T **ppBase: base of array.
 *  const T& obj: object to add to end of array.
 *  UINT *pcNum: number of objects in array.
 *  UINT *pcMax: max. number of objects in array.
 *
 * ppBase, pcNum and pcMax point to values that usually get modified
 * by the call. The only case when they are not updated is if there
 * is insufficient memory for the initial allocation (if *pBase is NULL)
 * or a second allocation if *pcNum==*pcMax.
 *
 * Return value: TRUE if allocation was successful
 *               FALSE if there was insufficient memory
 *      FALSE means nothing was added to the array, but nothing
 *        was lost either. No weird realloc semantics allowed!
 */
template<class T>
BOOL
AddToDynamicArray( T **ppBase, const T& obj, DWORD *pcNum, DWORD *pcMax )
{
    T *pBase = *ppBase;
    if ( ! pBase )
    {
        HeapValidate( GetProcessHeap(), 0, NULL );
        pBase = new T[2];
        *pcNum = 0;
        *pcMax = 2;
    }
    if ( *pcNum == *pcMax )
    {
        DWORD cNewMax = *pcMax*2;
        T *newarr = new T[cNewMax];
        if ( ! newarr )
            return FALSE;
        for (DWORD i = 0; i < *pcNum; i++)
            newarr[i] = pBase[i];
        *pcMax = cNewMax;
        delete []pBase;
        pBase = newarr;
    }
    pBase[ (*pcNum)++ ] = obj;
    *ppBase = pBase;
    return TRUE;
}

HRESULT 
SetVertexColor(SLoadedFace *pface, DWORD iPoint, D3DXCOLOR &color, SLoadVertex **prglvLoaded, DWORD *pcVertices, DWORD *pcVerticesMax)
{
    HRESULT hr = S_OK;
    SLoadVertex *plvVertex;
    SLoadVertex lvVertexNew;
    DWORD iVertex;
    DWORD iVertexNew;
    DWORD wCur;
    DWORD wHead;
    SLoadVertex *plvCur;

    GXASSERT(pface != NULL && prglvLoaded != NULL && pcVertices != NULL && pcVerticesMax != NULL);

    iVertex = pface->m_wIndices[iPoint];
    GXASSERT(iVertex < *pcVertices);

    plvVertex = &(*prglvLoaded)[iVertex];

    // if the vertex is not currently owned, take ownership
    if (!plvVertex->m_bOwned)
    {
        plvVertex->m_bOwned = true;
        plvVertex->m_color = color;
    }
    // else if the vertex is owned but the color is not the same 
    //   NOTE: if the same, then just ignore the fact that the color was being set
    else if (plvVertex->m_color != color)
    {
        lvVertexNew = *plvVertex;
        GXASSERT(lvVertexNew.m_bOwned);

        // UNDONE - this is currently rechecking the first vertex, might be possible
        //   to skip depending on sematics of the mesh file
        wHead = iVertex;
        wCur = wHead;
        do
        {
            plvCur = &( (*prglvLoaded)[wCur] );

            if (plvCur->m_color == color)
            {
                // found an equivalent wedge, just point at it
                pface->m_wIndices[iPoint] = wCur;
                goto e_Exit;
            }

            // go to next wedge in the vertex
            wCur = (*prglvLoaded)[wCur].m_wPointList;
        } while (wCur != wHead);

        // set both the color and the representative point to create a new vertex that is 
        //   logically the same in the mesh, but has a different color attribute
        lvVertexNew.m_color = color;
        lvVertexNew.m_wPointRep = (*prglvLoaded)[iVertex].m_wPointRep;

        iVertexNew = (DWORD)*pcVertices;

        if ( ! AddToDynamicArray( prglvLoaded, lvVertexNew, pcVertices, pcVerticesMax ) )
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        (*prglvLoaded)[iVertexNew].m_wPointList = (*prglvLoaded)[lvVertexNew.m_wPointRep].m_wPointList;
        (*prglvLoaded)[lvVertexNew.m_wPointRep].m_wPointList = iVertexNew;

        // now update the face to refer to the new point with the correct attributes
        pface->m_wIndices[iPoint] = iVertexNew;
    }

e_Exit:
    return hr;
}

HRESULT 
SetVertexAttributes(SLoadedFace *pface, DWORD iPoint, D3DXVECTOR3 *pvNormal, D3DXVECTOR2 *puvTex1, SLoadVertex **prglvLoaded, DWORD *pcVertices, DWORD *pcVerticesMax)
{
    HRESULT hr = S_OK;
    SLoadVertex *plvVertex;
    SLoadVertex lvVertexNew;
    DWORD iVertex;
    DWORD iVertexNew;
    DWORD wCur;
    DWORD wHead;
    SLoadVertex *plvCur;

    GXASSERT(pface != NULL && prglvLoaded != NULL && pcVertices != NULL && pcVerticesMax != NULL);

    iVertex = pface->m_wIndices[iPoint];
    GXASSERT(iVertex < *pcVertices);

    plvVertex = &(*prglvLoaded)[iVertex];

    // UNDONE craigp - It might be useful to search all instances of a logical vertex
    //   in order to find another one to share with

    // if the vertex is not currently owned, take ownership
    if (!plvVertex->m_bOwned)
    {
        plvVertex->m_bOwned = true;

        if (pvNormal != NULL)
            plvVertex->m_vNormal = *pvNormal;

        if (puvTex1 != NULL)
            plvVertex->m_uvTex1 = *puvTex1;
    }
    else if (((pvNormal != NULL) && (plvVertex->m_vNormal != *pvNormal))
            || ((puvTex1 != NULL) && (plvVertex->m_uvTex1 != *puvTex1)))
    {
        lvVertexNew = *plvVertex;
        GXASSERT(lvVertexNew.m_bOwned);

        // set both the attributes and the representative point to create a new vertex that is 
        //   logically the same in the mesh, but has a different vertex attributes

        if (pvNormal != NULL)
            lvVertexNew.m_vNormal = *pvNormal;

        if (puvTex1 != NULL)
            plvVertex->m_uvTex1 = *puvTex1;

        // UNDONE - this is currently rechecking the first vertex, might be possible
        //   to skip depending on sematics of the mesh file
        wHead = iVertex;
        wCur = wHead;
        do
        {
            plvCur = &( (*prglvLoaded)[wCur] );

            if (BEqualWedges(*plvCur, lvVertexNew))
            {
                // found an equivalent wedge, just point at it
                pface->m_wIndices[iPoint] = wCur;
                goto e_Exit;
            }

            // go to next wedge in the vertex
            wCur = (*prglvLoaded)[wCur].m_wPointList;
        } while (wCur != wHead);

        lvVertexNew.m_wPointRep = (*prglvLoaded)[iVertex].m_wPointRep;

        iVertexNew = (DWORD)*pcVertices;

        if ( ! AddToDynamicArray( prglvLoaded, lvVertexNew, pcVertices, pcVerticesMax ) )
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // link the vertex in to the vertex list
        (*prglvLoaded)[iVertexNew].m_wPointList = (*prglvLoaded)[lvVertexNew.m_wPointRep].m_wPointList;
        (*prglvLoaded)[lvVertexNew.m_wPointRep].m_wPointList = iVertexNew;

        // now update the face to refer to the new point with the correct attributes
        pface->m_wIndices[iPoint] = iVertexNew;
    }

e_Exit:
    return hr;
}

HRESULT 
SetSmoothingGroup(SLoadedFace *pface, DWORD iPoint, WORD iSmoothingGroup, D3DXVECTOR3 &vNormal, SLoadVertex **prglvLoaded, DWORD *pcVertices, DWORD *pcVerticesMax)
{
    HRESULT hr = S_OK;
    SLoadVertex *plvVertex;
    SLoadVertex lvVertexNew;
    DWORD iVertex;
    DWORD iVertexNew;
    DWORD wCur;
    DWORD wHead;
    DWORD wWedgeFound;
    SLoadVertex *plvCur;

    GXASSERT(pface != NULL && prglvLoaded != NULL && pcVertices != NULL && pcVerticesMax != NULL);

    iVertex = pface->m_wIndices[iPoint];
    GXASSERT(iVertex < *pcVertices);

    plvVertex = &(*prglvLoaded)[iVertex];

    // if the vertex is not currently owned, take ownership
    if (plvVertex->m_cSmoothingGroupFaces == 0)
    {
        GXASSERT(plvVertex->m_iSmoothingGroup == 0);

        plvVertex->m_cSmoothingGroupFaces = 1;
        plvVertex->m_iSmoothingGroup = iSmoothingGroup;

        plvVertex->m_vNormal = vNormal;
    }
    else
    {       

        if (iSmoothingGroup > 0)
        {
            wHead = iVertex;
            wCur = wHead;
            wWedgeFound = UNUSED32;
            do
            {
                plvCur = &( (*prglvLoaded)[wCur] );

                if (plvVertex->m_iSmoothingGroup == iSmoothingGroup)
                {
                    // found a wedge of this smoothing group
                    wWedgeFound = wCur;
                    break;
                }

                // go to next wedge in the vertex
                wCur = (*prglvLoaded)[wCur].m_wPointList;
            } while (wCur != wHead);
        }
        else  // in the 0 case, need to approximate flat shading so always
            //  insert a new wedge
        {           
            wHead = iVertex;
            wCur = wHead;
            wWedgeFound = UNUSED32;
            do
            {
                plvCur = &( (*prglvLoaded)[wCur] );

                if ((plvVertex->m_iSmoothingGroup == iSmoothingGroup) &&
                    (plvVertex->m_vNormal == vNormal))
                {
                    // found a wedge of this smoothing group
                    wWedgeFound = wCur;
                    break;
                }

                // go to next wedge in the vertex
                wCur = (*prglvLoaded)[wCur].m_wPointList;
            } while (wCur != wHead);
        }

        // if a wedge was found, add the current normal in to the wedge
        if (wWedgeFound != UNUSED32)
        {
            (*prglvLoaded)[wWedgeFound].m_cSmoothingGroupFaces += 1;
            (*prglvLoaded)[wWedgeFound].m_vNormal += vNormal;

            // now update the face to refer to the new point with the correct attributes
            pface->m_wIndices[iPoint] = wWedgeFound;
        }
        else  // if no matching wedge found.  add a new wedge
        {
            lvVertexNew = *plvVertex;

            lvVertexNew.m_wPointRep = (*prglvLoaded)[iVertex].m_wPointRep;

            lvVertexNew.m_iSmoothingGroup = iSmoothingGroup;
            lvVertexNew.m_cSmoothingGroupFaces = 1;
            lvVertexNew.m_vNormal = vNormal;

            iVertexNew = (DWORD)*pcVertices;

            if ( ! AddToDynamicArray( prglvLoaded, lvVertexNew, pcVertices, pcVerticesMax ) )
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            // link the vertex in to the vertex list
            (*prglvLoaded)[iVertexNew].m_wPointList = (*prglvLoaded)[lvVertexNew.m_wPointRep].m_wPointList;
            (*prglvLoaded)[lvVertexNew.m_wPointRep].m_wPointList = iVertexNew;

            // now update the face to refer to the new point with the correct attributes
            pface->m_wIndices[iPoint] = iVertexNew;
        }

    }

e_Exit:
    return hr;
}

DWORD 
FindPoint(PDWORD pwIndices, DWORD iPointSearch, SLoadVertex *rglvVerts)
{
    DWORD iPoint;

    // get the representative for the point, so that we can compare
    //   them, this function compares the logical points in the mesh
    DWORD wPointSearchRep = rglvVerts[iPointSearch].m_wPointRep;

    for (iPoint = 0; iPoint < 3; iPoint++)
    {
        if (rglvVerts[pwIndices[iPoint]].m_wPointRep == wPointSearchRep)
        {
            break;
        }
    }

    return iPoint;
}

HRESULT  
InitVertices(SLoadVertex *&rglvVertices, DWORD cVertices)
{
    HRESULT hr = S_OK;
    DWORD iVertex;

    rglvVertices = new SLoadVertex[cVertices];
    if (rglvVertices == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        rglvVertices[iVertex].m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        rglvVertices[iVertex].m_vNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
//        rglvVertices[iVertex].m_color = D3DXCOLOR(0.9f, 0.6f, 0.4f, 0.0f);
        rglvVertices[iVertex].m_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
        rglvVertices[iVertex].m_uvTex1 = D3DXVECTOR2(0.0f, 0.0f);
        rglvVertices[iVertex].m_wPointRep = iVertex;
        rglvVertices[iVertex].m_wPointList = iVertex;
        rglvVertices[iVertex].m_bOwned = false;
        rglvVertices[iVertex].m_iFVFDataOffset = iVertex;

        rglvVertices[iVertex].m_iSmoothingGroup = 0;
        rglvVertices[iVertex].m_cSmoothingGroupFaces = 0;
    }

e_Exit:
    return hr;
}

HRESULT 
InitCorners(SCorner *&rgCorners, DWORD cCorners)
{
    HRESULT hr = S_OK;
    DWORD iCorner;

    rgCorners = new SCorner[cCorners];
    if (rgCorners == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    for (iCorner = 0; iCorner < cCorners; iCorner++)
    {
        rgCorners[iCorner].m_wPoint = UNUSED32;
        rgCorners[iCorner].m_wFace = UNUSED32;

        rgCorners[iCorner].m_vNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        rgCorners[iCorner].m_uvTex1 = D3DXVECTOR2(0.0f, 0.0f);

        rgCorners[iCorner].m_bNormalSpecified = false;
        rgCorners[iCorner].m_bUvSpecified = false;
    }

e_Exit:
    return hr;
}

HRESULT 
InitFaces(SLoadedFace *&rglfFaces, DWORD cFaces)
{
    HRESULT hr = S_OK;
    DWORD iFace;

    rglfFaces = new SLoadedFace[cFaces];
    if (rglfFaces == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    for (iFace = 0; iFace < cFaces; iFace++)
    {
        rglfFaces[iFace].InitEmpty();
    }

e_Exit:
    return hr;
}

bool
BEqualWedges(SLoadVertex &lv1, SLoadVertex &lv2)
{

    return (lv1.m_color == lv2.m_color) 
        && (lv1.m_uvTex1 == lv2.m_uvTex1)
        && (lv1.m_vNormal == lv2.m_vNormal);
}


D3DCOLOR
ConvertColor(D3DXCOLOR &color)
{
    DWORD _r, _g, _b;
    if (color.r < 0.0f) _r = 0;
    else if (color.r > 1.0f) _r = 0xff;
    else _r = (DWORD) (color.r*255.0f+0.5f);
    if (color.g < 0.0f) _g = 0;
    else if (color.g > 1.0f) _g = 0xff;
    else _g = (DWORD) (color.g*255.0f+0.5f);
    if (color.b < 0.0f) _b = 0;
    else if (color.b > 1.0f) _b = 0xff;
    else _b = (DWORD) (color.b*255.0f+0.5f);
    return D3DCOLOR_XRGB( _r, _g, _b );
}

HRESULT 
SetMesh(SLoadedFace *&rglfFaces, DWORD cFaces, 
                SLoadVertex *&rglvLoaded, DWORD cVertices, 
                SCorner *rgCorners, DWORD cCorners,
                SFVFData *pFVFData, BOOL bUsePointRepData,
                DWORD dwOptions, DWORD dwFVF, LPDIRECT3DDEVICE8 pD3DDevice, 
                LPD3DXMESH *ppMesh, LPD3DXBUFFER *ppbufAdjacency)
{
    HRESULT hr = S_OK;
    PBYTE pvPoints = NULL;
    D3DXVECTOR3 vNormal;

    DWORD iVertex;
    D3DXVECTOR3 *pvNormal;
    D3DXVECTOR2 *puvTex1;
    DWORD iPoint;
    DWORD iFace;
    DWORD iCorner;
    PBYTE pvCurPoint;
    DWORD iWeight;
    DWORD iTexCoord;
    DWORD iInitialTexCoord;
    PBYTE pbCur;
    DWORD cFacesActual;
    DWORD iFaceActual;

    ID3DXMesh *ptmMesh = NULL;
    DWORD *rgdwFaces = NULL;
    UINT16 *rgwFaces = NULL;
    DWORD *rgiAttribIds = NULL;
    DWORD *pdwFaceCur;
    UINT16 *pwFaceCur;
    PVOID pvFaces = NULL;

    DWORD cVerticesMax = cVertices;
    BOOL bSmoothingGroups;

    DWORD *rgdwPointRepsTemp = NULL;
    LPD3DXBUFFER pbufAdjacency = NULL;
    DWORD *pdwAdjacency;

    GXASSERT(ppMesh != NULL);
    GXASSERT(rglfFaces != NULL);
    GXASSERT(rglvLoaded != NULL);
    // Corners are allowed to be NULL
    DXCrackFVF  cfvf(dwFVF);                                
    DXCrackFVF  cfvfOtherData(pFVFData != NULL ? pFVFData->dwFVF : D3DFVF_XYZ);                                

    if ((cFaces == 0) || (cVertices == 0))
    {
        hr = D3DXERR_INVALIDDATA;
        goto e_Exit;
    }

    // handle smoothing groups
    bSmoothingGroups = false;
    for (iFace = 0; iFace < cFaces; iFace++)
    {
        if (rglfFaces[iFace].m_bSmoothingGroupSpecified)
        {
            bSmoothingGroups = true;
        }
    }

    // if there are smoothing groups, and the normals are there is a 
    //   reason to compute smoothing groups, then compute them.
    if (bSmoothingGroups && cfvf.BNormal())
    {
        SLoadedFace *plfFace;
        SLoadVertex *plvPos0;
        SLoadVertex *plvPos1;
        SLoadVertex *plvPos2;
        D3DXVECTOR3 vEdge1;
        D3DXVECTOR3 vEdge2;
        SLoadVertex *plvVert;
        DWORD iVert;

        for (iFace = 0 ; iFace < cFaces; iFace++)
        {
            plfFace = &rglfFaces[iFace];
            if (plfFace->m_bSmoothingGroupSpecified)
            {
                plvPos0 = &rglvLoaded[plfFace->m_wIndices[0]];
                plvPos1 = &rglvLoaded[plfFace->m_wIndices[1]];
                plvPos2 = &rglvLoaded[plfFace->m_wIndices[2]];

                vEdge1 = plvPos0->m_vPos - plvPos1->m_vPos;
                vEdge2 = plvPos0->m_vPos - plvPos2->m_vPos;

                // calculate the normal of the face from the two edge vectors
                D3DXVec3Cross(&vNormal, &vEdge1, &vEdge2);
                vNormal /= D3DXVec3Length(&vNormal);
                //vNormal *= -1;

                for (iPoint = 0; iPoint < 3; iPoint++)
                {
                    hr = SetSmoothingGroup(&rglfFaces[iFace], iPoint, rglfFaces[iFace].m_iSmoothingGroup, vNormal, &rglvLoaded, &cVertices, &cVerticesMax);
                    if (FAILED(hr))
                        goto e_Exit;
                }
            }
        }

        DWORD iTest;
        iTest = 0;
        // average and renormalize vertices shared among faces of the same smoothing group
        for (iVert = 0; iVert < cVertices; iVert++)
        {
            plvVert = &rglvLoaded[iVert];

            if (plvVert->m_cSmoothingGroupFaces > 1)
            {
                /*plvVert->m_vNormal /= plvVert->m_cSmoothingGroupFaces*/;
                plvVert->m_vNormal /= D3DXVec3Length(&plvVert->m_vNormal);
            }

            if (plvVert->m_iSmoothingGroup == 0)
            {
                iTest += 1;
            }
        }

    }

    if (cfvf.BDiffuse())
    {
        // propagate face color attributes into vertex attributes
        for (iFace = 0; iFace < cFaces; iFace++)
        {
            if (rglfFaces[iFace].m_bColorSpecified)
            {
                for (iPoint = 0; iPoint < 3; iPoint++)
                {
                    hr = SetVertexColor(&rglfFaces[iFace], iPoint, rglfFaces[iFace].m_colorFace, &rglvLoaded, &cVertices, &cVerticesMax);
                    if (FAILED(hr))
                        goto e_Exit;
                }
            }

        }
    }

    // after dealing face attributes, reset owned flags to false, and reprocess
    // the 
    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        rglvLoaded[iVertex].m_bOwned = false;
    }

    // propogate corner attributes into vertex attributes
    for (iCorner = 0; iCorner < cCorners; iCorner++)
    {
        if (rgCorners[iCorner].m_bNormalSpecified || rgCorners[iCorner].m_bUvSpecified)
        {
            iFace = rgCorners[iCorner].m_wFace;
			if ((iFace >= cFaces) || (rglfFaces[iFace].m_wIndices[0] == UNUSED32))
				continue;

            iPoint = FindPoint(rglfFaces[iFace].m_wIndices, rgCorners[iCorner].m_wPoint, rglvLoaded);

            GXASSERT(iPoint < 3);

            pvNormal = NULL;
            if (rgCorners[iCorner].m_bNormalSpecified && cfvf.BNormal())
            {
                pvNormal = &rgCorners[iCorner].m_vNormal;
            }
            
            puvTex1 = NULL;
            if (rgCorners[iCorner].m_bUvSpecified && cfvf.BTex1())
            {
                puvTex1 = &rgCorners[iCorner].m_uvTex1;
            }

            hr = SetVertexAttributes(&rglfFaces[iFace], iPoint, pvNormal, puvTex1, &rglvLoaded, &cVertices, &cVerticesMax);
            if (FAILED(hr))
                goto e_Exit;
        }
    }


    if ((cFaces >= UNUSED16) || (cVertices >= UNUSED16))
    {
        dwOptions |= D3DXMESH_32BIT;
    }

    // need to remove degenerate triangles
    cFacesActual = 0;
    for (iFace = 0; iFace < cFaces; iFace++)
    {
        if ((rglfFaces[iFace].m_wIndices[0] == rglfFaces[iFace].m_wIndices[1])
            || (rglfFaces[iFace].m_wIndices[0] == rglfFaces[iFace].m_wIndices[2])
            || (rglfFaces[iFace].m_wIndices[1] == rglfFaces[iFace].m_wIndices[2]))
        {
            rglfFaces[iFace].m_wIndices[0] = UNUSED32;
            rglfFaces[iFace].m_wIndices[1] = UNUSED32;
            rglfFaces[iFace].m_wIndices[2] = UNUSED32;
        }
        else
        {
            cFacesActual += 1;
        }
    }

    // create the mesh now that we know the correct size
    hr = D3DXCreateMeshFVF(cFacesActual, cVertices, dwOptions, dwFVF, pD3DDevice, &ptmMesh);
    if (FAILED(hr))
        goto e_Exit;

    // get the vertex buffer, fill the vertices in place
    hr = ptmMesh->LockVertexBuffer(0, &pvPoints);
    if (FAILED(hr))
        goto e_Exit;

    // transform the points into the proper FVF format
    for (iPoint=0, pvCurPoint = pvPoints; iPoint < cVertices; iPoint++, pvCurPoint+=cfvf.m_cBytesPerVertex)
    {
        cfvf.SetPosition(pvCurPoint, &rglvLoaded[iPoint].m_vPos);

        cfvf.SetNormal(pvCurPoint, &rglvLoaded[iPoint].m_vNormal);
        cfvf.SetDiffuse(pvCurPoint, ConvertColor(rglvLoaded[iPoint].m_color));
        cfvf.SetTex1(pvCurPoint, &rglvLoaded[iPoint].m_uvTex1);

        if (pFVFData != NULL)
        {
            pbCur = (PBYTE)&pFVFData->rgiFVFData[pFVFData->cBytesPerVertex/sizeof(DWORD) * rglvLoaded[iPoint].m_iFVFDataOffset];

            if (cfvfOtherData.CWeights() > 0)
            {
                for (iWeight = 0; iWeight < cfvfOtherData.CWeights(); iWeight++)
                {
                    cfvf.SetWeight(pvCurPoint, iWeight, *(float*)pbCur);
                    pbCur += sizeof(float);
                }
            }

            if (cfvfOtherData.BNormal())
            {
                cfvf.SetNormal(pvCurPoint, (D3DXVECTOR3*)pbCur);
                pbCur += sizeof(D3DXVECTOR3);
            }

            if (cfvfOtherData.BDiffuse())
            {
                cfvf.SetDiffuse(pvCurPoint, *(D3DCOLOR*)pbCur);
                pbCur += sizeof(D3DCOLOR);
            }

            if (cfvfOtherData.BSpecular())
            {
                cfvf.SetSpecular(pvCurPoint, *(D3DCOLOR*)pbCur);
                pbCur += sizeof(D3DCOLOR);
            }

            if (cfvfOtherData.CTexCoords() > 0)
            {
                // UNDONE UNDONE - fix when 3d tex coords are handled
                //iInitialTexCoord = cfvf.BTex1() ? 1 : 0;
                iInitialTexCoord = pFVFData->iTexCoordOffset;

                for (iTexCoord = iInitialTexCoord; iTexCoord < iInitialTexCoord + cfvfOtherData.CTexCoords(); iTexCoord++)
                {
                    cfvf.SetTexCoord(pvCurPoint, iTexCoord, (D3DXVECTOR2*)pbCur);
                    pbCur += sizeof(D3DXVECTOR2);
                }
            }
        }
    }

    hr = ptmMesh->LockIndexBuffer(0, (PBYTE*)&pvFaces);
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmMesh->LockAttributeBuffer(0, &rgiAttribIds);
    if (FAILED(hr))
        goto e_Exit;

    // copy the material data to arrays understood by ID3DXMesh::SetMesh
    for (iFace = iFaceActual = 0; iFace < cFaces; iFace++)
    {
        if (rglfFaces[iFace].m_wIndices[0] == UNUSED32)
            continue;

        if (rglfFaces[iFace].m_bAttributeSpecified)
        {
            rgiAttribIds[iFaceActual] = rglfFaces[iFace].m_attr;
        }
        else
        {
            //rgiAttribIds[iFace] = UNUSED32;
            rgiAttribIds[iFaceActual] = 0;
        }

        iFaceActual += 1;
    }

    if ( dwOptions & D3DXMESH_32BIT)
    {
        rgdwFaces = (DWORD*)pvFaces;

        for (iFace = 0, pdwFaceCur = rgdwFaces; iFace < cFaces; iFace++)
        {
            if (rglfFaces[iFace].m_wIndices[0] == UNUSED32)
                continue;

            pdwFaceCur[0] = rglfFaces[iFace].m_wIndices[0];
            pdwFaceCur[1] = rglfFaces[iFace].m_wIndices[1];
            pdwFaceCur[2] = rglfFaces[iFace].m_wIndices[2];
            pdwFaceCur += 3;
        }
    }
    else // 16 bit indices
    {
        rgwFaces = (UINT16*)pvFaces;

        for (iFace = 0, pwFaceCur = rgwFaces; iFace < cFaces; iFace++)
        {
            if (rglfFaces[iFace].m_wIndices[0] == UNUSED32)
                continue;

            pwFaceCur[0] = (UINT16)rglfFaces[iFace].m_wIndices[0];
            pwFaceCur[1] = (UINT16)rglfFaces[iFace].m_wIndices[1];
            pwFaceCur[2] = (UINT16)rglfFaces[iFace].m_wIndices[2];

            pwFaceCur += 3;
        }
    }

    ptmMesh->UnlockAttributeBuffer();
    rgiAttribIds = NULL;
    ptmMesh->UnlockIndexBuffer();
    pvFaces = NULL;
    ptmMesh->UnlockVertexBuffer();
    pvPoints = NULL;

    // if adjacency information is desired, generate it from point rep information
    if (ppbufAdjacency != NULL)
    {
        hr = D3DXCreateBuffer(cFacesActual * 3 * sizeof(DWORD), &pbufAdjacency);
        if (FAILED(hr))
            goto e_Exit;
        
        pdwAdjacency = (DWORD*)pbufAdjacency->GetBufferPointer();
        GXASSERT(cFacesActual * 3 * sizeof(DWORD) == pbufAdjacency->GetBufferSize());

        rgdwPointRepsTemp = new DWORD[cVertices];
        if (rgdwPointRepsTemp == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // load the point reps into the temp array
        for (iPoint = 0; iPoint < cVertices; iPoint++)
        {
            rgdwPointRepsTemp[iPoint] = rglvLoaded[iPoint].m_wPointRep;
        }

        if (bUsePointRepData)
        {
            hr = ptmMesh->ConvertPointRepsToAdjacency(rgdwPointRepsTemp, pdwAdjacency);
            if (FAILED(hr))
                goto e_Exit;
        }
        else
        {
            hr = ptmMesh->GenerateAdjacency(1.0e-6f, pdwAdjacency);
            if (FAILED(hr))
                goto e_Exit;
        }

        *ppbufAdjacency = pbufAdjacency;
        pbufAdjacency = NULL;
    }

    *ppMesh = ptmMesh;
e_Exit:
    if (rgiAttribIds != NULL)
    {
        ptmMesh->UnlockAttributeBuffer();
    }
    if (pvFaces != NULL)
    {
        ptmMesh->UnlockIndexBuffer();
    }

    delete []rgdwPointRepsTemp;

    GXRELEASE(pbufAdjacency);

    if (pvPoints != NULL)
    {
        ptmMesh->UnlockVertexBuffer();
    }

    if (FAILED(hr))
        GXRELEASE(ptmMesh);

    return hr;
}


HRESULT
CreateMaterialBuffer(LPD3DXMATERIAL rgmat, DWORD cmat, LPD3DXBUFFER *ppbufMaterials)
{
    HRESULT hr = S_OK;
    DWORD cbTotalStringSize;
    DWORD iCurOffset;
    DWORD imat;
    LPD3DXBUFFER pbufMaterialsOut = NULL;
    LPD3DXMATERIAL rgmatOut;
    DWORD cbName;

    // first calculate the amount of memory needed for the string buffers
    cbTotalStringSize = 0;
    for (imat = 0; imat < cmat; imat++)
    {
        if (rgmat[imat].pTextureFilename != NULL)
        {
            cbTotalStringSize += strlen(rgmat[imat].pTextureFilename) + 1;
        }
    }

    hr = D3DXCreateBuffer(sizeof(D3DXMATERIAL) * cmat + cbTotalStringSize, &pbufMaterialsOut);
    if (FAILED(hr))
        goto e_Exit;

    rgmatOut = (LPD3DXMATERIAL)pbufMaterialsOut->GetBufferPointer();

    // fist copy the materials info into the new array (note: string pointers are now incorrect)
    memcpy(rgmatOut, rgmat, sizeof(D3DXMATERIAL) * cmat);

    // start allocating strings just after the last material
    iCurOffset = sizeof(D3DXMATERIAL) * cmat;
    for (imat = 0; imat < cmat; imat++)
    {
        if (rgmat[imat].pTextureFilename != NULL)
        {
            rgmatOut[imat].pTextureFilename = ((char*)rgmatOut) + iCurOffset;

            cbName = strlen(rgmat[imat].pTextureFilename) + 1;
            memcpy(rgmatOut[imat].pTextureFilename, rgmat[imat].pTextureFilename, cbName);

            iCurOffset += cbName;
        }
    }

    GXASSERT(iCurOffset == sizeof(D3DXMATERIAL) * cmat + cbTotalStringSize);

    *ppbufMaterials = pbufMaterialsOut;
    pbufMaterialsOut = NULL;

e_Exit:
    GXRELEASE(pbufMaterialsOut);
    return hr;
}

HRESULT
MergeMaterialBuffers(LPD3DXBUFFER pbufMat1, DWORD cmat1, LPD3DXBUFFER pbufMat2, DWORD cmat2, LPD3DXBUFFER *ppbufMaterials)
{
    HRESULT hr = S_OK;
    D3DXMATERIAL *rgmatTemp = NULL;
    D3DXMATERIAL *rgmat1;
    D3DXMATERIAL *rgmat2;

    if (pbufMat1 == NULL)
    {
        if (pbufMat2 == NULL)
        {
            *ppbufMaterials = NULL;
        }
        else
        {
            *ppbufMaterials = pbufMat2;
            pbufMat2->AddRef();
        }
    }
    else if (pbufMat2 == NULL)
    {
        *ppbufMaterials = pbufMat1;
        pbufMat1->AddRef();
    }
    else  // both have materials
    {
        rgmat1 = (LPD3DXMATERIAL)pbufMat1->GetBufferPointer();
        rgmat2 = (LPD3DXMATERIAL)pbufMat2->GetBufferPointer();

        // make an array contains the color info and pointers to the original strings
        //   that is the two arrays combined
        rgmatTemp = new D3DXMATERIAL[cmat1 + cmat2];
        if (rgmatTemp == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        memcpy(rgmatTemp, rgmat1, sizeof(D3DXMATERIAL) * cmat1);
        memcpy(rgmatTemp + cmat1, rgmat2, sizeof(D3DXMATERIAL) * cmat2);

        // then use the CreateMaterialBuffer call to take and make a buffer out of the "merged" array
        hr = CreateMaterialBuffer(rgmatTemp, cmat1 + cmat2, ppbufMaterials);
        if (FAILED(hr))
            goto e_Exit;
    }

e_Exit:
    delete []rgmatTemp;
    return hr;

}
