/*//////////////////////////////////////////////////////////////////////////////
//
// File: pmeshhe.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
// @@BEGIN_MSINTERNAL
//
// History:
// -@- 08/01/99 (craigp)    - created 
//
// @@END_MSINTERNAL
//
//////////////////////////////////////////////////////////////////////////////*/

// -------------------------------------------------------------------------------
//  function    GXHalfEdgePMesh constructor
//
//   devnote    initialize data members so that destruction is possible
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::GXHalfEdgePMesh
    ( 
    LPDIRECT3DDEVICE8 pD3DDevice, 
    DWORD dwFVF, 
    DWORD dwOptions
    )
    :m_cRef(1),
    m_tmTriMesh(pD3DDevice, dwFVF, dwOptions),
    m_rgvsVsplits(NULL),
    m_rgiMaterialNew(NULL),
    m_cMaterialNewCur(0),
    m_cMaterialNewMax(0),
    m_cBaseVertices(0),
    m_iCurPos(0),
    m_cMaxFaces(0),
    m_cMaxWedges(0),
    m_cBaseFaces(0),
    m_rgaeAttributeTableFull(0)
{
#ifdef _DEBUG
    m_rgbVertexSeen = NULL;
#endif

    // has material indices, but no attribute bundles per face
    m_tmTriMesh.m_dwOptionsInt |= D3DXMESHINT_FACEADJACENCY | D3DXMESHINT_ATTRIBINDEX
                                            | D3DXMESHINT_ATTRIBUTETABLE;
    m_tmTriMesh.m_dwOptionsInt &= ~D3DXMESHINT_ATTRIBID;

    m_vsi.rgpwLCLWtoRCCW = NULL;
    m_vsi.cpwLCLWtoRCCWMax = 0;  

    m_tmTriMesh.m_punkOuter = (IUnknown*)this;

}

// -------------------------------------------------------------------------------
//  function    GXHalfEdgePMesh destructor
//
//   devnote    frees all local data
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::~GXHalfEdgePMesh()
{

    // release the shared data - all happens inside the interlocked object
    //delete []m_rgvsVsplits;
    //delete []m_rgiMaterialNew;
    GXRELEASE(m_pPMSharedData);

    delete []m_vsi.rgpwLCLWtoRCCW;
    delete m_rgaeAttributeTableFull;
}

// -------------------------------------------------------------------------------
//  function    GetAdjacency
//
//   devnote    Converts internal adjacency into external in the buffer provided
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::GetAdjacency
    (
    DWORD *rgdwAdjacency
    )
{
    DWORD iEntry;
    DWORD cEntries;
    UINT_IDX *pwSrc;
    DWORD *pdwDest;

    pwSrc = m_tmTriMesh.m_rgpniNeighbors[0].m_iNeighbors;
    pdwDest = rgdwAdjacency;

    cEntries = m_tmTriMesh.m_cFaces * 3;
    for (iEntry = 0; iEntry < cEntries; iEntry++)
    {
        if (*pwSrc != UNUSED)
            *pdwDest = (DWORD)*pwSrc;
        else
            *pdwDest = UNUSED32;

        pdwDest += 1;
        pwSrc += 1;
    }

    return S_OK;
}

// -------------------------------------------------------------------------------
//  function    OptimizeMesh
//
//   devnote    
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::Optimize
    (
    DWORD dwFlags, 
    DWORD* rgdwAdjacencyOut, 
    DWORD* faceRemap, 
    LPD3DXBUFFER *ppbufVertexRemap,  
    LPD3DXMESH* ppOptMesh
    )
{
    return CloneMeshInternal(m_tmTriMesh.m_dwOptions, m_tmTriMesh.m_dwFVF, 
                                m_tmTriMesh.m_pD3DDevice, FALSE, 
                                dwFlags, rgdwAdjacencyOut, faceRemap, ppbufVertexRemap, ppOptMesh);
}

