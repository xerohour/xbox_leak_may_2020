
// identifier was truncated to '255' characters in the browser information
#pragma warning(disable: 4786)

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
AddToDynamicArray( T **ppBase, const T& obj, UINT *pcNum, UINT *pcMax )
{
    T *pBase = *ppBase;
    if ( ! pBase )
    {
        HeapValidate( GetProcessHeap(), 0, NULL );
        pBase = new T[2];
        if ( ! pBase )
            return FALSE;

        *pcNum = 0;
        *pcMax = 2;
    }
    if ( *pcNum == *pcMax )
    {
        T *newarr = new T[*pcMax*2];
        if ( ! newarr )
            return FALSE;
        for (UINT i = 0; i < *pcNum; i++)
            newarr[i] = pBase[i];
        *pcMax *= 2;
        delete []pBase;
        pBase = newarr;
    }
    pBase[ (*pcNum)++ ] = obj;
    *ppBase = pBase;
    return TRUE;
}

/*
 * Template routine to handle dynamic allocation, based on Heap* Win32 APIs.
 *
 *  T **ppBase: base of array.
 *  UINT cNewMax:  size to grow the array to
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
ResizeDynamicArray( T **ppBase, UINT cNewMax, UINT cCur, UINT *pcMax )
{
    T *pBase = *ppBase;
    if ( ! pBase )
    {
        HeapValidate( GetProcessHeap(), 0, NULL );
        pBase = new T[cNewMax];
        *pcMax = cNewMax;
    }
    else if ( cNewMax > *pcMax )
    {
        T *newarr = new T[cNewMax];
        if ( ! newarr )
            return FALSE;
        for (UINT i = 0; i < cCur; i++)
            newarr[i] = pBase[i];
        *pcMax = cNewMax;
        delete []pBase;
        pBase = newarr;
    }
    *ppBase = pBase;
    return TRUE;
}

inline DWORD
TransformedFVF( DWORD dwFVF )
{
    dwFVF &= ~(D3DFVF_POSITION_MASK | D3DFVF_NORMAL);
    dwFVF |= D3DFVF_XYZRHW;
    dwFVF |= D3DFVF_DIFFUSE;    // always need diffuse in output
    dwFVF |= D3DFVF_SPECULAR;
    return dwFVF;
}

inline DWORD
FVFSize( DWORD dwFVF )
{
    DXCrackFVF cfvfTemp(dwFVF);

    return cfvfTemp.m_cBytesPerVertex;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::GXTri3Mesh(LPDIRECT3DDEVICE8 pD3DDevice, DWORD dwFVF, DWORD dwOptions)
    :m_dwFVF(dwFVF),
    m_dwOptions(dwOptions),
    m_dwOptionsInt(D3DXMESHINT_ATTRIBID),
    m_pD3D(NULL),
    m_pD3DDevice(NULL),
    m_pVBVertices(NULL),
    m_cBytesPerVertex(FVFSize(dwFVF)),
    m_cVertices(0),
    m_cMaxVertices(0),
    m_pibFaces(NULL),
    m_pFaces(0),
    m_cFaces(0),
    m_cMaxFaces(0),
    m_rgpniNeighbors(NULL),
    m_rgiAttributeIds(NULL),
    m_rgiMaterialIndex(NULL),
    m_rgaeAttributeTable(NULL),
    m_caeAttributeTable(0),
    m_rgwPointReps(NULL),
    m_cRef(1),
    m_punkOuter(NULL)
{
    m_pD3DDevice = pD3DDevice;

    // IB settings
    m_dwPoolIB = D3DPOOL_DEFAULT;
    m_dwUsageIB = 0;
    if (m_dwOptions & D3DXMESH_IB_MANAGED)
        m_dwPoolIB  = D3DPOOL_MANAGED;
    if (m_dwOptions & D3DXMESH_IB_SYSTEMMEM)
        m_dwPoolIB  = D3DPOOL_SYSTEMMEM;
    if (m_dwOptions & D3DXMESH_IB_WRITEONLY)
        m_dwUsageIB |= D3DUSAGE_WRITEONLY;
    if (m_dwOptions & D3DXMESH_POINTS)
        m_dwUsageIB |= D3DUSAGE_POINTS;
    if (m_dwOptions & D3DXMESH_RTPATCHES)
        m_dwUsageIB |= D3DUSAGE_RTPATCHES;
    if (m_dwOptions & D3DXMESH_IB_DYNAMIC)
        m_dwUsageIB |= D3DUSAGE_DYNAMIC;


    // VB settings
    m_dwPoolVB = D3DPOOL_DEFAULT;
    m_dwUsageVB = 0;
    if (m_dwOptions & D3DXMESH_VB_MANAGED)
        m_dwPoolVB  = D3DPOOL_MANAGED;
    if (m_dwOptions & D3DXMESH_VB_SYSTEMMEM)
        m_dwPoolVB  = D3DPOOL_SYSTEMMEM;
    if (m_dwOptions & D3DXMESH_VB_WRITEONLY)
        m_dwUsageVB |= D3DUSAGE_WRITEONLY;
    if (m_dwOptions & D3DXMESH_POINTS)
        m_dwUsageVB |= D3DUSAGE_POINTS;
    if (m_dwOptions & D3DXMESH_RTPATCHES)
        m_dwUsageVB |= D3DUSAGE_RTPATCHES;
    if (m_dwOptions & D3DXMESH_VB_DYNAMIC)
        m_dwUsageVB |= D3DUSAGE_DYNAMIC;

    if (m_dwOptions & D3DXMESH_VB_SHARE)
        m_dwOptionsInt |= D3DXMESHINT_SHAREDVB;


    m_pD3DDevice->AddRef();

    m_pD3DDevice->GetDirect3D(&m_pD3D);
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::~GXTri3Mesh()
{
    GXRELEASE(m_pD3D);
    GXRELEASE(m_pD3DDevice);

    GXRELEASE(m_pVBVertices);

    GXRELEASE(m_pibFaces);

    delete []m_rgpniNeighbors;
    delete []m_rgiAttributeIds;
    delete []m_rgiMaterialIndex;
    delete []m_rgaeAttributeTable;
    delete []m_rgwPointReps;
}

// -------------------------------------------------------------------------------
//  method    CreateEmptyMesh
//
//  devnote
//              "empties" the mesh, but does not free the memory allocated
//                  for the various arrays
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::CreateEmptyMesh()
{
    m_dwOptionsInt &= ~D3DXMESHINT_ATTRIBUTETABLE;

    // zero out all counts
    m_cVertices = 0;
    m_cFaces = 0;

    return S_OK;
}

// -------------------------------------------------------------------------------
//  method    CloneHelper32
//
//  devnote
//            Shared code between GXTri3Mesh::CloneMesh and PMesh::ClonePMesh
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::CloneHelper32(
                GXTri3Mesh<tp32BitIndex> *ptmNewMesh32 )
{
    HRESULT hr = S_OK;
    UINT *pwFace32Dest;
    UINT_IDX *pwFace32Src;
    UINT iPoint;
    UINT iFace;

    GXTri3Face<UINT_IDX> *pwFacesSrc = NULL;
    GXTri3Face<UINT> *pwFacesDest = NULL;

   // now setup all internal data
    ptmNewMesh32->m_dwOptionsInt = (m_dwOptionsInt & (~D3DXMESHINT_SHAREDVB))  | (ptmNewMesh32->m_dwOptionsInt & D3DXMESHINT_SHAREDVB);

    hr = ptmNewMesh32->Resize(m_cFaces, m_cVertices);
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmNewMesh32->LockIB((PBYTE*)&pwFacesDest);
    if (FAILED(hr))
        goto e_Exit;

    hr = LockIB((PBYTE*)&pwFacesSrc);
    if (FAILED(hr))
        goto e_Exit;

    if (BHasPerFaceAttributeId())
        memcpy(ptmNewMesh32->m_rgiAttributeIds, m_rgiAttributeIds, m_cFaces * sizeof(DWORD));

    if (BHasPerFaceAttributeIndex())
        memcpy(ptmNewMesh32->m_rgiMaterialIndex, m_rgiMaterialIndex, m_cFaces * sizeof(WORD));

    // copy the attribute table if present
    if (m_rgaeAttributeTable != NULL)
    {
        ptmNewMesh32->m_rgaeAttributeTable = new D3DXATTRIBUTERANGE[m_caeAttributeTable];
        if (ptmNewMesh32->m_rgaeAttributeTable == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        memcpy(ptmNewMesh32->m_rgaeAttributeTable, m_rgaeAttributeTable,
                            m_caeAttributeTable * sizeof(D3DXATTRIBUTERANGE));

        ptmNewMesh32->m_caeAttributeTable = m_caeAttributeTable;
    }

    // copy/convert the indices
    for (iFace = 0; iFace < m_cFaces; iFace++)
    {
        pwFace32Dest = pwFacesDest[iFace].m_wIndices;
        pwFace32Src = pwFacesSrc[iFace].m_wIndices;
        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            if (pwFace32Src[iPoint] != UNUSED)
                pwFace32Dest[iPoint] = pwFace32Src[iPoint];
            else
                pwFace32Dest[iPoint] = UNUSED32;
        }
    }

    if (BHasNeighborData())
    {
        // copy the neighbor data
        for (iFace = 0; iFace < m_cFaces; iFace++)
        {
            pwFace32Dest = ptmNewMesh32->m_rgpniNeighbors[iFace].m_iNeighbors;
            pwFace32Src = m_rgpniNeighbors[iFace].m_iNeighbors;
            for (iPoint = 0; iPoint < 3; iPoint++)
            {
                if (pwFace32Src[iPoint] != UNUSED)
                    pwFace32Dest[iPoint] = pwFace32Src[iPoint];
                else
                    pwFace32Dest[iPoint] = UNUSED32;
            }
        }
    }

e_Exit:
    if (pwFacesSrc != NULL)
    {
        UnlockIB();
    }
    if (pwFacesDest != NULL)
    {
        ptmNewMesh32->UnlockIB();
    }

    return hr;
}

// -------------------------------------------------------------------------------
//  method    CloneHelper16
//
//  devnote
//            Shared code between GXTri3Mesh::CloneMesh and PMesh::ClonePMesh
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::CloneHelper16(
                GXTri3Mesh<tp16BitIndex> *ptmNewMesh16 )
{
    HRESULT hr = S_OK;
    UINT16 *pwFace16Dest;
    UINT_IDX *pwFace16Src;
    UINT iPoint;
    UINT iFace;

    GXTri3Face<UINT_IDX> *pwFacesSrc = NULL;
    GXTri3Face<UINT16> *pwFacesDest = NULL;

    // now setup all internal data
    ptmNewMesh16->m_dwOptionsInt = (m_dwOptionsInt & (~D3DXMESHINT_SHAREDVB)) | (ptmNewMesh16->m_dwOptionsInt & D3DXMESHINT_SHAREDVB);

    hr = ptmNewMesh16->Resize(m_cFaces, m_cVertices);
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmNewMesh16->LockIB((PBYTE*)&pwFacesDest);
    if (FAILED(hr))
        goto e_Exit;

    hr = LockIB((PBYTE*)&pwFacesSrc);
    if (FAILED(hr))
        goto e_Exit;

    if (BHasPerFaceAttributeId())
        memcpy(ptmNewMesh16->m_rgiAttributeIds, m_rgiAttributeIds, m_cFaces * sizeof(DWORD));

    if (BHasPerFaceAttributeIndex())
        memcpy(ptmNewMesh16->m_rgiMaterialIndex, m_rgiMaterialIndex, m_cFaces * sizeof(WORD));

    // copy the attribute table if present
    if (m_rgaeAttributeTable != NULL)
    {
        ptmNewMesh16->m_rgaeAttributeTable = new D3DXATTRIBUTERANGE[m_caeAttributeTable];
        if (ptmNewMesh16->m_rgaeAttributeTable == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        memcpy(ptmNewMesh16->m_rgaeAttributeTable, m_rgaeAttributeTable,
                            m_caeAttributeTable * sizeof(D3DXATTRIBUTERANGE));

        ptmNewMesh16->m_caeAttributeTable = m_caeAttributeTable;
    }

    // copy/convert the indices
    for (iFace = 0; iFace < m_cFaces; iFace++)
    {
        pwFace16Dest = pwFacesDest[iFace].m_wIndices;
        pwFace16Src = pwFacesSrc[iFace].m_wIndices;
        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            if (pwFace16Src[iPoint] != UNUSED)
                pwFace16Dest[iPoint] = pwFace16Src[iPoint];
            else
                pwFace16Dest[iPoint] = UNUSED16;
        }
    }

    if (BHasNeighborData())
    {
        // copy the neighbor data
        for (iFace = 0; iFace < m_cFaces; iFace++)
        {
            pwFace16Dest = ptmNewMesh16->m_rgpniNeighbors[iFace].m_iNeighbors;
            pwFace16Src = m_rgpniNeighbors[iFace].m_iNeighbors;
            for (iPoint = 0; iPoint < 3; iPoint++)
            {
                if (pwFace16Src[iPoint] != UNUSED)
                    pwFace16Dest[iPoint] = pwFace16Src[iPoint];
                else
                    pwFace16Dest[iPoint] = UNUSED16;
            }
        }
    }

e_Exit:
    if (pwFacesSrc != NULL)
    {
        UnlockIB();
    }
    if (pwFacesDest != NULL)
    {
        ptmNewMesh16->UnlockIB();
    }

    return hr;
}



// -------------------------------------------------------------------------------
//  method    CloneVertexBuffer
//
//  devnote
//            Copies the vertex buffer, does FVF conversion if required
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::CloneVertexBuffer
    (
    DWORD dwFVFNew, 
    LPDIRECT3DVERTEXBUFFER8 pVertexBuffer
    )
{
    HRESULT hr = S_OK;
    PBYTE pvPointsDest = NULL;
    PBYTE pvPointsSrc = NULL;
    UINT iVertex;

    // had better not be shared
    GXASSERT(pVertexBuffer != m_pVBVertices);

    hr = pVertexBuffer->Lock(0,0, &pvPointsDest, 0 );
    if (FAILED(hr))
        goto e_Exit;

    hr = LockVB(&pvPointsSrc);
    if (FAILED(hr))
        goto e_Exit;

    // if the same FVF, just copy all the vertices
    if (dwFVFNew == m_dwFVF)
    {
        memcpy(pvPointsDest, pvPointsSrc, m_cVertices * m_cBytesPerVertex);
    }
    else
    {
        PBYTE pvCurPointSrc;
        PBYTE pvCurPointDest;
        SVertexCopyContext vcc(m_dwFVF, dwFVFNew);

        pvCurPointSrc = (PBYTE)pvPointsSrc;
        pvCurPointDest = (PBYTE)pvPointsDest;
        for (iVertex = 0; iVertex < m_cVertices; iVertex++)
        {
            vcc.CopyVertex(pvCurPointSrc, pvCurPointDest);

            pvCurPointSrc += vcc.cfvfSrc.m_cBytesPerVertex;
            pvCurPointDest += vcc.cfvfDest.m_cBytesPerVertex;
        }
    }

e_Exit:
    if (pvPointsDest != NULL)
    {
        GXASSERT(pVertexBuffer != NULL);
        pVertexBuffer->Unlock();
    }

    if (pvPointsSrc != NULL)
    {
        UnlockVB();
    }

    return hr;
}

// -------------------------------------------------------------------------------
//  method    Clone (external version)
//
//  devnote
//            Makes an exact copy of the given mesh in the current mesh
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::CloneMesh(
                DWORD dwOptionsNew, CONST DWORD *pDeclaration,
                LPDIRECT3DDEVICE8 pD3DDevice, LPD3DXMESH* ppCloneMesh )
{
    DWORD dwFVF;
    HRESULT hr;

    if (pDeclaration == NULL)
    {
        dwFVF = m_dwFVF;
    }
    else
    {
        hr = D3DXFVFFromDeclarator(pDeclaration, &dwFVF);
        if (FAILED(hr))
        {
            DPF(0,"CloneMesh: Declaration cannot be converted to FVF");
            return hr;
        }
    }

    return CloneMeshFVF(dwOptionsNew, dwFVF, pD3DDevice, ppCloneMesh);
}

// -------------------------------------------------------------------------------
//  method    Clone (external version)
//
//  devnote
//            Makes an exact copy of the given mesh in the current mesh
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::CloneMeshFVF(
                DWORD dwOptionsNew, DWORD dwFVFNew,
                LPDIRECT3DDEVICE8 pD3DDevice, LPD3DXMESH* ppCloneMesh )
{
    HRESULT hr = S_OK;
    GXTri3Mesh<tp32BitIndex> *ptmNewMesh32 = NULL;
    GXTri3Mesh<tp16BitIndex> *ptmNewMesh16 = NULL;
    ID3DXMesh *ptmNewMesh = NULL;
    LPDIRECT3DVERTEXBUFFER8 pVertexBuffer;

    // untested code
    //GXASSERT(0);

    if ((dwOptionsNew & ~D3DXMESH_VALIDCLONEBITS) != 0)
    {
        hr = D3DERR_INVALIDCALL;
        DPF(0, "CloneMeshFVF: Invalid options bit passed to CloneMesh\n");
        goto e_Exit;
    }

    if ((dwOptionsNew & D3DXMESH_VB_SHARE) && (dwFVFNew != m_dwFVF))
    {
        hr = D3DERR_INVALIDCALL;
        DPF(0, "CloneMeshFVF: VB_SHARE can only be specified if both meshes have identical FVF\n");
        goto e_Exit;
    }

    if ((dwOptionsNew & D3DXMESH_VB_SHARE) && (pD3DDevice != m_pD3DDevice))
    {
        hr = D3DERR_INVALIDCALL;
        DPF(0, "CloneMeshFVF: VB_SHARE can only be specified if both meshes share the same device\n");
        goto e_Exit;
    }


    if (dwOptionsNew & D3DXMESH_32BIT)
    {
        ptmNewMesh32 = new GXTri3Mesh<tp32BitIndex>(pD3DDevice, dwFVFNew, dwOptionsNew);

        if (ptmNewMesh32 == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        hr = CloneHelper32(ptmNewMesh32);
        if (FAILED(hr))
            goto e_Exit;

        if (dwOptionsNew & D3DXMESH_VB_SHARE)
        {
            // mark this mesh as shared as well
            m_dwOptionsInt |= D3DXMESHINT_SHAREDVB;
            GXASSERT(ptmNewMesh32->BSharedVB());

            // share the vertex buffer from this mesh with the other mesh
            ptmNewMesh32->m_pVBVertices = m_pVBVertices;
            m_pVBVertices->AddRef();
        }

        pVertexBuffer = ptmNewMesh32->m_pVBVertices;
        ptmNewMesh = ptmNewMesh32;
        ptmNewMesh32 = NULL;
    }
    else
    {
        if ((m_cFaces > UNUSED16) || (m_cVertices > UNUSED16))
        {
            hr = D3DERR_INVALIDCALL;
            goto e_Exit;
        }

        ptmNewMesh16 = new GXTri3Mesh<tp16BitIndex>(pD3DDevice, dwFVFNew, dwOptionsNew);
        if (ptmNewMesh16 == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        hr = CloneHelper16(ptmNewMesh16);
        if (FAILED(hr))
            goto e_Exit;

        if (dwOptionsNew & D3DXMESH_VB_SHARE)
        {
            // mark this mesh as shared as well
            m_dwOptionsInt |= D3DXMESHINT_SHAREDVB;
            GXASSERT(ptmNewMesh16->BSharedVB());

            // share the vertex buffer from this mesh with the other mesh
            ptmNewMesh16->m_pVBVertices = m_pVBVertices;
            m_pVBVertices->AddRef();
        }

        pVertexBuffer = ptmNewMesh16->m_pVBVertices;
        ptmNewMesh = ptmNewMesh16;
        ptmNewMesh16 = NULL;
    }

    // only do a copy if the VBs aren't shared
    if ( !(dwOptionsNew & D3DXMESH_VB_SHARE))
    {
        // now that the internals and face index data are loaded, load/convert the vertex data
        hr = CloneVertexBuffer(dwFVFNew, pVertexBuffer);
        if (FAILED(hr))
            goto e_Exit;
    }

    *ppCloneMesh = ptmNewMesh;
    ptmNewMesh = NULL;

e_Exit:
    GXRELEASE(ptmNewMesh);
    GXRELEASE(ptmNewMesh16);
    GXRELEASE(ptmNewMesh32);

    return hr;
}


// -------------------------------------------------------------------------------
//  method    CopyMesh (internal)
//
//  devnote
//            Makes an exact copy of the given mesh in the current mesh
//                  No FVF or index size changes possible
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::CopyMesh(
                GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED> *ptmSrc)
{
    HRESULT hr = S_OK;

    PBYTE pvPointsDest = NULL;
    PBYTE pvPointsSrc = NULL;

    UINT_IDX *pwFacesSrc = NULL;
    UINT_IDX *pwFacesDest = NULL;

    // both meshes should be the same size
    GXASSERT((ptmSrc->m_cVertices == m_cVertices) || (m_cVertices == 0));
    GXASSERT(ptmSrc->m_cFaces == m_cFaces);
    GXASSERT((ptmSrc->m_dwFVF == m_dwFVF) || (m_cVertices == 0));
    GXASSERT((ptmSrc->m_dwOptions &~(D3DXMESH_SYSTEMMEM|D3DXMESH_VB_SHARE))== (m_dwOptions &~(D3DXMESH_SYSTEMMEM|D3DXMESH_VB_SHARE)));

    hr = LockIB((PBYTE*)&pwFacesDest);
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmSrc->LockIB((PBYTE*)&pwFacesSrc);
    if (FAILED(hr))
        goto e_Exit;

    memcpy(pwFacesDest, pwFacesSrc, sizeof(GXTri3Face<UINT_IDX>) * m_cFaces);

    if (BHasNeighborData() && ptmSrc->BHasNeighborData())
    {
        memcpy(m_rgpniNeighbors, ptmSrc->m_rgpniNeighbors, sizeof(NeighborInfo<UINT_IDX>) * m_cFaces);
    }

    if (BHasPerFaceAttributeId() && ptmSrc->BHasPerFaceAttributeId())
    {
        memcpy(m_rgiAttributeIds, ptmSrc->m_rgiAttributeIds, sizeof(DWORD) * m_cFaces);
    }

    if (BHasPerFaceAttributeIndex() && ptmSrc->BHasPerFaceAttributeIndex())
    {
        memcpy(m_rgiMaterialIndex, ptmSrc->m_rgiMaterialIndex, sizeof(UINT16) * m_cFaces);
    }

    // treat the attribute table a little differently, create if not present locally
    if (ptmSrc->BHasAttributeTable())
    {
        if (!BHasAttributeTable() || (m_caeAttributeTable != ptmSrc->m_caeAttributeTable))
        {
            delete []m_rgaeAttributeTable;

            m_caeAttributeTable = ptmSrc->m_caeAttributeTable;
            m_rgaeAttributeTable = new D3DXATTRIBUTERANGE[m_caeAttributeTable];
            if (m_rgaeAttributeTable == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            m_dwOptionsInt |= D3DXMESHINT_ATTRIBUTETABLE;
        }

        memcpy(m_rgaeAttributeTable, ptmSrc->m_rgaeAttributeTable, sizeof(D3DXATTRIBUTERANGE) * m_caeAttributeTable);
    }

    // if vertices should be copied, then do so
    if (m_cVertices > 0)
    {
        if (BHasPointRepData() && ptmSrc->BHasPointRepData())
        {
            memcpy(m_rgwPointReps, ptmSrc->m_rgwPointReps, sizeof(UINT_IDX) * m_cVertices);
        }

        // if the dest is not sharing the VB with the src, then copy it
        //  NOTE: the src could be sharing with someone though, so always check dest
        if (!BSharedVB())
        {
            // copy the vertices
            hr = ptmSrc->LockVB(&pvPointsSrc);
            if (FAILED(hr))
                goto e_Exit;

            hr = LockVB(&pvPointsDest);
            if (FAILED(hr))
                goto e_Exit;

            memcpy(pvPointsDest, pvPointsSrc, m_cVertices * m_cBytesPerVertex);
        }
    }

e_Exit:
    if (pvPointsDest != NULL)
    {
        UnlockVB();
    }

    if (pvPointsSrc != NULL)
    {
        ptmSrc->UnlockVB();
    }

    if (pwFacesSrc != NULL)
    {
        ptmSrc->UnlockIB();
    }
    if (pwFacesDest != NULL)
    {
        UnlockIB();
    }

    if (FAILED(hr))
    {
        CreateEmptyMesh();
    }

    return hr;
}

// -------------------------------------------------------------------------------
//  method    Resize
//
//  devnote
//            Resizes the arrays in the mesh to be of at least the size
//                  provided, otherwise it asserts
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::Resize(UINT cFacesNewMax, UINT cVerticesNewMax)
{
    PBYTE pInVertices = NULL;
    PBYTE pOutVertices = NULL;
    LPDIRECT3DVERTEXBUFFER8 pVBNew = NULL;
    D3DVERTEXBUFFER_DESC desc;
    HRESULT hr = S_OK;
    UINT cMaxNeighbors = m_cMaxFaces;
    UINT cMaxAttribs = m_cMaxFaces;
    UINT cMaxAttribIndices = m_cMaxFaces;
    UINT cMaxFUserData = m_cMaxFaces;
    UINT cwMaxPointReps = m_cMaxVertices;
    UINT cpvMaxVUserData = m_cMaxVertices;
    D3DFORMAT format;
    LPDIRECT3DINDEXBUFFER8 pibNew = NULL;
    PBYTE pInFaces = NULL;
    PBYTE pOutFaces = NULL;
    //D3DPOOL dwPool;

    memset(&desc, 0, sizeof(desc));

    if ((cFacesNewMax >= UNUSED) || (cVerticesNewMax >= UNUSED))
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    if (m_cMaxFaces < cFacesNewMax)
    {
        if (b16BitIndex)
        {
            format = D3DFMT_INDEX16;
        }
        else
        {
            hr = D3DERR_INVALIDCALL;
            goto e_Exit;
        }


        hr = m_pD3DDevice->CreateIndexBuffer(sizeof(UINT_IDX) * 3 * cFacesNewMax, m_dwUsageIB, format, m_dwPoolIB, &pibNew);
        if ( FAILED(hr) )
            goto e_Exit;

        if (m_cFaces > 0)
        {
            hr = m_pibFaces->Lock(0,0, &pInFaces, 0 );
            if ( FAILED(hr) )
                goto e_Exit;
            hr = pibNew->Lock(0,0, &pOutFaces, 0 );
            if ( FAILED(hr) )
                goto e_Exit;

            memcpy( pOutFaces, pInFaces, m_cFaces * 3 * sizeof(UINT_IDX) );

            memset( (PBYTE)pOutFaces + m_cFaces * 3 * sizeof(UINT_IDX), 0,
                                        (cFacesNewMax - m_cFaces) * 3 * sizeof(UINT_IDX));

            hr = pibNew->Unlock( );
            if ( FAILED(hr) )
                goto e_Exit;
            pOutFaces = NULL;
            hr = m_pibFaces->Unlock( );
            if ( FAILED(hr) )
                goto e_Exit;
            pInFaces = NULL;
        }

        GXRELEASE( m_pibFaces );
        m_pibFaces = pibNew;
        pibNew = NULL;

        m_cMaxFaces = cFacesNewMax;

        if (BHasNeighborData())
        {
            if ( ! ResizeDynamicArray( &m_rgpniNeighbors, cFacesNewMax, m_cFaces, &cMaxNeighbors ) )
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
            GXASSERT(m_cMaxFaces == cMaxNeighbors);
        }

        if (BHasPerFaceAttributeId())
        {
            if ( ! ResizeDynamicArray( &m_rgiAttributeIds, cFacesNewMax, m_cFaces, &cMaxAttribs ) )
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
            GXASSERT(m_cMaxFaces == cMaxAttribs);

            memset(m_rgiAttributeIds + m_cFaces, 0, (cFacesNewMax - m_cFaces) * sizeof(DWORD));
        }

        if (BHasPerFaceAttributeIndex())
        {
            if ( ! ResizeDynamicArray( &m_rgiMaterialIndex, cFacesNewMax, m_cFaces, &cMaxAttribIndices ) )
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
            GXASSERT(m_cMaxFaces == cMaxAttribIndices);
        }

    }

    // only resize VB if not shared
    if ((m_cMaxVertices < cVerticesNewMax))
    {
        if ( BHasPointRepData() )
        {
            if ( ! ResizeDynamicArray( &m_rgwPointReps, cVerticesNewMax, m_cVertices, &cwMaxPointReps ) )
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
        }

        if (!BSharedVB())
        {
            hr = m_pD3DDevice->CreateVertexBuffer(cVerticesNewMax * m_cBytesPerVertex, m_dwUsageVB, m_dwFVF, m_dwPoolVB, &pVBNew);
            if ( FAILED(hr) )
                goto e_Exit;

            if (m_cVertices > 0)
            {
                hr = m_pVBVertices->Lock(0,0, &pInVertices, 0 );
                if ( FAILED(hr) )
                    goto e_Exit;
                hr = pVBNew->Lock(0,0, &pOutVertices, 0 );
                if ( FAILED(hr) )
                    goto e_Exit;

                memcpy( pOutVertices, pInVertices, m_cVertices * m_cBytesPerVertex );

                memset( (PBYTE)pOutVertices + m_cVertices * m_cBytesPerVertex, 0,
                                            (cVerticesNewMax - m_cVertices) * m_cBytesPerVertex );

                hr = pVBNew->Unlock( );
                if ( FAILED(hr) )
                    goto e_Exit;
                pOutVertices = NULL;
                hr = m_pVBVertices->Unlock( );
                if ( FAILED(hr) )
                    goto e_Exit;
                pInVertices = NULL;
            }

            GXRELEASE( m_pVBVertices );
            m_pVBVertices = pVBNew;
            pVBNew = NULL;

            m_cMaxVertices = cVerticesNewMax;
            GXASSERT(!BHasPointRepData() || cwMaxPointReps == m_cMaxVertices);

#ifdef _DEBUG
            // in debug, zero out unused vertices
            if (m_cVertices < m_cMaxVertices)
            {
                hr = m_pVBVertices->Lock(0,0, &pInVertices, 0 );
                if ( FAILED(hr) )
                    goto e_Exit;

                memset((BYTE*)pInVertices + m_cBytesPerVertex * m_cVertices, 0xff, m_cBytesPerVertex * (m_cMaxVertices - m_cVertices));

                pInVertices = NULL;
                hr = m_pVBVertices->Unlock();
                if ( FAILED(hr) )
                    goto e_Exit;
            }
#endif
        }
    }

    // setup info in mesh
    m_cFaces = cFacesNewMax;
    m_cVertices = cVerticesNewMax;




e_Exit:
    if (pOutFaces != NULL)
    {
        pibNew->Unlock( );
    }
    if (pInFaces != NULL)
    {
        m_pibFaces->Unlock( );
    }
    if (pOutVertices != NULL)
    {
        pVBNew->Unlock( );
    }
    if (pInVertices != NULL)
    {
        m_pVBVertices->Unlock( );
    }

    return hr;
}

// -------------------------------------------------------------------------------
//  method    AddVertex
//
//  devnote     Adds the given point to the vertex buffer and vertex info array
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::AddVertex( PBYTE pvPoint, UINT_IDX wPointRep)
{
    PBYTE pInVertices = NULL;
    PBYTE pOutVertices = NULL;
    LPDIRECT3DVERTEXBUFFER8 pVBNew = NULL;
    D3DVERTEXBUFFER_DESC desc;
    HRESULT hr;
    UINT cNewMaxVertices;
    UINT cwPointReps = m_cVertices;
    UINT cwMaxPointReps = m_cMaxVertices;
    //D3DPOOL dwPool;

    // had better not be adding vertices to a shared vertex buffer
    GXASSERT(!BSharedVB());

    memset(&desc, 0, sizeof(desc));

    if (BHasPointRepData())
    {
        // add the vertex info to the vertex info array for this point
        if ( ! AddToDynamicArray( &m_rgwPointReps, wPointRep, &cwPointReps, &cwMaxPointReps ) )
            return E_OUTOFMEMORY;
    }

    if ( ! m_pVBVertices )
    {
        m_cVertices = 0;
        m_cMaxVertices = 2;

        hr = m_pD3DDevice->CreateVertexBuffer(m_cMaxVertices * m_cBytesPerVertex, m_dwUsageVB, m_dwFVF, m_dwPoolVB, &m_pVBVertices);
        if ( FAILED(hr) )
            goto e_Exit;
    }

    if ( m_cVertices == m_cMaxVertices )
    {

        cNewMaxVertices = min(2*m_cMaxVertices, 65535);

        hr = m_pD3DDevice->CreateVertexBuffer(cNewMaxVertices * m_cBytesPerVertex, m_dwUsageVB, m_dwFVF, m_dwPoolVB, &pVBNew);
        if ( FAILED(hr) )
            goto e_Exit;


        hr = m_pVBVertices->Lock(0,0, &pInVertices, 0 );
        if ( FAILED(hr) )
            goto e_Exit;
        hr = pVBNew->Lock(0,0, &pOutVertices, 0 );
        if ( FAILED(hr) )
            goto e_Exit;

        memcpy( pOutVertices, pInVertices, m_cVertices * m_cBytesPerVertex );

        hr = pVBNew->Unlock( );
        if ( FAILED(hr) )
            goto e_Exit;
        pOutVertices = NULL;
        hr = m_pVBVertices->Unlock( );
        if ( FAILED(hr) )
            goto e_Exit;
        pInVertices = NULL;

        GXRELEASE( m_pVBVertices );
        m_pVBVertices = pVBNew;
        pVBNew = NULL;

        m_cMaxVertices = cNewMaxVertices;
    }
    GXASSERT(!BHasPointRepData() || (cNewMaxVertices == UNUSED) || (cwMaxPointReps == m_cMaxVertices));

    hr = LockVB( &pInVertices );
    if ( FAILED(hr) )
        goto e_Exit;

    memcpy( ((PBYTE)pInVertices)+m_cVertices*m_cBytesPerVertex, pvPoint, m_cBytesPerVertex );
    m_cVertices += 1;

e_Exit:
    if (b16BitIndex)
    {
        if ( pInVertices )
            m_pVBVertices->Unlock( );
        if ( pOutVertices )
            pVBNew->Unlock( );
        GXRELEASE( pVBNew );
    }

    return hr;
}

// -------------------------------------------------------------------------------
//  function    BValid
//
//   devnote    Walks the triangle mesh structure verifying that the topology and point
//                  information is consistent
//
//   returns    returns true, if consistent, and ASSERTS if not consistent
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
bool
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::BValidAttributeGroups(GXTri3Face<UINT_IDX> *pwFaces)
{
    UINT iae;
    UINT iFaceOffset;
    UINT cFacesSubset;
    DWORD attrCur;
    UINT iFace;
    UINT iPoint;

    for (iae = 0; iae < m_caeAttributeTable; iae++)
    {
        iFaceOffset = m_rgaeAttributeTable[iae].FaceStart;
        cFacesSubset = m_rgaeAttributeTable[iae].FaceCount + iFaceOffset;
        attrCur = m_rgaeAttributeTable[iae].AttribId;

        for (iFace = iFaceOffset; iFace < cFacesSubset; iFace++)
        {
            if (BHasPerFaceAttributeId() && m_rgiAttributeIds[iFace] != attrCur)
            {
                GXASSERT(0);
                return false;
            }

            //if (BHasPerFaceMaterialIndex() && m_rgiMaterialIndex[iFace] != iae)
            //{
                //GXASSERT(0);
                //return false;
            //}

            // had better not be an unused face
            GXASSERT(pwFaces[iFace].m_wIndices[0] != UNUSED);

            for (iPoint = 0; iPoint < 3; iPoint++)
            {
                GXASSERT(pwFaces[iFace].m_wIndices[iPoint] >= m_rgaeAttributeTable[iae].VertexStart);
                GXASSERT(pwFaces[iFace].m_wIndices[iPoint] < (m_rgaeAttributeTable[iae].VertexStart + m_rgaeAttributeTable[iae].VertexCount));
            }

        }
    }

    return true;
}

// -------------------------------------------------------------------------------
//  function    BValid
//
//   devnote    Walks the triangle mesh structure verifying that the topology and point
//                  information is consistent
//
//   returns    returns true, if consistent, and ASSERTS if not consistent
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
bool
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::BValid(GXTri3Face<UINT_IDX> *pwFacesLocked)
{
    UINT iFace;
    UINT_IDX *pwFace;
    UINT_IDX *pwNeighbors;
    UINT_IDX *pwFaceNeighbor;
    UINT_IDX *pwNeighborsNeighbor;
    UINT iPoint1;
    UINT iPoint2;
    UINT iPointNeighbor1;
    UINT iPointNeighbor2;
    UINT iNeighborEdge;
    UINT iNeighborEdgeBack;
    GXTri3Face<UINT_IDX> *pwFaces;
    bool bRet = false;
    HRESULT hr;

    if (pwFacesLocked != NULL)
    {
        pwFaces = pwFacesLocked;
    }
    else if (m_pFaces != NULL)
    {
        pwFacesLocked = m_pFaces;
        pwFaces = m_pFaces;
    }
    else
    {
        hr = LockIB((PBYTE*)&pwFaces);
        if (FAILED(hr))
            goto e_Exit;
    }

    for (iFace = 0; iFace < m_cFaces; iFace++)
    {
        pwFace = pwFaces[iFace].m_wIndices;
        pwNeighbors = m_rgpniNeighbors[iFace].m_iNeighbors;

        // if the face is not present, skip checking the topology
        if (pwFace[0] == UNUSED)
        {
            // all three should be UINT max, if one is
            GXASSERT(pwFace[0] == pwFace[1] && pwFace[1] == pwFace[2]);
            if (!(pwFace[0] == pwFace[1] && pwFace[1] == pwFace[2]))
                goto e_Exit;

            if (BHasNeighborData())
            {
                GXASSERT(pwNeighbors[0] == UNUSED
                    && pwNeighbors[0] == pwNeighbors[1] && pwNeighbors[1] == pwNeighbors[2]);
                if (!(pwNeighbors[0] == UNUSED
                    && pwNeighbors[0] == pwNeighbors[1] && pwNeighbors[1] == pwNeighbors[2]))
                    goto e_Exit;
            }

            continue;
        }

        if (BHasPointRepData())
        {
            GXASSERT((WGetPointRep(pwFace[0]) != WGetPointRep(pwFace[1]))
                        && (WGetPointRep(pwFace[1]) != WGetPointRep(pwFace[2]))
                        && (WGetPointRep(pwFace[2]) != WGetPointRep(pwFace[0])));
        }
        else
        {
            GXASSERT(((pwFace[0]) != (pwFace[1]))
                        && ((pwFace[1]) != (pwFace[2]))
                        && ((pwFace[2]) != (pwFace[0])));
        }

        if (BHasNeighborData())
        {
            for (iNeighborEdge = 0; iNeighborEdge < 3; iNeighborEdge++)
            {

                iPoint1 = pwFace[iNeighborEdge];
                iPoint2 = pwFace[(iNeighborEdge + 1) % 3];

                GXASSERT(iPoint1 < m_cVertices);
                GXASSERT(iPoint2 < m_cVertices);
                if (!(iPoint1 < m_cVertices))
                    goto e_Exit;
                if (!(iPoint2 < m_cVertices))
                    goto e_Exit;

                if (pwNeighbors[iNeighborEdge] != UNUSED)
                {

                    // RESTRICTION: does not  support sharing a side with the same triangle more than once
                    for (UINT i = 0; i < 3; i++)
                    {
                        if (i != iNeighborEdge)
                            GXASSERT(pwNeighbors[iNeighborEdge] != pwNeighbors[i]);
                    }

                    // assert that it is a valid id
                    GXASSERT(pwNeighbors[iNeighborEdge] < m_cFaces);
                    if (!(pwNeighbors[iNeighborEdge] < m_cFaces))
                        goto e_Exit;

                    pwFaceNeighbor = pwFaces[pwNeighbors[iNeighborEdge]].m_wIndices;
                    pwNeighborsNeighbor = m_rgpniNeighbors[pwNeighbors[iNeighborEdge]].m_iNeighbors;

                    // find the edge that points to this triangle in the neighbor
                    iNeighborEdgeBack = FindEdge(pwNeighborsNeighbor, (UINT_IDX)iFace);

                    // the neighbor had better point back to this face
                    GXASSERT(iNeighborEdgeBack < 3);
                    if (!(iNeighborEdgeBack < 3))
                        goto e_Exit;

                    if (BHasPointRepData())
                    {
                        // get the two points on that neighbor's edge
                        iPointNeighbor1 = pwFaceNeighbor[iNeighborEdgeBack];
                        iPointNeighbor2 = pwFaceNeighbor[(iNeighborEdgeBack + 1) % 3];

                        UINT_IDX wPoint1Rep = WGetPointRep(iPoint1);
                        UINT_IDX wPoint2Rep = WGetPointRep(iPoint2);
                        UINT_IDX wPointNeighbor1Rep = WGetPointRep(iPointNeighbor1);
                        UINT_IDX wPointNeighbor2Rep = WGetPointRep(iPointNeighbor2);

                        GXASSERT(BEqualPoints(iPoint1, iPointNeighbor2) && BEqualPoints(iPoint2, iPointNeighbor1));

                        // the two points along the shared edge had better match up
                        GXASSERT((BEqualPoints(iPoint1, iPointNeighbor1) && BEqualPoints(iPoint2, iPointNeighbor2))
                                    || (BEqualPoints(iPoint2, iPointNeighbor1) && BEqualPoints(iPoint1, iPointNeighbor2)));
                        if (!((BEqualPoints(iPoint1, iPointNeighbor1) && BEqualPoints(iPoint2, iPointNeighbor2))
                                    || (BEqualPoints(iPoint2, iPointNeighbor1) && BEqualPoints(iPoint1, iPointNeighbor2))))
                            goto e_Exit;
                    }
                }
            }
        }
    }

    // if there is an attribute group table, then make sure it is valid
    if ((m_dwOptionsInt & D3DXMESHINT_ATTRIBUTETABLE) && (m_rgaeAttributeTable != NULL) && !BValidAttributeGroups(pwFaces))
        goto e_Exit;

    bRet = true;
e_Exit:
    if ((pwFaces != NULL) && (pwFacesLocked == NULL))
    {
        UnlockIB();
    }

    return bRet;
}





//const int x_cHashSize = 60;

struct hashentry
{
    DWORD v1;
    DWORD v2;
    DWORD f;
    hashentry* next;
};
typedef hashentry* PHASHENTRY;


inline void HashAdd(DWORD va, DWORD vb, DWORD f, PHASHENTRY* hashtable, PHASHENTRY hashentries, DWORD dwHashSize, int& freeptr)
{
    // UNDONE, replace with a halfway decent hash function
    DWORD v = va % dwHashSize;

    for (PHASHENTRY* t = &(hashtable[v]); *t; t = &((*t)->next));
    PHASHENTRY p = &(hashentries[freeptr++]);
    p->f = f;
    p->v1 = va;
    p->v2 = vb;
    p->next = NULL;
    *t=p;
}

inline DWORD HashFind(DWORD va, DWORD vb, PHASHENTRY* hashtable, DWORD dwHashSize, DWORD UNUSED)
{
    PHASHENTRY pheCur;
    PHASHENTRY phePrev;
    DWORD iFace;

    // UNDONE, replace with a halfway decent hash function
    DWORD v = va % dwHashSize;

    pheCur = hashtable[v];
    phePrev = NULL;

    iFace = UNUSED;
    while (pheCur != NULL)
    {
        if ((pheCur->v2 == vb) && (pheCur->v1 == va))
        {
            iFace = pheCur->f;
            break;
        }

        phePrev = pheCur;
        pheCur = pheCur->next;
    }

    if (iFace != UNUSED)
    {
        GXASSERT(pheCur != NULL);

        // if not the head, trim out pheCur
        if (phePrev != NULL)
        {
            phePrev->next = pheCur->next;
        }
        else
        {
            hashtable[v] = pheCur->next;
        }
    }

    return iFace;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::ConvertAdjacencyToPointReps
    (
    CONST DWORD* rgdwNeighbors, 
    DWORD* rgdwPointReps
    )
{
    UINT_IDX wPointRep;
    UINT_IDX wWedge;
    UINT_IDX *pwFace;
    UINT_IDX iCurFace;
    UINT iFace;
    UINT iPoint;
    UINT iVertex;
    UINT iIterated;
    CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(this);
    GXTri3Face<UINT_IDX> *pwFaces = NULL;
    CONST DWORD *pdwCur;
    HRESULT hr = S_OK;

    if (rgdwNeighbors == NULL)
    {
        DPF(0, "ConvertAdjacencyToPointReps: Failed because Adjacency array must not be NULL\n");
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }
    if (rgdwPointReps == NULL)
    {
        DPF(0, "ConvertAdjacencyToPointReps: Failed because PointRep output array must not be NULL\n");
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    // create a temporary array to enable facelistiter's to work
    m_rgpniNeighbors = new NeighborInfo<UINT_IDX>[m_cFaces];
    if (m_rgpniNeighbors == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    for (iFace = 0, pdwCur = rgdwNeighbors; iFace < m_cFaces; iFace++)
    {
        m_rgpniNeighbors[iFace].m_iNeighbors[0] = (UINT_IDX)*pdwCur;
        pdwCur++;
        m_rgpniNeighbors[iFace].m_iNeighbors[1] = (UINT_IDX)*pdwCur;
        pdwCur++;
        m_rgpniNeighbors[iFace].m_iNeighbors[2] = (UINT_IDX)*pdwCur;
        pdwCur++;
    }

    // first set all point reps to the UNUSED value to know whether or not we've seen
    //   a vertex before
    memset(rgdwPointReps, 0xff, sizeof(DWORD) * m_cVertices);

    hr = LockIB((PBYTE*)&pwFaces);
    if (FAILED(hr))
        goto e_Exit;
    m_pFaces = pwFaces;

    for (iFace = 0; iFace < m_cFaces; iFace++)
    {
        pwFace = pwFaces[iFace].m_wIndices;

        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            wPointRep = pwFace[iPoint];
            if (wPointRep == UNUSED)
                continue;

            // if the point rep is unused, then we haven't circled this vertex yet
            if (rgdwPointReps[wPointRep] == UNUSED32)
            {
                // now that we need to setup the point rep, make a round of the
                //     faces, marking all wedges of the given vertex with the new pointrep
                fli.Init(iFace, wPointRep, x_iAllFaces);
                iIterated = 0;
                while (!fli.BEndOfList())
                {
                    iCurFace = fli.GetNextFace();

                    wWedge = pwFaces[iCurFace].m_wIndices[fli.IGetPointIndex()];
                    rgdwPointReps[wWedge] = wPointRep;

                    iIterated += 1;
                    if (iIterated > 10000)
                    {
                        DPF(0, "ConvertAdjacencyToPointReps: Possible bad adjacency loop found cannot continue");
                        DPF(0, "ConvertAdjacencyToPointReps:   Try calling D3DXValidMesh for more information");
                        hr = D3DXERR_INVALIDMESH;
                        goto e_Exit;
                    }
                }
            }
        }
    }

    for (iVertex = 0; iVertex < m_cVertices; iVertex++)
    {
        // if the point is present in the mesh but not used, just set it to point to itself
        if (rgdwPointReps[iVertex] == UNUSED32)
        {
            rgdwPointReps[iVertex] = iVertex;
        }
    }

e_Exit:
    if (pwFaces != NULL)
    {
        UnlockIB();
    }
    m_pFaces = NULL;

    delete []m_rgpniNeighbors;
    m_rgpniNeighbors = NULL;

    return hr;
}

struct SVertexHashEntry
{
    D3DXVECTOR3 v;
    DWORD iIndex;
    SVertexHashEntry* pNext;
};


template <class UINT_IDX>
BOOL BEdgePresent
    (
    DWORD iVertex1, 
    DWORD iVertex2, 
    DWORD *rgdwVertexToCorner, 
    DWORD *rgdwVertexCornerList, 
    GXTri3Face<UINT_IDX> *pwFaces
    )
{
    DWORD dwHead;
    UINT_IDX *pwFace;

    GXASSERT(iVertex1 != iVertex2);

    // look at all triangles that contain iVertex1 to see if they contain iVertex2 as well.
    dwHead = rgdwVertexToCorner[iVertex1];
    while (dwHead != UNUSED32)
    {
        pwFace = pwFaces[dwHead / 3].m_wIndices;

        // iVertex1 had better be in the face
        GXASSERT((pwFace[0] == iVertex1) || 
                 (pwFace[1] == iVertex1) || 
                 (pwFace[2] == iVertex1));

        // if the other vertex is in the triangle then there is an edge
        if ((pwFace[0] == iVertex2) || 
            (pwFace[1] == iVertex2) || 
            (pwFace[2] == iVertex2))
        {
            return TRUE;
        }

        // move to next entry
        dwHead = rgdwVertexCornerList[dwHead];
    }

    return FALSE;
}


 
template <class UINT_IDX>
BOOL VertexHashAdd
    (
    D3DXVECTOR3 *pv,
    DWORD iIndex, 
    SVertexHashEntry **rgpHashTable, 
    SVertexHashEntry *rgHashEntries, 
    DWORD dwHashSize, 
    DWORD *rgdwVertexToCorner, 
    DWORD *rgdwVertexCornerList, 
    GXTri3Face<UINT_IDX> *pwFaces, 
    DWORD *piFreeEntry, 
    DWORD *piFound
    )
{
    SVertexHashEntry *pCur;
    SVertexHashEntry *pNewEntry;
    //DWORD iHash = ((DWORD)pv->x * (17 * 17) + ((DWORD)pv->y * 17) + (DWORD)pv->z) % dwHashSize;
    DWORD iHash = (*(DWORD*)&pv->x + *(DWORD*)&pv->y + *(DWORD*)&pv->z) % dwHashSize;

    *piFound = UNUSED32;

    for (pCur = rgpHashTable[iHash]; pCur != NULL; pCur = pCur->pNext)
    {
        if ((pCur->v == *pv) && (!BEdgePresent(iIndex, pCur->iIndex, rgdwVertexToCorner, rgdwVertexCornerList, pwFaces)))
        {
            *piFound = pCur->iIndex;
            return TRUE;
        }
    }

    // if vertex not found, then add

    pNewEntry = &rgHashEntries[*piFreeEntry];
    *piFreeEntry += 1;

    pNewEntry->v = *pv;
    pNewEntry->iIndex = iIndex;
    pNewEntry->pNext = rgpHashTable[iHash];
    rgpHashTable[iHash] = pNewEntry;

    return FALSE;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::GenerateAdjacency
    (
    FLOAT fEpsilon, 
    DWORD* rgdwNeighbors
    )
{
    HRESULT hr = S_OK;
    DWORD *rgdwPointReps = NULL;
    float *rgfMinDist = NULL;
    DWORD iVertex;
    DWORD iVertexInner;
    PBYTE pbVertices = NULL;
    DXCrackFVF cfvf(m_dwFVF);
    float fEpsilonSq;
    float fDist;
    D3DXVECTOR3 *pvInner;
    D3DXVECTOR3 *pvOuter;
    D3DXVECTOR3 vDiff;
    PBYTE pbVertexCur;
    DWORD dwHashSize = m_cVertices / 3;
    SVertexHashEntry **rgpHashTable = NULL;
    // An entry for each 3 edges of each face in base mesh
    SVertexHashEntry *rgHashEntries = NULL;
    DWORD iFreeEntry = 0;
    DWORD iVertexMatch;
    DWORD *rgdwVertexToCorner = NULL;
    DWORD *rgdwVertexCornerList = NULL;
    GXTri3Face<UINT_IDX> *pwFaces = NULL;
    UINT_IDX *pwFace;
    DWORD iFace;
    DWORD iPoint;

    if (rgdwNeighbors == NULL)
    {
        DPF(0, "GenerateAdjacency: Adjacency out parameter must not be NULL\n");
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    rgpHashTable = new SVertexHashEntry*[dwHashSize];
    rgHashEntries = new SVertexHashEntry[m_cVertices];
    rgdwVertexToCorner = new DWORD[m_cVertices];
    rgdwVertexCornerList = new DWORD[m_cFaces * 3];
    rgdwPointReps = new DWORD[m_cVertices];
    if ((rgpHashTable == NULL)|| (rgHashEntries == NULL) 
        || (rgdwVertexToCorner == NULL) || (rgdwVertexCornerList == NULL)
        || (rgdwPointReps == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memset(rgpHashTable, 0, sizeof(SVertexHashEntry*) * dwHashSize);
    memset(rgdwVertexToCorner, 0xff, sizeof(DWORD) * m_cVertices);

    hr = LockIB((PBYTE*)&pwFaces);
    if (FAILED(hr))
    {
        goto e_Exit;
    }

    // generate a list for each vertex of faces touched by that vertex
    for (iFace = 0; iFace < m_cFaces; iFace++)
    {
        pwFace = pwFaces[iFace].m_wIndices;
        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            // now add the current corner to the list (if no corner before, then it is an
            //    empty list setup above)

            rgdwVertexCornerList[iFace * 3 + iPoint] = rgdwVertexToCorner[pwFace[iPoint]];
            rgdwVertexToCorner[pwFace[iPoint]] = iFace * 3 + iPoint;
        }
    }

    hr = LockVB(&pbVertices);
    if (FAILED(hr))
        goto e_Exit;


    pbVertexCur = pbVertices;
    for (iVertex = 0; iVertex < m_cVertices; iVertex++)
    {
        if (VertexHashAdd(cfvf.PvGetPosition(pbVertexCur), iVertex, rgpHashTable, 
                                        rgHashEntries, dwHashSize,
                                        rgdwVertexToCorner, rgdwVertexCornerList, pwFaces,
                                        &iFreeEntry, &iVertexMatch))
        {
            rgdwPointReps[iVertex] = iVertexMatch;
        }
        else
        {
            rgdwPointReps[iVertex] = iVertex;
        }

        pbVertexCur += cfvf.m_cBytesPerVertex;
    }



    // N^2 algorithm to generate closest point info, needs a bit to generate point reps though
#if 0
    for (iVertex = 0; iVertex < m_cVertices; iVertex++)
    {
        pbVertexCur = pvVertices;
        pvOuter = cfvf.PvGetPosition(cfvf.GetArrayElem(pbVertices, iVertex));

        fDistMin = fEpsilonSq;
        rgdwPointReps[iVertex] = iVertex;

        for (iVertexInner = 0; iVertexInner < m_cVertices; iVertexInner++)
        {
            if (iVertex != iVertexInner)
            {
                pvInner = cfvf.PvGetPosition(pbVertexCur);

                vDiff = *pvInner;
                vDiff -= *pvOuter;
                fDist = D3DXVec3LengthSq(&vDiff);

                if (fDist < fDistMin)
                {
                    rgdwPointReps[iVertex] = iVertexInner;
                }
            }

            pbVertexCur += cfvf.m_cBytesPerVertex;
        }

#if 0
        if (rgdwPointReps[iVertex] == UNUSED32)
        {
            rgdwPointReps[iVertex] = iVertex;
            rgfMinDist[iVertex] = 0.0f;

            pbVertexCur = cfvf.GetArrayElem(pbVertices, iVertex);
            pvOuter = cfvf.PvGetPosition(pbVertexCur);

            for (iVertexInner = iVertex + 1; iVertexInner < m_cVertices; iVertexInner++)
            {
                pvInner = cfvf.PvGetPosition(pbVertexCur);

                vDiff = *pvInner;
                vDiff -= *pvOuter;
                fDist = D3DXVec3LengthSq(&vDiff);

                if (fDist < fEpsilonSq)
                {
                    if ((rgdwPointReps[iVertexInner] == UNUSED32) || (fDist < rgfMinDist[iVertex]))
                    {
                        rgdwPointReps[iVertexInner] = iVertex;
                        rgfMinDist[iVertex] = fDist;
                    }
                }

                pbVertexCur += cfvf.m_cBytesPerVertex;
            }
        }
#endif
    }
#endif

    // Will be implemented in a follow up checkin
    hr = ConvertPointRepsToAdjacency(rgdwPointReps, rgdwNeighbors);
    if (FAILED(hr)) 
        goto e_Exit;

e_Exit:
    delete []rgpHashTable;
    delete []rgHashEntries;
    delete []rgdwVertexToCorner;
    delete []rgdwVertexCornerList;

    delete []rgdwPointReps;
    if (pbVertices != NULL)
        UnlockVB();
    if (pwFaces != NULL)
        UnlockIB();

    return hr;
}


template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::ConvertPointRepsToAdjacency(CONST DWORD* rgdwPointReps, DWORD* rgdwNeighbors)
{
    HRESULT hr = S_OK;

    UINT_IDX *pwFace;
    DWORD *pdwNeighbors;
    DWORD rgPoint[3];
    DWORD iPoint;
    DWORD iNPoint;
    DWORD iCurNPoint;
    UINT iFace;
    GXTri3Face<UINT_IDX> *pwFaces = NULL;
    int freeptr = 0;
    int maxptr = m_cFaces*3;
    DWORD dwHashSize = m_cVertices / 3;
    PHASHENTRY *hashtable = NULL;
    // An entry for each 3 edges of each face in base mesh
    PHASHENTRY hashentries = NULL;

    if (rgdwNeighbors == NULL)
    {
        DPF(0, "ConvertPointRepsToAdjacency: Adjacency out parameter must not be NULL\n");
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    hashtable = new PHASHENTRY[dwHashSize];
    hashentries = new hashentry[maxptr];
    if ((hashtable == NULL)|| (hashentries == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }


    hr = LockIB((PBYTE*)&pwFaces);
    if (FAILED(hr))
    {
        goto e_Exit;
    }

    memset(hashtable, 0, sizeof(PHASHENTRY)*dwHashSize);

    for (iFace = 0; iFace < m_cFaces; iFace++)
    {
        pwFace = pwFaces[iFace].m_wIndices;
        pdwNeighbors = &rgdwNeighbors[iFace * 3];

        if (pwFace[0] == UNUSED)
            continue;

        // For each face in group
        if (rgdwPointReps != NULL)
        {
            rgPoint[0] = rgdwPointReps[pwFace[0]];
            rgPoint[1] = rgdwPointReps[pwFace[1]];
            rgPoint[2] = rgdwPointReps[pwFace[2]];
        }
        else
        {
            rgPoint[0] = pwFace[0];
            rgPoint[1] = pwFace[1];
            rgPoint[2] = pwFace[2];
        }

        // don't include degenerate triangles in hashing
        if ((rgPoint[0] != rgPoint[1]) && (rgPoint[0] != rgPoint[2]) && (rgPoint[1] != rgPoint[2]))
        {
            for (iPoint = 0; iPoint < 3; iPoint++)
            {
                // lookup to see if edge already entered, if not enter it
                pdwNeighbors[iPoint] = HashFind(rgPoint[(iPoint+1)%3],rgPoint[iPoint],hashtable, dwHashSize, UNUSED32);

                // if matching edge found, mark neighbor to point back
                if (pdwNeighbors[iPoint] != UNUSED32)
                {
                    // now search for the edge in the neighbor triangle
                    for (iNPoint = 0; iNPoint < 3; iNPoint++)
                    {
                        // lookup the value of the current point
                        iCurNPoint = pwFaces[pdwNeighbors[iPoint]].m_wIndices[iNPoint];
                        if (rgdwPointReps != NULL)
                        {
                            iCurNPoint = rgdwPointReps[iCurNPoint];
                        }

                        if (iCurNPoint == rgPoint[(iPoint+1)%3])
                            break;
                    }
#ifdef DEBUG
                        // assert that the second point of the edge matches what we are looking for
                        DWORD iTestPoint;
                        iTestPoint = pwFaces[pdwNeighbors[iPoint]].m_wIndices[(iNPoint+1)%3];
                        if (rgdwPointReps != NULL)
                            iTestPoint = rgdwPointReps[iCurNPoint];
                        GXASSERT(iTestPoint == rgPoint[iPoint];
#endif

                    // the neighbor had better not point at someone else
                    GXASSERT(rgdwNeighbors[pdwNeighbors[iPoint] * 3 + iNPoint] == UNUSED32);

                    // update the neighbor to point back to this face on the matching edge
                    rgdwNeighbors[pdwNeighbors[iPoint] * 3 + iNPoint] = iFace;
                }
                else
                {
                    // lookup to see if edge already entered, if not enter it
                    HashAdd(rgPoint[iPoint],rgPoint[(iPoint+1)%3],iFace, hashtable, hashentries, dwHashSize, freeptr);

                    // Mark as UNUSED, if another edge matches this one, it will fix it up
                    pdwNeighbors[iPoint] = UNUSED32;
                }
            }
        }
        else
        {
            pdwNeighbors[0] = UNUSED32;
            pdwNeighbors[1] = UNUSED32;
            pdwNeighbors[2] = UNUSED32;
        }
    }
    if (freeptr > maxptr)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    GXASSERT(BValid(pwFaces));

    GXASSERT(CheckAdjacency(rgdwNeighbors, m_cFaces));

e_Exit:
    if (pwFaces != NULL)
    {
        UnlockIB();
    }

    delete [] hashtable;
    delete [] hashentries;
    return hr;
}


template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::GetVertexBuffer(LPDIRECT3DVERTEXBUFFER8* ppVB)
{

    if (ppVB == NULL)
    {
        DPF(0, "ppVB pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    *ppVB = m_pVBVertices;

    m_pVBVertices->AddRef();

    return S_OK;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
DWORD
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::GetNumVertices()
{
    return m_cVertices;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::GetIndexBuffer(LPDIRECT3DINDEXBUFFER8 *ppIB)
{
    //HRESULT hr;

    if (ppIB == NULL)
    {
        DPF(0, "ppIB pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    *ppIB = m_pibFaces;
    m_pibFaces->AddRef();

    return S_OK;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
DWORD
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::GetNumFaces()
{
    return m_cFaces;
}


template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
DWORD
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::GetFVF()
{
    return m_dwFVF;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::GetDeclaration(DWORD Declaration[MAX_FVF_DECL_SIZE])
{
    return D3DXDeclaratorFromFVF(m_dwFVF, Declaration);
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
DWORD
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::GetOptions()
{
    return m_dwOptions;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::GetAttributeTable(
          D3DXATTRIBUTERANGE *rgaeAttribTable, DWORD* pcAttribTableSize)
{
    // if count desired, provided it
    if (pcAttribTableSize != NULL)
    {
        *pcAttribTableSize = m_caeAttributeTable;
    }

    // if attribute table desired, provided it
    if ((rgaeAttribTable != NULL) && (m_rgaeAttributeTable != NULL))
    {
        memcpy(rgaeAttribTable, m_rgaeAttributeTable,
            sizeof(D3DXATTRIBUTERANGE) * m_caeAttributeTable);
    }

    return S_OK;
}
