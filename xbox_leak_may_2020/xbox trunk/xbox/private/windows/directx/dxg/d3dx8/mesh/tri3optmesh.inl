

// -------------------------------------------------------------------------------
//  method    Compact
//
//  devnote
//            Moves all unused faces to the end of the arrays
//              and moves all unused vertices to the end of the arrays
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::Compact
    (
    UINT *rgiFaceRemap,
    UINT *rgiFaceRemapInverse,
    UINT *rgiVertRemap
    )
{

    HRESULT hr = S_OK;
    UINT_IDX cFacesNew;
    UINT_IDX cVertsNew;
    UINT_IDX iVert;
    UINT_IDX iFace;
    UINT_IDX *pwFace;


    // only initialize the vertex array, the face array will be initialized
    //   by the first loop, which calculates where the faces move to
    for (iVert = 0; iVert < m_cVertices; iVert++)
    {
        rgiVertRemap[iVert] = UNUSED;
    }

    for (iFace = 0; iFace < m_cFaces; iFace++)
    {
        rgiFaceRemapInverse[iFace] = iFace;
    }

    // first calculate how to remap the faces, and mark vertices
    //   as used
    cFacesNew = 0;
    for (iFace = 0; iFace < m_cFaces; iFace++)
    {
        pwFace = m_pFaces[iFace].m_wIndices;

        if (pwFace[0] != UNUSED)
        {
            rgiFaceRemap[iFace] = cFacesNew;
            rgiFaceRemapInverse[cFacesNew] = iFace;
            cFacesNew += 1;

            // mark the vertices as in use
            rgiVertRemap[pwFace[0]] = 1;
            rgiVertRemap[pwFace[1]] = 1;
            rgiVertRemap[pwFace[2]] = 1;

        }
        else
        {
            rgiFaceRemap[iFace] = UNUSED;
        }
    }

    for (iFace = cFacesNew; iFace < m_cFaces; iFace++)
    {
        rgiFaceRemapInverse[iFace] = UNUSED;
    }

    // next calculate the new positions of the vertices, and move them there
    cVertsNew = 0;
    for (iVert = 0; iVert < m_cVertices; iVert++)
    {
        if (rgiVertRemap[iVert] != UNUSED)
        {
            rgiVertRemap[iVert] = cVertsNew;
            cVertsNew += 1;

        }
    }

    return hr;
}

// used to hold data for sorting attribute bundles
struct SSortInfo
{
    UINT cEntries;
    UINT iCurOffset;
    UINT cVertexEntries;
    UINT iCurVertexOffset;
};

// -------------------------------------------------------------------------------
//  method    VertexOptimize
//
//  devnote
//            Reorder vertices to appear in the same order as used in triangles
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::VertexOptimize(UINT *rgiFaceRemapInverse, UINT *rgiVertRemap)
{
    HRESULT hr = S_OK;
    UINT_IDX iVert;
    UINT_IDX iFaceIndex;
    UINT_IDX iFace;
    UINT_IDX iPoint;
    UINT_IDX *pwFace;
    UINT_IDX iCurVertex;

    // reinitialize all to point to unused
    for (iVert = 0; iVert < m_cVertices; iVert++)
    {
        rgiVertRemap[iVert] = UNUSED;
    }

    // now go through and reorder them
    iCurVertex = 0;
    for (iFaceIndex = 0; iFaceIndex < m_cFaces; iFaceIndex++)
    {
        iFace = rgiFaceRemapInverse[iFaceIndex];
        if (iFace == UNUSED)
            continue;

        pwFace = m_pFaces[iFace].m_wIndices;
        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            // check to see if the current index of the face is in the array yet
            //    if not add it to the current end of the array
            if (rgiVertRemap[pwFace[iPoint]] == UNUSED)
            {
                rgiVertRemap[pwFace[iPoint]] = iCurVertex;
                iCurVertex++;
            }
        }
    }

    GXASSERT(iCurVertex <= m_cVertices);

    return hr;
}