// -------------------------------------------------------------------------------
//  function    CloneMesh
//
//   devnote    
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::CloneMesh
    (
    DWORD dwOptionsNew, 
    CONST DWORD *pDeclaration, 
    LPDIRECT3DDEVICE8 pD3DDevice, 
    LPD3DXMESH* ppCloneMesh
    )
{
    DWORD dwFVF;
    HRESULT hr;

    if (pDeclaration == NULL)
    {
        dwFVF = m_tmTriMesh.m_dwFVF;
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
//  function    ClonePM
//
//   devnote    Generate a sibling progressive mesh, sharing data when possible
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::CloneMeshFVF
    (
    DWORD dwOptionsNew, 
    DWORD dwFVFNew, 
    LPDIRECT3DDEVICE8 pD3DDevice, 
    LPD3DXMESH* ppCloneMesh
    )
{
    if ((dwOptionsNew & ~D3DXMESH_VALIDCLONEBITS) != 0)
    {
        DPF(0, "CloneMeshFVF: Invalid options bit passed to CloneMesh\n");
        return D3DERR_INVALIDCALL;
    }

    if ((dwOptionsNew & D3DXMESH_VB_SHARE) && (dwFVFNew != m_tmTriMesh.m_dwFVF))
    {
        DPF(0, "CloneMeshFVF: VB_SHARE can only be specified if both meshes have identical FVF\n");
        return D3DERR_INVALIDCALL;
    }
    if ((dwOptionsNew & D3DXMESH_VB_SHARE) && (pD3DDevice != m_tmTriMesh.m_pD3DDevice))
    {
        DPF(0, "CloneMeshFVF: VB_SHARE can only be specified if both meshes are on the same device\n");
        return D3DERR_INVALIDCALL;
    }

    return CloneMeshInternal(dwOptionsNew, dwFVFNew, pD3DDevice, FALSE, 0, NULL, NULL, NULL, ppCloneMesh);
}

// -------------------------------------------------------------------------------
//  function    CloneMesh
//
//   devnote    Clones the current LOD of the pmesh
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::CloneMeshInternal
    (
    DWORD dwOptionsNew, 
    DWORD dwFVFNew, 
    LPDIRECT3DDEVICE8 pD3DDevice, 
    BOOL bIgnoreVertices, 
    DWORD dwFlags, 
    DWORD* rgdwAdjacencyOut, 
    DWORD* faceRemap, 
    LPD3DXBUFFER *ppbufVertexRemap,  
    LPD3DXMESH* ppCloneMesh
    )
{	
    HRESULT hr = S_OK;
    LPD3DXMESH pTempMesh = NULL;
    DWORD iattr;
    DWORD iFaceEnd;
    DWORD iFace;
    DWORD *rgdwAdjacency = NULL;
    DWORD iFaceStart;

    hr = m_tmTriMesh.LockIB((PBYTE*)&m_tmTriMesh.m_pFaces);
    if (FAILED(hr))
        return hr;

    // need to set all unused faces to truly be unused for optimization purposes
    for (iattr = 0; iattr < m_tmTriMesh.m_caeAttributeTable; iattr++)
    {
        iFaceEnd = m_rgaeAttributeTableFull[iattr].FaceStart + m_rgaeAttributeTableFull[iattr].FaceCount;
        iFaceStart = m_rgaeAttributeTableFull[iattr].FaceStart + m_tmTriMesh.m_rgaeAttributeTable[iattr].FaceCount;
        for (iFace = iFaceStart; iFace < iFaceEnd; iFace++)
        {
            m_tmTriMesh.m_pFaces[iFace].m_wIndices[0] = UNUSED;
            m_tmTriMesh.m_pFaces[iFace].m_wIndices[1] = UNUSED;
            m_tmTriMesh.m_pFaces[iFace].m_wIndices[2] = UNUSED;
            m_tmTriMesh.m_rgpniNeighbors[iFace].m_iNeighbors[0] = UNUSED;
            m_tmTriMesh.m_rgpniNeighbors[iFace].m_iNeighbors[1] = UNUSED;
            m_tmTriMesh.m_rgpniNeighbors[iFace].m_iNeighbors[2] = UNUSED;
        }
    }

    hr = m_tmTriMesh.UnlockIB();
    if (FAILED(hr))
        return hr;

    // the ignore vertices path should never set dwFlags
    GXASSERT(!bIgnoreVertices || (dwFlags == 0));

    // expand the attribute indices into attribute ids
    m_tmTriMesh.m_rgiAttributeIds = new DWORD[m_tmTriMesh.m_cMaxFaces];
    if (m_tmTriMesh.m_rgiAttributeIds == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    m_tmTriMesh.m_dwOptionsInt &= ~D3DXMESHINT_ATTRIBINDEX;
    m_tmTriMesh.m_dwOptionsInt |= D3DXMESHINT_ATTRIBID;

    for (iattr = 0; iattr < m_tmTriMesh.m_caeAttributeTable; iattr++)
    {
        iFaceEnd = m_tmTriMesh.m_rgaeAttributeTable[iattr].FaceStart + m_tmTriMesh.m_rgaeAttributeTable[iattr].FaceCount;
        for (iFace = m_tmTriMesh.m_rgaeAttributeTable[iattr].FaceStart; iFace < iFaceEnd; iFace++)
        {
            m_tmTriMesh.m_rgiAttributeIds[iFace] = m_tmTriMesh.m_rgaeAttributeTable[iattr].AttribId;
        }
    }

    hr = m_tmTriMesh.CloneMeshFVF(dwOptionsNew, dwFVFNew, pD3DDevice, &pTempMesh);
    if (FAILED(hr))
        goto e_Exit;

    // if doing an optimization that needs adjacency, or the user wants adjacency back
    if ((rgdwAdjacencyOut != NULL) || (dwFlags & (D3DXMESHOPT_VERTEXCACHE | D3DXMESHOPT_STRIPREORDER)))
    {
        rgdwAdjacency = new DWORD[m_tmTriMesh.m_cFaces * 3];
        if (rgdwAdjacency == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        hr = GetAdjacency(rgdwAdjacency);
        if (FAILED(hr))
            goto e_Exit;
    }

    if (bIgnoreVertices)
    {
        // remove deadspace from the mesh, but leave the vertices as they are
        hr = pTempMesh->Optimize(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_IGNOREVERTS, NULL, NULL, NULL, NULL, ppCloneMesh);
        if (FAILED(hr))
            goto e_Exit;
    }
    else
    {
        // if we should share the VB, make certain that the mesh with the unused faces removed still shares the VB
        if (dwOptionsNew & D3DXMESH_VB_SHARE)
        {
            // remove deadspace from the mesh
            hr = pTempMesh->Optimize(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_SHAREVB | dwFlags, rgdwAdjacency, rgdwAdjacencyOut, faceRemap, ppbufVertexRemap, ppCloneMesh);
            if (FAILED(hr))
                goto e_Exit;
        }
        else
        {
            // remove deadspace from the mesh
            hr = pTempMesh->Optimize(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | dwFlags, rgdwAdjacency, rgdwAdjacencyOut, faceRemap, ppbufVertexRemap, ppCloneMesh);
            if (FAILED(hr))
                goto e_Exit;
        }
    }

    // make certain that the vertex buffer is shared if it should be
#ifdef _DEBUG
    if (dwOptionsNew & D3DXMESH_VB_SHARE)
    {
        LPDIRECT3DVERTEXBUFFER8 pVBTest;
        (*ppCloneMesh)->GetVertexBuffer(&pVBTest);
        GXASSERT(pVBTest == m_tmTriMesh.m_pVBVertices);
    }
#endif

e_Exit:
    // remove them, should be done on the temp mesh, but then need to work around mesh types, etc
    m_tmTriMesh.m_dwOptionsInt |= D3DXMESHINT_ATTRIBINDEX;
    m_tmTriMesh.m_dwOptionsInt &= ~D3DXMESHINT_ATTRIBID;
    delete []m_tmTriMesh.m_rgiAttributeIds;
    delete []rgdwAdjacency;
    m_tmTriMesh.m_rgiAttributeIds = NULL;
    
    GXRELEASE(pTempMesh);

    return hr;
}

// -------------------------------------------------------------------------------
//  function    ClonePM
//
//   devnote    Generate a sibling progressive mesh, sharing data when possible
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::ClonePMesh
    (
    DWORD dwOptionsNew, 
    CONST DWORD *pDeclaration, 
    LPDIRECT3DDEVICE8 pD3DDevice, 
    LPD3DXPMESH* ppClonePMesh
    )
{
    DWORD dwFVF;
    HRESULT hr;

    if (pDeclaration == NULL)
    {
        dwFVF = m_tmTriMesh.m_dwFVF;
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

    return ClonePMeshFVF(dwOptionsNew, dwFVF, pD3DDevice, ppClonePMesh);
}

// -------------------------------------------------------------------------------
//  function    ClonePM
//
//   devnote    Generate a sibling progressive mesh, sharing data when possible
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::ClonePMeshFVF
    (
    DWORD dwOptionsNew, 
    DWORD dwFVFNew, 
    LPDIRECT3DDEVICE8 pD3DDevice, 
    LPD3DXPMESH* ppClonePMesh
    )
{	
    HRESULT hr = S_OK;
    GXHalfEdgePMesh<tp32BitIndex> *ptmNewPMesh32 = NULL;
    GXHalfEdgePMesh<tp16BitIndex> *ptmNewPMesh16 = NULL;
    ID3DXPMesh *ptmNewPMesh = NULL;
    LPDIRECT3DVERTEXBUFFER8 pVertexBuffer;

    if ((dwOptionsNew & ~D3DXMESH_VALIDCLONEBITS) != 0)
    {
        hr = D3DERR_INVALIDCALL;
        DPF(0, "ClonePMeshFVF: Invalid options bit passed to CloneMesh\n");
        goto e_Exit;
    }

    if ((dwOptionsNew & D3DXMESH_VB_SHARE) && (dwFVFNew != m_tmTriMesh.m_dwFVF))
    {
        hr = D3DERR_INVALIDCALL;
        DPF(0, "ClonePMeshFVF: VB_SHARE can only be specified if both meshes have identical FVF\n");
        goto e_Exit;
    }
    if ((dwOptionsNew & D3DXMESH_VB_SHARE) && (pD3DDevice != m_tmTriMesh.m_pD3DDevice))
    {
        DPF(0, "ClonePMeshFVF: VB_SHARE can only be specified if both pmeshes are on the same device\n");
        return D3DERR_INVALIDCALL;
    }

    if (dwOptionsNew & D3DXMESH_32BIT)
    {
        ptmNewPMesh32 = new GXHalfEdgePMesh<tp32BitIndex>(pD3DDevice, dwFVFNew, dwOptionsNew);
        if (ptmNewPMesh32 == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // link up to the shared data
        ptmNewPMesh32->m_pPMSharedData = m_pPMSharedData;
        m_pPMSharedData->AddRef();

        hr = m_tmTriMesh.CloneHelper32(&ptmNewPMesh32->m_tmTriMesh);
        if (FAILED(hr))
            goto e_Exit;

        if (dwOptionsNew & D3DXMESH_VB_SHARE)
        {
            // mark this mesh as shared as well
            m_tmTriMesh.m_dwOptionsInt |= D3DXMESHINT_SHAREDVB;
            GXASSERT(ptmNewPMesh32->m_tmTriMesh.BSharedVB());

            // share the vertex buffer from this mesh with the other mesh
            ptmNewPMesh32->m_tmTriMesh.m_pVBVertices = m_tmTriMesh.m_pVBVertices;
            m_tmTriMesh.m_pVBVertices->AddRef();
        }

        // setup the mispredicted material array
        ptmNewPMesh32->m_cMaterialNewMax = m_cMaterialNewMax;
        ptmNewPMesh32->m_cMaterialNewCur = m_cMaterialNewCur;
        ptmNewPMesh32->m_rgiMaterialNew = ptmNewPMesh32->m_pPMSharedData->m_rgiMaterialNew;
    
        // make sure that the attribute table was copied
        GXASSERT(ptmNewPMesh32->m_tmTriMesh.BHasAttributeTable());
        GXASSERT(ptmNewPMesh32->m_tmTriMesh.m_rgaeAttributeTable != NULL);

        ptmNewPMesh32->m_cBaseVertices = m_cBaseVertices;
        ptmNewPMesh32->m_cMaxVertices = m_cMaxVertices;
        ptmNewPMesh32->m_cMaxWedges = m_cMaxWedges;
        ptmNewPMesh32->m_cBaseWedges = m_cBaseWedges;
        ptmNewPMesh32->m_cCurrentWedges = m_cCurrentWedges;
        ptmNewPMesh32->m_cBaseFaces = m_cBaseFaces;
        ptmNewPMesh32->m_cCurrentFaces = m_cCurrentFaces;
        ptmNewPMesh32->m_cMaxFaces = m_cMaxFaces;
        ptmNewPMesh32->m_iCurPos = m_iCurPos;


        // copy the array used for storing indices around the vertex collapsed
        ptmNewPMesh32->m_vsi.cpwLCLWtoRCCWMax = m_vsi.cpwLCLWtoRCCWMax;
        ptmNewPMesh32->m_vsi.rgpwLCLWtoRCCW = new UINT32*[ptmNewPMesh32->m_vsi.cpwLCLWtoRCCWMax];
        if (ptmNewPMesh32->m_vsi.rgpwLCLWtoRCCW == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // allocate the vsplit records
        ptmNewPMesh32->m_cvsVsplits = m_cvsVsplits;
        ptmNewPMesh32->m_rgvsVsplits = ptmNewPMesh32->m_pPMSharedData->m_rgvsVsplits;

        pVertexBuffer = ptmNewPMesh32->m_tmTriMesh.m_pVBVertices;
        ptmNewPMesh = ptmNewPMesh32;
        ptmNewPMesh32 = NULL;
    }
    else
    {
        if ((m_tmTriMesh.m_cFaces > UNUSED16) || (m_tmTriMesh.m_cVertices > UNUSED16))
        {
            hr = D3DERR_INVALIDCALL;
            goto e_Exit;
        }

        ptmNewPMesh16 = new GXHalfEdgePMesh<tp16BitIndex>(pD3DDevice, dwFVFNew, dwOptionsNew);
        if (ptmNewPMesh16 == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // link up to the shared data
        ptmNewPMesh16->m_pPMSharedData = m_pPMSharedData;
        m_pPMSharedData->AddRef();

        hr = m_tmTriMesh.CloneHelper16(&ptmNewPMesh16->m_tmTriMesh);
        if (FAILED(hr))
            goto e_Exit;

        if (dwOptionsNew & D3DXMESH_VB_SHARE)
        {
            // mark this mesh as shared as well
            m_tmTriMesh.m_dwOptionsInt |= D3DXMESHINT_SHAREDVB;
            GXASSERT(ptmNewPMesh16->m_tmTriMesh.BSharedVB());

            // share the vertex buffer from this mesh with the other mesh
            ptmNewPMesh16->m_tmTriMesh.m_pVBVertices = m_tmTriMesh.m_pVBVertices;
            m_tmTriMesh.m_pVBVertices->AddRef();
        }

        // setup the mispredicted material array
        ptmNewPMesh16->m_cMaterialNewMax = m_cMaterialNewMax;
        ptmNewPMesh16->m_cMaterialNewCur = m_cMaterialNewCur;
        ptmNewPMesh16->m_rgiMaterialNew = new UINT16[m_cMaterialNewMax];
        ptmNewPMesh16->m_rgiMaterialNew = ptmNewPMesh16->m_pPMSharedData->m_rgiMaterialNew;
    
        // make sure that the attribute table was copied
        GXASSERT(ptmNewPMesh16->m_tmTriMesh.BHasAttributeTable());
        GXASSERT(ptmNewPMesh16->m_tmTriMesh.m_rgaeAttributeTable != NULL);

        ptmNewPMesh16->m_cBaseVertices = m_cBaseVertices;
        ptmNewPMesh16->m_cMaxVertices = m_cMaxVertices;
        ptmNewPMesh16->m_cMaxWedges = m_cMaxWedges;
        ptmNewPMesh16->m_cBaseWedges = m_cBaseWedges;
        ptmNewPMesh16->m_cCurrentWedges = m_cCurrentWedges;
        ptmNewPMesh16->m_cBaseFaces = m_cBaseFaces;
        ptmNewPMesh16->m_cCurrentFaces = m_cCurrentFaces;
        ptmNewPMesh16->m_cMaxFaces = m_cMaxFaces;
        ptmNewPMesh16->m_iCurPos = m_iCurPos;


        // copy the array used for storing indices around the vertex collapsed
        ptmNewPMesh16->m_vsi.cpwLCLWtoRCCWMax = m_vsi.cpwLCLWtoRCCWMax;
        ptmNewPMesh16->m_vsi.rgpwLCLWtoRCCW = new UINT16*[ptmNewPMesh16->m_vsi.cpwLCLWtoRCCWMax];
        if (ptmNewPMesh16->m_vsi.rgpwLCLWtoRCCW == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // link to shared vsplit records
        ptmNewPMesh16->m_cvsVsplits = m_cvsVsplits;
        ptmNewPMesh16->m_rgvsVsplits = ptmNewPMesh16->m_pPMSharedData->m_rgvsVsplits;

        pVertexBuffer = ptmNewPMesh16->m_tmTriMesh.m_pVBVertices;
        ptmNewPMesh = ptmNewPMesh16;
        ptmNewPMesh16 = NULL;
    }

    // only do a copy if the VBs aren't shared
    if ( !(dwOptionsNew & D3DXMESH_VB_SHARE))
    {
        // now that the internals and face index data are loaded, load/convert the vertex data
        hr = m_tmTriMesh.CloneVertexBuffer(dwFVFNew, pVertexBuffer);
        if (FAILED(hr))
            goto e_Exit;
    }

    *ppClonePMesh = ptmNewPMesh;
    ptmNewPMesh = NULL;

e_Exit:
    GXRELEASE(ptmNewPMesh);
    GXRELEASE(ptmNewPMesh16);
    GXRELEASE(ptmNewPMesh32);

    return hr;
}


// -------------------------------------------------------------------------------
//  method    QueryInterface
//
//  devnote     Returns the requested interface, assuming it is supported
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::QueryInterface
    (
    REFIID riid, 
    LPVOID FAR *ppv
    )
{	
    *ppv=NULL;
    if (riid == IID_IUnknown)
        *ppv=(IUnknown*)this;
    else if (riid == IID_ID3DXBaseMesh)
        *ppv=(ID3DXBaseMesh*)this;
    else if (riid == IID_ID3DXPMesh)
        *ppv=(ID3DXPMesh*)this;
    else
        return E_NOINTERFACE;
    ((LPUNKNOWN)*ppv)->AddRef();

    return S_OK;
}



// -------------------------------------------------------------------------------
//  method    SetNumVertices
//
//  devnote     Does either vsplits or edge collapses to move the current number
//                  of vertices to the desired number of vertices
//          NOTE: it will cap the input value at the upper and lower bounds
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::SetNumVertices
    (
    DWORD cWedges
    )
{
    HRESULT hr = S_OK;
    
    hr = m_tmTriMesh.LockIB((PBYTE*)&m_tmTriMesh.m_pFaces);
    if (FAILED(hr))
        return hr;

    // make certain that we have a valid goal to achieve
    cWedges = min(cWedges, m_cMaxWedges);
    cWedges = max(cWedges, m_cBaseWedges);
    //iEndPos = cVertices - m_cBaseVertices;

    // if more vertices, perform the required number of splits
    if (cWedges > m_cCurrentWedges)
    {
        DWORD iMaxPos = m_cMaxVertices - m_cBaseVertices;
        while ((cWedges > m_cCurrentWedges) && (m_iCurPos < iMaxPos))
        {
            ApplyVSplit(m_rgvsVsplits[m_iCurPos]);
            m_iCurPos += 1;
        }

        // if we overshot our goal, then take a step back
        if (cWedges < m_cCurrentWedges)
        {
            m_iCurPos -= 1;
            UndoVSplit(m_rgvsVsplits[m_iCurPos]);
        }
    }
    // else peform the required number of collapses
    else if (cWedges < m_cCurrentWedges)
    { 
        while ((cWedges < m_cCurrentWedges) && (m_iCurPos > 0))
        {
            m_iCurPos -= 1;
            UndoVSplit(m_rgvsVsplits[m_iCurPos]);
        }
    }

    if (m_tmTriMesh.m_pFaces != NULL)
    {
        m_tmTriMesh.m_pFaces = NULL;
        m_tmTriMesh.UnlockIB();
    }

    GXASSERT(BValid());

    return hr;
}


// -------------------------------------------------------------------------------
//  method    GetMaxVertices
//
//  devnote     Returns maximum number of vertices in the mesh
//                  NOTE: Vertices not WEDGES
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
DWORD
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::GetMaxVertices()
{
    return m_cMaxWedges;
}


// -------------------------------------------------------------------------------
//  method    GetMinVertices
//
//  devnote     Returns minimum number of vertices in the mesh
//                  NOTE: Vertices not WEDGES
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
DWORD
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::GetMinVertices()
{
    return m_cBaseWedges;
}

// -------------------------------------------------------------------------------
//  method    GetMaxFaces
//
//  devnote     Returns maximum number of faces in the mesh
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
DWORD
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::GetMaxFaces()
{
    return m_cMaxFaces;
}

// -------------------------------------------------------------------------------
//  method    GetMinFaces
//
//  devnote     Returns minimum number of faces in the mesh
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
DWORD
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::GetMinFaces()
{
    return m_cBaseFaces;    
}


// -------------------------------------------------------------------------------
//  method    SetNumFaces
//
//  devnote     Sets the current number of faces, or as close as possible
//                  NOTE: currently not implemented!
//
//  returns     S_OK if suceeded, else error code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::SetNumFaces
    (
    DWORD cFaces
    )
{
    HRESULT hr = S_OK;
    
    hr = m_tmTriMesh.LockIB((PBYTE*)&m_tmTriMesh.m_pFaces);
    if (FAILED(hr))
        return hr;

    // make certain that we have a valid goal to achieve
    cFaces = min(cFaces, m_cMaxFaces);
    cFaces = max(cFaces, m_cBaseFaces);

    // if more vertices, perform the required number of splits
    if (cFaces > m_cCurrentFaces)
    {
        DWORD iMaxPos = m_cMaxVertices - m_cBaseVertices;
        while ((cFaces > m_cCurrentFaces) && (m_iCurPos < iMaxPos))
        {
            ApplyVSplit(m_rgvsVsplits[m_iCurPos]);
            m_iCurPos += 1;
        }

        // if we overshot our goal, then take a step back
        if (cFaces < m_cCurrentFaces)
        {
            m_iCurPos -= 1;
            UndoVSplit(m_rgvsVsplits[m_iCurPos]);
        }
    }
    // else peform the required number of collapses
    else if (cFaces < m_cCurrentFaces)
    { 
        while ((cFaces < m_cCurrentFaces) && (m_iCurPos > 0))
        {
            m_iCurPos -= 1;
            UndoVSplit(m_rgvsVsplits[m_iCurPos]);
        }
    }

    if (m_tmTriMesh.m_pFaces != NULL)
    {
        m_tmTriMesh.m_pFaces = NULL;
        m_tmTriMesh.UnlockIB();
    }

    GXASSERT(BValid());

    return hr;
}

#ifdef _DEBUG
// -------------------------------------------------------------------------------
//  method    BValid
//
//  devnote     Does a validity check of the progressive mesh
//                  asserts if anything is fishy
//
//  returns     returns false if not valid
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
bool
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::BValid() 
{ 
    return m_tmTriMesh.BValid();
}
#endif

// -------------------------------------------------------------------------------
//   Load and Save code
// -------------------------------------------------------------------------------

inline READ_DWORD(PBYTE &pbCur) 
{
    DWORD dwTemp = *(DWORD*)pbCur;
    pbCur += sizeof(DWORD);
    return dwTemp;
}

inline READ_WORD(PBYTE &pbCur) 
{
    WORD wTemp = *(WORD*)pbCur;
    pbCur += sizeof(WORD);
    return wTemp;
}

#define WRITE_DWORD(pbCur, dword) {*(DWORD*)pbCur = dword; pbCur += sizeof(DWORD); }
#define WRITE_WORD(pbCur,  word) {*(DWORD*)pbCur = word;  pbCur += sizeof(WORD);  }

HRESULT WINAPI D3DXSaveMeshToXEx
    (
    char *szFilename,
    LPD3DXMESH pMesh,
    CONST DWORD *rgdwAdjacency,
    CONST LPD3DXMATERIAL rgMaterials,
    DWORD cMaterials,
    DWORD xFormat,
    PBYTE pbPMData,
    DWORD cbPMData
    );

// -------------------------------------------------------------------------------
//  method    Save
//
//  devnote     Save a half edge progressive mesh to the given stream
//                  NOTE: currently NOT IMPLEMENTED
//
//  returns     returns false if not valid
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::Save
    (
    IStream *pstream, 
    D3DXMATERIAL *rgMaterials, 
    DWORD cMaterials
    )
{
#if 1
    return E_NOTIMPL;
#else
    HRESULT hr = S_OK;
    LPD3DXMESH ptmMesh = NULL;
    DWORD dwOptions;
    DWORD cVerticesOrig = m_cCurrentWedges;
    HANDLE hfile = INVALID_HANDLE_VALUE;
    DWORD cch;
    DWORD cbRead;
    DWORD cbWritten;
    DWORD iMat;
    DWORD iae;
    DWORD ivs;
    PBYTE pbData = NULL;
    PBYTE pbCur;
    DWORD cbData;
    //DWORD cTemp;

    char szTempPath[MAX_PATH];
    char *rgbBuffer = szTempPath;
    char szTempFileName[MAX_PATH];
    BOOL bFileCreated = FALSE;

    // goto the lowest LOD
    hr = SetNumVertices(1);
    if (FAILED(hr))
        goto e_Exit;

    // make a clone of the mesh, so that it is easier to save to a file
    dwOptions = m_tmTriMesh.m_dwOptions | D3DXMESH_SYSTEMMEM;
    GXASSERT(!(dwOptions & D3DXMESH_VB_SHARE));
    hr = CloneMeshInternal(dwOptions, m_tmTriMesh.m_dwFVF, m_tmTriMesh.m_pD3DDevice, TRUE, 0, NULL, NULL, NULL, &ptmMesh);
    if (FAILED(hr))
        goto e_Exit;

    cch = GetTempPath(sizeof(szTempPath), szTempPath);
    if ((cch == 0) || (cch > sizeof(szTempPath)))
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    if (GetTempFileName(szTempPath, "pmx", 0, szTempFileName) == 0)
    {
        hr = E_FAIL;
        goto e_Exit;
    }
    bFileCreated = true;

    cbData = sizeof(DWORD) // nAttributes
             + m_tmTriMesh.m_caeAttributeTable*sizeof(DWORD)*6 // PM AttributeRanges
             + sizeof(DWORD) // nMaxValence
             + sizeof(DWORD) // nMinLogicalVertices
             + sizeof(DWORD) // nMaxLogicalVertices
             + sizeof(DWORD) // nVSplits
             + m_cvsVsplits * (sizeof(DWORD) + sizeof(DWORD) * 2) // rgVSplits
             + sizeof(DWORD) // nMaterial mispredicts
             + m_cMaterialNewMax * sizeof(DWORD);  // material mispredicts

    pbData = pbCur = new BYTE[cbData];
    if (pbData == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // add the attribute table to the data
    WRITE_DWORD(pbCur, m_tmTriMesh.m_caeAttributeTable);
    for (iae = 0; iae < m_tmTriMesh.m_caeAttributeTable; iae++)
    {
        WRITE_DWORD(pbCur, m_tmTriMesh.m_rgaeAttributeTable[iae].FaceStart);
        WRITE_DWORD(pbCur, m_tmTriMesh.m_rgaeAttributeTable[iae].FaceCount);
        WRITE_DWORD(pbCur, m_rgaeAttributeTableFull[iae].FaceCount);

        WRITE_DWORD(pbCur, m_tmTriMesh.m_rgaeAttributeTable[iae].VertexStart);
        WRITE_DWORD(pbCur, m_tmTriMesh.m_rgaeAttributeTable[iae].VertexCount);
        WRITE_DWORD(pbCur, m_rgaeAttributeTableFull[iae].VertexCount);
    }

    // append the max valence used for internal buffer sizes
    WRITE_DWORD(pbCur, m_vsi.cpwLCLWtoRCCWMax);

    // write logical vertex values that are hard to recompute from data in attribute table
    WRITE_DWORD(pbCur, m_cBaseVertices);
    WRITE_DWORD(pbCur, m_cMaxVertices);

    WRITE_DWORD(pbCur, m_cvsVsplits);
    for (ivs = 0; ivs < m_cvsVsplits; ivs++)
    {
        WRITE_DWORD(pbCur, m_rgvsVsplits[ivs].m_iFlclw);
         WRITE_DWORD(pbCur, m_rgvsVsplits[ivs].m_oVlrOffset);
         WRITE_DWORD(pbCur, m_rgvsVsplits[ivs].m_code);
    }

    WRITE_DWORD(pbCur, m_cMaterialNewMax);
    for (iMat = 0; iMat < m_cMaterialNewMax; iMat++)
    {
        WRITE_DWORD(pbCur, m_rgiMaterialNew[iMat]);
    } 

    hr = D3DXSaveMeshToXEx(szTempFileName, ptmMesh, NULL, rgMaterials, cMaterials, DXFILEFORMAT_BINARY, pbData, cbData);
    if (FAILED(hr))
        goto e_Exit;


    // now reopen the file to write it to the stream
    hfile = CreateFile(szTempFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile == INVALID_HANDLE_VALUE)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // copy the file into the stream
    while (1)
    {
        if (ReadFile(hfile, rgbBuffer, MAX_PATH, &cbRead, NULL) == 0)
        {
            hr = E_FAIL;
            goto e_Exit;
        }

        if (cbRead == 0)
            break;

        hr = pstream->Write(rgbBuffer, cbRead, &cbWritten);
        if (FAILED(hr))
            goto e_Exit;
        if (cbWritten < cbRead)
        {
            hr = E_FAIL;
            goto e_Exit;
        }
    }

e_Exit:
    GXRELEASE(ptmMesh);
    delete []pbData;

    // reset LOD on entry
    if (m_iCurPos != cVerticesOrig)
    {
        SetNumVertices(cVerticesOrig);
    }

    if (hfile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hfile);
    }

    if (bFileCreated)
    {
        DeleteFile(szTempFileName);
    }

    return hr;
#endif
}

// -------------------------------------------------------------------------------
//  method    LoadFromData
//
//  devnote     Load a half edge progressive mesh from the data provided
//                  NOTE: INTERNAL function, used by D3DXCreatePMeshFromStream
//
//  returns     returns false if not valid
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::LoadFromData
    (
    LPD3DXMESH ptmD3DXMesh, 
    DWORD *rgdwAdjacency, 
    PBYTE pbPMData,
    DWORD cbPMData
    )
{
    HRESULT hr = S_OK;
    DWORD iMat;
    DWORD iae;
    DWORD ivs;
    DWORD iFace;
    DWORD iPoint;
    PBYTE pbCur = pbPMData;
    DWORD dwMaterial;
    DWORD *pdwAdjacency;
    LPDIRECT3DVERTEXBUFFER8 pvbSrc = NULL;
    LPDIRECT3DINDEXBUFFER8  pibSrc = NULL;
    PBYTE pbPointsSrc = NULL;
    PBYTE pbPointsDest = NULL;
    UINT_IDX *pwFaceSrc;
    UINT_IDX *rgwFacesSrc = NULL;
    GXTri3Face<UINT_IDX> *rgwFacesDest = NULL;
    D3DXATTRIBUTERANGE *pae;
    DWORD iFaceEnd;
    DWORD iCurEnd;
    DWORD iPrevEnd;
    DWORD iaeSearch;
    DWORD iCurFileAttribEnd;
    DWORD iPrevFileAttribEnd;


    GXASSERT(ptmD3DXMesh != NULL);
    GXASSERT(rgdwAdjacency != NULL);
    GXASSERT(pbPMData != NULL);
    GXASSERT(cbPMData > 0);

    // make certain that the mesh is the same from
    if ((ptmD3DXMesh->GetOptions() & D3DXMESH_32BIT) != (GetOptions() & D3DXMESH_32BIT))
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    // allocate the object to handle shared data
    m_pPMSharedData = new CPMSharedData;
    if (m_pPMSharedData == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // allocate the attribute tables
    m_tmTriMesh.m_caeAttributeTable = READ_DWORD(pbCur);
    m_tmTriMesh.m_rgaeAttributeTable = new D3DXATTRIBUTERANGE[m_tmTriMesh.m_caeAttributeTable];
    m_rgaeAttributeTableFull = new D3DXATTRIBUTERANGE[m_tmTriMesh.m_caeAttributeTable];
    if ((m_tmTriMesh.m_rgaeAttributeTable == NULL) || (m_rgaeAttributeTableFull == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // load the attribute tables
    m_cMaxWedges = 0;
    m_cMaxFaces = 0;
    m_cBaseWedges = 0;
    for (iae = 0; iae < m_tmTriMesh.m_caeAttributeTable; iae++)
    {
        m_tmTriMesh.m_rgaeAttributeTable[iae].AttribId = iae;
        m_rgaeAttributeTableFull[iae].AttribId = iae;

        m_tmTriMesh.m_rgaeAttributeTable[iae].FaceStart = READ_DWORD(pbCur);
        m_tmTriMesh.m_rgaeAttributeTable[iae].FaceCount = READ_DWORD(pbCur);
        m_rgaeAttributeTableFull[iae].FaceStart = m_tmTriMesh.m_rgaeAttributeTable[iae].FaceStart;
        m_rgaeAttributeTableFull[iae].FaceCount = READ_DWORD(pbCur);

        m_tmTriMesh.m_rgaeAttributeTable[iae].VertexStart = READ_DWORD(pbCur);
        m_tmTriMesh.m_rgaeAttributeTable[iae].VertexCount = READ_DWORD(pbCur);
        m_rgaeAttributeTableFull[iae].VertexStart = m_tmTriMesh.m_rgaeAttributeTable[iae].VertexStart;
        m_rgaeAttributeTableFull[iae].VertexCount = READ_DWORD(pbCur);

        m_cMaxWedges += m_rgaeAttributeTableFull[iae].VertexCount;
        m_cMaxFaces += m_rgaeAttributeTableFull[iae].FaceCount;
        m_cBaseWedges += m_tmTriMesh.m_rgaeAttributeTable[iae].VertexCount;
    }

    // init internal vars
    m_cBaseFaces = ptmD3DXMesh->GetNumFaces();
    m_cCurrentFaces = m_cBaseFaces;
    m_iCurPos = 0;
    m_cCurrentWedges = m_cBaseWedges;

    // read the max valence used for internal buffer sizes
    m_vsi.cpwLCLWtoRCCWMax = READ_DWORD(pbCur);
    m_vsi.rgpwLCLWtoRCCW = new UINT_IDX*[m_vsi.cpwLCLWtoRCCWMax];
    if (m_vsi.rgpwLCLWtoRCCW == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    
    m_cBaseVertices = READ_DWORD(pbCur);
    m_cMaxVertices = READ_DWORD(pbCur);

    // allocate the vsplit records
    m_cvsVsplits = READ_DWORD(pbCur);
    m_rgvsVsplits = new CHalfEdgeVSplit[m_cvsVsplits];
    m_pPMSharedData->m_rgvsVsplits = m_rgvsVsplits;
    if (m_rgvsVsplits == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // load the vsplit records
    for (ivs = 0; ivs < m_cvsVsplits; ivs++)
    {
        m_rgvsVsplits[ivs].m_iFlclw = READ_DWORD(pbCur);
        m_rgvsVsplits[ivs].m_oVlrOffset = READ_DWORD(pbCur);
        m_rgvsVsplits[ivs].m_code = READ_DWORD(pbCur);
    }

    // allocate the material mispredict array
    m_cMaterialNewMax = READ_DWORD(pbCur);
    m_rgiMaterialNew = new UINT16[m_cMaterialNewMax];
    m_pPMSharedData->m_rgiMaterialNew = m_rgiMaterialNew;
    if (m_rgiMaterialNew == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    for (iMat = 0; iMat < m_cMaterialNewMax; iMat++)
    {
        dwMaterial = READ_DWORD(pbCur);

        if (dwMaterial > UNUSED16)
        {
            hr = D3DERR_INVALIDCALL;
            goto e_Exit;
        }

        m_rgiMaterialNew[iMat] = (UINT16)dwMaterial;
    } 
    
    // resize the mesh in the pm to be large enough for max LOD
    hr = m_tmTriMesh.Resize(m_cMaxFaces, m_cMaxWedges);
    if (FAILED(hr))
        goto e_Exit;

    // next get all the src/dest face and vertex pointers
    hr = ptmD3DXMesh->GetIndexBuffer(&pibSrc);
    if (FAILED(hr))
        goto e_Exit;

    hr = pibSrc->Lock(0, 0, (PBYTE*)&rgwFacesSrc, 0 );
    if (FAILED(hr))
        goto e_Exit;

    hr = m_tmTriMesh.LockIB((PBYTE*)&rgwFacesDest);
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmD3DXMesh->GetVertexBuffer(&pvbSrc);
    if (FAILED(hr))
        goto e_Exit;

    hr = pvbSrc->Lock(0, 0, &pbPointsSrc, 0 );
    if (FAILED(hr))
        goto e_Exit;

    hr = m_tmTriMesh.LockVB(&pbPointsDest);
    if (FAILED(hr))
        goto e_Exit;

    // now copy the face data, splitting it into the various attribute pieces
    pdwAdjacency = rgdwAdjacency;
    pwFaceSrc = rgwFacesSrc;
    iPrevFileAttribEnd = 0;
    iCurFileAttribEnd = 0;
    for (iae = 0; iae < m_tmTriMesh.m_caeAttributeTable; iae++)
    {
        pae = &m_tmTriMesh.m_rgaeAttributeTable[iae];

        // first copy the data in the base mesh into the proper attribute section
        iFaceEnd = pae->FaceStart + pae->FaceCount;
        iPrevFileAttribEnd = iCurFileAttribEnd;
        iCurFileAttribEnd += pae->FaceCount;
        for (iFace = pae->FaceStart; iFace < iFaceEnd; iFace++)
        {
            // setup material indices
            m_tmTriMesh.m_rgiMaterialIndex[iFace] = (UINT16)iae;

            // now copy the neighbor and face data
            for (iPoint = 0; iPoint < 3; iPoint++)
            {
                if (*pdwAdjacency == UNUSED32)
                    m_tmTriMesh.m_rgpniNeighbors[iFace].m_iNeighbors[iPoint] = UNUSED;
                else if (((UINT_IDX)*pdwAdjacency >= iPrevFileAttribEnd) && ((UINT_IDX)*pdwAdjacency < iCurFileAttribEnd))
                    m_tmTriMesh.m_rgpniNeighbors[iFace].m_iNeighbors[iPoint] = (UINT_IDX)*pdwAdjacency - (UINT_IDX)iPrevFileAttribEnd + (UINT_IDX)m_rgaeAttributeTableFull[iae].FaceStart;
                else
                {
                    iCurEnd = 0;
                    for (iaeSearch = 0; iaeSearch < m_tmTriMesh.m_caeAttributeTable; iaeSearch++)
                    {
                        iPrevEnd = iCurEnd;
                        iCurEnd += m_tmTriMesh.m_rgaeAttributeTable[iaeSearch].FaceCount;

                        if ((UINT_IDX)*pdwAdjacency < iCurEnd)
                        {
                            m_tmTriMesh.m_rgpniNeighbors[iFace].m_iNeighbors[iPoint] = (UINT_IDX)(*pdwAdjacency - iPrevEnd + m_rgaeAttributeTableFull[iaeSearch].FaceStart);
                            break;
                        }

                    }
                    GXASSERT(iaeSearch < m_tmTriMesh.m_caeAttributeTable);


                }

                pdwAdjacency += 1;

                rgwFacesDest[iFace].m_wIndices[iPoint] = *pwFaceSrc;
                pwFaceSrc += 1;
            }
        }

        // now intialize the rest of the data for the skipped section
        iFaceEnd = pae->FaceStart + m_rgaeAttributeTableFull[iae].FaceCount;
        for (iFace = pae->FaceStart + pae->FaceCount; iFace < iFaceEnd; iFace++)
        {
            // same as before, just a lookup to see what attribute it is in
            m_tmTriMesh.m_rgiMaterialIndex[iFace] = (UINT16)iae;

            // initialize skipped faces to unused
            for (iPoint = 0; iPoint < 3; iPoint++)
            {
                m_tmTriMesh.m_rgpniNeighbors[iFace].m_iNeighbors[iPoint] = UNUSED;
                rgwFacesDest[iFace].m_wIndices[iPoint] = UNUSED;
            }
        }
    }

    // the vertices are already set up in the correct order, just copy them
    memcpy(pbPointsDest, pbPointsSrc, m_tmTriMesh.m_cBytesPerVertex * m_tmTriMesh.m_cVertices);

    GXASSERT(m_tmTriMesh.BValid());

e_Exit:

    if (rgwFacesDest != NULL)
    {
        m_tmTriMesh.UnlockIB();
    }

    if (rgwFacesSrc != NULL)
    {
        pibSrc->Unlock();
    }
    GXRELEASE(pibSrc);

    if (pbPointsDest != NULL)
    {
        m_tmTriMesh.UnlockVB();
    }

    if (pbPointsSrc != NULL)
    {
        pvbSrc->Unlock();
    }
    GXRELEASE(pvbSrc);


    return hr;
}

// -------------------------------------------------------------------------------
//   VSplit and ECollapse code
// -------------------------------------------------------------------------------

// -------------------------------------------------------------------------------
//  method    GatherVSplitInfo
//
//  devnote   Collects information required for an vsplit into m_vsi
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::GatherVSplitInfo
    (
    CHalfEdgeVSplit &vsNext
    )
{
    CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(&m_tmTriMesh);
    UINT_IDX iOffset;
    UINT_IDX iFace;
    int iTemp;
	WORD code = vsNext.m_code;
    D3DXATTRIBUTERANGE *pae;

#ifdef _DEBUG
	UINT_IDX iVertexTest = UNUSED;
	DWORD cVerticesStart = m_tmTriMesh.m_cVertices;
#endif

    m_vsi.cpwLCLWtoRCCW = 0;

    // if the offset to the RCCW is greater than zero, then there is an R face
    m_vsi.bIsFaceR = vsNext.m_oVlrOffset > 0;

    // first find iFlclw
    m_vsi.iFaceLCLW = (UINT_IDX)vsNext.m_iFlclw;
    m_vsi.iEdgeLCLW = vsNext.IGetVsIndex();

    // next find the wedge of the vertex
    m_vsi.iVertex1 = m_tmTriMesh.m_pFaces[m_vsi.iFaceLCLW].m_wIndices[m_vsi.iEdgeLCLW];

    // find iFlccw
    m_vsi.iFaceLCCW = m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceLCLW].m_iNeighbors[MOD3(m_vsi.iEdgeLCLW+2)];
    if (m_vsi.iFaceLCCW != UNUSED)
    {
        m_vsi.iEdgeLCCW = FindEdge(m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceLCCW].m_iNeighbors, m_vsi.iFaceLCLW);
    }

    // determine the face material for the left face
    if (!(code & CHalfEdgeVSplit::FLN_MASK))
    {
        m_vsi.iMaterialIndexLeft = m_tmTriMesh.m_rgiMaterialIndex[m_vsi.iFaceLCLW];
    }
    else
    {
        GXASSERT(m_cMaterialNewCur < m_cMaterialNewMax);
        m_vsi.iMaterialIndexLeft  = m_rgiMaterialNew[m_cMaterialNewCur];
        m_cMaterialNewCur += 1;
    }

    // the face index is the next in the given material
    pae = &m_tmTriMesh.m_rgaeAttributeTable[m_vsi.iMaterialIndexLeft];
    m_vsi.iFaceL = (UINT_IDX)pae->FaceStart + ((UINT_IDX)pae->FaceCount);
    pae->FaceCount += 1;
    m_cCurrentFaces += 1;

    // if there is a right face, collect the info
    if (m_vsi.bIsFaceR)
    {
        // start at iFlclw and walk to the ccw on for the right
        fli.Init(m_vsi.iFaceLCLW, m_vsi.iVertex1, x_iClockwise);
        for (iOffset = 0; iOffset < vsNext.m_oVlrOffset; iOffset++) 
        {
            GXASSERT(!fli.BEndOfList());
            iFace = fli.GetNextFace();

#ifdef _DEBUG
			// if another Wedge was not found yet, then see if we are still on the original one
			//    once we switch off of the original one, we must always be on the second one
			if (iVertexTest == UNUSED)
			{			
				if (m_tmTriMesh.m_pFaces[iFace].m_wIndices[fli.IGetPointIndex()] != m_vsi.iVertex1)
					iVertexTest = m_tmTriMesh.m_pFaces[iFace].m_wIndices[fli.IGetPointIndex()];
			}
			else
			{
				GXASSERT(m_tmTriMesh.m_pFaces[iFace].m_wIndices[fli.IGetPointIndex()] == iVertexTest);
			}
#endif

            // store off a pointer to the wedge for the current face
            m_vsi.rgpwLCLWtoRCCW[iOffset] = &(m_tmTriMesh.m_pFaces[iFace].m_wIndices[fli.IGetPointIndex()]);
        }
        m_vsi.iFaceRCCW = iFace;
        m_vsi.iEdgeRCCW = fli.IGetPointIndex();

        // setup the cpwLCLWtoRCCW from VlrOffset
        m_vsi.cpwLCLWtoRCCW = vsNext.m_oVlrOffset;
        GXASSERT(m_vsi.cpwLCLWtoRCCW <= m_vsi.cpwLCLWtoRCCWMax);


        // if we are at the end of the list, then there is no CLW face for the right
        //   else, retrieve it
        if (fli.BEndOfList())
        {
            m_vsi.iFaceRCLW = UNUSED;
        }
        else
        {
            fli.PeekNextFace(m_vsi.iFaceRCLW, iTemp);
            m_vsi.iEdgeRCLW = iTemp;
        }

        // determine the face material for the right face
        if (!(code & CHalfEdgeVSplit::FRN_MASK))
        {
            m_vsi.iMaterialIndexRight = m_tmTriMesh.m_rgiMaterialIndex[m_vsi.iFaceRCCW];
        }
        else
        {
            GXASSERT(m_cMaterialNewCur < m_cMaterialNewMax);
            m_vsi.iMaterialIndexRight = m_rgiMaterialNew[m_cMaterialNewCur];
            m_cMaterialNewCur += 1;
        }

        // the face index is the next in the given material
        pae = &m_tmTriMesh.m_rgaeAttributeTable[m_vsi.iMaterialIndexRight];
        m_vsi.iFaceR = (UINT_IDX)pae->FaceStart + ((UINT_IDX)pae->FaceCount);
        pae->FaceCount += 1;
        m_cCurrentFaces += 1;


        // now generate the values of the wedges

		m_vsi.iWedgeVtfl = GetNextWedge(m_vsi.iMaterialIndexLeft);

		if (!(code & CHalfEdgeVSplit::T_LSAME) && ! (code & CHalfEdgeVSplit::S_LSAME) && !( (code & CHalfEdgeVSplit::S_CSAME) && (code & CHalfEdgeVSplit::S_RSAME)) )
			m_vsi.iWedgeVsfl = GetNextWedge(m_vsi.iMaterialIndexLeft);
		else if (code & CHalfEdgeVSplit::T_LSAME)
			m_vsi.iWedgeVsfl = m_vsi.iVertex1;
		else 
			m_vsi.iWedgeVsfl = m_tmTriMesh.m_pFaces[m_vsi.iFaceLCCW].m_wIndices[m_vsi.iEdgeLCCW];


		if (code & CHalfEdgeVSplit::L_NEW)
			m_vsi.iWedgeL = GetNextWedge(m_vsi.iMaterialIndexLeft);
		else if ( !(code & CHalfEdgeVSplit::L_BELOW) )
			m_vsi.iWedgeL = m_tmTriMesh.m_pFaces[m_vsi.iFaceLCLW].m_wIndices[MOD3(m_vsi.iEdgeLCLW+2)];
		else // BELOW
			m_vsi.iWedgeL = m_tmTriMesh.m_pFaces[m_vsi.iFaceLCCW].m_wIndices[MOD3(m_vsi.iEdgeLCCW+1)];


		if (code & CHalfEdgeVSplit::T_CSAME)
			m_vsi.iWedgeVtfr = m_vsi.iWedgeVtfl;
		else 
			m_vsi.iWedgeVtfr = GetNextWedge(m_vsi.iMaterialIndexRight);

		if (code & CHalfEdgeVSplit::S_CSAME)
			m_vsi.iWedgeVsfr = m_vsi.iWedgeVsfl;
		else if (!(code & CHalfEdgeVSplit::T_RSAME) && !(code & CHalfEdgeVSplit::S_RSAME))
			m_vsi.iWedgeVsfr = GetNextWedge(m_vsi.iMaterialIndexRight);
		else if (code & CHalfEdgeVSplit::T_RSAME)
			m_vsi.iWedgeVsfr = m_tmTriMesh.m_pFaces[m_vsi.iFaceRCCW].m_wIndices[m_vsi.iEdgeRCCW];
		else 
			m_vsi.iWedgeVsfr = m_tmTriMesh.m_pFaces[m_vsi.iFaceRCLW].m_wIndices[m_vsi.iEdgeRCLW];


		if (code & CHalfEdgeVSplit::R_NEW)
			m_vsi.iWedgeR = GetNextWedge(m_vsi.iMaterialIndexRight);
		else if ( !(code & CHalfEdgeVSplit::R_BELOW) )
	        m_vsi.iWedgeR = m_tmTriMesh.m_pFaces[m_vsi.iFaceRCCW].m_wIndices[MOD3(m_vsi.iEdgeRCCW+1)];
		else // BELOW
			m_vsi.iWedgeR = m_tmTriMesh.m_pFaces[m_vsi.iFaceRCLW].m_wIndices[MOD3(m_vsi.iEdgeRCLW+2)];

		// if iFlclw shouldn't get iWedgeVtfl stamped on it (i.e. it should get iWedgeVtfr)
		//		then change iVertex1 so that the check when applying the vsplit fails
		//			The only time this is necessary is if iFlclw's wedge is different from Fl's
		if (! (code & CHalfEdgeVSplit::T_LSAME) )
			m_vsi.iVertex1 = UNUSED;


        // sanity checks
        GXASSERT((code & CHalfEdgeVSplit::S_CSAME) || (m_vsi.iWedgeVsfr != m_vsi.iWedgeVsfl));
        GXASSERT((code & CHalfEdgeVSplit::T_CSAME) || (m_vsi.iWedgeVtfr != m_vsi.iWedgeVtfl));
    }
    else  // just record UNUSED
    {
        m_vsi.iFaceRCLW = UNUSED;
        m_vsi.iFaceRCCW = UNUSED;
        m_vsi.iFaceR = UNUSED;

        // Rotate around and record all wedges CLW from iWedgeLCLW.
        fli.Init(m_vsi.iFaceLCLW, m_vsi.iVertex1, x_iClockwise);
        while (!fli.BEndOfList())
        {
            iFace = fli.GetNextFace();

			// can only be one wedge above
			GXASSERT(m_tmTriMesh.m_pFaces[iFace].m_wIndices[fli.IGetPointIndex()] == m_vsi.iVertex1);

            // store off a pointer to the wedge for the current face
            m_vsi.rgpwLCLWtoRCCW[m_vsi.cpwLCLWtoRCCW] = &(m_tmTriMesh.m_pFaces[iFace].m_wIndices[fli.IGetPointIndex()]);
            m_vsi.cpwLCLWtoRCCW += 1;
        }

		m_vsi.iWedgeVtfl = GetNextWedge(m_vsi.iMaterialIndexLeft);

		if (!(code & CHalfEdgeVSplit::T_LSAME) && !(code & CHalfEdgeVSplit::S_LSAME))
			m_vsi.iWedgeVsfl = GetNextWedge(m_vsi.iMaterialIndexLeft);
		else
			m_vsi.iWedgeVsfl = m_vsi.iVertex1;

		if (code & CHalfEdgeVSplit::L_NEW)
			m_vsi.iWedgeL = GetNextWedge(m_vsi.iMaterialIndexLeft);
		else if ( !(code & CHalfEdgeVSplit::L_BELOW) )
			m_vsi.iWedgeL = m_tmTriMesh.m_pFaces[m_vsi.iFaceLCLW].m_wIndices[MOD3(m_vsi.iEdgeLCLW+2)];
		else // BELOW
			m_vsi.iWedgeL = m_tmTriMesh.m_pFaces[m_vsi.iFaceLCCW].m_wIndices[MOD3(m_vsi.iEdgeLCCW+1)];

#ifdef _DEBUG
		m_vsi.iWedgeVtfr = UNUSED;
		m_vsi.iWedgeVsfr = UNUSED;
		m_vsi.iWedgeR = UNUSED;
#endif
    }

#ifdef _DEBUG
    if (!m_vsi.bIsFaceR)
        m_vsi.iWedgeR = UNUSED;
#endif
}

// -------------------------------------------------------------------------------
//  method    ApplyVSplit
//
//  devnote     Apply the given vsplit record
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::ApplyVSplit
    (
    CHalfEdgeVSplit &vsNext
    )
{
    UINT_IDX ipw;

    // gather info into m_vsi member variable
    GatherVSplitInfo(vsNext);

    // update the neighbors that need to use the new vertex to the new vertex
    for (ipw = 0; ipw < m_vsi.cpwLCLWtoRCCW; ipw++)
    {
		// UNDONE - sometimes neds to be Vtfl and sometimes Vtfr
		if (*m_vsi.rgpwLCLWtoRCCW[ipw] == m_vsi.iVertex1)
			*m_vsi.rgpwLCLWtoRCCW[ipw] = m_vsi.iWedgeVtfl;
		else
			*m_vsi.rgpwLCLWtoRCCW[ipw] = m_vsi.iWedgeVtfr;
    }

    // setup the new triangles
    //   NOTE: if we have already had the LOD this high, then the data
    //       should already be in place
    m_tmTriMesh.m_pFaces[m_vsi.iFaceL].m_wIndices[0] = m_vsi.iWedgeVsfl;
    m_tmTriMesh.m_pFaces[m_vsi.iFaceL].m_wIndices[1] = m_vsi.iWedgeVtfl;
    m_tmTriMesh.m_pFaces[m_vsi.iFaceL].m_wIndices[2] = m_vsi.iWedgeL;

    m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceL].m_iNeighbors[0]=m_vsi.iFaceR;
    m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceL].m_iNeighbors[1]=m_vsi.iFaceLCLW;
    m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceL].m_iNeighbors[2]=m_vsi.iFaceLCCW;

    m_tmTriMesh.m_rgiMaterialIndex[m_vsi.iFaceL] = m_vsi.iMaterialIndexLeft;

    // update the adjanceny of other neighbors
    if (m_vsi.iFaceLCCW != UNUSED) 
        m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceLCCW].m_iNeighbors[m_vsi.iEdgeLCCW]=m_vsi.iFaceL;

    if (m_vsi.iFaceLCLW != UNUSED) 
        m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceLCLW].m_iNeighbors[MOD3(m_vsi.iEdgeLCLW+2)]=m_vsi.iFaceL;

    // add in the right face if present
    if (m_vsi.bIsFaceR)
    {
        m_tmTriMesh.m_pFaces[m_vsi.iFaceR].m_wIndices[0] = m_vsi.iWedgeVsfr;
        m_tmTriMesh.m_pFaces[m_vsi.iFaceR].m_wIndices[1] = m_vsi.iWedgeR;
        m_tmTriMesh.m_pFaces[m_vsi.iFaceR].m_wIndices[2] = m_vsi.iWedgeVtfr;

        m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceR].m_iNeighbors[0]=m_vsi.iFaceRCLW;
        m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceR].m_iNeighbors[1]=m_vsi.iFaceRCCW;
        m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceR].m_iNeighbors[2]=m_vsi.iFaceL;

        m_tmTriMesh.m_rgiMaterialIndex[m_vsi.iFaceR] = m_vsi.iMaterialIndexRight;

        // update the adjanceny of other neighbors
        if (m_vsi.iFaceRCCW != UNUSED) 
            m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceRCCW].m_iNeighbors[m_vsi.iEdgeRCCW]=m_vsi.iFaceR;

        if (m_vsi.iFaceRCLW != UNUSED) 
            m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceRCLW].m_iNeighbors[MOD3(m_vsi.iEdgeRCLW+2)]=m_vsi.iFaceR;
    }    
}


// -------------------------------------------------------------------------------
//  method    GatherECollapseInfo
//
//  devnote   Collects information required for an edge collapse into m_vsi
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::GatherECollapseInfo
    (
    CHalfEdgeVSplit &vsLast
    )
{
	WORD code = vsLast.m_code;
    DWORD cVertexIndices;

    // if the offset to the RCCW is greater than zero, then there is an R face
    m_vsi.bIsFaceR = vsLast.m_oVlrOffset > 0;

    // update the mispredict array
    GXASSERT(((code & CHalfEdgeVSplit::FLN_MASK) >> CHalfEdgeVSplit::FLN_SHIFT) <= 1);
    m_cMaterialNewCur -= (code & CHalfEdgeVSplit::FLN_MASK) >> CHalfEdgeVSplit::FLN_SHIFT;

    // the flclw is in the vsplit record
    m_vsi.iFaceLCLW = (UINT_IDX)vsLast.m_iFlclw;
    m_vsi.iEdgeLCLW = vsLast.IGetVsIndex();

    // next find the wedge of the vertex
    m_vsi.iVertex1 = m_tmTriMesh.m_pFaces[m_vsi.iFaceLCLW].m_wIndices[m_vsi.iEdgeLCLW];

    // find iFaceL
    m_vsi.iFaceL = m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceLCLW].m_iNeighbors[MOD3(m_vsi.iEdgeLCLW+2)];

    // get the material index and update the face count
    m_vsi.iMaterialIndexLeft = m_tmTriMesh.m_rgiMaterialIndex[m_vsi.iFaceL];
    m_tmTriMesh.m_rgaeAttributeTable[m_vsi.iMaterialIndexLeft].FaceCount -= 1;
    m_cCurrentFaces -= 1;

    m_vsi.iFaceR = m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceL].m_iNeighbors[0];

    // if there is a right face, load the right info
    if (m_vsi.bIsFaceR)
    {
        GXASSERT(m_vsi.iFaceR != UNUSED);

        // get the material index and update the face count
        m_vsi.iMaterialIndexRight = m_tmTriMesh.m_rgiMaterialIndex[m_vsi.iFaceR];
        m_tmTriMesh.m_rgaeAttributeTable[m_vsi.iMaterialIndexRight].FaceCount -= 1;
        m_cCurrentFaces -= 1;

        // update the mispredict array
        GXASSERT(((code & CHalfEdgeVSplit::FRN_MASK) >> CHalfEdgeVSplit::FRN_SHIFT) <= 1);
        m_cMaterialNewCur -= (code & CHalfEdgeVSplit::FRN_MASK) >> CHalfEdgeVSplit::FRN_SHIFT;

        // known info from how the triangle was created
        m_vsi.iFaceRCLW = m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceR].m_iNeighbors[0];
        m_vsi.iFaceRCCW = m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceR].m_iNeighbors[1];

		m_vsi.iWedgeVsfr = m_tmTriMesh.m_pFaces[m_vsi.iFaceR].m_wIndices[0];
		m_vsi.iWedgeVtfr = m_tmTriMesh.m_pFaces[m_vsi.iFaceR].m_wIndices[2];

        // get the pointer to the left material vertex indices count
        //pcVertexIndices = &m_tmTriMesh.m_rgaeAttributeTable[m_vsi.iMaterialIndexLeft].VertexCount;
        cVertexIndices =1 ;

		if (!(code & CHalfEdgeVSplit::T_LSAME) && ! (code & CHalfEdgeVSplit::S_LSAME) && !( (code & CHalfEdgeVSplit::S_CSAME) && (code & CHalfEdgeVSplit::S_RSAME)) )
			cVertexIndices += 1;

		if (code & CHalfEdgeVSplit::L_NEW)
			cVertexIndices += 1;

        m_tmTriMesh.m_rgaeAttributeTable[m_vsi.iMaterialIndexLeft].VertexCount -= cVertexIndices; 
        m_cCurrentWedges -= cVertexIndices;

        // get the pointer to the right material vertex indices count
        //pcVertexIndices = &m_tmTriMesh.m_rgaeAttributeTable[m_vsi.iMaterialIndexRight].VertexCount;
        cVertexIndices = 0;

		if (!(code & CHalfEdgeVSplit::T_CSAME))
			cVertexIndices += 1;

		if ( !(code & CHalfEdgeVSplit::S_CSAME) && !(code & CHalfEdgeVSplit::T_RSAME) && !(code & CHalfEdgeVSplit::S_RSAME))
			cVertexIndices += 1;

		if (code & CHalfEdgeVSplit::R_NEW)
			cVertexIndices += 1;

        m_tmTriMesh.m_rgaeAttributeTable[m_vsi.iMaterialIndexRight].VertexCount -= cVertexIndices;
        m_cCurrentWedges -= cVertexIndices;
    }
    else
    {
        // get the pointer to the left material vertex indices count
        //pcVertexIndices = &m_tmTriMesh.m_rgaeAttributeTable[m_vsi.iMaterialIndexLeft].VertexCount;
        cVertexIndices = 1 ;

		if (!(code & CHalfEdgeVSplit::T_LSAME) && !(code & CHalfEdgeVSplit::S_LSAME))
			cVertexIndices += 1;

		if (code & CHalfEdgeVSplit::L_NEW)
			cVertexIndices += 1;

        m_tmTriMesh.m_rgaeAttributeTable[m_vsi.iMaterialIndexLeft].VertexCount -= cVertexIndices; 
        m_cCurrentWedges -= cVertexIndices;
    }

    // known info from how the triangle was created
    m_vsi.iFaceLCCW = m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceL].m_iNeighbors[2];

	m_vsi.iWedgeVsfl = m_tmTriMesh.m_pFaces[m_vsi.iFaceL].m_wIndices[0];
	m_vsi.iWedgeVtfl = m_tmTriMesh.m_pFaces[m_vsi.iFaceL].m_wIndices[1];
}

// -------------------------------------------------------------------------------
//  method    UndoVSplit
//
//  devnote     Undo the given vsplit record
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>::UndoVSplit
    (
    CHalfEdgeVSplit &vsLast
    )
{
    CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(&m_tmTriMesh);
    DWORD iEdge;
    DWORD iOffset;
    DWORD iFace;
	UINT_IDX *pwCur;

    // collect the info required for the edge collapse into m_vsi
    GatherECollapseInfo(vsLast);

    // UNDONE UNDONE craigp - could make this look like the vsplit version
    if (m_vsi.bIsFaceR)
    {
        // start at iFlclw and walk to the ccw on for the right
        //          to make things easier, I start at iFaceL and increment oVlrOffset by 1
        //              otherwise I need to know which wedge is present in iFlclw to rotate around
        fli.Init(m_vsi.iFaceL, m_vsi.iWedgeVtfl, x_iClockwise);
        for (iOffset = 0; iOffset < (DWORD)(vsLast.m_oVlrOffset+1); iOffset++) 
        {
            GXASSERT(!fli.BEndOfList());
            iFace = fli.GetNextFace();

			pwCur = &m_tmTriMesh.m_pFaces[iFace].m_wIndices[fli.IGetPointIndex()];

            // just update all references of iWedgeVtfl to iWedgeVsfl and the same for
				//								iWedgeVtfr to iWedgeVsfr
			if (*pwCur == m_vsi.iWedgeVtfl)
				*pwCur = m_vsi.iWedgeVsfl;
			else
				*pwCur = m_vsi.iWedgeVsfr;
        }
    }
    else // no FaceR, but might be other triangles sharing iWedgeVtfl with iFlclw
    {
        // Rotate around and record all wedges CLW from iWedgeLCLW.
        fli.Init(m_vsi.iFaceL, m_vsi.iWedgeVtfl, x_iClockwise);
        while (!fli.BEndOfList())
        {
            iFace = fli.GetNextFace();

			pwCur = &m_tmTriMesh.m_pFaces[iFace].m_wIndices[fli.IGetPointIndex()];

            // just update all references of iWedgeVtfl to iWedgeVsfl and the same for
				//								iWedgeVtfr to iWedgeVsfr
			if (*pwCur == m_vsi.iWedgeVtfl)
				*pwCur = m_vsi.iWedgeVsfl;
			else
				*pwCur = m_vsi.iWedgeVsfr;
        }
    }

    // update the neighborhood to link around iFaceL
    if (m_vsi.iFaceLCCW != UNUSED) 
    {
        iEdge = FindEdge(m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceLCCW].m_iNeighbors, m_vsi.iFaceL);
        GXASSERT(iEdge < 3);

        m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceLCCW].m_iNeighbors[iEdge] = m_vsi.iFaceLCLW;
    }

    if (m_vsi.iFaceLCLW != UNUSED) 
    {
        iEdge = FindEdge(m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceLCLW].m_iNeighbors, m_vsi.iFaceL);
        GXASSERT(iEdge < 3);

        m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceLCLW].m_iNeighbors[iEdge] = m_vsi.iFaceLCCW;
    }


    // update the neighborhood to link around iFaceR
    if (m_vsi.bIsFaceR)
    {
        if (m_vsi.iFaceRCCW != UNUSED) 
        {
            iEdge = FindEdge(m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceRCCW].m_iNeighbors, m_vsi.iFaceR);
            GXASSERT(iEdge < 3);

            m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceRCCW].m_iNeighbors[iEdge] = m_vsi.iFaceRCLW;
        }

        if (m_vsi.iFaceRCLW != UNUSED) 
        {
            iEdge = FindEdge(m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceRCLW].m_iNeighbors, m_vsi.iFaceR);
            GXASSERT(iEdge < 3);

            m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceRCLW].m_iNeighbors[iEdge] = m_vsi.iFaceRCCW;
        }
    }