// -------------------------------------------------------------------------------
//  method    CollectAttributeIds
//
//  devnote
//            Create an array of all the attribute ids found in the mesh
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::CollectAttributeIds
    (
    DWORD **prgiAttribIds,
    LPDWORD pcattr
    )
{
    HRESULT hr = S_OK;
    UINT iFace;
    UINT iattr;
    UINT cattr = 0;
    UINT cattrMax = 0;
    DWORD *rgiAttribIds = NULL;
    DWORD *rgiAttribIdsTemp = NULL;
    DWORD attrCur;
    DWORD attrPrev;

    // if no faces, then there are no attribute bundles
    if (m_cFaces == 0)
    {
        *pcattr = 0;
        *prgiAttribIds = NULL;
        goto e_Exit;
    }

    // allocate an initial array of attribute bundles
    cattrMax = 10;
    rgiAttribIds = new DWORD[cattrMax];
    if (rgiAttribIds == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // for each face, see if the attribute bundle is in the array
    attrPrev = 0;
    for (iFace = 0; iFace < m_cFaces; iFace++)
    {
        if (m_pFaces[iFace].m_wIndices[0] == UNUSED)
            continue;

        attrCur = m_rgiAttributeIds[iFace];

        // if same as previous, then it is already in the array
        if ((attrCur == attrPrev) && (cattr != 0))
        {
            continue;
        }

        attrPrev = attrCur;

        // search for the current one in the array
        for (iattr = 0; iattr < cattr; iattr++)
        {
            if (rgiAttribIds[iattr] == attrCur)
                break;
        }
        GXASSERT(iattr <= cattr);

        // if not in the array, then add it
        if (iattr == cattr)
        {
            // if we have run out of space in the array, then reallocate it
            if (cattr == cattrMax)
            {
                cattrMax *= 2;
                rgiAttribIdsTemp = new DWORD[cattrMax];
                if (rgiAttribIdsTemp == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }

                memcpy (rgiAttribIdsTemp, rgiAttribIds, cattr * sizeof(DWORD));

                delete []rgiAttribIds;
                rgiAttribIds = rgiAttribIdsTemp;
            }

            rgiAttribIds[cattr] = attrCur;
            cattr += 1;
        }
    }

    *prgiAttribIds = rgiAttribIds;
    *pcattr = cattr;
    rgiAttribIds = NULL;


e_Exit:
    delete []rgiAttribIds;

    return hr;
}

// Prototype. Defined in createmesh.cpp
int __cdecl CmpFunc(const void* elem1, const void * elem2);

// -------------------------------------------------------------------------------
//  method    AttributeSort
//
//  devnote
//            Sort all the faces based on their attribute bundles
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::AttributeSort
    (
    UINT *rgiFaceRemap,
    UINT *rgiFaceRemapInverse,
    UINT *rgiVertRemap,
    BOOL bIgnoreVerts
    )
{

    HRESULT hr = S_OK;
    UINT_IDX iFace;
    UINT_IDX iFaceIndex;
    SSortInfo *rgSortInfo = NULL;
    UINT iIndex;
    UINT iIndexPrev;
    DWORD attrPrev;
    DWORD attrCur;
    UINT iTotal;
    UINT iVertexTotal;
    DWORD *rgiAttrPresent = NULL;
    DWORD ciAttrPresent;
    UINT iVert;
    UINT iPoint;
    D3DXATTRIBUTERANGE *rgaeAttributeTableOld = NULL;

    // if not moving vertices and there is an attribute table, keep the all
    //   the attribute ranges that are present (some may not have any faces
    //   due to being from a PM at lowest LOD)
    if (bIgnoreVerts && (m_rgaeAttributeTable != NULL))
    {
        ciAttrPresent = m_caeAttributeTable;
        rgiAttrPresent = new DWORD[ciAttrPresent];
        if (rgiAttrPresent == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        for (iIndex = 0; iIndex < ciAttrPresent; iIndex++)
        {
            rgiAttrPresent[iIndex] = m_rgaeAttributeTable[iIndex].AttribId;
        }
    }
    else // get the attribute ids out of the attribute buffer
    {
        hr = CollectAttributeIds(&rgiAttrPresent, &ciAttrPresent);
        if (FAILED(hr))
            goto e_Exit;

        qsort(rgiAttrPresent, ciAttrPresent, sizeof(DWORD), CmpFunc);
    }

    rgSortInfo = new SSortInfo[ciAttrPresent];
    if (rgSortInfo == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // initialize the number of entries in the sort bucket array
    for (iIndex = 0; iIndex < ciAttrPresent; iIndex++)
    {
        rgSortInfo[iIndex].cEntries = 0;
        rgSortInfo[iIndex].cVertexEntries = 0;
    }

    // initialize the vertex array
    for (iVert = 0; iVert < m_cVertices; iVert++)
    {
        rgiVertRemap[iVert] = UNUSED;
    }

    // setup pattrPrev and iIndexPrev - NOTE: iIndexPrev is invalid if no NULL attribute bundles
    //   in the mesh
    attrPrev = NULL;
    for (iIndex = 0; iIndex < ciAttrPresent; iIndex++)
    {
        if (rgiAttrPresent[iIndex] == attrPrev)
            break;
    }
    iIndexPrev = iIndex;

    // first calculate how to remap the faces, and mark vertices
    //   as used
    for (iFaceIndex = 0; iFaceIndex < m_cFaces; iFaceIndex++)
    {
        iFace = rgiFaceRemapInverse[iFaceIndex];
        if (iFace == UNUSED)
            continue;

        attrCur = m_rgiAttributeIds[iFace];

        // if same as previous, then use its idnex, otherwise find it in the attr array
        if (attrCur == attrPrev)
        {
            iIndex = iIndexPrev;
        }
        else
        {
            for (iIndex = 0; iIndex < ciAttrPresent; iIndex++)
            {
                if (rgiAttrPresent[iIndex] == attrCur)
                    break;
            }
            GXASSERT(iIndex < ciAttrPresent);

            attrPrev = attrCur;
            iIndexPrev = iIndex;
        }

        // mark the vertices as in use
        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            iVert = m_pFaces[iFace].m_wIndices[iPoint];
            if (rgiVertRemap[iVert] == UNUSED)
            {
                rgiVertRemap[iVert] = iIndexPrev;
                rgSortInfo[iIndex].cVertexEntries += 1;
            }
            else  // already marked
            {
                // had better have been marked with the same attribute before
                GXASSERT(rgiVertRemap[iVert] == iIndexPrev);
            }
        }

        rgSortInfo[iIndex].cEntries += 1;
    }

    // allocate a new attribute table
    rgaeAttributeTableOld = m_rgaeAttributeTable;

    m_caeAttributeTable = ciAttrPresent;
    m_rgaeAttributeTable = new D3DXATTRIBUTERANGE[m_caeAttributeTable];
    if (m_rgaeAttributeTable == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // calculate the ranges for the various sort infos
    iTotal = 0;
    iVertexTotal = 0;
    for (iIndex = 0; iIndex < ciAttrPresent; iIndex++)
    {
        rgSortInfo[iIndex].iCurOffset = iTotal;

        m_rgaeAttributeTable[iIndex].AttribId = rgiAttrPresent[iIndex];
        m_rgaeAttributeTable[iIndex].FaceStart = iTotal;
        m_rgaeAttributeTable[iIndex].FaceCount = rgSortInfo[iIndex].cEntries;

        iTotal += rgSortInfo[iIndex].cEntries;

        rgSortInfo[iIndex].iCurVertexOffset = iVertexTotal;

        m_rgaeAttributeTable[iIndex].VertexStart = iVertexTotal;
        m_rgaeAttributeTable[iIndex].VertexCount = rgSortInfo[iIndex].cVertexEntries;

        iVertexTotal += rgSortInfo[iIndex].cVertexEntries;
    }

    // reinstate the vertex counts, if not collapseing unused verts
    if (bIgnoreVerts && (rgaeAttributeTableOld != NULL))
    {
        for (iIndex = 0; iIndex < ciAttrPresent; iIndex++)
        {
            m_rgaeAttributeTable[iIndex].VertexStart = rgaeAttributeTableOld[iIndex].VertexStart;
            m_rgaeAttributeTable[iIndex].VertexCount = rgaeAttributeTableOld[iIndex].VertexCount;
        }
    }

    // setup attrPrev and iIndexPrev - NOTE: iIndexPrev is invalid if no NULL attribute bundles
    //   in the mesh
    attrPrev = NULL;
    for (iIndex = 0; iIndex < ciAttrPresent; iIndex++)
    {
        if (rgiAttrPresent[iIndex] == attrPrev)
            break;
    }
    iIndexPrev = iIndex;

    // now actually remap the faces
    for (iFaceIndex = 0; iFaceIndex < m_cFaces; iFaceIndex++)
    {
        iFace = rgiFaceRemapInverse[iFaceIndex];
        if (iFace == UNUSED)
            continue;


        attrCur = m_rgiAttributeIds[iFace];

        // if same as previous, then use its idnex, otherwise find it in the attr array
        if (attrCur == attrPrev)
        {
            iIndex = iIndexPrev;
        }
        else
        {
            for (iIndex = 0; iIndex < ciAttrPresent; iIndex++)
            {
                if (rgiAttrPresent[iIndex] == attrCur)
                    break;
            }
            GXASSERT(iIndex < ciAttrPresent);

            attrPrev = attrCur;
            iIndexPrev = iIndex;
        }

        rgiFaceRemap[iFace] = rgSortInfo[iIndex].iCurOffset;
        rgSortInfo[iIndex].iCurOffset += 1;
    }

    // if not ignoring verts, then just remap then into place
    if (!bIgnoreVerts)   
    {
        // now actually remap the vertices
        for (iVert = 0; iVert < m_cVertices; iVert++)
        {
            if (rgiVertRemap[iVert] != UNUSED)
            {
                // reload the cached index value    
                iIndex = rgiVertRemap[iVert];

                rgiVertRemap[iVert] = rgSortInfo[iIndex].iCurVertexOffset;
                rgSortInfo[iIndex].iCurVertexOffset += 1;
            }
        }
    }
    else  // just leave the vertex mappings alone
    {
        for (iVert = 0; iVert < m_cVertices; iVert++)
        {
            // if not unused, need to set remap value and check to see if in proper attribute group
            if (rgiVertRemap[iVert] != UNUSED)  
            {
                // should be in the correct attribute range
                if ( ! ((m_rgaeAttributeTable[rgiVertRemap[iVert]].VertexStart <= iVert)
                    && (m_rgaeAttributeTable[rgiVertRemap[iVert]].VertexStart + m_rgaeAttributeTable[rgiVertRemap[iVert]].VertexCount > iVert)))
                {
                    char *szTemp;

                    szTemp = (char*)_alloca(256);
                    sprintf(szTemp, "ID3DXMeshOptimize: Cannot do AttributeSort because %d vertex needs to be moved and either IgnoreVerts or ShareVB is set\n", iVert);
                    DPF(0, szTemp);

                    hr = D3DXERR_CANNOTATTRSORT;
                    goto e_Exit;
                }

                rgiVertRemap[iVert] = iVert;
            }
        }
    }

#ifdef _DEBUG
    for (iIndex = 0; iIndex < ciAttrPresent; iIndex++)
    {
        GXASSERT(rgSortInfo[iIndex].iCurOffset == (rgSortInfo[iIndex].cEntries + m_rgaeAttributeTable[iIndex].FaceStart));
        GXASSERT(bIgnoreVerts || rgSortInfo[iIndex].iCurVertexOffset == (rgSortInfo[iIndex].cVertexEntries + m_rgaeAttributeTable[iIndex].VertexStart));
    }
#endif

    // reinitialize all inverse pointers to point to UNUSED
    for (iFaceIndex = 0; iFaceIndex < m_cFaces; iFaceIndex++)
    {
        rgiFaceRemapInverse[iFaceIndex] = UNUSED;
    }

    // fixup the RemapInverse array by rebuilding it
    for (iFaceIndex = 0; iFaceIndex < m_cFaces; iFaceIndex++)
    {
        iFace = rgiFaceRemap[iFaceIndex];

        if (iFace != UNUSED)
        {
            rgiFaceRemapInverse[iFace] = iFaceIndex;
        }
    }

    // mark as optimized
    m_dwOptionsInt |= D3DXMESHINT_ATTRIBUTETABLE;

e_Exit:
    delete []rgSortInfo;
    delete []rgiAttrPresent;
    delete []rgaeAttributeTableOld;

    // if failed, fall back to attribute bundle array
    if (FAILED(hr))
    {
        m_dwOptionsInt &= ~D3DXMESHINT_ATTRIBUTETABLE;
        delete []m_rgaeAttributeTable;
        m_rgaeAttributeTable = NULL;
    }

    return hr;
}

// -------------------------------------------------------------------------------
//  method    SplitSharedVertices
//
//  devnote
//              Duplicates vertices shared between attribute ids so that
//                  proper vertex bracketing can be achieved
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::SplitSharedVertices
    (
    DWORD **prgdwNewVertexOrigin,
    UINT *pcNewVertices
    )
{
    HRESULT hr = S_OK;
    UINT_IDX *rgiVertexSplitList = NULL;
    UINT_IDX *rgiNewVertexSplitList = NULL;
    DWORD *rgiVertexAttributeId = NULL;
    DWORD *rgiNewVertexAttributeId = NULL;
    UINT_IDX wCurNewWedge;
    UINT_IDX wNewWedge;
    UINT_IDX wNewWedgeIndex;
    UINT iVertex;
    UINT iFace;
    UINT cNewVertexAttribIds = 0;
    UINT cNewVertexAttribIdsMax = 0;
    UINT ciNewVertexSplitList = 0;
    UINT ciNewVertexSplitListMax = 0;
    DWORD attribFaceId;
    UINT_IDX *pwFace;
    UINT iPoint;
    BOOL bFound;
    UINT cNewVerticesMax = 0;
    DWORD cVerticesOrig;

    // remember the original count of vertices to convert new vertex indices
    //   into new vertex positions
    cVerticesOrig = m_cVertices;

    *prgdwNewVertexOrigin = NULL;
    *pcNewVertices = 0;

    // allocate vertex split tracking arrays
    rgiVertexSplitList = new UINT_IDX[m_cVertices];
    rgiVertexAttributeId = new DWORD[m_cVertices];
    if ((rgiVertexSplitList == NULL) || (rgiVertexAttributeId == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    memset(rgiVertexAttributeId, 0xff, sizeof(DWORD) * m_cVertices);

    // intialize all vertices to point to UNUSED
    //   kind of like a wedge list, but only linked if DUPLICATED vertices, and non cirular
    memset(rgiVertexSplitList, 0xff, sizeof(UINT_IDX) * m_cVertices);

    // goto each face, and make sure that each of the vertices is not
    //   shared across attribute boundarires
    for (iFace = 0; iFace < m_cFaces; iFace++)
    {
        pwFace = m_pFaces[iFace].m_wIndices;
        if (pwFace[0] == UNUSED)
            continue;

        attribFaceId = m_rgiAttributeIds[iFace];
        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            // if we find a vertex that doesn't match the current
            //         faces attrib ID, we might need to duplicate the vertex
            if (rgiVertexAttributeId[pwFace[iPoint]] != attribFaceId)
            {
                // the vertex might not have been marked yet
                //          if not market, just take ownership
                if (rgiVertexAttributeId[pwFace[iPoint]] == UNUSED32)
                {
                    rgiVertexAttributeId[pwFace[iPoint]] = attribFaceId;
                }
                else  // next need to see if a duplicate was already created
                {
                    wCurNewWedge = rgiVertexSplitList[pwFace[iPoint]];
                    bFound = FALSE;
                    while (wCurNewWedge != UNUSED)
                    {
                        if (rgiNewVertexAttributeId[wCurNewWedge] == attribFaceId)
                        {
                            bFound = TRUE;
                            break;
                        }

                        // go to next wedge in the vertex
                        wCurNewWedge = rgiNewVertexSplitList[wCurNewWedge];
                    } 

                    // if found, just remap to the correct wedge, else
                    if (bFound)
                    {
                        // convert from index into new list into new vertex position
                        pwFace[iPoint] = wCurNewWedge + (UINT_IDX)cVerticesOrig;
                    }
                    else // need to add a new wedge
                    {
                        // do a pre-allocate just to cut down on the number of reallocs
                        //   at the beginning
                        if (cNewVertexAttribIdsMax == 0)
                        {
                            ciNewVertexSplitListMax = 256;
                            cNewVertexAttribIdsMax = ciNewVertexSplitListMax;
                            cNewVerticesMax = ciNewVertexSplitListMax;

                            rgiNewVertexSplitList = new UINT_IDX[ciNewVertexSplitListMax];
                            rgiNewVertexAttributeId = new DWORD[cNewVertexAttribIdsMax];
                            *prgdwNewVertexOrigin = new DWORD[cNewVerticesMax];
                            if ((rgiNewVertexSplitList == NULL) 
                                        || (rgiNewVertexAttributeId == NULL)
                                        || (*prgdwNewVertexOrigin == NULL))
                            {
                                hr = E_OUTOFMEMORY;
                                goto e_Exit;
                            }
                        }

                        // save off the index of the new vertex
                        wNewWedge = m_cVertices;
                        m_cVertices += 1;

                        wNewWedgeIndex = *pcNewVertices;

                        // grow the attrib tracking arrays
                        if (!AddToDynamicArray(&rgiNewVertexAttributeId, attribFaceId,
                                                    &cNewVertexAttribIds, &cNewVertexAttribIdsMax))
                        {
                            hr = E_OUTOFMEMORY;
                            goto e_Exit;
                        }

                        if (!AddToDynamicArray(&rgiNewVertexSplitList, wNewWedge,
                                                    &ciNewVertexSplitList, &ciNewVertexSplitListMax))
                        {
                            hr = E_OUTOFMEMORY;
                            goto e_Exit;
                        }


                        if (!AddToDynamicArray(prgdwNewVertexOrigin, (DWORD)pwFace[iPoint],
                                                    pcNewVertices, &cNewVerticesMax))
                        {
                            hr = E_OUTOFMEMORY;
                            goto e_Exit;
                        }

                        // link the new vertex into the original vertex
                        rgiNewVertexSplitList[wNewWedgeIndex] = rgiVertexSplitList[pwFace[iPoint]];
                        rgiVertexSplitList[pwFace[iPoint]] = wNewWedgeIndex;

                        // update the current point to the new wedge just created
                        pwFace[iPoint] = wNewWedge;
                    }
                }

            }
        }
    }

e_Exit:
    delete []rgiVertexSplitList;
    delete []rgiNewVertexSplitList;
    delete []rgiVertexAttributeId;
    delete []rgiNewVertexAttributeId;
    return hr;
}

// -------------------------------------------------------------------------------
//  method    Optimize
//
//  devnote
//              Reorder the faces for performance reasons
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::OptimizeInplace
    (
    DWORD flags,
    CONST DWORD* adjacency,
    DWORD* optAdj,
    DWORD* faceRemap,
    LPD3DXBUFFER *ppbufVertexRemap
    )
{
    HRESULT hr = S_OK;

    hr = LockIB((PBYTE*)&m_pFaces);
    if (FAILED(hr))
        goto e_Exit;

    // if the VB is shared, we can't change it
    if (BSharedVB())
    {
        flags |= D3DXMESHOPT_IGNOREVERTS;
    }

    GXASSERT(CheckAdjacency(adjacency, m_cFaces));

    hr = OptimizeInternal(flags, adjacency, optAdj, faceRemap, ppbufVertexRemap, NULL, m_dwFVF);
    if (FAILED(hr))
        goto e_Exit;

    GXASSERT(CheckAdjacency(optAdj, m_cFaces));

e_Exit:
    if (m_pFaces != NULL)
    {
        UnlockIB();
        m_pFaces = NULL;
    }

    return hr;
}

// -------------------------------------------------------------------------------
//  method    Optimize
//
//  devnote
//              Reorder the faces for performance reasons
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::Optimize
    (
    DWORD flags,
    CONST DWORD* adjacency,
    DWORD* optAdj,
    DWORD* faceRemap,
    LPD3DXBUFFER *ppbufVertexRemap,
    LPD3DXMESH* ppOptMesh
    )
{
    HRESULT hr = S_OK;
    GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED> *ptmNewMesh = NULL;
    DWORD dwOptions;

    // only shared the VB, if flags specify it (even if this one is shared)
    if (flags & D3DXMESHOPT_SHAREVB)
        dwOptions = m_dwOptions | D3DXMESH_VB_SHARE;
    else
        dwOptions = m_dwOptions & ~D3DXMESH_VB_SHARE;

    ptmNewMesh = new GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>(m_pD3DDevice, m_dwFVF, dwOptions);
    if (ptmNewMesh == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    hr = ptmNewMesh->Resize(m_cFaces, ptmNewMesh->BSharedVB() ? m_cVertices : 0);
    if (FAILED(hr))
        goto e_Exit;

    // if marked as sharing, then link to the VB
    if (ptmNewMesh->BSharedVB())
    {
        m_dwOptionsInt |= D3DXMESHINT_SHAREDVB;

        // share the vertex buffer from this mesh with the other mesh
        ptmNewMesh->m_pVBVertices = m_pVBVertices;
        m_pVBVertices->AddRef();

        // if sharing VB, then better not reorder the verts
        flags |= D3DXMESHOPT_IGNOREVERTS;
    }

    hr = ptmNewMesh->CopyMesh(this);
    if (FAILED(hr))
        goto e_Exit;

    // set the m_cVertices field even though there is no vertex buffer.  This is
    //   used to inform various functions of the size the vertex buffer would be if materialized
    if (!ptmNewMesh->BSharedVB())
    {
        GXASSERT((ptmNewMesh->m_cVertices == 0) && (ptmNewMesh->m_cMaxVertices == 0));
        ptmNewMesh->m_cVertices = m_cVertices;
    }

    GXASSERT((ptmNewMesh->m_dwOptions &~(D3DXMESH_VB_SHARE))== (m_dwOptions &~(D3DXMESH_VB_SHARE)));

    hr = ptmNewMesh->LockIB((PBYTE*)&ptmNewMesh->m_pFaces);
    if (FAILED(hr))
        goto e_Exit;

    GXASSERT(CheckAdjacency(adjacency, m_cFaces));

    hr = ptmNewMesh->OptimizeInternal(flags, adjacency, optAdj, faceRemap, ppbufVertexRemap, m_pVBVertices, m_dwFVF);
    if (FAILED(hr))
        goto e_Exit;

    GXASSERT(CheckAdjacency(optAdj, ptmNewMesh->m_cFaces));

    if (ptmNewMesh->m_pFaces != NULL)
    {
        ptmNewMesh->UnlockIB();
        ptmNewMesh->m_pFaces = NULL;
    }

    *ppOptMesh = ptmNewMesh;
    ptmNewMesh = NULL;

e_Exit:
    if ((ptmNewMesh != NULL) && (ptmNewMesh->m_pFaces != NULL))
    {
        ptmNewMesh->UnlockIB();
    }

    GXRELEASE(ptmNewMesh);
    return hr;
}

// -------------------------------------------------------------------------------
//  method    Optimize2
//
//  devnote
//              Reorder the faces for performance reasons
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::Optimize2
    (
    DWORD options,
    DWORD flags,
    DWORD* adjacency,
    DWORD* optAdj,
    DWORD* faceRemap,
    LPD3DXBUFFER *ppbufVertexRemap,
    DWORD FVF,
    GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>** ppOptMesh
    )
{
    HRESULT hr = S_OK;
    GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED> *ptmNewMesh = NULL;

    ptmNewMesh = new GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>(m_pD3DDevice, FVF, options);
    if (ptmNewMesh == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // internal function not allowed to optimize to different bitness
    GXASSERT((m_dwOptions & D3DXMESH_32BIT) == (ptmNewMesh->m_dwOptions & D3DXMESH_32BIT));

    GXASSERT(options == ptmNewMesh->m_dwOptions);

    // this internal function does not support vb sharing, must use normal optimize call
    GXASSERT(!ptmNewMesh->BSharedVB());

    // just create the index buffer and other face data
    hr = ptmNewMesh->Resize(m_cFaces, 0);
    if (FAILED(hr))
        goto e_Exit;

    // copy the index data
    hr = ptmNewMesh->CopyMesh(this);
    if (FAILED(hr))
        goto e_Exit;

    // set the m_cVertices field even though there is no vertex buffer.  This is
    //   used to inform various functions of the size the vertex buffer would be if materialized
    GXASSERT((ptmNewMesh->m_cVertices == 0) && (ptmNewMesh->m_cMaxVertices == 0));
    ptmNewMesh->m_cVertices = m_cVertices;


    hr = ptmNewMesh->LockIB((PBYTE*)&ptmNewMesh->m_pFaces);
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmNewMesh->OptimizeInternal(flags, adjacency, optAdj, faceRemap, ppbufVertexRemap, m_pVBVertices, m_dwFVF);
    if (FAILED(hr))
        goto e_Exit;

    if (ptmNewMesh->m_pFaces != NULL)
    {
        ptmNewMesh->UnlockIB();
        ptmNewMesh->m_pFaces = NULL;
    }

    *ppOptMesh = ptmNewMesh;
    ptmNewMesh = NULL;

e_Exit:
    if ((ptmNewMesh != NULL) && (ptmNewMesh->m_pFaces != NULL))
    {
        ptmNewMesh->UnlockIB();
    }

    GXRELEASE(ptmNewMesh);
    return hr;
}

// -------------------------------------------------------------------------------
//  method    OptimizeInternal
//
//  devnote
//              Reorder the faces for performance reasons
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::OptimizeInternal
    (
    DWORD dwOptions,
    CONST DWORD *rgdwNeighbors,
    DWORD* rgdwNeighborsOut,
    DWORD* rgiFaceRemapInverseOut,
    LPD3DXBUFFER *ppbufVertexRemapInverseOut,
    LPDIRECT3DVERTEXBUFFER8 pOrigVertexBuffer,
    DWORD dwOrigFVF
    )
{
    HRESULT hr = S_OK;
    UINT *rgiFaceRemap = NULL;
    UINT *rgiFaceRemapInverse = NULL;
    UINT *rgiFaceRemapInverseBackup = NULL;
    UINT *rgiVertRemap = NULL;
    UINT *rgiVertRemapTemp = NULL;
    PBYTE pvPoints = NULL;
    UINT iFace;
    UINT iVert;
    UINT iVertexIndex;
    UINT iVertex;
    CONST DWORD *rgdwNeighborIn;
    DWORD *rgdwNeighborOut;
    UINT iPoint;
    UINT iFaceIndex;
    DWORD *rgdwNewVertexOrigins = NULL;
    UINT cNewVertices = 0;
    DWORD iNewVertexMin = 0;
    LPD3DXBUFFER pbufVertexRemapInverseOut = NULL;
    DWORD *rgdwVertexRemapInverse = NULL;
    DWORD cVerticesOrig;
    PBYTE pbCurVertex;
    PBYTE pbOldVertex;
    PBYTE pbVerticesOld = NULL;
    DWORD cVerticesTemp;

    LPDIRECT3DVERTEXBUFFER8 pOldVertexBuffer = NULL;
    UINT_IDX *rgwOldPointReps = NULL;

    GXASSERT(dwOrigFVF != 0);

    if (dwOptions == 0)
        goto e_Exit;

    if ((dwOptions & ~D3DXMESHOPT_VALIDBITS) != 0)
    {
        DPF(0, "ID3DXMesh::Optimize: Invalid optimization flags");
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    if ((rgdwNeighbors != NULL) && (rgdwNeighbors == rgdwNeighborsOut))
    {
        DPF(0, "ID3DXMesh::Optimize: In/Out Adjacency buffers must be different (non inplace operation)");
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    // UNDONE - should every optimization go through compact to remove UNUSED faces?
    dwOptions |= D3DXMESHOPT_COMPACT;

    if (dwOptions & (D3DXMESHOPT_STRIPREORDER | D3DXMESHOPT_VERTEXCACHE))
        dwOptions |= D3DXMESHOPT_ATTRSORT;

    // if doing attribute sorting, make sure that all vertices
    //   belong to one attribute and one attribute only
    if (dwOptions & D3DXMESHOPT_ATTRSORT)
    {
        iNewVertexMin = m_cVertices;

        hr = SplitSharedVertices(&rgdwNewVertexOrigins, &cNewVertices);
        if (FAILED(hr))
            goto e_Exit;

        if ((cNewVertices > 0) && (dwOptions & D3DXMESHOPT_IGNOREVERTS))
        {
            DPF(0, "Shared vertices during an attribute sort without optimizing vertices is not possible");
            hr = D3DXERR_CANNOTATTRSORT;
            goto e_Exit;
        }

    }

    rgiFaceRemap = new UINT[m_cFaces];
    rgiFaceRemapInverse = new UINT[m_cFaces];
    rgiVertRemap = new UINT[m_cVertices];
    if ((rgiFaceRemap == NULL)
        || (rgiFaceRemapInverse == NULL)
        || (rgiVertRemap == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    cVerticesOrig = m_cVertices;

    // if we need to do two ReorderData's, we need to remember the FaceRemapInverse array
    if (dwOptions & (D3DXMESHOPT_STRIPREORDER | D3DXMESHOPT_VERTEXCACHE))
    {
        rgiFaceRemapInverseBackup = new UINT[m_cFaces];
        if (rgiFaceRemapInverseBackup == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
    }

    // first see if a compact operation is desired.  Compact will initialize the arrays
    if (dwOptions & D3DXMESHOPT_COMPACT)
    {
        hr = Compact(rgiFaceRemap, rgiFaceRemapInverse, rgiVertRemap);
        if (FAILED(hr))
            goto e_Exit;
    }
    else  // if no compact, then initialize the remap arrays
    {
        for (iFace=0; iFace < m_cFaces; iFace++)
        {
            rgiFaceRemapInverse[iFace] = iFace;
            rgiFaceRemap[iFace] = iFace;
        }

        for (iVert=0; iVert < m_cVertices; iVert++)
        {
            rgiVertRemap[iVert] = iVert;
        }
    }

    // if attribute sorting desired, then sort them based on attributes
    if (dwOptions & D3DXMESHOPT_ATTRSORT)
    {
        hr = AttributeSort(rgiFaceRemap, rgiFaceRemapInverse, rgiVertRemap, dwOptions & D3DXMESHOPT_IGNOREVERTS);
        if (FAILED(hr))
            goto e_Exit;
    }

    // if not touching vertices, then just set remapping to identity
    if (dwOptions & D3DXMESHOPT_IGNOREVERTS)
    {
        for (iVert = 0; iVert < m_cVertices; iVert++)
        {
            rgiVertRemap[iVert] = iVert;
        }
    }

    // now actually reorder the mesh and the data
    hr = ReorderFaceData(rgiFaceRemap, rgiVertRemap);
    if (FAILED(hr))
        goto e_Exit;


    // if strip based reordering desired, then sort them based on strips
    if (dwOptions & (D3DXMESHOPT_STRIPREORDER | D3DXMESHOPT_VERTEXCACHE))
    {
        // make a backup of the remap inverse array
        memcpy(rgiFaceRemapInverseBackup, rgiFaceRemapInverse, sizeof(UINT) * m_cFaces);

        // even though it takes the remap arrays, it actually reinitializes them,
        //   only cares about rgiFaceRemapInverse, so that it can interpret the
        //   provided adjacency information
        hr = StripReorder(dwOptions, rgiFaceRemap, rgiFaceRemapInverse, rgdwNeighbors);
        if (FAILED(hr))
            goto e_Exit;


        // need an intermediate vert remap to represent the vertex changes between the
        //   two ReorderFaceData's... while the vertices haven't moved yet, the faces
        //   have been updated to where the vertices would have moved, so the remap
        //   array for ReorderFaceData needs to reflect that, but the one for ReorderVertexDAta
        //   needs to be one that encompasses both sets of vertex changes
        rgiVertRemapTemp = new UINT[m_cVertices];
        if (rgiVertRemapTemp == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // reorder vertices into usage order if reordering vertices allowed
        if ( !(dwOptions & D3DXMESHOPT_IGNOREVERTS) )
        {
            // NOTE: this ignores previous vertex remappings
            hr = VertexOptimize(rgiFaceRemapInverse, rgiVertRemapTemp);
            if (FAILED(hr))
                goto e_Exit;

            // now merge the new remap into the original
            for (iVert = 0; iVert < m_cVertices; iVert++)
            {
                if (rgiVertRemap[iVert] != UNUSED)
                {
                    rgiVertRemap[iVert] = rgiVertRemapTemp[rgiVertRemap[iVert]];
                }
            }

        }
        else
        {
            for (iVert = 0; iVert < m_cVertices; iVert++)
            {
                rgiVertRemapTemp[iVert] = iVert;
            }
        }

        // now actually reorder the mesh and the data
        hr = ReorderFaceData(rgiFaceRemap, rgiVertRemapTemp);
        if (FAILED(hr))
            goto e_Exit;

        // composite both levels of remapping into one for output
        for (iFace = 0; iFace < m_cFaces; iFace++)
        {
            if (rgiFaceRemapInverse[iFace] != UNUSED32)
                rgiFaceRemapInverse[iFace] = rgiFaceRemapInverseBackup[rgiFaceRemapInverse[iFace]];
            else
                rgiFaceRemapInverse[iFace] = UNUSED32;
        }

        // reinitialize all inverse pointers to point to UNUSED
        for (iFace = 0; iFace < m_cFaces; iFace++)
        {
            rgiFaceRemap[iFace] = UNUSED;
        }

        // fixup the Remap array by rebuilding it
        for (iFace = 0; iFace < m_cFaces; iFace++)
        {
            iFaceIndex = rgiFaceRemapInverse[iFace];

            if (iFaceIndex != UNUSED)
            {
                rgiFaceRemap[iFaceIndex] = iFace;
            }
        }
    }

    // if vertex remap desired, generate it taking split vertices into account
    // NOTE: must happen before strip reordering (since it re-initializes the vert remap array)
    //    even though it DOES NOT affect the vertex reorder
    if ((ppbufVertexRemapInverseOut != NULL) || (cNewVertices > 0) || (m_cMaxVertices == 0))
    {
        hr = D3DXCreateBuffer(sizeof(DWORD) * m_cVertices, &pbufVertexRemapInverseOut);
        if (FAILED(hr))
            goto e_Exit;

        rgdwVertexRemapInverse = (DWORD*)pbufVertexRemapInverseOut->GetBufferPointer();

        // reinitialize all inverse pointers to point to UNUSED
        for (iVertexIndex = 0; iVertexIndex < m_cVertices; iVertexIndex++)
        {
            rgdwVertexRemapInverse[iVertexIndex] = UNUSED32;
        }

        // fixup the RemapInverse array by rebuilding it
        for (iVertexIndex = 0; iVertexIndex < cVerticesOrig; iVertexIndex++)
        {
            iVertex = rgiVertRemap[iVertexIndex];

            if (iVertex != UNUSED)
            {
                GXASSERT(iVertex < m_cVertices);
                rgdwVertexRemapInverse[iVertex] = iVertexIndex;
            }
        }

        // if new vertices were added.  change position from to be from their origin
        //   instead of where they were when inserted
        if (cNewVertices > 0)
        {
            for (iVertexIndex = 0; iVertexIndex < m_cVertices; iVertexIndex++)
            {
                // if one of the new vertices
                if ((rgdwVertexRemapInverse[iVertexIndex] != UNUSED32) 
                                    && (rgdwVertexRemapInverse[iVertexIndex] >= iNewVertexMin))
                {
                    GXASSERT(iNewVertexMin > rgdwNewVertexOrigins[rgdwVertexRemapInverse[iVertexIndex] - iNewVertexMin]);

                    // then lookup the origin of the vertex and store that instead
                    rgdwVertexRemapInverse[iVertexIndex] =
                        rgdwNewVertexOrigins[rgdwVertexRemapInverse[iVertexIndex] - iNewVertexMin];
                }
            }

        }
    }

    // if no new vertices, then rgiVertRemap is a permutation that can be done inplace
    //     if no vertex buffer (m_cMaxVertices == 0), then need to create one, so goto else
    if ((cNewVertices == 0) && (m_cMaxVertices > 0))
    {
        hr = LockVB(&pvPoints);
        if ( FAILED(hr) )
            goto e_Exit;

        // now that the rgiVertRemap is fully prepped, reorder the vertices
        hr = ReorderVertexData(rgiVertRemap, pvPoints);
        if (FAILED(hr))
            goto e_Exit;
    }
    else  // create a new vertex buffer and use the inverse vertex map to build the new one out of place
    {
        GXASSERT((cNewVertices > 0) || (m_cMaxVertices == 0));
        GXASSERT(rgdwVertexRemapInverse != NULL);

        // remember the original vertex data
        cVerticesTemp = m_cVertices;
        pOldVertexBuffer = m_pVBVertices;
        rgwOldPointReps = m_rgwPointReps;

        if (m_cMaxVertices == 0)
        {
            pOldVertexBuffer = pOrigVertexBuffer;
            pOrigVertexBuffer->AddRef();
        }

        // remove the vertex data so that it is forced to create a new vertex buffer
        m_pVBVertices = NULL;
        m_cVertices = 0; 
        m_cMaxVertices = 0;
        m_rgwPointReps = NULL;

        // create a new vertex buffer
        hr = Resize(m_cFaces, cVerticesTemp);
        if (FAILED(hr))
            goto e_Exit;

        hr = LockVB(&pvPoints);
        if (FAILED(hr))
            goto e_Exit;

        hr = pOldVertexBuffer->Lock(0, 0, &pbVerticesOld, D3DLOCK_READONLY);
        if (FAILED(hr))
            goto e_Exit;
        
        // if the same FVF, then do memcopy, else use routine
        if (m_dwFVF == dwOrigFVF)
        {
            // fill the new vertex buffer using the old one and the inverse remapping (same as would be output)
            pbCurVertex = pvPoints;
            for (iVertex = 0; iVertex < m_cVertices; iVertex++)
            {
                pbOldVertex = pbVerticesOld + rgdwVertexRemapInverse[iVertex] * m_cBytesPerVertex;

                memcpy(pbCurVertex, pbOldVertex, m_cBytesPerVertex);

                pbCurVertex += m_cBytesPerVertex;
            }        
        }
        else  // not the same FVF, use vertex copy context
        {
            SVertexCopyContext vcc(dwOrigFVF, m_dwFVF);
            GXASSERT(vcc.cfvfDest.m_cBytesPerVertex == m_cBytesPerVertex);

            // fill the new vertex buffer using the old one and the inverse remapping (same as would be output)
            pbCurVertex = pvPoints;
            for (iVertex = 0; iVertex < m_cVertices; iVertex++)
            {
                pbOldVertex = pbVerticesOld + rgdwVertexRemapInverse[iVertex] * vcc.cfvfSrc.m_cBytesPerVertex;

                vcc.CopyVertex(pbOldVertex, pbCurVertex);

                pbCurVertex += m_cBytesPerVertex;
            }        
        }

        // if point reps, rebuild the new point rep array
        if (rgwOldPointReps != NULL)
        {
            GXASSERT(m_rgwPointReps != NULL);

            for (iVertex = 0; iVertex < m_cVertices; iVertex++)
            {
                GXASSERT(rgdwVertexRemapInverse[iVertex] < cVerticesOrig);
                GXASSERT(rgwOldPointReps[rgdwVertexRemapInverse[iVertex]] < m_cVertices);
                m_rgwPointReps[iVertex] = rgiVertRemap[rgwOldPointReps[rgdwVertexRemapInverse[iVertex]]];
            }        
        }

        cVerticesTemp = 0;
        for (iVertex = 0; iVertex < m_cVertices; iVertex++)
        {
            if (rgiVertRemap[iVertex] != UNUSED)
            {
                cVerticesTemp += 1;
            }
            else
            {
                rgiVertRemap[iVertex] = UINT32UNUSED;
            }
        }

        m_cVertices = cVerticesTemp;
    }

    // if face remap info desired, copy it
    if (rgiFaceRemapInverseOut != NULL)
    {
        GXASSERT(sizeof(UINT) == sizeof(DWORD));
        memcpy(rgiFaceRemapInverseOut, rgiFaceRemapInverse, sizeof(DWORD) * m_cFaces);
    }

    // if updated adjacency desired, generate it
    if (rgdwNeighborsOut != NULL)
    {
        for (iFace = 0; iFace < m_cFaces; iFace++)
        {
            rgdwNeighborIn = &rgdwNeighbors[rgiFaceRemapInverse[iFace] * 3];
            rgdwNeighborOut = &rgdwNeighborsOut[iFace * 3];

            for (iPoint = 0; iPoint < 3; iPoint++)
            {
                if (rgdwNeighborIn[iPoint] != UNUSED32)
                    rgdwNeighborOut[iPoint] = rgiFaceRemap[rgdwNeighborIn[iPoint]];
                else
                    rgdwNeighborOut[iPoint] = UNUSED32;

                GXASSERT((rgdwNeighborOut[iPoint] == UNUSED32) ||
                        (rgdwNeighborOut[iPoint] < m_cFaces));
            }
        }
    }

    if (ppbufVertexRemapInverseOut != NULL)
    {
        *ppbufVertexRemapInverseOut = pbufVertexRemapInverseOut;
        pbufVertexRemapInverseOut = NULL;
    }

    GXASSERT(BValid());
e_Exit:
    delete []rgiVertRemap;
    delete []rgiFaceRemap;
    delete []rgiFaceRemapInverse;
    delete []rgiFaceRemapInverseBackup;
    delete []rgdwNewVertexOrigins;
    delete []rgiVertRemapTemp;

    if (m_rgwPointReps != rgwOldPointReps)
    {
        delete []rgwOldPointReps;
    }

    if (pvPoints != NULL)
    {
        HRESULT hrFail = UnlockVB( );
        GXASSERT (!FAILED(hrFail));
    }
    if (pbVerticesOld != NULL)
    {
        GXASSERT(pOldVertexBuffer != NULL);
        HRESULT hrFail = pOldVertexBuffer->Unlock();
        GXASSERT (!FAILED(hrFail));
    }

    GXRELEASE(pbufVertexRemapInverseOut);
    GXRELEASE(pOldVertexBuffer);

    return hr;
}

// -------------------------------------------------------------------------------
//  method    Reorder
//
//  devnote
//              Reorders the faces using the provided remap
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::ReorderFaceData
    (
    UINT *rgiFaceRemap,
    UINT *rgiVertRemap
    )
{
    HRESULT hr = S_OK;
    BYTE *rgbTempBuffer = NULL;
    BYTE *rgbFaceMoved;
    BYTE *rgbVertexMoved;
    UINT iVertSrc;
    UINT iVertDest;
    UINT iCurHeadOffset;
    UINT iCurVertex;
    UINT iVert;
    UINT iCurFace;
    UINT iFaceSrc;
    UINT iFaceDest;
    UINT cVertsNew;
    UINT cFacesNew;
    UINT iFace;
    UINT iVertIndex;
    UINT_IDX wOldRep;

    rgbTempBuffer = new BYTE[max(m_cVertices, m_cFaces)];
    if (rgbTempBuffer == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // start with moving faces
    rgbFaceMoved = rgbTempBuffer;
    memset(rgbFaceMoved, 0, sizeof(BYTE) * m_cFaces);

    // make sure that all faces are moved that need to be
    for(iCurHeadOffset = 0; iCurHeadOffset < m_cFaces; iCurHeadOffset++)
    {
        if (rgbFaceMoved[iCurHeadOffset])
            continue;

        iCurFace = rgiFaceRemap[iCurHeadOffset];

        if (iCurFace == UNUSED)
            continue;

        // while there is a cycle
        while (iCurFace != iCurHeadOffset)
        {
            iFaceSrc = iCurHeadOffset;
            iFaceDest = iCurFace;

            SwapFace(iFaceSrc, iFaceDest);
            RemapFace(iFaceDest, rgiVertRemap, rgiFaceRemap);

            // mark the vertex as moved
            rgbFaceMoved[iCurFace] = true;

            // move to next item in the cycle
            iCurFace = rgiFaceRemap[iCurFace];

            if ((iCurFace == UNUSED) || (rgbFaceMoved[iCurFace]))
                 goto NextFaceCycle;
        }

        // just need to remap the pointers in the vertex
        RemapFace(iCurHeadOffset, rgiVertRemap, rgiFaceRemap);

NextFaceCycle:;
    }

    cFacesNew = 0;
    for (iFace = 0; iFace < m_cFaces; iFace++)
    {
        if (rgiFaceRemap[iFace] != UNUSED)
        {
            cFacesNew += 1;
        }
        else
        {
            rgiFaceRemap[iFace] = UINT32UNUSED;
        }
    }

    m_cFaces = cFacesNew;


e_Exit:
    delete []rgbTempBuffer;

    return hr;
}

// -------------------------------------------------------------------------------
//  method    Reorder
//
//  devnote
//              Reorders the faces using the provided remap
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::ReorderVertexData
    (
    UINT *rgiVertRemap,
    PBYTE pvPoints
    )
{
    HRESULT hr = S_OK;
    BYTE *rgbTempBuffer = NULL;
    BYTE *rgbFaceMoved;
    BYTE *rgbVertexMoved;
    UINT iVertSrc;
    UINT iVertDest;
    UINT iCurHeadOffset;
    UINT iCurVertex;
    UINT iVert;
    UINT iCurFace;
    UINT iFaceSrc;
    UINT iFaceDest;
    UINT cVertsNew;
    UINT cFacesNew;
    UINT iFace;
    UINT iVertIndex;
    UINT_IDX wOldRep;

#ifdef _DEBUG
    UINT *rgbTest = NULL;
#endif

    rgbTempBuffer = new BYTE[max(m_cVertices, m_cFaces)];
    if (rgbTempBuffer == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    if (BHasPointRepData())
    {
        // before doing any swapping/remapping, make sure and handle any point
        //   representatives that are being removed
        for (iVert = 0; iVert < m_cVertices; iVert++)
        {
            if (rgiVertRemap[iVert] != UNUSED)
            {
                // if the representative vertex has been removed, then scan
                //      the rest of the vertex buffer looking for other vertices
                //      using the same representative, and update them to point to
                //      the unremapped index for this vertex.  i.e. make this vertex the rep
                if (rgiVertRemap[m_rgwPointReps[iVert]] == UNUSED)
                {
                    wOldRep = m_rgwPointReps[iVert];

                    // become the new vertex rep
                    m_rgwPointReps[iVert] = iVert;

                    // force others that pointed to the old vertex rep to point to this one
                    for (iVertIndex = iVert+1; iVertIndex < m_cVertices; iVertIndex++)
                    {
                        if (m_rgwPointReps[iVertIndex] == wOldRep)
                        {
                            m_rgwPointReps[iVertIndex] = iVert;
                        }
                    }
                }

            }
        }
    }

#ifdef _DEBUG
    UINT wTemp;

    rgbTest = new UINT[m_cVertices];
    if (rgbTest == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    for (iCurHeadOffset = 0; iCurHeadOffset < m_cVertices; iCurHeadOffset++)
    {
        rgbTest[iCurHeadOffset] = iCurHeadOffset;
    }
#endif

    // now do vertices
    rgbVertexMoved = rgbTempBuffer;
    memset(rgbVertexMoved, 0, sizeof(BYTE) * m_cVertices);

    // make sure to catch all cycles, check to make sure that each vertex is moved
    for(iCurHeadOffset = 0; iCurHeadOffset < m_cVertices; iCurHeadOffset++)
    {
        if (rgbVertexMoved[iCurHeadOffset])
            continue;

        iCurVertex = rgiVertRemap[iCurHeadOffset];

        if (iCurVertex == UNUSED)
            continue;

        // while there is a cycle
        while (iCurVertex != iCurHeadOffset)
        {
            iVertSrc = iCurHeadOffset;
            iVertDest = iCurVertex;

            SwapVertex(iVertSrc, iVertDest, (PBYTE)pvPoints);
            RemapVertex(iVertDest, rgiVertRemap);

#ifdef _DEBUG
            wTemp = rgbTest[iVertDest];
            rgbTest[iVertDest] = rgbTest[iVertSrc];
            rgbTest[iVertSrc] = wTemp;
#endif

            // mark the vertex as moved
            rgbVertexMoved[iCurVertex] = true;

            // move to next item in the cycle
            iCurVertex = rgiVertRemap[iCurVertex];

            if ((iCurVertex == UNUSED) || (rgbVertexMoved[iCurVertex]))
                 goto NextVertCycle;
        }

        // just need to remap the pointers in the vertex
        RemapVertex(iCurHeadOffset, rgiVertRemap);

NextVertCycle:;
    }

#ifdef _DEBUG
    for (iCurHeadOffset = 0; iCurHeadOffset < m_cVertices; iCurHeadOffset++)
    {
        if (rgiVertRemap[iCurHeadOffset] != UNUSED)
        {
            GXASSERT(rgbTest[rgiVertRemap[iCurHeadOffset]] == iCurHeadOffset);
        }
    }
#endif

    cVertsNew = 0;
    for (iVert = 0; iVert < m_cVertices; iVert++)
    {
        if (rgiVertRemap[iVert] != UNUSED)
        {
            cVertsNew += 1;
        }
        else
        {
            rgiVertRemap[iVert] = UINT32UNUSED;
        }
    }

    m_cVertices = cVertsNew;


e_Exit:
#ifdef _DEBUG
    delete []rgbTest;
#endif
    delete []rgbTempBuffer;

    return hr;
}


// -------------------------------------------------------------------------------
//  method    Reorder
//
//  devnote
//              Reorders the faces using the provided remap
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::Reorder
    (
    UINT *rgiVertRemap,
    UINT ciVertRemapUser,
    UINT *rgiFaceRemap,
    UINT ciFaceRemapUser
    )
{
    HRESULT hr = S_OK;
    PBYTE pvPoints = NULL;
    UINT iFace;
    UINT iVert;
    ULONGLONG iSum;
    ULONGLONG iTotal;
    ULONGLONG iMaxIndex;

    // first make sure that we have the correct arrays
    if ((rgiVertRemap == NULL) || (ciVertRemapUser < m_cVertices)
            || (rgiFaceRemap == NULL) || (ciFaceRemapUser < m_cFaces))
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    // now verify that every triangle gets remapped to a position
    //   with no duplicates

    // calculate the sum of the vertex remap indices
    iSum = 0;
    for (iVert = 0; iVert < m_cVertices; iVert++)
    {
        if (rgiVertRemap[iVert] == UNUSED)
        {
            hr = D3DERR_INVALIDCALL;
            goto e_Exit;
        }

        iSum += rgiVertRemap[iVert];
    }

    iMaxIndex = (ULONGLONG)m_cVertices - 1;
    // calculate the total that we should have assuming an array of 0->m_cVertices-1
    //          sum(1->n) = (n^2 + n) / 2
    iTotal =  iMaxIndex * iMaxIndex + iMaxIndex;
    iTotal /= 2;

    if (iSum != iTotal)
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    // calculate the sum of the face remap indices
    iSum = 0;
    for (iFace = 0; iFace < m_cFaces; iFace++)
    {
        if (rgiFaceRemap[iFace] == UNUSED)
        {
            hr = D3DERR_INVALIDCALL;
            goto e_Exit;
        }

        iSum += rgiFaceRemap[iFace];
    }

    iMaxIndex = (ULONGLONG)m_cFaces - 1;
    // calculate the total that we should have assuming an array of 0->m_cFaces-1
    //          sum(1->n) = (n^2 + n) / 2
    iTotal =  iMaxIndex * iMaxIndex + iMaxIndex;
    iTotal /= 2;

    if (iSum != iTotal)
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    hr = LockVB(&pvPoints);
    if ( FAILED(hr) )
        goto e_Exit;


    hr = ReorderFaceData(rgiFaceRemap, rgiVertRemap);
    if (FAILED(hr))
        goto e_Exit;

    hr = ReorderVertexData(rgiVertRemap, pvPoints);
    if (FAILED(hr))
        goto e_Exit;

    GXASSERT(BValid());
e_Exit:
    if (pvPoints != NULL)
    {
        HRESULT hrFail = UnlockVB( );
        GXASSERT (!FAILED(hr));
    }

    return hr;
}

// -------------------------------------------------------------------------------
//  method    SwapFace
//
//  devnote
//            moves a face from one position to another, and remaps all data in the
//              face (moved to iFaceDest) according to the face and vertex maps passed in
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::SwapFace
    (
    UINT iFaceSrc,
    UINT iFaceDest
    )
{
    UINT_IDX *pwFaceSrc;
    UINT_IDX *pwFaceDest;
    UINT_IDX *pwNeighborsSrc;
    UINT_IDX *pwNeighborsDest;
    UINT_IDX iPoint;
    UINT_IDX wTemp;
    UINT16 iIndexTemp;
    DWORD attrTemp;

    pwFaceSrc = m_pFaces[iFaceSrc].m_wIndices;
    pwFaceDest = m_pFaces[iFaceDest].m_wIndices;
    pwNeighborsSrc = m_rgpniNeighbors[iFaceSrc].m_iNeighbors;
    pwNeighborsDest = m_rgpniNeighbors[iFaceDest].m_iNeighbors;

    for (iPoint = 0; iPoint < 3; iPoint++)
    {
        // swap the current point
        wTemp = pwFaceDest[iPoint];
        pwFaceDest[iPoint] = pwFaceSrc[iPoint];
        pwFaceSrc[iPoint] = wTemp;

        // if neighbors present, then swap them
        if (BHasNeighborData())
        {
            wTemp = pwNeighborsDest[iPoint];
            pwNeighborsDest[iPoint] = pwNeighborsSrc[iPoint];
            pwNeighborsSrc[iPoint] = wTemp;
        }
    }

    // move material data
    if (BHasPerFaceAttributeId())
    {
        attrTemp = m_rgiAttributeIds[iFaceDest];
        m_rgiAttributeIds[iFaceDest] = m_rgiAttributeIds[iFaceSrc];
        m_rgiAttributeIds[iFaceSrc] = attrTemp;
    }

    if (BHasPerFaceAttributeIndex())
    {
        iIndexTemp = m_rgiMaterialIndex[iFaceDest];
        m_rgiMaterialIndex[iFaceDest] = m_rgiMaterialIndex[iFaceSrc];
        m_rgiMaterialIndex[iFaceSrc] = iIndexTemp;
    }
}

// -------------------------------------------------------------------------------
//  method    RemapFace
//
//  devnote
//            and remaps all data in the face according to the face and vertex maps passed in
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::RemapFace
    (
    UINT iFace,
    UINT *rgiVertRemap,
    UINT *rgiFaceRemap
    )
{
    UINT_IDX *pwFace;
    UINT_IDX *pwNeighbors;
    UINT_IDX iPoint;


    pwFace = m_pFaces[iFace].m_wIndices;
    pwNeighbors = m_rgpniNeighbors[iFace].m_iNeighbors;

    if (pwFace[0] != UNUSED)
    {
        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            // remap each point in the process of copying
            pwFace[iPoint] = rgiVertRemap[pwFace[iPoint]];
            GXASSERT(pwFace[iPoint] != UNUSED);

            if (BHasNeighborData())
            {
                // remap the neighbors in the process of copying
                if (pwNeighbors[iPoint] != UNUSED)
                {
                    pwNeighbors[iPoint] = rgiFaceRemap[pwNeighbors[iPoint]];
                }
                else
                {
                    pwNeighbors[iPoint] = UNUSED;
                }

            }
        }
    }
}
// -------------------------------------------------------------------------------
//  method    SwapVertex
//
//  devnote
//            moves a vertex from one position to another, and remaps all data in the
//              vertex moved to iVertDest according to the vertex map passed in
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::SwapVertex
    (
    UINT iVertSrc,
    UINT iVertDest,
    PBYTE pvPoints
    )
{
    PBYTE pvSrc;
    PBYTE pvDest;
    PBYTE pvTemp = (PBYTE)_alloca(m_cBytesPerVertex);
    UINT_IDX wTempRep;

    // only move the FVFs if the index changes
    GXASSERT(iVertSrc != iVertDest);

    pvSrc = pvPoints + m_cBytesPerVertex * iVertSrc;
    pvDest = pvPoints + m_cBytesPerVertex * iVertDest;

    memcpy(pvTemp, pvDest, m_cBytesPerVertex);
    memcpy(pvDest, pvSrc, m_cBytesPerVertex);
    memcpy(pvSrc, pvTemp, m_cBytesPerVertex);

    // move the point representatives
    if (BHasPointRepData())
    {
        wTempRep = m_rgwPointReps[iVertDest];
        m_rgwPointReps[iVertDest] = m_rgwPointReps[iVertSrc];
        m_rgwPointReps[iVertSrc] = wTempRep;
    }
}

// -------------------------------------------------------------------------------
//  method    RemapVertex
//
//  devnote
//            Remaps all the data in the vertex.  The vertex either hasn't moved in a reorder
//              or it was swapped and now needs its data updated
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::RemapVertex
    (
    UINT iVert,
    UINT *rgiVertRemap
    )
{
    if (BHasPointRepData())
    {
        m_rgwPointReps[iVert] = rgiVertRemap[m_rgwPointReps[iVert]];
        GXASSERT(m_rgwPointReps[iVert] != UNUSED);
    }
}


template <class UINT_IDX, unsigned int UNUSED>
class CIndexQueue
{
public:
    CIndexQueue(UINT cMaxElements)
        :m_rgiQueue(NULL), m_cElementsMax(cMaxElements) {}

    ~CIndexQueue()
        {
            delete []m_rgiQueue;
        }

    HRESULT Init()
    {
        UINT iElement;

        if (m_rgiQueue == NULL)
        {
            m_rgiQueue = new UINT_IDX[m_cElementsMax];
            if (m_rgiQueue == NULL)
                return E_OUTOFMEMORY;
        }

        for (iElement = 0; iElement < m_cElementsMax; iElement++)
        {
            m_rgiQueue[iElement] = UNUSED;
        }

        m_cElements = 0;
        m_iNextElement = 0;
        m_iHead = 0;

        return S_OK;
    }

    HRESULT Copy(CIndexQueue<UINT_IDX,UNUSED> &iqSrc)
    {
        GXASSERT(m_cElementsMax == iqSrc.m_cElementsMax);

        if (m_rgiQueue == NULL)
        {
            m_rgiQueue = new UINT_IDX[m_cElementsMax];
            if (m_rgiQueue == NULL)
                return E_OUTOFMEMORY;
        }

        memcpy(m_rgiQueue, iqSrc.m_rgiQueue, sizeof(UINT_IDX) * m_cElementsMax);

        m_cElements = iqSrc.m_cElements;
        m_iNextElement = iqSrc.m_iNextElement;
        m_iHead = iqSrc.m_iHead;

        return S_OK;
    }

    bool BElementPresent(UINT_IDX iElementTest)
    {
        UINT iElement;

        for (iElement = 0; iElement < m_cElementsMax; iElement++)
        {
            if (m_rgiQueue[iElement] == iElementTest)
                return true;
        }

        return false;
    }

    void Push(UINT_IDX iElementNew)
    {
        InsertElement(iElementNew);
    }

    void InsertElement(UINT_IDX iElementNew)
    {
        m_rgiQueue[m_iNextElement] = iElementNew;

        m_cElements += 1;

        m_iNextElement = m_iNextElement + 1;
        if (m_iNextElement == m_cElementsMax)
            m_iNextElement = 0;
    }

    bool BEmpty() { return m_cElements == 0; }

    UINT_IDX IPop()
    {
        UINT_IDX iRet;

        GXASSERT(!BEmpty());
        iRet = m_rgiQueue[m_iHead];

        m_cElements -= 1 ;

        m_iHead = m_iHead + 1;
        if (m_iHead == m_cElementsMax)
            m_iHead = 0;

        return iRet;
    }

    UINT_IDX IFront()
    {
        GXASSERT(!BEmpty());
        return m_rgiQueue[m_iHead];
    }

private:
    UINT m_iHead;
    UINT m_iNextElement;
    UINT m_cElements;
    UINT m_cElementsMax;

    UINT_IDX *m_rgiQueue;
};


const int x_cDefaultVertexCacheSize = 12;

template <class UINT_IDX, unsigned int UNUSED>
class CSimVertexCache
{
public:
    CSimVertexCache(UINT cVertexCacheSize): m_iqCache(cVertexCacheSize) {}

    HRESULT Clear()
    {
        return m_iqCache.Init();
    }

    HRESULT Copy(CSimVertexCache<UINT_IDX,UNUSED> &svcSrc)
    {
        return m_iqCache.Copy(svcSrc.m_iqCache);
    }

    // mark as using the vertex, if in the cache set bHit to true, else
    //   just add to the cache
    void AccessVertex(UINT_IDX iVertex, bool &bHit)
    {
        GXASSERT(iVertex != UNUSED);

        bHit = m_iqCache.BElementPresent(iVertex);
        if (!bHit)
        {
            m_iqCache.InsertElement(iVertex);
        }
    }

private:
    CIndexQueue<UINT_IDX,UNUSED> m_iqCache;
};


template <class UINT_IDX>
struct SMSListElem
{
        BYTE     m_bProcessed;
        BYTE     m_cUnprocessedNeighbors;
        UINT_IDX m_iPrevElem;
        UINT_IDX m_iNextElem;
};

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
class CMeshStatus
{
        friend struct SMSListElem;
public:
    CMeshStatus():m_ptmTriMesh(NULL), m_rgiListElems(NULL), m_rgiProcessedSim(NULL) {}
    CMeshStatus(GXTri3Mesh<UINT_IDX, b16BitIndex, UNUSED> *ptmTriMesh, NeighborInfo<UINT_IDX> *rgpniNeighbors, UINT_IDX cMaxElements);
    ~CMeshStatus();

        HRESULT Init(UINT_IDX iFaceOffset, UINT_IDX cFaces, bool bSimulationsRequired);

    void MarkAsProcessed(UINT_IDX iFace);
    BYTE BProcessed(UINT_IDX  iFace) const
                {
                        GXASSERT((iFace >= m_iFaceOffset) && (iFace < (m_iFaceOffset + m_cFaces)));
                        return m_rgiListElems[iFace - m_iFaceOffset].m_bProcessed;
                }

    UINT_IDX CUnprocessedNeighbors(UINT_IDX iFace) const
                {
                        GXASSERT((iFace >= m_iFaceOffset) && (iFace < (m_iFaceOffset + m_cFaces)));
                        return m_rgiListElems[iFace - m_iFaceOffset].m_cUnprocessedNeighbors;
                }

    UINT_IDX IFindInitialFace()
        {
            UINT_IDX iUnprocessed;

            for (iUnprocessed = 0; iUnprocessed < 4; iUnprocessed++)
            {
                if (m_rgiUnprocessed[iUnprocessed] != UNUSED)
                    return m_rgiUnprocessed[iUnprocessed] + m_iFaceOffset;
            }

            return UNUSED;
        }

    UINT_IDX IFindNextFace(UINT_IDX iFace);

    UINT_IDX *PwGetNeighbors(UINT_IDX iFace) { return m_rgpniNeighbors[iFace].m_iNeighbors; }

// simulation member functions
    UINT BProcessedSim(UINT_IDX iFace)
        {
        GXASSERT(m_rgiProcessedSim != NULL);
                GXASSERT((iFace >= m_iFaceOffset) && (iFace < (m_iFaceOffset + m_cFaces)));
        return (m_rgiProcessedSim[iFace - m_iFaceOffset] >= m_iCurSimLevel);
        }

    void MarkAsProcessedSim(UINT_IDX iFace)
        {
        GXASSERT(m_rgiProcessedSim != NULL);
        GXASSERT((iFace >= m_iFaceOffset) && (iFace < (m_iFaceOffset + m_cFaces)));
        m_rgiProcessedSim[iFace - m_iFaceOffset] = m_iCurSimLevel;
        }

    void NextSimulationLevel()
        {
        m_iCurSimLevel += 1;
        }

 private:
    void RemoveElement(UINT_IDX iFaceIndex);
    void PrependElement(UINT_IDX iFaceIndex);
    void DecrementCount(UINT_IDX iFaceIndex);

        GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED> *m_ptmTriMesh;
        NeighborInfo<UINT_IDX> *m_rgpniNeighbors;

        UINT_IDX        m_rgiUnprocessed[4];

        SMSListElem<UINT_IDX> *m_rgiListElems;

        // mesh status can work on a subset of the mesh, i.e. separate based on attribute bundles
        UINT_IDX                m_iFaceOffset;
        UINT_IDX                m_cFaces;

        UINT_IDX                m_cFacesMax;

    UINT            m_iCurSimLevel;
    UINT            *m_rgiProcessedSim;
};

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
CMeshStatus<UINT_IDX,b16BitIndex,UNUSED>::CMeshStatus
                                        (GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED> *ptmTriMesh,
                                        NeighborInfo<UINT_IDX> *rgpniNeighbors,
                                        UINT_IDX cMaxElements)
   :m_ptmTriMesh(ptmTriMesh),
        m_rgpniNeighbors(rgpniNeighbors),
        m_rgiListElems(NULL),
        m_cFacesMax(cMaxElements),
    m_rgiProcessedSim(NULL)
{
}


template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
CMeshStatus<UINT_IDX,b16BitIndex,UNUSED>::~CMeshStatus()
{
        delete []m_rgiListElems;
    delete []m_rgiProcessedSim;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
CMeshStatus<UINT_IDX,b16BitIndex,UNUSED>::Init
    (
    UINT_IDX iFaceOffset,
    UINT_IDX cFaces,
    bool bSimulationsRequired
    )
{
        HRESULT hr = S_OK;
    UINT_IDX *pwNeighbors;
    UINT_IDX iFaceIndex;
    UINT_IDX iFace;
    UINT_IDX iNeighbor;
    BYTE cUnprocessedNeighbors;

        if (m_rgiListElems == NULL)
        {
                m_rgiListElems = new SMSListElem<UINT_IDX>[m_cFacesMax];

                if (m_rgiListElems == NULL)
                {
                        hr = E_OUTOFMEMORY;
                        goto e_Exit;
                }

        if (bSimulationsRequired)
        {
            m_rgiProcessedSim = new UINT[m_cFacesMax];
            if (m_rgiProcessedSim == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
        }
        }

        // initialize subset
        m_iFaceOffset = iFaceOffset;
        m_cFaces = cFaces;

        GXASSERT(m_cFaces <= m_cFacesMax);
        GXASSERT(m_rgiListElems != NULL);

        // initialize lists of unprocessed faces
        m_rgiUnprocessed[0] = UNUSED;
        m_rgiUnprocessed[1] = UNUSED;
        m_rgiUnprocessed[2] = UNUSED;
        m_rgiUnprocessed[3] = UNUSED;

        for (iFace = m_iFaceOffset; iFace < m_iFaceOffset + cFaces; iFace++)
        {
                pwNeighbors = m_rgpniNeighbors[iFace].m_iNeighbors;

                cUnprocessedNeighbors = 0;

                for (iNeighbor = 0; iNeighbor < 3; iNeighbor++)
                {
                        if (pwNeighbors[iNeighbor] != UNUSED)
                        {
                                cUnprocessedNeighbors += 1;

                                // make certain that there are only links within this subset
                                GXASSERT((pwNeighbors[iNeighbor] >= iFaceOffset)
                                        && (pwNeighbors[iNeighbor] < m_iFaceOffset + cFaces));
                        }
                }

                GXASSERT((cUnprocessedNeighbors >= 0) && (cUnprocessedNeighbors <= 3));

                // calculate where is its list element in the array
                iFaceIndex = iFace - iFaceOffset;

                // initialize other members of the list element
                m_rgiListElems[iFaceIndex].m_bProcessed = false;
                m_rgiListElems[iFaceIndex].m_cUnprocessedNeighbors = cUnprocessedNeighbors;

                // insert at head of list
        PrependElement(iFaceIndex);
        }

    if (m_rgiProcessedSim != NULL)
    {
        memset(m_rgiProcessedSim, 0, sizeof(UINT) * cFaces);
        m_iCurSimLevel = 0;
    }

e_Exit:
        if (FAILED(hr))
        {
                delete []m_rgiListElems;
        }

        return hr;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
CMeshStatus<UINT_IDX,b16BitIndex,UNUSED>::RemoveElement
    (
    UINT_IDX iFaceIndex
    )
{
    UINT_IDX iPrevElem;
    UINT_IDX iNextElem;
    UINT_IDX cUnprocessedNeighbors;

    GXASSERT(iFaceIndex < m_cFaces);

    // if the previous element is not unused, then it is in the middle or end of the list
    if (m_rgiListElems[iFaceIndex].m_iPrevElem != UNUSED)
    {
        // had better not be at the head of the list with an non null prev elem
        GXASSERT(m_rgiUnprocessed[m_rgiListElems[iFaceIndex].m_cUnprocessedNeighbors] != iFaceIndex);

        iPrevElem = m_rgiListElems[iFaceIndex].m_iPrevElem;
        iNextElem = m_rgiListElems[iFaceIndex].m_iNextElem;

        // link the previous element around this element (known to exist)
        m_rgiListElems[iPrevElem].m_iNextElem = iNextElem;

        // link the next element around this element if it exists
        if (iNextElem != UNUSED)
        {
            m_rgiListElems[iNextElem].m_iPrevElem = iPrevElem;
        }
    }
    else // if prev is UNUSED, then it is at the head of the list
    {
        GXASSERT(m_rgiUnprocessed[m_rgiListElems[iFaceIndex].m_cUnprocessedNeighbors] == iFaceIndex);

        cUnprocessedNeighbors = m_rgiListElems[iFaceIndex].m_cUnprocessedNeighbors;

        m_rgiUnprocessed[cUnprocessedNeighbors] = m_rgiListElems[iFaceIndex].m_iNextElem;

        if (m_rgiUnprocessed[cUnprocessedNeighbors] != UNUSED)
            m_rgiListElems[m_rgiUnprocessed[cUnprocessedNeighbors]].m_iPrevElem = UNUSED;
    }

    m_rgiListElems[iFaceIndex].m_iNextElem = UNUSED;
    m_rgiListElems[iFaceIndex].m_iPrevElem = UNUSED;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
CMeshStatus<UINT_IDX,b16BitIndex,UNUSED>::PrependElement
    (
    UINT_IDX iFaceIndex
    )
{
    UINT_IDX iHead;
    BYTE cUnprocessedNeighbors;

    GXASSERT(iFaceIndex < m_cFaces);

    cUnprocessedNeighbors = m_rgiListElems[iFaceIndex].m_cUnprocessedNeighbors;

    //prepend to the list based on number of unprocessed neighbors
        iHead = m_rgiUnprocessed[cUnprocessedNeighbors];
        m_rgiListElems[iFaceIndex].m_iNextElem = iHead;

    if (iHead != UNUSED)
            m_rgiListElems[iHead].m_iPrevElem = iFaceIndex;

    // reset head pointer
        m_rgiUnprocessed[cUnprocessedNeighbors] = iFaceIndex;

    m_rgiListElems[iFaceIndex].m_iPrevElem = UNUSED;
}


template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
CMeshStatus<UINT_IDX,b16BitIndex,UNUSED>::DecrementCount
    (
    UINT_IDX iFace
    )
{
    UINT_IDX iFaceIndex = iFace - m_iFaceOffset;

    GXASSERT(!BProcessed(iFace));
        GXASSERT((iFace >= m_iFaceOffset) && (iFace < (m_iFaceOffset + m_cFaces)));
    GXASSERT((m_rgiListElems[iFaceIndex].m_cUnprocessedNeighbors >= 1) &&
                    (m_rgiListElems[iFaceIndex].m_cUnprocessedNeighbors <= 3));

    // remove from current list
    RemoveElement(iFaceIndex);

    m_rgiListElems[iFaceIndex].m_cUnprocessedNeighbors -= 1;

    // add to new list
    PrependElement(iFaceIndex);
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
CMeshStatus<UINT_IDX,b16BitIndex,UNUSED>::MarkAsProcessed
    (
    UINT_IDX iFace
    )
{
    UINT_IDX iFaceIndex = iFace - m_iFaceOffset;
    UINT_IDX *pwNeighbors;
    UINT_IDX iNeighbor;

        GXASSERT((iFace >= m_iFaceOffset) && (iFace < (m_iFaceOffset + m_cFaces)));

    // mark as processed
    GXASSERT(m_rgiListElems[iFaceIndex].m_bProcessed == false);
    m_rgiListElems[iFaceIndex].m_bProcessed = true;

    // the face should not be in anly lists once processed
    RemoveElement(iFaceIndex);

    pwNeighbors = m_rgpniNeighbors[iFace].m_iNeighbors;
    for (iNeighbor = 0; iNeighbor < 3; iNeighbor++)
    {
        // if there is a neighbor, and it hasn't been processed already, then decrement
        //   its unprocessed neighbor count
        if ((pwNeighbors[iNeighbor] != UNUSED) && (!BProcessed(pwNeighbors[iNeighbor])))
        {
            DecrementCount(pwNeighbors[iNeighbor]);
        }
    }

    // if simulations required, mark off processed faces in both the simulation array
    //   and in the standard location
    if (m_rgiProcessedSim != NULL)
    {
        m_rgiProcessedSim[iFaceIndex] = UINT_MAX;
    }
}

// -------------------------------------------------------------------------------
//  method    CalculateMissRate
//
//  devnote
//            Reorder the faces to be in strip order
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::CalculateMissRate
    (
    UINT *rgiFaceRemapInverse,
    PUINT pcMisses
    )
{
    HRESULT hr = S_OK;
    CSimVertexCache<UINT_IDX, UNUSED> svc(x_cDefaultVertexCacheSize);
    UINT_IDX iFace;
    UINT_IDX iFaceIndex;
    UINT_IDX iPoint;
    UINT_IDX *pwFace;
    UINT cMisses;
    bool bHit;

    hr = svc.Clear();
    if (FAILED(hr))
        goto e_Exit;

    cMisses = 0;
    for (iFaceIndex = 0; iFaceIndex < m_cFaces; iFaceIndex++)
    {
        iFace = rgiFaceRemapInverse[iFaceIndex];
        if (iFace == UNUSED)
            continue;

        pwFace = m_pFaces[iFace].m_wIndices;
        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            svc.AccessVertex(pwFace[iPoint], bHit);
            cMisses += !bHit;
        }
    }

    *pcMisses = cMisses;
e_Exit:

    return hr;
}


// -------------------------------------------------------------------------------
//  method    StripReorder
//
//  devnote
//            Reorder the faces to be in strip order
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::StripReorder
    (
    DWORD dwOptions,
    UINT *rgiFaceRemap,
    UINT *rgiFaceRemapInverse,
    CONST DWORD *rgdwNeighbors
    )
{
    HRESULT hr = S_OK;
    NeighborInfo<UINT_IDX> *rgpniPhysicalNeighbors;
    DWORD iFace;
    UINT iNeighbor;
    UINT cFacesSubset;
    UINT iae;
    UINT iFaceOffset;
    UINT iFaceMax;

    UINT iPoint1;
    UINT iPoint2;
    UINT iPointNeighbor1;
    UINT iPointNeighbor2;

    UINT_IDX wNeighbor;
    CONST DWORD *pdwNeighbors;
    UINT_IDX *pwDest;

    UINT iNeighborEdgeBack;

#ifdef ICECAP
    StartProfile(PROFILE_THREADLEVEL, PROFILE_CURRENTID);
#endif

    CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> msMeshStatus;

    // must attribute sorted before doing strip reordering
    GXASSERT((m_caeAttributeTable != 0) && (m_rgaeAttributeTable != NULL));

    // need neighbor infor to perform these optimizations
    if (rgdwNeighbors == NULL) // !BHasNeighborData() || !BHasPointRepData())
    {
        DPF(0, "The operation needs adjacency information");
        return D3DERR_INVALIDCALL;
    }

    // outer code should only call this function if strip reordering required
    GXASSERT((dwOptions & D3DXMESHOPT_STRIPREORDER) || (dwOptions & D3DXMESHOPT_VERTEXCACHE));

    // make sure that only one type of reordering is specified
    if ((dwOptions & D3DXMESHOPT_STRIPREORDER) && (dwOptions & D3DXMESHOPT_VERTEXCACHE))
    {
        DPF(0, "Both D3DXMESHOPT_STRIPREORDER and D3DXMESHOPT_VERTEXCACHE can't be specified together. "
               "Choose one of them");
        return D3DERR_INVALIDCALL;
    }

    // find the attribute bundle with the most faces, needed for creating the
    //    mesh status structure
    cFacesSubset = 0;
    for (iae = 0; iae < m_caeAttributeTable; iae++)
    {
        if (cFacesSubset < m_rgaeAttributeTable[iae].FaceCount)
        {
            cFacesSubset = m_rgaeAttributeTable[iae].FaceCount;
        }
    }

    // reordering has a different idea of neighbors.  The two differences are that
    //   the wedges must be IDENTICAL (not equivalent), and that a neighbor must
    //   be in the same attribute group
    rgpniPhysicalNeighbors = new NeighborInfo<UINT_IDX>[m_cFaces];
    if (rgpniPhysicalNeighbors == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // build the desired adjacency information
    for (iae = 0; iae < m_caeAttributeTable; iae++)
    {
        iFaceOffset = m_rgaeAttributeTable[iae].FaceStart;
        iFaceMax = iFaceOffset + m_rgaeAttributeTable[iae].FaceCount;

        for (iFace = iFaceOffset; iFace < iFaceMax; iFace++)
        {
            pdwNeighbors = &rgdwNeighbors[rgiFaceRemapInverse[iFace] * 3];
            pwDest = rgpniPhysicalNeighbors[iFace].m_iNeighbors;

            for (iNeighbor = 0; iNeighbor < 3; iNeighbor++)
            {
                if (pdwNeighbors[iNeighbor] != UNUSED32)
                {
                    wNeighbor = (UINT_IDX)pdwNeighbors[iNeighbor];
                    wNeighbor = rgiFaceRemap[wNeighbor];

                    // if the link is outside the attribute bundle group, then drop it
                    //      or if the neighbor is referenced more than once in this triangle
                    //          ignore all links to that neighbor 
                    //  (BTW, must check original value of neighbor, or check remapped values)
                    if ((wNeighbor < iFaceOffset) || (wNeighbor >= iFaceMax) 
                        || (pdwNeighbors[iNeighbor] == pdwNeighbors[(iNeighbor + 1) % 3])
                        || (pdwNeighbors[iNeighbor] == pdwNeighbors[(iNeighbor + 2) % 3]))
                    {
                        wNeighbor = UNUSED;
                    }
                    else
                    {
                        // find the edge that points to this triangle in the neighbor
                        iNeighborEdgeBack = FindEdge(&rgdwNeighbors[rgiFaceRemapInverse[wNeighbor] * 3], (DWORD)rgiFaceRemapInverse[iFace]);
                        GXASSERT(iNeighborEdgeBack < 3);

                        iPoint1 = m_pFaces[iFace].m_wIndices[iNeighbor];
                        iPoint2 = m_pFaces[iFace].m_wIndices[(iNeighbor + 1) % 3];

                        // get the two points on that neighbor's edge
                        iPointNeighbor1 = m_pFaces[wNeighbor].m_wIndices[iNeighborEdgeBack];
                        iPointNeighbor2 = m_pFaces[wNeighbor].m_wIndices[(iNeighborEdgeBack + 1) % 3];

                        // if the wedges are not identical on the shared edge, then drop
                        //   the link
                        if ((iPoint1 != iPointNeighbor2) || (iPoint2 != iPointNeighbor1))
                        {
                            wNeighbor = UNUSED;
                        }
                    }

                    pwDest[iNeighbor] = wNeighbor;
                }
                else
                {
                    pwDest[iNeighbor] = UNUSED;
                }
            }
        }
    }

    for (iFace=0; iFace < m_cFaces; iFace++)
    {
        rgiFaceRemap[iFace] = iFace;
        rgiFaceRemapInverse[iFace] = iFace;
    }

#ifdef _DEBUG
    UINT cMissesBefore;
    hr = CalculateMissRate(rgiFaceRemapInverse, &cMissesBefore);
    if (FAILED(hr))
        goto e_Exit;

    GXTRACE("Miss rate before optimization: %f\n", (float)cMissesBefore / (float)m_cVertices);
#endif

    // construct the mesh status structure
    msMeshStatus = CMeshStatus<UINT_IDX,b16BitIndex,UNUSED>(this, rgpniPhysicalNeighbors, cFacesSubset);

    if (dwOptions & D3DXMESHOPT_STRIPREORDER)
    {
        // now that the mesh status structure is constructed, do the reordering per attribute group
        hr = SGIStripReorder(&msMeshStatus, rgiFaceRemap, rgiFaceRemapInverse);
        if (FAILED(hr))
            goto e_Exit;
    }
    else
    {
        // now that the mesh status structure is constructed, do the reordering per attribute group
        hr = VCacheStripReorder(&msMeshStatus, rgiFaceRemap, rgiFaceRemapInverse);
        if (FAILED(hr))
            goto e_Exit;

        // now that the mesh status structure is constructed, do the reordering per attribute group
        //hr = OldVCacheStripReorder(&msMeshStatus, rgiFaceRemap, rgiFaceRemapInverse);
        //if (FAILED(hr))
            //goto e_Exit;
    }

#ifdef _DEBUG
    UINT cMissesAfter;
    hr = CalculateMissRate(rgiFaceRemapInverse, &cMissesAfter);
    if (FAILED(hr))
        goto e_Exit;

    GXTRACE("Miss rate after optimization: %f\n", (float)cMissesAfter / (float)m_cVertices);
#endif

e_Exit:
    delete []rgpniPhysicalNeighbors;

#ifdef ICECAP
    StopProfile(PROFILE_THREADLEVEL, PROFILE_CURRENTID);
#endif

    return hr;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
UINT_IDX
CMeshStatus<UINT_IDX,b16BitIndex,UNUSED>::IFindNextFace
    (
    UINT_IDX iFace
    )
{
    UINT_IDX iRet=3;
    UINT_IDX cMinUnprocessedNeighbors = UNUSED;
    UINT_IDX cMinNextUnprocessedNeighbors;
    UINT_IDX *pwNeighbors;
    UINT_IDX *pwNeighborsTemp;
    UINT_IDX cUnprocessedNeighbors;
    UINT_IDX cNextUnprocessedNeighbors;
    UINT_IDX cMinTemp;
    UINT_IDX wNeighborTemp;
    UINT_IDX wNeighbor;
    UINT_IDX iNeighbor;
    UINT_IDX iNeighborTemp;

    pwNeighbors = PwGetNeighbors(iFace);

    for (iNeighbor = 0; iNeighbor < 3; iNeighbor++)
    {
        wNeighbor = pwNeighbors[iNeighbor];

        // if not present, or already processed, then skip
        if ((wNeighbor == UNUSED) || (BProcessed(wNeighbor)))
            continue;

        cUnprocessedNeighbors = CUnprocessedNeighbors(wNeighbor);
        GXASSERT(cUnprocessedNeighbors<3);

        cMinTemp = UNUSED;
        pwNeighborsTemp = PwGetNeighbors(wNeighbor);

        for (iNeighborTemp = 0; iNeighborTemp < 3; iNeighborTemp++)
        {
            wNeighborTemp = pwNeighborsTemp[iNeighborTemp];

            if ((wNeighborTemp == UNUSED) || (BProcessed(wNeighborTemp)))
                continue;

            cNextUnprocessedNeighbors = CUnprocessedNeighbors(wNeighborTemp);
            if (cNextUnprocessedNeighbors < cMinTemp)
                cMinTemp = cNextUnprocessedNeighbors;
        }

        if (cMinTemp == UNUSED)
            cMinTemp = 0; // the best

        // if fewer unprocessed neighbors, then save off its info
        if (cUnprocessedNeighbors < cMinUnprocessedNeighbors)
        {
            iRet = iNeighbor;
            cMinUnprocessedNeighbors = cUnprocessedNeighbors;
            cMinNextUnprocessedNeighbors = cMinTemp;
        }
        // if same number of unprocessed neighbors, but has fewer neighbors
        //   that are unprocessed, then save it
        else if ((cUnprocessedNeighbors == cMinUnprocessedNeighbors)
            && (cMinTemp < cMinNextUnprocessedNeighbors))
        {
            iRet=iNeighbor;
            cMinNextUnprocessedNeighbors = cMinTemp;
        }
    }

    return iRet;
}

// -------------------------------------------------------------------------------
//  method    SGIStripReorder
//
//  devnote
//            Reorder the faces to be in strip order
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::SGIStripReorder
    (
    CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus,
    UINT *rgiFaceRemap,
    UINT *rgiFaceRemapInverse
    )
{
    HRESULT hr = S_OK;
    UINT iae;
    UINT_IDX iFace;
    UINT_IDX *pwNeighbors;
    UINT_IDX iNext;
    UINT cFacesSubset;
    UINT iFaceOffset;
    UINT cFacesCur;
    UINT iFaceIndex;

    for (iae = 0; iae < m_caeAttributeTable; iae++)
    {
        cFacesSubset = m_rgaeAttributeTable[iae].FaceCount;
        iFaceOffset = m_rgaeAttributeTable[iae].FaceStart;
        cFacesCur = 0;

        hr = pmsMeshStatus->Init(iFaceOffset, cFacesSubset, false /* no simulations */);
        if (FAILED(hr))
            goto e_Exit;

        while (1)
        {
            // find the first
            iFace = pmsMeshStatus->IFindInitialFace();
            if (iFace == UNUSED)
                break;

            pmsMeshStatus->MarkAsProcessed(iFace);

            // find where to jump to next
            iNext = pmsMeshStatus->IFindNextFace(iFace);

            while (1)
            {
                // add the current face to be next in the reording
                rgiFaceRemap[iFace] = iFaceOffset + cFacesCur;
                cFacesCur += 1;

                // if at end of strip, break out and try another strip
                if (iNext == 3)
                    break;

                // move to the next face
                pwNeighbors = pmsMeshStatus->PwGetNeighbors(iFace);
                iFace = pwNeighbors[iNext];
                pmsMeshStatus->MarkAsProcessed(iFace);

                // find where to jump to next
                iNext = pmsMeshStatus->IFindNextFace(iFace);
            }
        }

        GXASSERT(cFacesCur == cFacesSubset);
    }

    // reinitialize all inverse pointers to point to UNUSED
    for (iFaceIndex = 0; iFaceIndex < m_cFaces; iFaceIndex++)
    {
        rgiFaceRemapInverse[iFaceIndex] = UNUSED;
    }

    // fixup the RemapInverse array by rebuilding it
    for (iFaceIndex = 0; iFaceIndex < m_cFaces; iFaceIndex++)
    {
        iFace = rgiFaceRemap[iFaceIndex];

        if (iFace != UNUSED)
        {
            rgiFaceRemapInverse[iFace] = iFaceIndex;
        }
    }

e_Exit:
    return hr;
}

const BYTE x_iNeighbor0Processed = 0x1;
const BYTE x_iNeighbor1Processed = 0x2;
const BYTE x_iNeighbor2Processed = 0x4;

const BYTE x_rgbNextUnprocessed[8] =
    {
    0,           //  0 0 0   // all unprocessed, just pick one
    1,           //  1 0 0   // neighbor 0 processed, pick the ccw
    2,           //  0 1 0   // neighbor 1 processed, pick the ccw
    2,           //  1 1 0   // pick the unprocessed one
    0,           //  0 0 1   // neighbor 2 processed, pick the ccw
    1,           //  1 0 1   // pick the unprocessed one
    0,           //  0 1 1   // pick the unprocessed one
    3            //  1 1 1   // none unprocessed, so return nowhere
    };

const BYTE x_rgbNextOtherUnprocessed[8] =
    {
    1,           //  0 0 0   // all unprocessed, just pick one
    2,           //  1 0 0   // neighbor 0 processed, pick the ccw
    0,           //  0 1 0   // neighbor 1 processed, pick the ccw
    3,           //  1 1 0   // pick the unprocessed one
    1,           //  0 0 1   // neighbor 2 processed, pick the ccw
    3,           //  1 0 1   // pick the unprocessed one
    3,           //  0 1 1   // pick the unprocessed one
    3            //  1 1 1   // none unprocessed, so return nowhere
    };

// find a new face to restart the strip at
//   first checks the queue and then the mesh status structure
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
UINT_IDX
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::IVCRestartStrip
    (
    CIndexQueue<UINT_IDX,UNUSED> &iqPossibleRestarts,
    CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus
    )
{
    bool bFound;
    HRESULT hr;
    UINT_IDX iFace;

    // check the queue for a new place to restart at
    bFound = false;
    while (!iqPossibleRestarts.BEmpty())
    {
        // pop the first entry off the queue
        iFace = iqPossibleRestarts.IPop();

        // if it hasn't been processed start there
        if (!pmsMeshStatus->BProcessed(iFace))
        {
            bFound = true;
            break;
        }
    }

    // clear the queue
    hr = iqPossibleRestarts.Init();
    GXASSERT(!FAILED(hr));  // no memory allocations can occur here

    // if no entries found in the queue, then fall back to the mesh status structure
    if (!bFound)
    {
        iFace = pmsMeshStatus->IFindInitialFace();
    }

    return iFace;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::VCSimulate
    (
    UINT cFacesToContinue,
    UINT_IDX iFace,
    UINT cFacesInStripOrig,
    CIndexQueue<UINT_IDX,UNUSED> &iqPossibleRestartsOrig,
    CIndexQueue<UINT_IDX,UNUSED> &iqPossibleRestarts,
    CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus,
    CSimVertexCache<UINT_IDX,UNUSED> &svcOrig,
    CSimVertexCache<UINT_IDX,UNUSED> &svc,
    UINT cMaxFacesPerStrip,
    float &fEstimate
    )
{
    UINT_IDX cFacesInStrip;
    UINT_IDX cFacesVisited;
    UINT_IDX cFacesBeforeEvent;
    UINT_IDX iFaceNext;
    UINT_IDX iPoint;
    UINT_IDX *pwFace;
    HRESULT hr = S_OK;
    bool bHit;
    UINT cHitsTotal;
    UINT_IDX *pwNeighbors;
    BYTE bProcessedNeighbors;
    UINT_IDX iNextEdge;
    UINT_IDX iOtherEdge;

    // move to next simulation level for processed checks
    pmsMeshStatus->NextSimulationLevel();

    // make copies of other data structures that are going to be modified
    hr = iqPossibleRestarts.Copy(iqPossibleRestartsOrig);
    if (FAILED(hr))
        goto e_Exit;

    hr = svc.Copy(svcOrig);
    if (FAILED(hr))
        goto e_Exit;

    cFacesBeforeEvent = cFacesToContinue;
    cFacesVisited = 0;
    cFacesInStrip = cFacesInStripOrig;
    cHitsTotal = 0;

    GXASSERT(!pmsMeshStatus->BProcessed(iFace));
    iFaceNext = iFace;
    while (1)
    {
        if (cFacesVisited == cFacesBeforeEvent)
        {
            if (cFacesVisited == cMaxFacesPerStrip)
                break;

            cFacesBeforeEvent = cFacesVisited < cFacesToContinue ? cFacesToContinue : cMaxFacesPerStrip;

            iFaceNext = UNUSED;
            while (1)
            {
                if (iqPossibleRestarts.BEmpty())
                {
                    iFaceNext = UNUSED;
                    break;
                }

                iFaceNext = iqPossibleRestarts.IPop();

                if (!pmsMeshStatus->BProcessedSim(iFaceNext))
                    break;
            }

            if (iFaceNext == UNUSED)
                break;

            iqPossibleRestarts.Init();
            cFacesInStrip=0;
        }

        cFacesVisited++;
        pmsMeshStatus->MarkAsProcessedSim(iFaceNext);

        // find out the total number of hits for the selected face
        pwFace = m_pFaces[iFaceNext].m_wIndices;
        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            svc.AccessVertex(pwFace[iPoint], bHit);

            cHitsTotal += bHit;
        }


        bProcessedNeighbors = 0;
        pwNeighbors = pmsMeshStatus->PwGetNeighbors(iFaceNext);

        // first find out which neighbors haven't been processed
        if ((pwNeighbors[0] == UNUSED) || (pmsMeshStatus->BProcessedSim(pwNeighbors[0])))
            bProcessedNeighbors |= x_iNeighbor0Processed;

        if ((pwNeighbors[1] == UNUSED) || (pmsMeshStatus->BProcessedSim(pwNeighbors[1])))
            bProcessedNeighbors |= x_iNeighbor1Processed;

        if ((pwNeighbors[2] == UNUSED) || (pmsMeshStatus->BProcessedSim(pwNeighbors[2])))
            bProcessedNeighbors |= x_iNeighbor2Processed;

        // then use the bits to look up the first ccw one
        iNextEdge = x_rgbNextUnprocessed[bProcessedNeighbors];
        GXASSERT(iNextEdge <= 3);

        iOtherEdge = x_rgbNextOtherUnprocessed[bProcessedNeighbors];
        GXASSERT(iNextEdge <= 3);


        if (iNextEdge < 3)
        {
            iFaceNext = pwNeighbors[iNextEdge];

            if (iOtherEdge < 3)
            {
                iqPossibleRestarts.Push(pwNeighbors[iOtherEdge]);
            }
        }
        else // nowhere to jump to, restart the strip
        {
            cFacesBeforeEvent = cFacesVisited;
        }
    }


    fEstimate = (cFacesVisited == 0) ? 0.f : (float)cHitsTotal / (float)cFacesVisited;

e_Exit:
    return hr;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
UINT_IDX
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::IVCFindNextFace
    (
    UINT_IDX iFace,
    UINT_IDX &cFacesToContinueBest,
    UINT_IDX &cFacesInStrip,
    CIndexQueue<UINT_IDX,UNUSED> &iqPossibleRestarts,
    CIndexQueue<UINT_IDX,UNUSED> &iqTemp,
    CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus,
    CSimVertexCache<UINT_IDX,UNUSED> &svc,
    CSimVertexCache<UINT_IDX,UNUSED> &svcTemp
    )
{
    UINT_IDX iRet=3;
    HRESULT hr = S_OK;
    UINT_IDX cMaxFacesInStrip;
    float fBestEstimate;
    float fEstimate;
    float fRestartEstimate;
    bool bRestart;
    UINT_IDX cFacesToContinue;
    UINT_IDX cFacesToContinueBestOrig;
    UINT_IDX *pwNeighbors;
    BYTE bProcessedNeighbors;
    UINT_IDX iNextEdge;
    UINT_IDX iOtherEdge;
    UINT_IDX iFaceNext;
    bool bHit;
    UINT iPoint;
    UINT_IDX *pwFace;

    // figure out which is the next face from iFace
    {
        bProcessedNeighbors = 0;
        pwNeighbors = pmsMeshStatus->PwGetNeighbors(iFace);;

        // first find out which neighbors haven't been processed
        if ((pwNeighbors[0] == UNUSED) || (pmsMeshStatus->BProcessed(pwNeighbors[0])))
            bProcessedNeighbors |= x_iNeighbor0Processed;

        if ((pwNeighbors[1] == UNUSED) || (pmsMeshStatus->BProcessed(pwNeighbors[1])))
            bProcessedNeighbors |= x_iNeighbor1Processed;

        if ((pwNeighbors[2] == UNUSED) || (pmsMeshStatus->BProcessed(pwNeighbors[2])))
            bProcessedNeighbors |= x_iNeighbor2Processed;

        // then use the bits to look up the first ccw one
        iNextEdge = x_rgbNextUnprocessed[bProcessedNeighbors];
        GXASSERT(iNextEdge <= 3);

        iOtherEdge = x_rgbNextOtherUnprocessed[bProcessedNeighbors];
        GXASSERT(iNextEdge <= 3);

        // if there is no next, return 3 to restart another strip
        if (iNextEdge == 3)
        {
            GXASSERT(iRet == 3);
            goto e_Exit;
        }
        else if (iOtherEdge < 3)
        {
            iqPossibleRestarts.Push(pwNeighbors[iOtherEdge]);
        }

        // found the next face
        iFaceNext = pwNeighbors[iNextEdge];
        GXASSERT(iFaceNext != UNUSED);
    }

    // prune the queue of any entries that we would not want to start at
    //    only look at the entries at the head
    while (!iqPossibleRestarts.BEmpty() &&
                ((pmsMeshStatus->BProcessed(iqPossibleRestarts.IFront())) ||
                            (iqPossibleRestarts.IFront() == iFaceNext)))
    {
        iqPossibleRestarts.IPop();
    }

    // maximum number of faces in strip before thrashing occurs
    cMaxFacesInStrip = x_cDefaultVertexCacheSize + 5;

    // simulate a restart of the strip
    hr = VCSimulate(0, iFaceNext, cFacesInStrip, iqPossibleRestarts, iqTemp, pmsMeshStatus,
              svc, svcTemp, cMaxFacesInStrip, fRestartEstimate);
    if (FAILED(hr))
        goto e_Exit;

    // if the last number of faces to continue the strip is
    //   larger than the maximum strip length, truncate
    if (cFacesToContinueBest >= cMaxFacesInStrip)
        cFacesToContinueBest = cMaxFacesInStrip - 1;

    hr = VCSimulate(cFacesToContinueBest, iFaceNext, cFacesInStrip, iqPossibleRestarts, iqTemp, pmsMeshStatus,
              svc, svcTemp, cMaxFacesInStrip, fBestEstimate);
    if (FAILED(hr))
        goto e_Exit;

    // if it is better to go the same distance as before,
    //    instead of restarting, just go to the next face
    if (fBestEstimate >= fRestartEstimate)
    {
        bRestart = false;
    }
    else
    {
        cFacesToContinueBestOrig = cFacesToContinueBest;
        for (cFacesToContinue = 1; cFacesToContinue < cMaxFacesInStrip - 1; cFacesToContinue++)
        {
            // skip the quick estimate done above
            if (cFacesToContinue == cFacesToContinueBestOrig)
                continue;

            hr = VCSimulate(cFacesToContinue, iFaceNext, cFacesInStrip, iqPossibleRestarts, iqTemp, pmsMeshStatus,
                  svc, svcTemp, cMaxFacesInStrip, fEstimate);
            if (FAILED(hr))
                goto e_Exit;

            if (fEstimate >= fBestEstimate)
            {
                fBestEstimate = fEstimate;
                cFacesToContinueBest = cFacesToContinue;

                if (fBestEstimate >= fRestartEstimate)
                    break;
            }
        }

        // don't restart unless restartings cost is better than the best obtained by continueing
        bRestart= !(fBestEstimate >= fRestartEstimate);
    }


    // if not restarting, figure out what the next face is
    if (!bRestart)
    {
        cFacesInStrip += 1;
        iRet = iNextEdge;

        // find out the total number of hits for the selected face
        pwFace = m_pFaces[iFaceNext].m_wIndices;
        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            svc.AccessVertex(pwFace[iPoint], bHit);
        }
    }
    else
    {
        // restarting, so iRet should be 3
        GXASSERT(iRet == 3);
    }

    //GXASSERT(cFacesInStrip <= cMaxFacesInStrip);

e_Exit:
    GXASSERT(!FAILED(hr));
    return iRet;
}


// -------------------------------------------------------------------------------
//  method    VCacheStripReorder
//
//  devnote
//            Reorder the faces to be in strip order with respect to vertex caching
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::OldVCacheStripReorder
    (
    CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus,
    UINT *rgiFaceRemap,
    UINT *rgiFaceRemapInverse
    )
{
    HRESULT hr = S_OK;
    UINT iae;
    UINT_IDX iFace;
    UINT_IDX *pwNeighbors;
    UINT_IDX iNext;
    UINT_IDX cFacesToContinueBest;
    UINT_IDX cFacesInStrip = 0;
    UINT cFacesSubset;
    UINT iFaceOffset;
    UINT_IDX cFacesCur;
    UINT_IDX iFaceIndex;
    CSimVertexCache<UINT_IDX,UNUSED> svc(x_cDefaultVertexCacheSize);
    CSimVertexCache<UINT_IDX,UNUSED> svcTemp(x_cDefaultVertexCacheSize);
    CIndexQueue<UINT_IDX,UNUSED> iqPossibleRestarts(x_cDefaultVertexCacheSize*2);
    CIndexQueue<UINT_IDX,UNUSED> iqTemp(x_cDefaultVertexCacheSize*2);

    UINT_IDX cStrips;

    cStrips = 0;
    for (iae = 0; iae < m_caeAttributeTable; iae++)
    {
        cFacesSubset = m_rgaeAttributeTable[iae].FaceCount;
        iFaceOffset = m_rgaeAttributeTable[iae].FaceStart;
        cFacesCur = 0;

        hr = pmsMeshStatus->Init(iFaceOffset, cFacesSubset, true /* simulations */);
        if (FAILED(hr))
            goto e_Exit;

        hr = svc.Clear();
        if (FAILED(hr))
            goto e_Exit;

        hr = svcTemp.Clear();
        if (FAILED(hr))
            goto e_Exit;

        hr = iqPossibleRestarts.Init();
        if (FAILED(hr))
            goto e_Exit;

        hr = iqTemp.Init();
        if (FAILED(hr))
            goto e_Exit;

        while (1)
        {
            // find the first
            iFace = IVCRestartStrip(iqPossibleRestarts, pmsMeshStatus);
            if (iFace == UNUSED)
                break;
            cFacesToContinueBest = x_cDefaultVertexCacheSize / 2;
            cFacesInStrip = 1;

            pmsMeshStatus->MarkAsProcessed(iFace);

            // find where to jump to next
            iNext = IVCFindNextFace(iFace, cFacesToContinueBest, cFacesInStrip, iqPossibleRestarts, iqTemp, pmsMeshStatus, svc, svcTemp);

            while (1)
            {
                // add the current face to be next in the reording
                rgiFaceRemap[iFace] = iFaceOffset + cFacesCur;
                cFacesCur += 1;

                // if at end of strip, break out and try another strip
                if (iNext == 3)
                    break;

                // move to the next face
                pwNeighbors = pmsMeshStatus->PwGetNeighbors(iFace);
                iFace = pwNeighbors[iNext];
                pmsMeshStatus->MarkAsProcessed(iFace);

                // find where to jump to next
                iNext = IVCFindNextFace(iFace, cFacesToContinueBest, cFacesInStrip, iqPossibleRestarts, iqTemp, pmsMeshStatus, svc, svcTemp);
            }

            cStrips += 1;
        }

        GXASSERT(cFacesCur == cFacesSubset);
    }

    GXTRACE("# of strips: %d   Avg. strip length: %f\n", cStrips, (float)m_cFaces/(float)cStrips);

    // reinitialize all inverse pointers to point to UNUSED
    for (iFaceIndex = 0; iFaceIndex < m_cFaces; iFaceIndex++)
    {
        rgiFaceRemapInverse[iFaceIndex] = UNUSED;
    }

    // fixup the RemapInverse array by rebuilding it
    for (iFaceIndex = 0; iFaceIndex < m_cFaces; iFaceIndex++)
    {
        iFace = rgiFaceRemap[iFaceIndex];

        if (iFace != UNUSED)
        {
            rgiFaceRemapInverse[iFace] = iFaceIndex;
        }
    }

e_Exit:
    return hr;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
SFaceCorner
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::CcwCorner
    (
    SFaceCorner &cn,
    CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus
    )
{
    DWORD iEdge = (cn.iPoint + 2) % 3;
    UINT_IDX *pwNeighbors = pmsMeshStatus->PwGetNeighbors((UINT_IDX)cn.iFace);
    DWORD iNeighbor = pwNeighbors[iEdge];
    DWORD iNeighborPoint = (iNeighbor == UNUSED) ? UNUSED : FindEdge(pmsMeshStatus->PwGetNeighbors((UINT_IDX)iNeighbor), (UINT_IDX)cn.iFace);
    GXASSERT((iNeighborPoint < 3) || (iNeighborPoint == UNUSED));
    return SFaceCorner(iNeighbor, iNeighborPoint);
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
SFaceCorner
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::ClwCorner
    (
    SFaceCorner &cn,
    CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus
    )
{
    DWORD iEdge = (cn.iPoint + 1) % 3;
    UINT_IDX *pwNeighbors = pmsMeshStatus->PwGetNeighbors((UINT_IDX)cn.iFace);
    DWORD iNeighbor = pwNeighbors[iEdge];
    DWORD iNeighborPoint = (iNeighbor == UNUSED) ? UNUSED : FindEdge(pmsMeshStatus->PwGetNeighbors((UINT_IDX)iNeighbor), (UINT_IDX)cn.iFace);
    GXASSERT((iNeighborPoint < 3) || (iNeighborPoint == UNUSED));
    return SFaceCorner(iNeighbor, iNeighborPoint);
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
SFaceCorner
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::ClwFaceCorner
    (
    SFaceCorner &cn
    )
{
    return SFaceCorner(cn.iFace, (cn.iPoint + 2) % 3);
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
SFaceCorner
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::CcwFaceCorner
    (
    SFaceCorner &cn
    )
{
    return SFaceCorner(cn.iFace, (cn.iPoint + 1) % 3);
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::TryStartingEarlier
    (
    SFaceCorner &cnCur,
    CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus
    )
{
    // HMMMM... must be a bug, I get better numbers without this section
    return ;

    DWORD iFaceStart = cnCur.iFace;
    SFaceCorner cnNext = CcwCorner(cnCur, pmsMeshStatus);
    if ((cnNext.iFace == UNUSED) || pmsMeshStatus->BProcessed((UINT_IDX)cnNext.iFace))
        return;

    cnCur = CcwFaceCorner(cnCur);

    for (;;) 
    {
        cnNext = ClwCorner(cnCur, pmsMeshStatus);
        if ((cnNext.iFace == UNUSED) || pmsMeshStatus->BProcessed((UINT_IDX)cnNext.iFace) || (iFaceStart == cnNext.iFace))
            break;
        cnCur = cnNext;
    }
}

// -------------------------------------------------------------------------------
//  method    VCacheStripReorder
//
//  devnote
//            Reorder the faces to be in strip order with respect to vertex caching
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::VCacheStripReorder
    (
    CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus,
    UINT *rgiFaceRemap,
    UINT *rgiFaceRemapInverse
    )
{
    CSimVertexCache<UINT_IDX,UNUSED> vcache(x_cDefaultVertexCacheSize);

    int locnext=0;              // dont_care (only care if cnext!=0)
    SFaceCorner cnCur;
    SFaceCorner cnNext;
    SFaceCorner cext;
    SFaceCorner cint;
    SFaceCorner cnTemp;
    SFaceCorner cnTempNext;
    const int desiredloc = x_cDefaultVertexCacheSize-9;

    bool bHit;
    UINT_IDX cStrips;
    UINT cFacesSubset;
    UINT iFaceOffset;
    UINT_IDX cFacesCur;
    UINT iae;
    HRESULT hr = S_OK;
    UINT_IDX *pwFace;
    UINT_IDX *pwNeighbors;
    UINT iPoint;
    UINT_IDX iFace;
    UINT_IDX iFaceIndex;

    cnNext.iFace = UNUSED;

    cStrips = 0;
    for (iae = 0; iae < m_caeAttributeTable; iae++)
    {
        cFacesSubset = m_rgaeAttributeTable[iae].FaceCount;
        iFaceOffset = m_rgaeAttributeTable[iae].FaceStart;
        cFacesCur = 0;

        hr = vcache.Clear();
        if (FAILED(hr))
            goto e_Exit;

        hr = pmsMeshStatus->Init(iFaceOffset, cFacesSubset, false /* no simulations */);
        if (FAILED(hr))
            goto e_Exit;

        for (;;)
        {
GOTO_STRIP_RESTART_FROM_SCRATCH:
            GXASSERT(cnNext.iFace == UNUSED);

            // find the first face
            //pmsMeshStatus->IFindInitialCorner(&cnCur, vcache);
            cnCur.iFace = pmsMeshStatus->IFindInitialFace();
            if (cnCur.iFace == UNUSED)
                break;

            // now find the corner to move from
            pwNeighbors = pmsMeshStatus->PwGetNeighbors((UINT_IDX)cnCur.iFace);
            if ((pwNeighbors[0] != UNUSED) && !pmsMeshStatus->BProcessed(pwNeighbors[0]))
            {
                cnCur.iPoint = 1;
            }
            else if ((pwNeighbors[1] != UNUSED) && !pmsMeshStatus->BProcessed(pwNeighbors[1]))
            {
                cnCur.iPoint = 2;
            }
            else
            {
                cnCur.iPoint = 0;
            }

            for (;;)
            {
                GXASSERT(!pmsMeshStatus->BProcessed((UINT_IDX)cnCur.iFace));

                // Decide to either add next RING OF FACES (e.g. 2) or restart.
                if (cnNext.iFace != UNUSED)
                {
                    int nf=1;
                    for (cnTemp = cnCur;;)
                    {
                        cnTempNext = CcwCorner(cnTemp, pmsMeshStatus);
                        if ((cnTempNext.iFace == UNUSED) || pmsMeshStatus->BProcessed((UINT_IDX)cnTempNext.iFace))
                            break;
                        nf++;
                        cnTemp = cnTempNext;
                    }

                    if (locnext + (nf-1) > desiredloc)
                    { // restart desired
                        for (;;)
                        {
                            if (!pmsMeshStatus->BProcessed((UINT_IDX)cnNext.iFace))
                            {
                                cnCur = cnNext;
                                cnNext.iFace = UNUSED;

                                //try_starting_earlier(ms,c);
                                TryStartingEarlier(cnCur, pmsMeshStatus);
                                break; // restart now!
                            }

                            cnNext.iFace = UNUSED;
                            break; // keep growing strip
                        }
                    }
                }
                for (;;)
                {
                    pmsMeshStatus->MarkAsProcessed((UINT_IDX)cnCur.iFace);

                    // add the current face to be next in the reording
                    rgiFaceRemap[cnCur.iFace] = iFaceOffset + cFacesCur;
                    cFacesCur += 1;

                    // find out the total number of hits for the selected face
                    pwFace = m_pFaces[cnCur.iFace].m_wIndices;
                    for (iPoint = 0; iPoint < 3; iPoint++)
                    {
                        vcache.AccessVertex(pwFace[iPoint], bHit);

                        locnext += (1-bHit);
                    }

                    cint = CcwCorner(cnCur, pmsMeshStatus);
                    cext = CcwCorner(ClwFaceCorner(cnCur), pmsMeshStatus);
                    bool fintnei=(cint.iFace != UNUSED) && !pmsMeshStatus->BProcessed((UINT_IDX)cint.iFace);
                    bool fextnei=(cext.iFace != UNUSED) && !pmsMeshStatus->BProcessed((UINT_IDX)cext.iFace);
                    if (fintnei)
                    {
                        if (fextnei)
                        {
                            if (cnNext.iFace == UNUSED)
                            {
                                cnNext = cext;
                                locnext=0;
                            }
                        }
                        cnCur=cint;
                    }
                    else if (fextnei)
                    {
                        cnCur=cext;
                        break;
                    }
                    else
                    {        // restart forced
                        cnCur = cnNext;
                        cnNext.iFace = UNUSED;
                        for (;;)
                        {
                            if ((cnCur.iFace != UNUSED) && !pmsMeshStatus->BProcessed((UINT_IDX)cnCur.iFace))
                            {
                                //try_starting_earlier(ms,c);
                                TryStartingEarlier(cnCur, pmsMeshStatus);
                                break; // restart now!
                            }

                            goto GOTO_STRIP_RESTART_FROM_SCRATCH;
                        }
                    }
                }
            }
        }
    }

    // reinitialize all inverse pointers to point to UNUSED
    for (iFaceIndex = 0; iFaceIndex < m_cFaces; iFaceIndex++)
    {
        rgiFaceRemapInverse[iFaceIndex] = UNUSED;
    }

    // fixup the RemapInverse array by rebuilding it
    for (iFaceIndex = 0; iFaceIndex < m_cFaces; iFaceIndex++)
    {
        iFace = rgiFaceRemap[iFaceIndex];

        if (iFace != UNUSED)
        {
            rgiFaceRemapInverse[iFace] = iFaceIndex;
        }
    }

e_Exit:
    return hr;
}