#ifdef _DEBUG
    // to avoid asserts, mark the faces removed as unused in debug
    m_tmTriMesh.m_pFaces[m_vsi.iFaceL].m_wIndices[0] = UNUSED;
    m_tmTriMesh.m_pFaces[m_vsi.iFaceL].m_wIndices[1] = UNUSED;
    m_tmTriMesh.m_pFaces[m_vsi.iFaceL].m_wIndices[2] = UNUSED;
    m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceL].m_iNeighbors[0] = UNUSED;
    m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceL].m_iNeighbors[1] = UNUSED;
    m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceL].m_iNeighbors[2] = UNUSED;

    if (m_vsi.bIsFaceR)
    {
        m_tmTriMesh.m_pFaces[m_vsi.iFaceR].m_wIndices[0] = UNUSED;
        m_tmTriMesh.m_pFaces[m_vsi.iFaceR].m_wIndices[1] = UNUSED;
        m_tmTriMesh.m_pFaces[m_vsi.iFaceR].m_wIndices[2] = UNUSED;
        m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceR].m_iNeighbors[0] = UNUSED;
        m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceR].m_iNeighbors[1] = UNUSED;
        m_tmTriMesh.m_rgpniNeighbors[m_vsi.iFaceR].m_iNeighbors[2] = UNUSED;
    }
#endif
}
