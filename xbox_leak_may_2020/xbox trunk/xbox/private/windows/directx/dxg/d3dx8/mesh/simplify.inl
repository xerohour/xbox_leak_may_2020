/*//////////////////////////////////////////////////////////////////////////////
//
// File: simplify.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
// @@BEGIN_MSINTERNAL
//
// History:
// -@-          (craigp)    - created 
// -@- 08/06/99 (mikemarr)  - prepend GX to all Luciform functions
//                          - started comment history
//
// @@END_MSINTERNAL
//
//////////////////////////////////////////////////////////////////////////////*/

#include "quadric.h"

const bool x_bGenerateHalfEdgePM = true;
const bool x_bRestrictForHalfEdgePM = true;

// maps of edge collapsed to how much index of points in triangle will
//   be after being generated from vsplit records
const int x_rgiLeftFaceEdgeAdjustment[3] = { 0, 2, 1 };
const int x_rgiRightFaceEdgeAdjustment[3] = { 2, 1, 0 };

const double x_fEpsilon = 1.0e-6;

// -------------------------------------------------------------------------------
//  class    CSimplificationHeap
//
//   devnote    implementation of a heap to priotize edges by cost
//
template <class UINT_IDX>
class CSimplificationHeap
{
public:
    CSimplificationHeap()
        :m_rgpeiHeap(NULL), m_cElements(0) {}

    ~CSimplificationHeap()
    {
        delete []m_rgpeiHeap;
    }

    HRESULT Init(UINT cElements);
    bool BValid();

    HRESULT Add(CEdgeInfo<UINT_IDX> *peiInfo);
    HRESULT Delete(CEdgeInfo<UINT_IDX> *peiInfo);
    HRESULT Update(CEdgeInfo<UINT_IDX> *peiInfo);
    CEdgeInfo<UINT_IDX> *PeiExtractMin();

#ifdef _DEBUG
    HRESULT DebugDump(NeighborInfo<UINT_IDX> *rgpniNeighbors);
#endif

private:
    void Switch(UINT iElement, UINT iOtherElement);
    void Adjust(UINT iElement, bool bUp, bool bDown);

    CEdgeInfo<UINT_IDX> **m_rgpeiHeap;
    UINT m_cElements;
    UINT m_cElementsMax;
};


// -------------------------------------------------------------------------------
//  struct    SEdgeCollapseContext
//
//   devnote    collection of data used to share between functions in GXSimplify::CollapseEdge
//
template <class UINT_IDX>
struct SEdgeCollapseContext
{
    UINT_IDX iFace1;
    UINT_IDX iEdge1;
    UINT_IDX iEdge1L;
    UINT_IDX iEdge1R;
    UINT_IDX iFace2;
    UINT_IDX iEdge2;
    UINT_IDX iEdge2L;
    UINT_IDX iEdge2R;
    UINT_IDX iNFace11;
    UINT_IDX iEdge11;
    UINT_IDX iNFace12;
    UINT_IDX iEdge12;
    UINT_IDX iNFace21;
    UINT_IDX iEdge21;
    UINT_IDX iNFace22;
    UINT_IDX iEdge22;
    UINT_IDX iPoint1;
    UINT_IDX iPoint2;
    UINT_IDX *pwNeighbors1;
    UINT_IDX *pwNeighbors2;
    D3DXVECTOR3 vPositionNew;
    UINT_IDX iWedge11;
    UINT_IDX iWedge12;
    UINT_IDX iWedge21;
    UINT_IDX iWedge22;
    UINT_IDX iWedgeL1;
    UINT_IDX iWedgeL2;
    UINT_IDX iWedgeR1;
    UINT_IDX iWedgeR2;
    UINT_IDX iWedgeRemove1;
    UINT_IDX iWedgeRemove2;
    UINT_IDX iWedgeRemove3;
    UINT_IDX iWedgeRemove4;
    UINT_IDX iWedgeRemove5;
    UINT_IDX iWedgeRemove6;
    UINT16 iMaterialLeft;
    UINT16 iMaterialRight;

    UINT ivsCurSplit;
};

// -------------------------------------------------------------------------------
//  class    CEdgeInfo
//
//   devnote    Encapsulate the idea of an edge in the mesh, used
//                  to put the edge in a priority heap and to access
//                  by the face/edge from the mesh
//
template <class UINT_IDX>
class CEdgeInfo
{

public:
    CEdgeInfo(UINT_IDX iEdge, UINT_IDX wFace, float dCost, D3DXVECTOR3 &vPosNew)
        :m_wFace(wFace), m_iEdge(iEdge), m_cost(dCost)
        {
        }

    UINT_IDX m_wFace;
    UINT_IDX m_iEdge;
    float m_cost;

    void SetHeapIndex(UINT iHeapIndex)      { m_iHeapIndex = iHeapIndex; }
    UINT IGetHeapIndex()                    { return m_iHeapIndex; }

    float DGetCost() { return m_cost; }

    // used to change ownership of the edge... i.e. two triangles are sharing
    //   an edge and one is removed.  the remaining one needs to make sure
    //   that the edge structure points at it.
    void SetOwner(UINT_IDX wFace, UINT_IDX iEdge)
    {
        m_wFace = wFace;
        m_iEdge = iEdge;
    }

private:
    UINT m_iHeapIndex;
};

// -------------------------------------------------------------------------------
//  function    Constructor for GXSimplifyMesh
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::GXSimplifyMesh( LPDIRECT3DDEVICE8 pD3DDevice, DWORD dwFVF, DWORD dwOptions)
   :m_cRef(1),
    m_cfvf(dwFVF),
    m_tmTriMesh(pD3DDevice, dwFVF, dwOptions | D3DXMESH_SYSTEMMEM),
    m_rgfeiEdges(NULL),
    m_rgiWedgeList(NULL),
    m_rgqVertexQuadrics(NULL),
    m_rgqFaceQuadrics(NULL),
    m_rgvFaceNormals(NULL),
    m_rgbVertexSeen(NULL),
    m_pheapCosts(NULL),
    m_rgvsSplits(NULL),
    m_rgbRemappedVSplit(NULL),
    m_cvsSplitsMax(0),
    m_cvsSplitCurFree(0),
    m_rgiFaceIndex(NULL),
    m_rgiEdgeAdjustment(NULL),
    m_pvHEVertexBuffer(NULL),
    m_rgiHEVertexBuffer(NULL),
    m_rgcvHEVertexBufferMax(NULL),
    m_rgcvHEVertexBufferFree(NULL),
    m_rgaeAttributeTableOrig(NULL),
    m_rgaeAttributeTableCur(NULL),
    m_rgiMaterialNew(NULL),
    m_cMaterialNewMax(0),
    m_cMaterialNewFree(0),
    m_rgbVertexDeleted(NULL),
    m_rgfVertexWeights(NULL),
    m_rgfTexWeights(NULL),
    m_cTexWeights(0),
    m_rgcolorDiffuse(NULL),
    m_rgcolorSpecular(NULL),
    m_cMaxFaces(0)
{
    m_tmTriMesh.m_punkOuter = (IUnknown*)this;

    m_tmTriMesh.m_dwOptionsInt |= D3DXMESHINT_FACEADJACENCY | D3DXMESHINT_POINTREP | D3DXMESHINT_ATTRIBID | D3DXMESHINT_ATTRIBINDEX;
}

// -------------------------------------------------------------------------------
//  function    destructor for GXSimplifyMesh
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::~GXSimplifyMesh()
{
    DWORD iFace;

    if (m_rgfeiEdges != NULL)
    {
        for (iFace = 0; iFace < m_cMaxFaces; iFace++)
        {
            delete m_rgfeiEdges[iFace].m_rgpeiEdges[0];
            delete m_rgfeiEdges[iFace].m_rgpeiEdges[1];
            delete m_rgfeiEdges[iFace].m_rgpeiEdges[2];
        }
        delete []m_rgfeiEdges;
    }

    delete m_pheapCosts;
    delete []m_rgiWedgeList;
    delete []m_rgqVertexQuadrics;
    delete []m_rgqFaceQuadrics;
    delete []m_rgvFaceNormals;

    delete []m_rgvsSplits;
    delete []m_pvHEVertexBuffer;
    delete []m_rgiHEVertexBuffer;
    delete []m_rgcvHEVertexBufferMax;
    delete []m_rgcvHEVertexBufferFree;
    delete []m_rgiMaterialNew;
    delete []m_rgaeAttributeTableOrig;
    delete []m_rgaeAttributeTableCur;
    delete []m_rgbRemappedVSplit;
    delete []m_rgiEdgeAdjustment;
    delete []m_rgiFaceIndex;
    delete []m_rgbVertexDeleted;
    delete []m_rgfVertexWeights;
    delete []m_rgfTexWeights;
    delete []m_rgcolorDiffuse;
    delete []m_rgcolorSpecular;

    delete []m_rgbVertexSeen;


}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
STDMETHODIMP_(ULONG) 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::AddRef(void)
{
    return ++m_cRef;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
STDMETHODIMP_(ULONG) 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::Release(void)
{
    if (--m_cRef != 0)
    {
        return m_cRef;
    }	
    
    delete this;
    return 0;
}


template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
STDMETHODIMP 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{	
    *ppv=NULL;
    if (riid == IID_IUnknown)
        *ppv=(IUnknown*)this;
    else if (riid == IID_ID3DXSPMesh)
        *ppv=(ID3DXSPMesh*)this;
    else
        return E_NOINTERFACE;
    ((LPUNKNOWN)*ppv)->AddRef();

    return S_OK;
}

// -------------------------------------------------------------------------------
//  function    ReduceVertices
//
//   devnote    Set new number of current vertices
//              Since this is mesh simplify, only does anything if this number decreases
//                  Performs edge collapses until the current number of vertices
//                  reaches the requested number
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::ReduceVertices
    (
    DWORD cVertices
    )
{
    HRESULT hr = S_OK;
    PBYTE pvPoints = NULL;
    CEdgeInfo<UINT_IDX> *peiInfo;

	unsigned int iFP;

    // make sure that we are in double precision
	iFP = _controlfp(0,0);
	_controlfp(_PC_53,_MCW_PC );

    hr = m_tmTriMesh.LockIB((PBYTE*)&m_tmTriMesh.m_pFaces);
    if (FAILED(hr))
        goto e_Exit;

#if 0
    if (0)
    {
        m_pheapCosts->DebugDump(m_tmTriMesh.m_rgpniNeighbors);
    }

    if (0)
    {
        delete []m_rgfeiEdges;
        m_rgfeiEdges = NULL;
        delete m_pheapCosts;
        m_pheapCosts = NULL;

        hr = CreateHeap();
        if (FAILED(hr))
            goto e_Exit;
    }
#endif

    // can't increase the number of vertices, so just return if asked to go back up
    if (cVertices >= m_cCurrentWedges)
    {
        // return success... they can ask for the number to go up, but is automatically
        //   "capped" just like with a PM
        goto e_Exit;
    }    

    // lock the points now to avoid looking for failures later
    hr = m_tmTriMesh.LockVB(&pvPoints);
    if (FAILED(hr))
    {
        goto e_Exit;
    }

    // collapsed edges until the number of vertices is correct
    while (m_cCurrentWedges > cVertices)
    {
#if 0
        DWORD cWedgesBefore = m_cCurrentWedges;
#endif
        peiInfo = m_pheapCosts->PeiExtractMin();

        if (peiInfo == NULL)
            break;

        hr = CollapseEdge(peiInfo, pvPoints);
        if (FAILED(hr))
            goto e_Exit;

#if 0
        if (cWedgesBefore != m_cCurrentWedges)
        {
            hr = ResetHeap();
            if (FAILED(hr))
                goto e_Exit;
        }
#endif
    }
    
    GXASSERT(BValid());
e_Exit:

    if (pvPoints != NULL)
    {
        HRESULT hrTest = m_tmTriMesh.UnlockVB();
        GXASSERT(!FAILED(hrTest));
        pvPoints = NULL;
    }

    if (m_tmTriMesh.m_pFaces != NULL)
    {
        m_tmTriMesh.UnlockIB();
    }

    // make sure that we restore the precision desired
	_controlfp(iFP,_MCW_PC );

    return hr;
}


// -------------------------------------------------------------------------------
//  function    GetMaxVertices
//
//   devnote    returns the maximum vertices possible in the mesh
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
DWORD
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::GetMaxVertices()
{
    return m_cMaxVertices;
}

// -------------------------------------------------------------------------------
//  function    GetNumVertices
//
//   devnote    returns the current number of vertices in the mesh
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
DWORD
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::GetNumVertices()
{
    return m_cCurrentWedges;
}

// -------------------------------------------------------------------------------
//  function    GetNumFaces
//
//   devnote    returns the current number of the facesin the mesh
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
DWORD 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::GetNumFaces()
{
    return m_cCurrentFaces;
}


// -------------------------------------------------------------------------------
//  function    GetMaxFaces
//
//   devnote    returns the maximum number of faces possible in the mesh
//                  with a Simplification mesh this is always the current number of faces
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
DWORD 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::GetMaxFaces()
{
    return m_cMaxFaces;
}

// -------------------------------------------------------------------------------
//  function    SetNumFaces
//
//   devnote    SetNumFaces, similar to SetNumVertices, sets the current number of faces
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::ReduceFaces
    (
    DWORD cFaces
    )
{
    HRESULT hr = S_OK;
    PBYTE pvPoints = NULL;
    CEdgeInfo<UINT_IDX> *peiInfo;

	unsigned int iFP;

    // make sure that we are in double precision
	iFP = _controlfp(0,0);
	_controlfp(_PC_53,_MCW_PC );

    hr = m_tmTriMesh.LockIB((PBYTE*)&m_tmTriMesh.m_pFaces);
    if (FAILED(hr))
        goto e_Exit;

    // can't increase the number of vertices, so just return if asked to go back up
    if (cFaces >= m_cCurrentFaces)
    {
        // return success... they can ask for the number to go up, but is automatically
        //   "capped" just like with a PM
        goto e_Exit;
    }    

    // lock the points now to avoid looking for failures later
    hr = m_tmTriMesh.LockVB(&pvPoints);
    if (FAILED(hr))
    {
        goto e_Exit;
    }

    // collapsed edges until the number of vertices is correct
    while (m_cCurrentFaces > cFaces)
    {
        peiInfo = m_pheapCosts->PeiExtractMin();

        if (peiInfo == NULL)
            break;

        hr = CollapseEdge(peiInfo, pvPoints);
        if (FAILED(hr))
            goto e_Exit;
    }
    
    GXASSERT(BValid());
e_Exit:

    if (pvPoints != NULL)
    {
        HRESULT hrTest = m_tmTriMesh.UnlockVB();
        GXASSERT(!FAILED(hrTest));
        pvPoints = NULL;
    }

    if (m_tmTriMesh.m_pFaces != NULL)
    {
        m_tmTriMesh.UnlockIB();
    }

    // make sure that we restore the precision desired
	_controlfp(iFP,_MCW_PC );

    return hr;
}

// -------------------------------------------------------------------------------
//  function    CreateEmptyMesh
//
//   devnote    internal function used to get rid of existing information
//                  and start anew
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::CreateEmptyMesh()
{
    delete []m_rgfeiEdges;
    m_rgfeiEdges = NULL;

    delete []m_rgiWedgeList;
    m_rgiWedgeList = NULL;

    delete []m_rgqVertexQuadrics;
    m_rgqVertexQuadrics = NULL;

    delete []m_rgqFaceQuadrics;
    m_rgqFaceQuadrics = NULL;

    delete []m_rgvFaceNormals;
    m_rgvFaceNormals = NULL;

    delete []m_rgvsSplits;
    m_rgvsSplits = NULL;

    delete []m_pvHEVertexBuffer;
    m_pvHEVertexBuffer = NULL;

    delete []m_rgiHEVertexBuffer;
    m_rgiHEVertexBuffer = NULL;

    delete []m_rgiMaterialNew;
    m_rgiMaterialNew = NULL;

    delete []m_rgaeAttributeTableOrig;
    m_rgaeAttributeTableOrig = NULL;

    delete []m_rgaeAttributeTableCur;
    m_rgaeAttributeTableCur = NULL;

    delete []m_rgbVertexDeleted;
    m_rgbVertexDeleted = NULL;

    delete []m_rgiFaceIndex;
    m_rgiFaceIndex = NULL;

    delete []m_rgiEdgeAdjustment;
    m_rgiEdgeAdjustment = NULL;

    delete []m_rgbVertexSeen;
    m_rgbVertexSeen = NULL;

    delete []m_rgfVertexWeights;
    m_rgfVertexWeights = NULL;

    delete []m_rgfTexWeights;
    m_rgfTexWeights = NULL;

    delete []m_rgcolorDiffuse;
    m_rgcolorDiffuse = NULL;

    delete []m_rgcolorSpecular;
    m_rgcolorSpecular = NULL;

    return m_tmTriMesh.CreateEmptyMesh();
}

// -------------------------------------------------------------------------------
//  function    ComputePointReps
//
//   devnote    Uses the face adjacency information to compute point reps for the mesh
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::ComputePointReps()
{
    UINT cVertices = m_tmTriMesh.m_cVertices;
    UINT cFaces = m_tmTriMesh.m_cFaces;
    UINT_IDX wPointRep;
    UINT_IDX wWedge;
    UINT_IDX *pwFace;
    UINT_IDX iCurFace;
    UINT iFace;
    UINT iPoint;
    CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(&m_tmTriMesh);
    GXASSERT(m_tmTriMesh.BHasPointRepData());

    // first set all point reps to the UNUSED value to know whether or not we've seen
    //   a vertex before
    memset(m_tmTriMesh.m_rgwPointReps, 0xff, sizeof(UINT_IDX) * cVertices);

    for (iFace = 0; iFace < cFaces; iFace++)
    {
        pwFace = m_tmTriMesh.m_pFaces[iFace].m_wIndices;

        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            wPointRep = pwFace[iPoint];
            if (wPointRep == UNUSED)
                continue;

            // if the point rep is unused, then we haven't circled this vertex yet
            if (m_tmTriMesh.m_rgwPointReps[wPointRep] == UNUSED)
            {
                // now that we need to setup the point rep, make a round of the 
                //     faces, marking all wedges of the given vertex with the new pointrep
                fli.Init(iFace, wPointRep, x_iAllFaces);
                while (!fli.BEndOfList())
                {
                    iCurFace = fli.GetNextFace();
                    
                    wWedge = m_tmTriMesh.m_pFaces[iCurFace].m_wIndices[fli.IGetPointIndex()];
                    m_tmTriMesh.m_rgwPointReps[wWedge] = wPointRep;
                }
            }
        }
    }

    return S_OK;
}


// -------------------------------------------------------------------------------
//  function    SetupWeightInfo
//
//   devnote    Use the user provided weights to setup simplification weight structures
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::SetupWeightInfo
    (
    CONST LPD3DXATTRIBUTEWEIGHTS pVertexAttributeWeights,
    CONST FLOAT *rgfVertexWeights,
    PBYTE pvPoints
    )
{
    HRESULT hr = S_OK;
    DWORD iVertex;
    DWORD iTexCoord;
    DWORD iWeight;
    BOOL bFound;
    DWORD rgdwTexCoordSizes[8];
    float *pfTexWeightCur;

    m_rgfVertexWeights = new float[m_tmTriMesh.m_cVertices];
    if (m_rgfVertexWeights == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // if the user provided vertex weights, then use them
    if (rgfVertexWeights != NULL)
    {
        memcpy(m_rgfVertexWeights, rgfVertexWeights, sizeof(float) * m_tmTriMesh.m_cVertices);
    }
    else  // no user provided weigthts, set tham all to 1
    {
        for (iVertex = 0; iVertex < m_tmTriMesh.m_cVertices; iVertex++)
        {
            m_rgfVertexWeights[iVertex] = 1.0f;
        }
    }

    // if the user specified component weights
    if (pVertexAttributeWeights != NULL)
    {
        memcpy(&m_AttributeWeights, pVertexAttributeWeights, sizeof(D3DXATTRIBUTEWEIGHTS));
    }
    else  // else use the default - just geometric and normal adjustment
    {
        memset(&m_AttributeWeights, 0, sizeof(D3DXATTRIBUTEWEIGHTS));

        m_AttributeWeights.Position = 1.0f;
        m_AttributeWeights.Normal = 1.0f;
        m_AttributeWeights.Boundary = 1.0f;
    }

    // include the internal weights for various components
    m_AttributeWeights.Normal *= (0.0004f * m_fMeshRadiusSq);
    m_AttributeWeights.Diffuse *= (0.01f * m_fMeshRadiusSq);
    m_AttributeWeights.Specular *= (0.01f * m_fMeshRadiusSq);
    m_AttributeWeights.Tex[0] *= (0.01f * m_fMeshRadiusSq);
    m_AttributeWeights.Tex[1] *= (0.01f * m_fMeshRadiusSq);
    m_AttributeWeights.Tex[2] *= (0.01f * m_fMeshRadiusSq);
    m_AttributeWeights.Tex[3] *= (0.01f * m_fMeshRadiusSq);
    m_AttributeWeights.Tex[4] *= (0.01f * m_fMeshRadiusSq);
    m_AttributeWeights.Tex[5] *= (0.01f * m_fMeshRadiusSq);
    m_AttributeWeights.Tex[6] *= (0.01f * m_fMeshRadiusSq);
    m_AttributeWeights.Tex[7] *= (0.01f * m_fMeshRadiusSq);

    // if there is a diffuse component, we need to convert to floating point
    if (m_cfvf.BDiffuse() && (m_AttributeWeights.Diffuse > 0.0f))
    {
        m_rgcolorDiffuse = new D3DXCOLOR[m_tmTriMesh.m_cVertices];
        if (m_rgcolorDiffuse == NULL)
        {
            hr = E_OUTOFMEMORY;
            return hr;
        }

        for (iVertex = 0; iVertex < m_tmTriMesh.m_cVertices; iVertex++)
        {
            m_rgcolorDiffuse[iVertex] = D3DXCOLOR(m_cfvf.ColorGetDiffuse(m_cfvf.GetArrayElem(pvPoints, iVertex)));
        }
    }

    // if there is a diffuse component, we need to convert to floating point
    if (m_cfvf.BSpecular() && (m_AttributeWeights.Specular > 0.0f))
    {
        m_rgcolorSpecular = new D3DXCOLOR[m_tmTriMesh.m_cVertices];
        if (m_rgcolorSpecular == NULL)
        {
            hr = E_OUTOFMEMORY;
            return hr;
        }

        for (iVertex = 0; iVertex < m_tmTriMesh.m_cVertices; iVertex++)
        {
            m_rgcolorSpecular[iVertex] = D3DXCOLOR(m_cfvf.ColorGetSpecular(m_cfvf.GetArrayElem(pvPoints, iVertex)));
        }
    }

    // if there are texture coordinates, might need to setup weight array
    if (m_cfvf.CTexCoords() > 0)
    {
        // see if there are any weights for the texture coordinates provided
        bFound = FALSE;
        for (iTexCoord = 0; iTexCoord < m_cfvf.CTexCoords(); iTexCoord++)
        {
            if (m_AttributeWeights.Tex[iTexCoord] > 0.0f)
            {
                bFound = TRUE;
                break;
            }
        }

        // if any weights found for existing texture coords, initialize internal arrays
        if (bFound)
        {
            m_cTexWeights = (m_cfvf.m_cBytesPerVertex - m_cfvf.m_oTex1) / sizeof(float);
            m_rgfTexWeights = new float[m_cTexWeights];
            if (m_rgfTexWeights == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
            memset(m_rgfTexWeights, 0, sizeof(float) * m_cTexWeights);

            m_cfvf.GetTexCoordSizes(rgdwTexCoordSizes);

            // now for each move tex coord level weights, to per float weights
            pfTexWeightCur = m_rgfTexWeights;
            for (iTexCoord = 0; iTexCoord < m_cfvf.CTexCoords(); iTexCoord++)
            {
                // convert from bytes to floats on individual sizes
                rgdwTexCoordSizes[iTexCoord] /= sizeof(float);

                for (iWeight = 0; iWeight < rgdwTexCoordSizes[iTexCoord]; iWeight++)
                {
                    *pfTexWeightCur = m_AttributeWeights.Tex[iTexCoord];
                    pfTexWeightCur += 1;
                }
            }

            GXASSERT(pfTexWeightCur == (m_rgfTexWeights + m_cTexWeights));
        }
    }

e_Exit:
    return hr;
}

// -------------------------------------------------------------------------------
//  function    ResetHeap
//
//   devnote    Reinitializes the priority heap from the current mesh
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::ResetHeap()
{
    HRESULT hr = S_OK;
    PBYTE pvPoints = NULL;

    // lock the points now to avoid looking for failures later
    hr = m_tmTriMesh.LockVB(&pvPoints);
    if (FAILED(hr))
        goto e_Exit;

    delete m_pheapCosts;
    m_pheapCosts = NULL;
    delete []m_rgfeiEdges;
    m_rgfeiEdges = NULL;

    hr = CreateHeap(pvPoints);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    if (pvPoints != NULL)
    {
        HRESULT hrTest = m_tmTriMesh.UnlockVB();
        GXASSERT(!FAILED(hrTest));
        pvPoints = NULL;
    }

    return hr;
}


// -------------------------------------------------------------------------------
//  function    SetupSimplfication
//
//   devnote    Sets up all information required to start simplification
//                  used after the mesh has been loaded with data
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::SetupSimplification
    (
    GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED> *ptmSrcMesh, 
    CONST DWORD *rgdwAdjacency,
    CONST LPD3DXATTRIBUTEWEIGHTS pVertexAttributeWeights,
    CONST FLOAT *pVertexWeights
    )
{
    HRESULT hr = S_OK;
    PBYTE pvPoints = NULL;
    UINT iFace;
    UINT iae;
    UINT iPoint;
    UINT iEndFace;
	unsigned int iFP;
    UINT cFaces = ptmSrcMesh->GetNumFaces();
    UINT cVertices = ptmSrcMesh->GetNumVertices();
    CONST DWORD *pdwCurAdjacency;
    D3DXVECTOR3 vCenter;
    DWORD cVerticesBefore;

    // make sure that we are in double precision
	iFP = _controlfp(0,0);
	_controlfp(_PC_53,_MCW_PC );

    hr = CreateEmptyMesh();
    if (FAILED(hr))
        goto e_Exit;

    GXASSERT(ptmSrcMesh->BValid());

    // resize the buffers to copy the mesh into
    hr = m_tmTriMesh.Resize(cFaces, cVertices);
    if (FAILED(hr))
        goto e_Exit;

    // copy the data from the dest mesh locally;
    hr = m_tmTriMesh.CopyMesh(ptmSrcMesh);
    if (FAILED(hr))
        goto e_Exit;

    hr = m_tmTriMesh.LockIB((PBYTE*)&m_tmTriMesh.m_pFaces);
    if (FAILED(hr))
        goto e_Exit;

    //UNDONE UNDONE, if adjacency is changed to always be 32 bit, then 
    //   this is a memcpy
    for (iFace = 0, pdwCurAdjacency = rgdwAdjacency; iFace < cFaces; iFace++, pdwCurAdjacency+=3)
    {
        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            m_tmTriMesh.m_rgpniNeighbors[iFace].m_iNeighbors[iPoint] = (UINT_IDX)pdwCurAdjacency[iPoint];
        }
    }

    // this must happen before defragmenting the wedges, requires adjacency info
    hr = ComputePointReps();
    if (FAILED(hr))
        goto e_Exit;

    // NOTE: this must happen before locking the vertex buffer
    hr = DefragmentWedges();
    if (FAILED(hr))
        goto e_Exit;

    // do after Defragment, so that the wedges can be assigned to materials
    cVerticesBefore = m_tmTriMesh.m_cVertices;
    hr = m_tmTriMesh.OptimizeInternal(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL, NULL, m_tmTriMesh.m_dwFVF);
    if (FAILED(hr))
        goto e_Exit;

    if (cVerticesBefore < m_tmTriMesh.m_cVertices)
    {
        delete []m_rgbVertexSeen;
        m_rgbVertexSeen = new BYTE[m_tmTriMesh.m_cVertices];
        if (m_rgbVertexSeen == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        memset(m_rgbVertexSeen, 0,m_tmTriMesh.m_cVertices);
    }

    // even though we attribute sorted the mesh, it won't be optimal for long.  we want to
    //   maintain the attribute table to track the numbers of triangles left per attribute
    //   but it doesn't reflect the used vs unused parts of the mesh
    m_tmTriMesh.m_dwOptionsInt &= ~D3DXMESHINT_ATTRIBUTETABLE;

    // allocate a duplicate attribute table, to remember what the counts of the various
    //     entries were before simplification
    m_rgaeAttributeTableOrig = new D3DXATTRIBUTERANGE[m_tmTriMesh.m_caeAttributeTable];
    m_rgaeAttributeTableCur = new D3DXATTRIBUTERANGE[m_tmTriMesh.m_caeAttributeTable];
    if ((m_rgaeAttributeTableOrig == NULL) || (m_rgaeAttributeTableCur == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memcpy(m_rgaeAttributeTableOrig, m_tmTriMesh.m_rgaeAttributeTable, sizeof(D3DXATTRIBUTERANGE) * m_tmTriMesh.m_caeAttributeTable);
    memcpy(m_rgaeAttributeTableCur, m_tmTriMesh.m_rgaeAttributeTable, sizeof(D3DXATTRIBUTERANGE) * m_tmTriMesh.m_caeAttributeTable);

    // setup the material indices based on the attribute table.
    for (iae = 0; iae < m_tmTriMesh.m_caeAttributeTable; iae++)
    {
        iEndFace = m_tmTriMesh.m_rgaeAttributeTable[iae].FaceStart + (m_tmTriMesh.m_rgaeAttributeTable[iae].FaceCount);
        for (iFace = m_tmTriMesh.m_rgaeAttributeTable[iae].FaceStart; iFace < iEndFace; iFace++)
        {
            m_tmTriMesh.m_rgiMaterialIndex[iFace] = iae;
        }
    }

    hr = GenerateWedgeList();
    if (FAILED(hr))
        goto e_Exit;

    // call after GenerateWedgeList, so that we have the number of vertices
    //   in the mesh, to get a rough idea of the possible number of split records
    hr = AllocateVSplitRecords();
    if (FAILED(hr))
        goto e_Exit;

    // lock the points now to avoid looking for failures later
    hr = m_tmTriMesh.LockVB(&pvPoints);
    if (FAILED(hr))
        goto e_Exit;

    hr = D3DXComputeBoundingSphere(pvPoints, m_tmTriMesh.m_cVertices, m_tmTriMesh.m_dwFVF, &vCenter, &m_fMeshRadiusSq);
    if (FAILED(hr))
        goto e_Exit;
    m_fMeshRadiusSq *= m_fMeshRadiusSq;

    // this must happen after computing the bounding sphere, needs to include MeshRadiusSq
    hr = SetupWeightInfo(pVertexAttributeWeights, pVertexWeights, pvPoints);
    if (FAILED(hr))
        goto e_Exit;

#ifdef VERTEX_QUADRICS
    hr = GenerateInitialQuadrics(pvPoints);
    if (FAILED(hr))
        goto e_Exit;
#endif

    hr = CreateHeap(pvPoints);
    if (FAILED(hr))
        goto e_Exit;

    m_cCurrentFaces = 0;
    for (iFace = 0; iFace < m_tmTriMesh.m_cFaces; iFace++)
    {
        if (m_tmTriMesh.m_pFaces[iFace].m_wIndices[0] == UNUSED)
            continue;

        m_cCurrentFaces += 1;
    }

    m_cCurrentWedges = m_tmTriMesh.m_cVertices;
    m_cMaxLogicalVertices = m_cLogicalVertices;

    m_cMaxVertices = m_cCurrentWedges;
    m_cMaxFaces = m_cCurrentFaces;

    // if this mesh has more faces than allowable in a 16bit PM,
    //   then need to setup a variable to track the point in the simplification
    //   that we go below 2^16 faces to know how many vsplit records to copy in a ClonePM
    if (m_cCurrentFaces < UINT16UNUSED)
    {
        m_cvsMax16BitPos = m_cvsSplitCurFree;
        m_cvsMaxFaces16BitPos = m_cCurrentFaces;
    }
    else
    {
        m_cvsMax16BitPos = UINT32UNUSED;
        m_cvsMaxFaces16BitPos = UINT32UNUSED;
    }

    m_rgbVertexDeleted = new BYTE[m_tmTriMesh.m_cVertices];
    if (m_rgbVertexDeleted == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memset(m_rgbVertexDeleted, 0, sizeof(BYTE) * m_tmTriMesh.m_cVertices);

    GXASSERT(BValid());
e_Exit:
    if (pvPoints != NULL)
    {
        HRESULT hrTest = m_tmTriMesh.UnlockVB();
        GXASSERT(!FAILED(hrTest));
        pvPoints = NULL;
    }

    if (FAILED(hr))
    {
        CreateEmptyMesh();
    }


    if (m_tmTriMesh.m_pFaces != NULL)
    {
        m_tmTriMesh.UnlockIB();
    }

    // make sure that we restore the precision desired
	_controlfp(iFP,_MCW_PC );

    return hr;
}

// -------------------------------------------------------------------------------
//  function    FixupVSplitRecords
//
//   devnote    For each vsplit, check to see if the face it references was removed
//                  in a subsequent edge collapse, if so remap both the iFlclw and
//                  adjust vs_index to match where the face will be generated
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::FixupVSplitRecords()
{
    UINT cvs;
    UINT ivs;
    CHalfEdgeVSplit *pvs;
    UINT_IDX iPointOffset;

    // UNDONE - add a flag field to mark fixed up records, so that it is 
    //   possible to run this many times with only the fixups happening if required

    cvs = m_cvsSplitsMax - m_cvsSplitCurFree - 1;
    for (ivs = m_cvsSplitCurFree+1; ivs < m_cvsSplitsMax; ivs++)
    {
        pvs = &m_rgvsSplits[ivs];

        // fixup triangles that are no longer in the base mesh.  
        //   need to remap teh iFlclw to where the triangle will appear
        //   and change the vs_index field to be adjusted to where the points
        //   will be generated by a vsplit
        if (m_rgiFaceIndex[pvs->m_iFlclw] != UNUSED)
        {
            iPointOffset = ((pvs->m_code & CHalfEdgeVSplit::VSINDEX_MASK) >> CHalfEdgeVSplit::VSINDEX_SHIFT);
            iPointOffset = (iPointOffset + m_rgiEdgeAdjustment[pvs->m_iFlclw]) % 3;

            pvs->m_code &= ~CHalfEdgeVSplit::VSINDEX_MASK;
            pvs->m_code |= (iPointOffset) << CHalfEdgeVSplit::VSINDEX_SHIFT;

            pvs->m_iFlclw = m_rgiFaceIndex[pvs->m_iFlclw];

            m_rgbRemappedVSplit[ivs] = true;
        }
    }

    return S_OK;
}

// -------------------------------------------------------------------------------
//  function    RemapVSplitRecords
//
//   devnote    remap all vsplit records that reference faces in the "base" mesh
//                  a vsplit record refernces faces in the "base" mesh only
//                  if m_rgbRemappedVSplit[i] is false
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::RemapVSplitRecords
    (
    UINT *rgiFaceRemap
    )
{
    UINT cvs;
    UINT ivs;
    CHalfEdgeVSplit *pvs;

    cvs = m_cvsSplitsMax - m_cvsSplitCurFree - 1;

    for (ivs = m_cvsSplitCurFree+1; ivs < m_cvsSplitsMax; ivs++)
    {
        pvs = &m_rgvsSplits[ivs];

        if (!m_rgbRemappedVSplit[ivs])
        {
            pvs->m_iFlclw = rgiFaceRemap[pvs->m_iFlclw];
        }
    }

    return S_OK;
}

// -------------------------------------------------------------------------------
//  function    RemapInternals
//
//   devnote    remap all arrays that are aligned with the face and vertex arrays
//                  NOTE: this is NOT a GENERIC remap, only handles compact style remaps
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::RemapInternals
    (
    UINT *rgiVertRemap, 
    UINT cVerticesPreRemap, 
    UINT *rgiFaceRemap, 
    UINT cFacesPreRemap
    )
{
    UINT iFace;
    UINT iFaceNew;
    UINT iVertex;
    UINT iVertexNew;
    CEdgeInfo<UINT_IDX> *peiEdge;
    UINT iEdge;
    UINT_IDX *pwNeighbors;
    UINT iae;
    UINT iVertexLast;

    for (iFace = 0; iFace < cFacesPreRemap; iFace++)
    {
        iFaceNew = rgiFaceRemap[iFace];
        if ((iFaceNew != UNUSED) && (m_tmTriMesh.m_pFaces[iFaceNew].m_wIndices[0] != UNUSED))
        {
            if (iFaceNew == iFace)
                continue;

            // the face to be blown away should have had its edges freed already
            GXASSERT(m_rgfeiEdges[iFaceNew].m_rgpeiEdges[0] == NULL);
            GXASSERT(m_rgfeiEdges[iFaceNew].m_rgpeiEdges[1] == NULL);
            GXASSERT(m_rgfeiEdges[iFaceNew].m_rgpeiEdges[2] == NULL);

            m_rgfeiEdges[iFaceNew] = m_rgfeiEdges[iFace];
            pwNeighbors = m_tmTriMesh.m_rgpniNeighbors[iFaceNew].m_iNeighbors;
            for (iEdge = 0; iEdge < 3; iEdge++)
            {
                peiEdge = m_rgfeiEdges[iFaceNew].m_rgpeiEdges[iEdge];
                m_rgfeiEdges[iFace].m_rgpeiEdges[iEdge] = NULL;

                peiEdge->m_wFace = iFaceNew;
                peiEdge->m_iEdge = iEdge;
            }

#ifdef VERTEX_QUADRICS
            m_rgqFaceQuadrics[iFaceNew] = m_rgqFaceQuadrics[iFace];
            m_rgvFaceNormals[iFaceNew] = m_rgvFaceNormals[iFace];
#endif
        }
        else
        {
            for (iEdge = 0; iEdge < 3; iEdge++)
            {
                delete m_rgfeiEdges[iFace].m_rgpeiEdges[iEdge];
                m_rgfeiEdges[iFace].m_rgpeiEdges[iEdge] = NULL;
            }
        }
    }

    for (iVertex = 0; iVertex < cVerticesPreRemap; iVertex++)
    {
        iVertexNew = rgiVertRemap[iVertex];
        if ((iVertexNew != UNUSED) && (!m_rgbVertexDeleted[iVertex]))
        {
            m_rgbVertexSeen[iVertexNew] = m_rgbVertexSeen[iVertex];
#ifdef VERTEX_QUADRICS
            m_rgqVertexQuadrics[iVertexNew] = m_rgqVertexQuadrics[iVertex];
#endif
        }
    }

    // rebuild the vertex deleted array from the current attribute table
    memset(m_rgbVertexDeleted, 1, sizeof(BYTE) * m_tmTriMesh.m_cVertices);
    for (iae = 0; iae < m_tmTriMesh.m_caeAttributeTable; iae++)
    {
        iVertexLast = m_rgaeAttributeTableCur[iae].VertexStart + m_rgaeAttributeTableCur[iae].VertexCount;
        for (iVertex = m_rgaeAttributeTableCur[iae].VertexStart; iVertex < iVertexLast; iVertex++)
        {
            m_rgbVertexDeleted[iVertex] = 0;
        }
    }

    return S_OK;
}


// -------------------------------------------------------------------------------
//  function    RemapWedgeList
//
//   devnote    remap all the wedges lists using the given vertex map
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::RemapWedgeList
    (
    UINT *rgiVertRemap, 
    UINT_IDX cVerts
    )
{
    UINT iVert;

    // remap all the wedge list pointers in the wedge list
    for (iVert = 0; iVert < cVerts; iVert++)
    {
        m_rgiWedgeList[iVert] = rgiVertRemap[m_rgiWedgeList[iVert]];
    }

    return S_OK;
}

// -------------------------------------------------------------------------------
//  function    CloneMesh
//
//   devnote    Generate a triangle mesh from the current level of the simplification mesh
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::CloneMesh
    (
    DWORD dwOptions, 
    CONST DWORD *pDeclaration, 
    LPDIRECT3DDEVICE8 pD3DDevice, 
    DWORD *rgdwAdjacency, 
    DWORD *rgdwVertexRemap,
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

    return CloneMeshFVF(dwOptions, dwFVF, pD3DDevice, rgdwAdjacency, rgdwVertexRemap, ppCloneMesh);
}

// -------------------------------------------------------------------------------
//  function    CloneMesh
//
//   devnote    Generate a triangle mesh from the current level of the simplification mesh
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::CloneMeshFVF
    (
    DWORD dwOptions, 
    DWORD dwFVF, 
    LPDIRECT3DDEVICE8 pD3DDevice, 
    DWORD *rgdwAdjacency,
    DWORD *rgdwVertexRemap,
    LPD3DXMESH* ppCloneMesh
    )
{
    HRESULT hr = S_OK;
    LPD3DXMESH ptmMesh = NULL;
    LPD3DXMESH ptmMeshTemp;
    DWORD iEntry;
    DWORD cEntries;
    UINT_IDX *pwSrc;
    DWORD *pdwDest;
    DWORD *rgdwAdjacencyIn = NULL;
    LPD3DXBUFFER *ppbufVertRemap = NULL;
    LPD3DXBUFFER pbufVertRemap = NULL;

    if ((dwOptions & ~D3DXMESH_VALIDCLONEBITS) != 0)
    {
        DPF(0, "CloneMeshFVF: Invalid mesh option specified");
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }
    if ((dwOptions & D3DXMESH_VB_SHARE) != 0)
    {
        DPF(0, "CloneMeshFVF: D3DXMESH_VB_SHARE is not a valid option for CloneMesh on a simplification mesh");
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }
    if (ppCloneMesh == NULL)
    {
        DPF(0, "CloneMeshFVF: A destination pointer for the new mesh must be specified");
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }
    if (pD3DDevice == NULL)
    {
        DPF(0, "CloneMeshFVF: A destination device for the new mesh must be specified");
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }


    // if output adjacency is desired, compute a form of adjacency convenient for optimization
    if (rgdwAdjacency != NULL)
    {
        rgdwAdjacencyIn = new DWORD[m_tmTriMesh.m_cFaces * 3];
        if (rgdwAdjacencyIn == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

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
    }

    // if vertex remap desired, set the pbuf pointer so that optimize generates the info
    if (rgdwVertexRemap != NULL)
    {
        ppbufVertRemap = &pbufVertRemap;
    }

    // compact and create a clone at the same time
    hr = m_tmTriMesh.Optimize(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT, 
                                rgdwAdjacencyIn, rgdwAdjacency, NULL, ppbufVertRemap, &ptmMesh);
    if (FAILED(hr))
        goto e_Exit;

    // if any options changed, then we need to do a clone, to convert the compacted mesh
    //   to the right format
    if ((dwOptions != m_tmTriMesh.m_dwOptions) || (dwFVF != m_tmTriMesh.m_dwOptions)
        || (m_tmTriMesh.m_pD3DDevice != pD3DDevice))
    {
        hr = ptmMesh->CloneMeshFVF(dwOptions, dwFVF, pD3DDevice, &ptmMeshTemp);
        if (FAILED(hr))
            goto e_Exit;

        GXRELEASE(ptmMesh);
        ptmMesh = ptmMeshTemp;
    }

    // if the vertex remap is desired, copy out the useful entries from the buffer
    if (rgdwVertexRemap != NULL)
    {
        GXASSERT(pbufVertRemap != NULL);

        memcpy(rgdwVertexRemap, pbufVertRemap->GetBufferPointer(), sizeof(DWORD) * ptmMesh->GetNumVertices());
    }

    *ppCloneMesh = ptmMesh;
    ptmMesh = NULL;

e_Exit:
    GXRELEASE(ptmMesh);
    GXRELEASE(pbufVertRemap);
    delete []rgdwAdjacencyIn;
    return hr;
}

// -------------------------------------------------------------------------------
//  function    Compact
//
//   devnote    Move all faces and vertices left to the front of the 
//                  material group that they are in
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::Compact
    (
    UINT *rgiVertRemap, 
    UINT cVertexRemap, 
    UINT *rgiFaceRemap, 
    UINT cFaceRemap
    )
{
    HRESULT hr = S_OK;
    UINT *rgiFaceOffset = NULL;
    UINT *rgiVertexOffset = NULL;
    UINT iVert;
    UINT iae;
    UINT iFace;
    UINT_IDX *pwFace;
    UINT iOffset;
    UINT iEndOffset;

    // fixup the point reps, so that deleted points aren't misunderstood by Mesh.Reorder
    for (iVert = 0; iVert < m_tmTriMesh.m_cVertices; iVert++)
    {
        if (m_rgbVertexDeleted[iVert])
            m_tmTriMesh.m_rgwPointReps[iVert] = iVert;
    }

    // allocate arrays to store the next offset for each material (for both faces and vertices)
    rgiFaceOffset = new UINT[m_tmTriMesh.m_caeAttributeTable];
    rgiVertexOffset = new UINT[m_tmTriMesh.m_caeAttributeTable];
    if ((rgiFaceOffset == NULL) || (rgiVertexOffset == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // load the offset to start each material group at
    for (iae = 0; iae < m_tmTriMesh.m_caeAttributeTable; iae++)
    {
        rgiFaceOffset[iae] = m_rgaeAttributeTableOrig[iae].FaceStart;
        rgiVertexOffset[iae] = m_rgaeAttributeTableOrig[iae].VertexStart;
    }

   // only initialize the vertex array, the face array will be initialized
    //   by the first loop, which calculates where the faces move to
    for (iVert = 0; iVert < m_tmTriMesh.m_cVertices; iVert++)
    {
        rgiVertRemap[iVert] = UNUSED;
    }

    // first calculate how to remap the faces, and mark vertices 
    //   as used
    for (iFace = 0; iFace < m_tmTriMesh.m_cFaces; iFace++)
    {
        pwFace = m_tmTriMesh.m_pFaces[iFace].m_wIndices;

        if (pwFace[0] != UNUSED)
        {
            iae = m_tmTriMesh.m_rgiMaterialIndex[iFace];

            rgiFaceRemap[iFace] = rgiFaceOffset[iae];
            rgiFaceOffset[iae] += 1;

            // assert that only one material is using the vertices of this triangle
            GXASSERT((rgiVertRemap[pwFace[0]] == iae) || (rgiVertRemap[pwFace[0]] == UNUSED));
            GXASSERT((rgiVertRemap[pwFace[1]] == iae) || (rgiVertRemap[pwFace[1]] == UNUSED));
            GXASSERT((rgiVertRemap[pwFace[2]] == iae) || (rgiVertRemap[pwFace[2]] == UNUSED));

            // mark the vertices as in use
            rgiVertRemap[pwFace[0]] = iae;
            rgiVertRemap[pwFace[1]] = iae;
            rgiVertRemap[pwFace[2]] = iae;

        }
        else
        {
            rgiFaceRemap[iFace] = UNUSED;
        }
    }

    // next calculate the new positions of the vertices, and move them there
    for (iVert = 0; iVert < m_tmTriMesh.m_cVertices; iVert++)
    {
        if (rgiVertRemap[iVert] != UNUSED)
        {
            // stored the material for this index above
            iae = rgiVertRemap[iVert];

            rgiVertRemap[iVert] = rgiVertexOffset[iae];
            rgiVertexOffset[iae] += 1;

        }
    }

    // since Reorder doesn't allow "removed" vertices, remap all unused vertices
    //          to unused positions in the array
    iVert = 0;
    for (iae = 0; iae < m_tmTriMesh.m_caeAttributeTable; iae++)
    {       
        iEndOffset = m_rgaeAttributeTableOrig[iae].VertexStart + m_rgaeAttributeTableOrig[iae].VertexCount;
        for (iOffset = rgiVertexOffset[iae]; iOffset < iEndOffset; iOffset++)
        {
            while (rgiVertRemap[iVert] != UNUSED)
            {
                iVert += 1;
            }

            rgiVertRemap[iVert] = iOffset;
        }
    }

    // since Reorder doesn't allow "removed" faces, remap all unused faces
    //          to unused positions in the array
    iFace = 0;
    for (iae = 0; iae < m_tmTriMesh.m_caeAttributeTable; iae++)
    {       
        iEndOffset = m_rgaeAttributeTableOrig[iae].FaceStart + (m_rgaeAttributeTableOrig[iae].FaceCount);
        for (iOffset = rgiFaceOffset[iae]; iOffset < iEndOffset; iOffset++)
        {
            while (rgiFaceRemap[iFace] != UNUSED)
            {
                iFace += 1;
                GXASSERT(iFace < m_tmTriMesh.m_cFaces);
            }

            rgiFaceRemap[iFace] = iOffset;
        }
    }

    hr = m_tmTriMesh.Reorder(rgiVertRemap, cVertexRemap, rgiFaceRemap, cFaceRemap);
    if (FAILED(hr))
        goto e_Exit;

    // for a half edge PM, even UNUSED faces/vertices are "in" the mesh
    //   just not in the attribute table
    m_tmTriMesh.m_cFaces = cFaceRemap;
    m_tmTriMesh.m_cVertices = cVertexRemap;

#ifdef _DEBUG
    UINT cFacesNew;
    UINT cVerticesNew;

    // just check to make sure that the numbers of faces add up
    cFacesNew = 0;
    cVerticesNew = 0;
    for (iae = 0; iae < m_tmTriMesh.m_caeAttributeTable; iae++)
    {
        cFacesNew += (m_rgaeAttributeTableOrig[iae].FaceCount);
        cVerticesNew += m_rgaeAttributeTableOrig[iae].VertexCount;
    }

    GXASSERT(cFacesNew == cFaceRemap);
    GXASSERT(cVerticesNew == cVertexRemap);
#endif

e_Exit:
    delete []rgiFaceOffset;
    delete []rgiVertexOffset;

    return hr;
}

// -------------------------------------------------------------------------------
//  function    ClonePM
//
//   devnote    Generate a progressive mesh from this simplification mesh
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::ClonePMesh
    (
    DWORD options, 
    CONST DWORD *pDeclaration, 
    LPDIRECT3DDEVICE8 pD3DDevice, 
    DWORD *rgdwVertexRemap,
    LPD3DXPMESH* ppPMesh
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

    return ClonePMeshFVF(options, dwFVF, pD3DDevice, rgdwVertexRemap, ppPMesh);
}

// -------------------------------------------------------------------------------
//  function    ClonePM
//
//   devnote    Generate a progressive mesh from this simplification mesh
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::ClonePMeshFVF
    (
    DWORD options, 
    DWORD fvf, 
    LPDIRECT3DDEVICE8 pD3D, 
    DWORD *rgdwVertexRemapOut,
    LPD3DXPMESH* ppPMesh
    )
{
    HRESULT hr = S_OK;

    UINT *rgiVertRemap = NULL;
    DWORD cvsSplitsNew;
    CHalfEdgeVSplit *rgvsSplitsNew = NULL;
    DWORD cVerticesPreRemap;
    DWORD cFacesPreRemap;
    DWORD iFace;
    PBYTE pvPoints = NULL;
    DWORD iae;
    DWORD iEndFace;
    DWORD iOffset;
    DWORD iOffsetMax;
    DWORD iMaterial;
    GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED> *ptmHalfEdge = NULL;
    DWORD iVertexIndex;
    DWORD iVertex;

    if ((options & ~D3DXMESH_VALIDCLONEBITS) != 0)
    {
        DPF(0, "ClonePMeshFVF: Invalid mesh option specified");
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }
    if ((options & D3DXMESH_VB_SHARE) != 0)
    {
        DPF(0, "ClonePMeshFVF: D3DXMESH_VB_SHARE is not a valid option for CloneMesh on a simplification mesh");
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }
    if (ppPMesh == NULL)
    {
        DPF(0, "ClonePMeshFVF: A destination pointer for the new mesh must be specified");
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }
    if (pD3D == NULL)
    {
        DPF(0, "ClonePMeshFVF: A destination device for the new mesh must be specified");
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    rgiVertRemap = new UINT[m_tmTriMesh.m_cVertices];
    if (rgiVertRemap == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    *ppPMesh = NULL;

    hr = FixupVSplitRecords();
    if (FAILED(hr))
        goto e_Exit;

    cVerticesPreRemap = m_tmTriMesh.m_cVertices;
    cFacesPreRemap = m_tmTriMesh.m_cFaces;

    hr = Compact(rgiVertRemap, cVerticesPreRemap, m_rgiFaceIndex, cFacesPreRemap);
    if (FAILED(hr))
        goto e_Exit;

    hr = RemapVSplitRecords(m_rgiFaceIndex);
    if (FAILED(hr))
        goto e_Exit;

    hr = RemapInternals(rgiVertRemap, cVerticesPreRemap, m_rgiFaceIndex, cFacesPreRemap);
    if (FAILED(hr))
        goto e_Exit;

    // reinitialize internal arrays for remaping future vsplit records

    // can't use a memset, since rgiFaceIndex is always 32bit, but UNUSED is
    //   sometimes 16 and sometimes 32 bit (0xffff or 0xffffffff)
    for (iFace = 0; iFace < m_tmTriMesh.m_cFaces; iFace++)
    {
        m_rgiFaceIndex[iFace] = UNUSED;
    }   
    memset(m_rgiEdgeAdjustment, 0xff, sizeof(UINT_IDX) * m_tmTriMesh.m_cFaces);

    // UNDONE currently there are unused vertices in the wedge list, fix so that this is not the case
    //hr = RemapWedgeList(rgiVertRemap, cMaxVertices);
    //if (FAILED(hr))
        //goto e_Exit;

    if (rgdwVertexRemapOut != NULL)
    {
        // reinitialize all inverse pointers to point to UNUSED
        for (iVertexIndex = 0; iVertexIndex < cVerticesPreRemap; iVertexIndex++)
        {
            rgdwVertexRemapOut[iVertexIndex] = UNUSED32;
        }

        // fixup the RemapInverse array by rebuilding it
        for (iVertexIndex = 0; iVertexIndex < cVerticesPreRemap; iVertexIndex++)
        {
            iVertex = rgiVertRemap[iVertexIndex];

            if (iVertex != UNUSED32)
            {
                rgdwVertexRemapOut[iVertex] = iVertexIndex;
            }
        }

        // copy the removed vertices of each material back into the vertex buffer
        //    in the area reserved for vertices of that material
        for (iMaterial = 0; iMaterial < m_tmTriMesh.m_caeAttributeTable; iMaterial++)
        {
            iOffset = m_rgcvHEVertexBufferFree[iMaterial] + 1;
            iOffsetMax = m_rgcvHEVertexBufferMax[iMaterial];

            GXASSERT(iOffset == (m_rgaeAttributeTableCur[iMaterial].VertexStart 
                                    + m_rgaeAttributeTableCur[iMaterial].VertexCount));
#ifdef _DEBUG
            for (iVertexIndex = m_rgaeAttributeTableCur[iMaterial].VertexStart; iVertexIndex < iOffset; iVertexIndex++)
            {
                GXASSERT(rgdwVertexRemapOut[iVertexIndex] != UNUSED32);
            }
#endif

            if (iOffset < iOffsetMax)
            {
                memcpy(rgdwVertexRemapOut + iOffset, m_rgiHEVertexBuffer + iOffset, sizeof(DWORD) * (iOffsetMax - iOffset));
            }
        }
    }


    ptmHalfEdge = new GXHalfEdgePMesh<UINT_IDX,b16BitIndex,UNUSED>(m_tmTriMesh.m_pD3DDevice, m_cfvf.m_dwFVF, m_tmTriMesh.m_dwOptions);
    if (ptmHalfEdge == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // allocate the object to handle shared data
    ptmHalfEdge->m_pPMSharedData = new CPMSharedData;
    if (ptmHalfEdge->m_pPMSharedData == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // both faces and vertices are constant and at max for a pm, just the 
    //   counts in the attribute table change
    hr = ptmHalfEdge->m_tmTriMesh.Resize(m_cMaxFaces, m_tmTriMesh.m_cVertices);
    if (FAILED(hr))
        goto e_Exit;

    // use the generic cloning function, to allow for 16/32 bit differences
    hr = ptmHalfEdge->m_tmTriMesh.CopyMesh(&m_tmTriMesh);
    if (FAILED(hr))
        goto e_Exit;

    // need to append the rest of the vertex buffer
    hr = ptmHalfEdge->m_tmTriMesh.LockVB(&pvPoints);
    if (FAILED(hr))
        goto e_Exit;

    // copy the removed vertices of each material back into the vertex buffer
    //    in the area reserved for vertices of that material
    for (iMaterial = 0; iMaterial < m_tmTriMesh.m_caeAttributeTable; iMaterial++)
    {
        iOffset = m_rgcvHEVertexBufferFree[iMaterial] + 1;
        iOffsetMax = m_rgcvHEVertexBufferMax[iMaterial];

        GXASSERT(iOffset == (m_rgaeAttributeTableCur[iMaterial].VertexStart 
                                + m_rgaeAttributeTableCur[iMaterial].VertexCount));

        if (iOffset < iOffsetMax)
        {
            // copy prepared vertices for the rest of the vertex buffer
            memcpy((PBYTE)pvPoints + (iOffset * m_cfvf.m_cBytesPerVertex), 
                (PBYTE)m_pvHEVertexBuffer + (iOffset * m_cfvf.m_cBytesPerVertex),
                 (iOffsetMax - iOffset) * m_cfvf.m_cBytesPerVertex);
        }
    }

    pvPoints = NULL;
    hr = ptmHalfEdge->m_tmTriMesh.UnlockVB();
    if (FAILED(hr))
        goto e_Exit;

    // setup the mispredicted material array
    ptmHalfEdge->m_cMaterialNewMax = m_cMaterialNewMax - (m_cMaterialNewFree + 1);
    ptmHalfEdge->m_cMaterialNewCur = 0;
    ptmHalfEdge->m_rgiMaterialNew = new UINT16[ptmHalfEdge->m_cMaterialNewMax];
    ptmHalfEdge->m_pPMSharedData->m_rgiMaterialNew = ptmHalfEdge->m_rgiMaterialNew;
    if (ptmHalfEdge->m_rgiMaterialNew == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    memcpy(ptmHalfEdge->m_rgiMaterialNew, m_rgiMaterialNew + (m_cMaterialNewFree + 1), 
                            sizeof(UINT16) * ptmHalfEdge->m_cMaterialNewMax);

    // setup the material indices based on the attribute table.
    //  NOTE: use the original table to do this, so that we can intialize it
    for (iae = 0; iae < m_tmTriMesh.m_caeAttributeTable; iae++)
    {
        iEndFace = m_rgaeAttributeTableOrig[iae].FaceStart + (m_rgaeAttributeTableOrig[iae].FaceCount);
        for (iFace = m_rgaeAttributeTableOrig[iae].FaceStart; iFace < iEndFace; iFace++)
        {
            ptmHalfEdge->m_tmTriMesh.m_rgiMaterialIndex[iFace] = (UINT16)iae;
        }
    }

    // create an attribute table for the half edge PM
    ptmHalfEdge->m_tmTriMesh.m_rgaeAttributeTable = new D3DXATTRIBUTERANGE[m_tmTriMesh.m_caeAttributeTable];
    ptmHalfEdge->m_rgaeAttributeTableFull = new D3DXATTRIBUTERANGE[m_tmTriMesh.m_caeAttributeTable];
    if ((ptmHalfEdge->m_tmTriMesh.m_rgaeAttributeTable == NULL) || (ptmHalfEdge->m_rgaeAttributeTableFull == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // copy the current table to the half edge pm
    memcpy(ptmHalfEdge->m_tmTriMesh.m_rgaeAttributeTable, m_rgaeAttributeTableCur, 
                            sizeof(D3DXATTRIBUTERANGE) * m_tmTriMesh.m_caeAttributeTable);
    ptmHalfEdge->m_tmTriMesh.m_caeAttributeTable = m_tmTriMesh.m_caeAttributeTable;

    // copy of the full LOD attribute table
    memcpy(ptmHalfEdge->m_rgaeAttributeTableFull, m_rgaeAttributeTableOrig, 
                            sizeof(D3DXATTRIBUTERANGE) * m_tmTriMesh.m_caeAttributeTable);

    // setup member variables
    ptmHalfEdge->m_cBaseVertices = m_cLogicalVertices;
    ptmHalfEdge->m_cMaxVertices = m_cMaxLogicalVertices;
    ptmHalfEdge->m_cMaxWedges = m_tmTriMesh.m_cVertices;
    ptmHalfEdge->m_cBaseWedges = m_cCurrentWedges;
    ptmHalfEdge->m_cCurrentWedges = m_cCurrentWedges;
    ptmHalfEdge->m_cBaseFaces = m_cCurrentFaces;
    ptmHalfEdge->m_cCurrentFaces = m_cCurrentFaces;
    ptmHalfEdge->m_cMaxFaces = m_cMaxFaces;
    ptmHalfEdge->m_iCurPos = 0;

    // setup array - UNDONE UNDONE need to figure out maximum size
    // GXASSERT(0)
    ptmHalfEdge->m_vsi.cpwLCLWtoRCCWMax = 200;

    // NOTE: must change to work for 16/32 differences, etc.
    ptmHalfEdge->m_vsi.rgpwLCLWtoRCCW = new UINT_IDX*[ptmHalfEdge->m_vsi.cpwLCLWtoRCCWMax];
    if (ptmHalfEdge->m_vsi.rgpwLCLWtoRCCW == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // allocate the vsplit records
    cvsSplitsNew = m_cvsMax16BitPos - m_cvsSplitCurFree;
    ptmHalfEdge->m_cvsVsplits = cvsSplitsNew;
    ptmHalfEdge->m_rgvsVsplits = new CHalfEdgeVSplit[cvsSplitsNew];
    ptmHalfEdge->m_pPMSharedData->m_rgvsVsplits = ptmHalfEdge->m_rgvsVsplits;
    if (ptmHalfEdge->m_rgvsVsplits == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // make a copy of the vsplit data
    memcpy(ptmHalfEdge->m_rgvsVsplits, m_rgvsSplits + (m_cvsSplitCurFree + 1),
                            sizeof(CHalfEdgeVSplit) * cvsSplitsNew);

    GXASSERT(ptmHalfEdge->BValid());

    if ((options != m_tmTriMesh.m_dwOptions) || (fvf != m_cfvf.m_dwFVF) || (pD3D != m_tmTriMesh.m_pD3DDevice))
    {
        hr = ptmHalfEdge->ClonePMeshFVF(options, fvf, pD3D, ppPMesh);
        if (FAILED(hr))
        {
            DPF(0, "ID3DXSPMesh::ClonePMeshFVF: ClonePMeshFVF failed");
            goto e_Exit;
        }
        // This is same as delete [] ptmHalfEdge;
        ptmHalfEdge->Release();
    }
    else
    {
        *ppPMesh = ptmHalfEdge;
    }

    // HACK HACK - satisfies later asserts
    delete []m_rgiWedgeList;
    m_rgiWedgeList = NULL;

    hr = GenerateWedgeList();
    if (FAILED(hr))
        goto e_Exit;

    GXASSERT(BValid());

e_Exit:
    if (FAILED(hr))
    {
        delete ptmHalfEdge;
    }

    delete []rgiVertRemap;

    return hr;
}


// -------------------------------------------------------------------------------
//  function    AllocateVSplitRecords
//
//   devnote    Helper function to allocate vsplit record arrays
//
//   returns    S_OK on success, FAILED(hr) on memory allocation failure
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::AllocateVSplitRecords()
{
    HRESULT hr = S_OK;
    UINT iFace;
    UINT iMaterial;

    GXASSERT(m_rgvsSplits == NULL);
    GXASSERT(m_rgiFaceIndex == NULL);
    GXASSERT(m_rgiEdgeAdjustment == NULL);

    m_cvsSplitsMax = m_cLogicalVertices;
    m_cvsSplitCurFree = m_cvsSplitsMax - 1;

    m_rgvsSplits = new CHalfEdgeVSplit[m_cvsSplitsMax];
    if (m_rgvsSplits == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }


    m_rgbRemappedVSplit = new BYTE[m_cvsSplitsMax];
    if (m_rgbRemappedVSplit == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memset(m_rgbRemappedVSplit, 0, sizeof(BYTE) * m_cvsSplitsMax);


    m_rgiFaceIndex = new UINT[m_tmTriMesh.m_cFaces];
    if (m_rgiFaceIndex == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // can't use a memset, since rgiFaceIndex is always 32bit, but UNUSED is
    //   sometimes 16 and sometimes 32 bit (0xffff or 0xffffffff)
    for (iFace = 0; iFace < m_tmTriMesh.m_cFaces; iFace++)
    {
        m_rgiFaceIndex[iFace] = UNUSED;
    }
    
    m_rgiEdgeAdjustment = new UINT_IDX[m_tmTriMesh.m_cFaces];
    if (m_rgiEdgeAdjustment == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memset(m_rgiEdgeAdjustment, 0xff, sizeof(UINT_IDX) * m_tmTriMesh.m_cFaces);

    m_pvHEVertexBuffer = new BYTE[m_tmTriMesh.m_cVertices * m_cfvf.m_cBytesPerVertex];
    m_rgiHEVertexBuffer = new DWORD[m_tmTriMesh.m_cVertices];
    m_rgcvHEVertexBufferFree = new ULONG[m_tmTriMesh.m_caeAttributeTable];
    m_rgcvHEVertexBufferMax = new ULONG[m_tmTriMesh.m_caeAttributeTable];

    m_cMaterialNewMax = m_tmTriMesh.m_cFaces;
    m_cMaterialNewFree = m_cMaterialNewMax - 1;
    m_rgiMaterialNew = new UINT16[m_cMaterialNewMax];

    if ((m_pvHEVertexBuffer == NULL) || (m_rgiHEVertexBuffer == NULL) || (m_rgiMaterialNew == NULL) 
        || (m_rgcvHEVertexBufferFree == NULL) || (m_rgcvHEVertexBufferMax == NULL))
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

    // setup the free indices for each material
    for (iMaterial = 0; iMaterial < m_tmTriMesh.m_caeAttributeTable; iMaterial++)
    {
        m_rgcvHEVertexBufferMax[iMaterial] = 
            m_rgaeAttributeTableOrig[iMaterial].VertexStart +
            m_rgaeAttributeTableOrig[iMaterial].VertexCount;

        m_rgcvHEVertexBufferFree[iMaterial] = m_rgcvHEVertexBufferMax[iMaterial] - 1;
    }

e_Exit:
    return hr;
}

// -------------------------------------------------------------------------------
//  function    GenerateWedgeList
//
//   devnote    Simple helper function to generate circular linked lists of
//                  all wedges that make up a point, or physical instances
//                  of vertices that compose a single logical vertex in the mesh
//                      Generates the info using the vertex representative information
//
//   returns    S_OK on success, FAILED(hr) on failure
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::GenerateWedgeList()
{
    HRESULT hr = S_OK;
    UINT_IDX cVertices = m_tmTriMesh.m_cVertices;
    UINT_IDX iVertex;
    UINT_IDX wPointRep;

    GXASSERT( m_rgiWedgeList == NULL );
    m_rgiWedgeList = new UINT_IDX[cVertices];
    if (m_rgiWedgeList == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    m_cLogicalVertices = 0;

    // initialize all entries to point to themselves
    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        m_rgiWedgeList[iVertex] = iVertex;
    }

    // now go back and link them up using the 
    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        // get the representative for this set of wedges
        wPointRep = m_tmTriMesh.WGetPointRep(iVertex);

        if (wPointRep == iVertex)
        {
            m_cLogicalVertices += 1;
        }
        else
        {
            // link the new point in just after the representative vertex
            m_rgiWedgeList[iVertex] = m_rgiWedgeList[wPointRep];
            m_rgiWedgeList[wPointRep] = iVertex;
        }
    }

e_Exit:
    if (FAILED(hr))
    {
        delete m_rgiWedgeList;
    }

    return hr;
}

// -------------------------------------------------------------------------------
//  function    BInArray
//
//   devnote    Helper function for DefragmentWedges. returns true if the given
//                  wedge is in the array of wedges
//
//   returns    S_OK on success, FAILED(hr) on failure
//
template <class UINT_IDX>
bool
BInArray(UINT_IDX rgiWedgesSeen[], UINT cWedges, UINT_IDX wWedge)
{
    UINT iWedge;

    for (iWedge = 0; iWedge < cWedges; iWedge++)
    {
        if (rgiWedgesSeen[iWedge] == wWedge)
            return true;
    }

    return false;
}

// -------------------------------------------------------------------------------
//  function    DefragmentWedges
//
//   devnote    Update all wedges around a vertex to be continguous. 
//
//   returns    S_OK on success, FAILED(hr) on failure
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::DefragmentWedges()
{
    HRESULT hr = S_OK;
    UINT_IDX iFace;
    UINT_IDX cFaces = m_tmTriMesh.m_cFaces;
    UINT_IDX iPoint;
    UINT_IDX wPointRep;
    UINT_IDX *pwFace;
    UINT_IDX *rgiWedgesSeen = NULL;
    UINT cWedges = 0;
    UINT cWedgesMax = 0;
    UINT_IDX wCurWedge;
    UINT_IDX wPrevWedge;
    UINT_IDX iNextPoint;
    PBYTE pvCurWedge;
    PBYTE pvWedgeBuffer;
    UINT_IDX iCurFace;
    UINT_IDX wNewWedge;
    UINT cbVertexSeen;
	bool bCCWFace;
    DWORD attrCur;
    DWORD attrPrev;
    PBYTE pvPoints;

    // allocate a temporary buffer for new wedges
    pvWedgeBuffer = (PBYTE)_alloca(m_cfvf.m_cBytesPerVertex);
 
    // do after Wedge list generation, so that vertices with one wedge can be skipped
    //  UNDONE change to create wedge list before here
    GXASSERT(m_rgiWedgeList == NULL);

    cbVertexSeen = m_tmTriMesh.m_cVertices;
    m_rgbVertexSeen = new BYTE[cbVertexSeen];
    if (m_rgbVertexSeen == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    memset(m_rgbVertexSeen, 0, m_tmTriMesh.m_cVertices * sizeof(BYTE));

#if 1
    for (iFace = 0; iFace < cFaces; iFace++)
    {
        if (m_tmTriMesh.m_pFaces[iFace].m_wIndices[0] == UNUSED)
            continue;

        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            wCurWedge = m_tmTriMesh.m_pFaces[iFace].m_wIndices[iPoint];
            wPointRep = m_tmTriMesh.WGetPointRep(wCurWedge);
            attrCur = m_tmTriMesh.m_rgiAttributeIds[iFace];

            if ((wPointRep == wCurWedge) && !m_rgbVertexSeen[wPointRep])
            {
                // in case another face points to this vertex, mark as seen to skip extra checks
                GXASSERT(wPointRep < m_tmTriMesh.m_cVertices);
                m_rgbVertexSeen[wPointRep] = true;

                cWedges = 0;
                CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(iFace, wCurWedge, &m_tmTriMesh, x_iAllFaces);

				// try to move to the CCW face
                bCCWFace = fli.MoveToCCWFace();

				// if no CCW face (i.e. complete circle)
				//   then we must find a wedge boundary to start on, to avoid the
				//   problem of starting and ending with the same wedge
				if (!bCCWFace)
				{
					// first look for a boundary starting at the initial face
					while(!fli.BEndOfList())
					{
						iCurFace = fli.GetNextFace();
						iNextPoint = fli.IGetPointIndex();

						// if we found a boundary, then break;
						if (attrCur != m_tmTriMesh.m_rgiAttributeIds[iCurFace])
							break;
					}

					// now that we found a boundary, restart the scan there
					fli.Init(iCurFace, m_tmTriMesh.m_pFaces[iCurFace].m_wIndices[iNextPoint], x_iAllFaces);
				}


                iCurFace = fli.GetNextFace();
                pwFace = m_tmTriMesh.m_pFaces[iCurFace].m_wIndices;

                iNextPoint = fli.IGetPointIndex();
                GXASSERT(iNextPoint < 3);

                wCurWedge = pwFace[iNextPoint];
                attrCur = m_tmTriMesh.m_rgiAttributeIds[iCurFace];

                // now look for the next face that has a different wedge
                //   and on the way update the wedge, if a new one was generated
                while (!fli.BEndOfList())
                {
                    iCurFace = fli.GetNextFace();
                    pwFace = m_tmTriMesh.m_pFaces[iCurFace].m_wIndices;

                    iNextPoint = m_tmTriMesh.FindPoint(pwFace, wPointRep);
                    GXASSERT(iNextPoint < 3);

                    attrPrev = attrCur;
                    attrCur = m_tmTriMesh.m_rgiAttributeIds[iCurFace];

                    wPrevWedge = wCurWedge;
                    wCurWedge = pwFace[iNextPoint];

                    if ((attrPrev != attrCur) && (wCurWedge == wPrevWedge))
                    {
AttrChangeDuringRemap:

                        wCurWedge = pwFace[iNextPoint];

                        hr = m_tmTriMesh.LockVB(&pvPoints);
                        if (FAILED(hr))
                            goto e_Exit;

                        // get the data for the shared wedge to duplicate
                        pvCurWedge = m_cfvf.GetArrayElem(pvPoints, wCurWedge);

                        // save off the index of the new vertex
                        wNewWedge = m_tmTriMesh.m_cVertices;

                        // make a copy on the heap, sine pvCurWedge will not be
                        //   valid after the vertex buffer is locked
                        memcpy(pvWedgeBuffer, pvCurWedge, m_cfvf.m_cBytesPerVertex);

                        // unlock the vertex buffer before trying to add to it
                        m_tmTriMesh.UnlockVB();

                        hr = m_tmTriMesh.AddVertex(pvWedgeBuffer, wPointRep);
                        if (FAILED(hr))
                            goto e_Exit;

                        // update the current point to the new wedge just created
                        pwFace[iNextPoint] = wNewWedge;

                        // update the previous attribute bundle, so that we 
                        //       can break out of the next loop if it changes
                        attrPrev = attrCur;

                        // update all similar wedges to the new wedge
                        while (!fli.BEndOfList())
                        {
                            iCurFace = fli.GetNextFace();
                            pwFace = m_tmTriMesh.m_pFaces[iCurFace].m_wIndices;
                            attrCur = m_tmTriMesh.m_rgiAttributeIds[iCurFace];

                            iNextPoint = m_tmTriMesh.FindPoint(pwFace, wPointRep);
                            GXASSERT(iNextPoint < 3);

                            if (pwFace[iNextPoint] != wCurWedge)
                                break;
                            else if (attrCur != attrPrev)
                                goto AttrChangeDuringRemap;

                            pwFace[iNextPoint] = wNewWedge;
                        }
                    
                    }
                }
            }
        }
    }

#endif
    // if wedges were added to defragment, then change the size of the array to match
    //   this doesn't have to be done in the loop above because it is only referenced
    //   by point representative, which in this case must be less than the original cbVertexSeen
    if (cbVertexSeen < m_tmTriMesh.m_cVertices)
    {
        delete []m_rgbVertexSeen;
        m_rgbVertexSeen = new BYTE[m_tmTriMesh.m_cVertices];
        if (m_rgbVertexSeen == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        cbVertexSeen = m_tmTriMesh.m_cVertices;
    }


    memset(m_rgbVertexSeen, 0, m_tmTriMesh.m_cVertices * sizeof(BYTE));

    for (iFace = 0; iFace < cFaces; iFace++)
    {
        if (m_tmTriMesh.m_pFaces[iFace].m_wIndices[0] == UNUSED)
            continue;

        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            wCurWedge = m_tmTriMesh.m_pFaces[iFace].m_wIndices[iPoint];
            wPointRep = m_tmTriMesh.WGetPointRep(wCurWedge);

            // if the vertex has not been seen, and it is not the same as the representative
            //   then check for fragmentation.  if it is the same as the representative and
            //   there are other wedges on the vertex, then other triangles with the vertex
            //   will succeed on this check
            if (/*(wPointRep != wCurWedge) && */!m_rgbVertexSeen[wPointRep])
            {
                // in case another face points to this vertex, mark as seen to skip extra checks
                GXASSERT(wPointRep < m_tmTriMesh.m_cVertices);
                m_rgbVertexSeen[wPointRep] = true;

                cWedges = 0;
                CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(iFace, wCurWedge, &m_tmTriMesh, x_iAllFaces);

				// try to move to the CCW face
                bCCWFace = fli.MoveToCCWFace();

				// if no CCW face (i.e. complete circle)
				//   then we must find a wedge boundary to start on, to avoid the
				//   problem of starting and ending with the same wedge
				if (!bCCWFace)
				{
					// first look for a boundary starting at the initial face
					while(!fli.BEndOfList())
					{
						iCurFace = fli.GetNextFace();
						iNextPoint = fli.IGetPointIndex();

						// if we found a boundary, then break;
						if (wCurWedge != m_tmTriMesh.m_pFaces[iCurFace].m_wIndices[iNextPoint])
							break;
					}

					// now that we found a boundary, restart the scan there
					fli.Init(iCurFace, m_tmTriMesh.m_pFaces[iCurFace].m_wIndices[iNextPoint], x_iAllFaces);
				}


				wCurWedge = UNUSED;
                attrPrev = NULL;
                attrCur = NULL;

                // now look for the next face that has a different wedge
                //   and on the way update the wedge, if a new one was generated
                while (!fli.BEndOfList())
                {
                    iCurFace = fli.GetNextFace();
                    pwFace = m_tmTriMesh.m_pFaces[iCurFace].m_wIndices;

                    iNextPoint = m_tmTriMesh.FindPoint(pwFace, wPointRep);
                    GXASSERT(iNextPoint < 3);

                    attrPrev = attrCur;
                    attrCur = m_tmTriMesh.m_rgiAttributeIds[iCurFace];

                    // if first time thru loop, no attribute change yet
                    if (wCurWedge == UNUSED)
                        attrPrev = attrCur;

                    // if a new wedge, see if in the array
                    if ((pwFace[iNextPoint] != wCurWedge) )//|| (attrPrev != attrCur))
                    {
WedgeChangeDuringRemap:

                        wCurWedge = pwFace[iNextPoint];

                        // if we have seen this wedge before, a new wedge must
                        //  be generated and this run of triangles must be updated
                        //  to point to the new wedge
                        if (BInArray(rgiWedgesSeen, cWedges, wCurWedge) /* || (attrPrev != attrCur)*/)
                        {
                            hr = m_tmTriMesh.LockVB(&pvPoints);
                            if (FAILED(hr))
                                goto e_Exit;

                            // get the data for the shared wedge to duplicate
                            pvCurWedge = m_cfvf.GetArrayElem(pvPoints, wCurWedge);

                            // save off the index of the new vertex
                            wNewWedge = m_tmTriMesh.m_cVertices;

                            // make a copy on the heap, sine pvCurWedge will not be
                            //   valid after the vertex buffer is locked
                            memcpy(pvWedgeBuffer, pvCurWedge, m_cfvf.m_cBytesPerVertex);

                            // unlock the vertex buffer before trying to add to it
                            m_tmTriMesh.UnlockVB();

                            hr = m_tmTriMesh.AddVertex(pvWedgeBuffer, wPointRep);
                            if (FAILED(hr))
                                goto e_Exit;

                            // update the current point to the new wedge just created
                            pwFace[iNextPoint] = wNewWedge;

                            // update the previous attribute bundle, so that we 
                            //       can break out of the next loop if it changes
                            attrPrev = attrCur;

                            // update all similar wedges to the new wedge
                            while (!fli.BEndOfList())
                            {
                                iCurFace = fli.GetNextFace();
                                pwFace = m_tmTriMesh.m_pFaces[iCurFace].m_wIndices;
                                attrCur = m_tmTriMesh.m_rgiAttributeIds[iCurFace];

                                iNextPoint = m_tmTriMesh.FindPoint(pwFace, wPointRep);
                                GXASSERT(iNextPoint < 3);

                                if ((pwFace[iNextPoint] != wCurWedge) )//|| (attrCur != attrPrev))
                                    goto WedgeChangeDuringRemap;

                                pwFace[iNextPoint] = wNewWedge;
                            }
                    
                        }
                        else  // entirely new wedge, just add to array and keep going
                        {
                            // add the current wedge to the array of wedges seen
                            if (!AddToDynamicArray(&rgiWedgesSeen, wCurWedge, &cWedges, &cWedgesMax))
                            {
                                hr = E_OUTOFMEMORY;
                                goto e_Exit;
                            }
                        }
                    }
                }
            }
        }
    }

e_Exit:
    if (m_rgbVertexSeen != NULL)
    {
        // if wedges were added to defragment, then change the size of the array to match
        //   this doesn't have to be done in the loop above because it is only referenced
        //   by point representative, which in this case must be less than the original cbVertexSeen
        if (cbVertexSeen < m_tmTriMesh.m_cVertices)
        {
            delete []m_rgbVertexSeen;
            m_rgbVertexSeen = new BYTE[m_tmTriMesh.m_cVertices];
            if (m_rgbVertexSeen == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
        }

        memset(m_rgbVertexSeen, 0, m_tmTriMesh.m_cVertices * sizeof(BYTE));
    }

    if (rgiWedgesSeen != NULL)
    {
        delete []rgiWedgesSeen;
    }

    return hr;
}

// -------------------------------------------------------------------------------
//  function    GenerateFaceQEM
//
//   devnote    Generates the QEM for the given face and updates the face normal
//
//   returns    nothing
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::GenerateFaceQEM
    (
    UINT_IDX iFace, 
    PBYTE pvPoints, 
    CQuadric &qemFace
    )
{
    float d;
    D3DXVECTOR3 *pvPos0;

    pvPos0 = m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, m_tmTriMesh.m_pFaces[iFace].m_wIndices[0]));

    d = -(D3DXVec3Dot(&m_rgvFaceNormals[iFace], pvPos0));
    qemFace.Generate(m_rgvFaceNormals[iFace].x, m_rgvFaceNormals[iFace].y, m_rgvFaceNormals[iFace].z, d);
}

// -------------------------------------------------------------------------------
//  function    GenerateVertexQuadrics
//
//   devnote    Simple helper function to generate the initial error quadrics
//                  for all vertices in the initial mesh
//
//   returns    S_OK on success, FAILED(hr) on failure
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::GenerateInitialQuadrics
    (
    PBYTE pvPoints
    )
{
    HRESULT hr = S_OK;
    UINT_IDX cVertices = m_tmTriMesh.m_cVertices;
    UINT_IDX iVertex;
    UINT_IDX cFaces = m_tmTriMesh.m_cFaces;
    UINT_IDX iFace;
    D3DXVECTOR3 vEdge1;
    D3DXVECTOR3 vEdge2;
    CQuadric qemFace;
    UINT_IDX *pwFace;
    UINT_IDX iPoint;
    CQuadric qBorder;
    D3DXVECTOR3 vBorderNormal;
    UINT_IDX iNeighbor;
    UINT_IDX iEdge;
    UINT_IDX iEdge1;

    D3DXVECTOR3 *pvPos0;
    D3DXVECTOR3 *pvPos1;
    D3DXVECTOR3 *pvPos2;
    float fLength;


    GXASSERT( m_rgqVertexQuadrics == NULL );
    m_rgqVertexQuadrics = new CQuadric[cVertices];
    if (m_rgqVertexQuadrics == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    GXASSERT( m_rgqFaceQuadrics == NULL );
    m_rgqFaceQuadrics = new CQuadric[cFaces];
    if (m_rgqFaceQuadrics == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    GXASSERT( m_rgvFaceNormals == NULL );
    m_rgvFaceNormals = new D3DXVECTOR3[cFaces];
    if (m_rgvFaceNormals == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // initialize all entries to point to themselves
    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        m_rgqVertexQuadrics[iVertex].Init();
    }

    // generate the face normals for all the faces
    for (iFace = 0; iFace < cFaces; iFace++)
    {
        pwFace = m_tmTriMesh.m_pFaces[iFace].m_wIndices;

        if (pwFace[0] == UNUSED)
            continue;

        pvPos0 = m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwFace[0]));
        pvPos1 = m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwFace[1]));
        pvPos2 = m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwFace[2]));

        // calculate the two of the edges of the face
        vEdge1 = *pvPos0 - *pvPos1;
        vEdge2 = *pvPos0 - *pvPos2;

        D3DXVec3Cross(&m_rgvFaceNormals[iFace], &vEdge1, &vEdge2);

        fLength = D3DXVec3Length(&m_rgvFaceNormals[iFace]);
        if (fLength > 0.0f)
        {
            // calculate the normal of the face from the two edge vectors
            m_rgvFaceNormals[iFace] /= fLength;
        }
    }

    //   generate the face lists per vertex, used for fast traversals
    //     of all affected faces
    for (iFace = 0; iFace < cFaces; iFace++)
    {
        pwFace = m_tmTriMesh.m_pFaces[iFace].m_wIndices;

        if (pwFace[0] == UNUSED)
            continue;

        // update the face normal and generate the face error quadric
        GenerateFaceQEM(iFace, pvPoints, qemFace);

        m_rgqFaceQuadrics[iFace] = qemFace;

        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            m_rgqVertexQuadrics[pwFace[iPoint]] += qemFace;
        }

        for (iEdge = 0; iEdge < 3; iEdge++)
        {
            iEdge1 = (iEdge + 1) %3;

            iNeighbor = m_tmTriMesh.m_rgpniNeighbors[iFace].m_iNeighbors[iEdge];

            if (BGenerateBorderQem(iFace, iEdge, iNeighbor, pvPoints, qBorder))
            {
                m_rgqVertexQuadrics[pwFace[iEdge]] += qBorder;
                m_rgqVertexQuadrics[pwFace[iEdge1]] += qBorder;
            }
        }

    }

e_Exit:
    if (FAILED(hr))
    {
        delete m_rgqVertexQuadrics;
        delete m_rgvFaceNormals;
    }

    return hr;
}

// -------------------------------------------------------------------------------
//  function    CalculateNormalCost
//
//   devnote    
//
//   returns    S_OK on success, FAILED(hr) on failure
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
float 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::CalculateNormalCost
    (
    UINT_IDX iVertex1,
    UINT_IDX iVertex2,
    PBYTE pvPoints
    )
{

    return 0;

    float fDot;
    D3DXVECTOR3 *pvNormal1 = m_cfvf.PvGetNormal(m_cfvf.GetArrayElem(pvPoints, iVertex1));
    D3DXVECTOR3 *pvNormal2 = m_cfvf.PvGetNormal(m_cfvf.GetArrayElem(pvPoints, iVertex2));

    // HACK HACK need to deal with no normal case as well
    GXASSERT(m_cfvf.BNormal());

    // need to handle the case with sharp edges, currently designed for smooth normals
    // GXASSERT(0);

    // calculate the dot product and the length
    fDot = D3DXVec3Dot(pvNormal1, pvNormal2);

    // reverse signifigance and make range between 2 and 0 (2 high error, 0 low error)
    fDot = 1 - fDot;

    // scale value into cost units (HACK HACK, just a guesstimate)
    //fDot *= 50;
    fDot *= 0.25;

    return fDot;
}

// -------------------------------------------------------------------------------
//  function    CreateHeap
//
//   devnote    Helper function to create the heap used to prioritize edge
//                  collapses... also generates a structure to track edges
//                  of triangles (stored both in the heap, and in a structure
//                  similar to NeighborInfo in the tri3mesh.
//
//   returns    S_OK on success, FAILED(hr) on failure
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::CreateHeap
    (
    PBYTE pvPoints
    )
{
    HRESULT hr = S_OK;
    CEdgeInfo<UINT_IDX> *peiInfo;
    UINT_IDX iFace;
    UINT_IDX iEdge;
    UINT_IDX iEdge1;
    UINT_IDX *pwFace;
    UINT_IDX *pwNeighbors;
    SFaceEdgeInfo<UINT_IDX> *pfei;
    UINT_IDX iNeighbor;
    UINT_IDX iNeighborEdge;
    UINT_IDX iVertex1;
    UINT_IDX iVertex2;
    PBYTE pvPoint;
    D3DXVECTOR3 *pvPos1;
    D3DXVECTOR3 *pvPos2;
    CQuadric qTemp;
    float fCost;
    float fNormalCost;
    UINT_IDX wHead;
    UINT_IDX wCur;

    // need to generate vertex quadrics before calling this function
#ifdef VERTEX_QUADRICS
    GXASSERT(m_rgqVertexQuadrics != NULL);
#endif

    GXASSERT( m_pheapCosts == NULL );
    m_pheapCosts = new CSimplificationHeap<UINT_IDX>();
    if (m_pheapCosts == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    hr = m_pheapCosts->Init(m_tmTriMesh.m_cFaces * 3);
    if (FAILED(hr))
        goto e_Exit;


    GXASSERT( m_rgfeiEdges == NULL );
    m_rgfeiEdges = new SFaceEdgeInfo<UINT_IDX>[m_tmTriMesh.m_cFaces];
    if (m_rgfeiEdges == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memset(m_rgfeiEdges, 0, sizeof(SFaceEdgeInfo<UINT_IDX>) * m_tmTriMesh.m_cFaces);

#ifndef VERTEX_QUADRICS
    for(iFace = 0; iFace < m_tmTriMesh.m_cFaces; ++iFace) 
    {
        pwFace = m_tmTriMesh.m_pFaces[iFace].m_wIndices;
        pwNeighbors = m_tmTriMesh.m_rgpniNeighbors[iFace].m_iNeighbors;
        pfei = &m_rgfeiEdges[iFace];

        if (pwFace[0] == UNUSED)
            continue;

        for(iEdge = 0; iEdge < 3; ++iEdge) 
        {
            GXASSERT (pfei->m_rgpeiEdges[iEdge] == NULL);

            iVertex1 = pwFace[iEdge];

            pvPoint = m_cfvf.GetArrayElem(pvPoints, iVertex1);
            pvPos1 = m_cfvf.PvGetPosition(pvPoint);

            peiInfo = new CEdgeInfo<UINT_IDX>(iEdge, iFace, 0.0f, *pvPos1);
            if (peiInfo == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
            // set the edge of the triangle to point at the edge data structure
            m_rgfeiEdges[iFace].m_rgpeiEdges[iEdge] = peiInfo;

            // now actually calculate the error for the edge
            CalcEdgeQEMS_Plane(iFace, iEdge, pvPoints);

            // now add to the heap
            hr = m_pheapCosts->Add(peiInfo);
            if (FAILED(hr))
                goto e_Exit;
        }
    }
#else
    for(iFace = 0; iFace < m_tmTriMesh.m_cFaces; ++iFace) 
    {
        pwFace = m_tmTriMesh.m_pFaces[iFace].m_wIndices;
        pwNeighbors = m_tmTriMesh.m_rgpniNeighbors[iFace].m_iNeighbors;
        pfei = &m_rgfeiEdges[iFace];

        if (pwFace[0] == UNUSED)
            continue;

        for(iEdge = 0; iEdge < 3; ++iEdge) 
        {
            if(pfei->m_rgpeiEdges[iEdge] == NULL) 
            {
                iEdge1 = (iEdge + 1) % 3;

                iNeighbor = pwNeighbors[iEdge];
                iVertex1 = pwFace[iEdge];
                iVertex2 = pwFace[iEdge1];


                // calculate the error quadric for the edge from
                //   all the wedges in the two vertices that form the edge
                qTemp.Init();

                wHead = iVertex1;
                wCur = wHead;
                do
                {
                    qTemp += m_rgqVertexQuadrics[wCur];

                    // go to next wedge in the vertex
                    wCur = m_rgiWedgeList[wCur];
                } while (wCur != wHead);

                wHead = iVertex2;
                wCur = wHead;
                do
                {
                    qTemp += m_rgqVertexQuadrics[wCur];

                    // go to next wedge in the vertex
                    wCur = m_rgiWedgeList[wCur];
                } while (wCur != wHead);


                pvPoint = m_cfvf.GetArrayElem(pvPoints, iVertex1);
                pvPos1 = m_cfvf.PvGetPosition(pvPoint);

                // create an edge to remove pvPos2
                fCost = qTemp.CalculateCost(*pvPos1);
                fNormalCost = CalculateNormalCost(iVertex1, iVertex2, pvPoints);

                fCost += fNormalCost;
                peiInfo = new CEdgeInfo<UINT_IDX>(iEdge, iFace, fCost, *pvPos1);
                if (peiInfo == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }

                hr = m_pheapCosts->Add(peiInfo);
                if (FAILED(hr))
                    goto e_Exit;

                // set the edge of the triangle to point at the edge data structure
                pfei->m_rgpeiEdges[iEdge] = peiInfo;

                // if there is a neighbor, create an edge to remove the other vertex
                if(iNeighbor != UNUSED)
                {
                    iNeighborEdge = FindEdge(m_tmTriMesh.m_rgpniNeighbors[iNeighbor].m_iNeighbors, iFace);
                    GXASSERT(iNeighborEdge < 3);

                    pvPoint = m_cfvf.GetArrayElem(pvPoints, iVertex2);
                    pvPos2 = m_cfvf.PvGetPosition(pvPoint);

                    fCost = qTemp.CalculateCost(*pvPos2);

                    fCost += fNormalCost;
                    peiInfo = new CEdgeInfo<UINT_IDX>(iNeighborEdge, iNeighbor, fCost, *pvPos2);
                    if (peiInfo == NULL)
                    {
                        hr = E_OUTOFMEMORY;
                        goto e_Exit;
                    }

                    hr = m_pheapCosts->Add(peiInfo);
                    if (FAILED(hr))
                        goto e_Exit;

                    // set the edge of the triangle to point at the edge data structure
                    m_rgfeiEdges[iNeighbor].m_rgpeiEdges[iNeighborEdge] = peiInfo;

                }
            }
        }
    }
#endif

e_Exit:
    return hr;
}

// -------------------------------------------------------------------------------
//  function    RecalcEdgeQEMS_GH
//
//   devnote    Recalculate the costs/split positions of edges affected by a collapse
//                  uses the Garland and Heckbert form of QEMs formed from QEM of wedges
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::RecalcEdgeQEMS_GH
    (
    UINT_IDX iPoint1, 
    UINT_IDX iPoint2, 
    UINT_IDX iFacePresent, 
    UINT_IDX iWedgePresent, 
    D3DXVECTOR3 &vPosNew, 
    PBYTE pvPoints
    )
{
    CQuadric qemNew;
    UINT_IDX iFace;
    UINT_IDX iEdge;
    UINT_IDX wHead;
    UINT_IDX wCur;
    UINT_IDX iVertex;
    float fNormalCost;

    //qemNew = m_rgqVertexQuadrics[iPoint1];
    //qemNew += m_rgqVertexQuadrics[iPoint2];

    qemNew.Init();

    wHead = m_tmTriMesh.WGetPointRep(iPoint1);
    wCur = wHead;
    do
    {
        qemNew += m_rgqVertexQuadrics[wCur];
        //m_rgqVertexQuadrics[wCur] = qemNew;

        // go to next wedge in the vertex
        wCur = m_rgiWedgeList[wCur];
    } while (wCur != wHead);



    CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(iFacePresent, iWedgePresent, &m_tmTriMesh, x_iAllFaces);
    D3DXVECTOR3 vTempPosNew;
    UINT_IDX iVertex1;
    UINT_IDX iVertex2;
    CEdgeInfo<UINT_IDX> *peiToUpdate1;
    CEdgeInfo<UINT_IDX> *peiToUpdate2;
    CQuadric qemTemp;
    UINT_IDX iNeighbor;
    UINT_IDX iNeighborEdge;
    float dCost;
    while (!fli.BEndOfList())
    {
        fli.GetNextEdgeFace(iEdge, iFace); 

        peiToUpdate1 = m_rgfeiEdges[iFace].m_rgpeiEdges[iEdge];

        // UNDONE not optimal!  could know the neighbor as part of the walk
        iNeighbor = m_tmTriMesh.m_rgpniNeighbors[iFace].m_iNeighbors[iEdge];
        if (iNeighbor != UNUSED)
        {
            iNeighborEdge = FindEdge(m_tmTriMesh.m_rgpniNeighbors[iNeighbor].m_iNeighbors, iFace);
            GXASSERT(iNeighborEdge < 3);

            peiToUpdate2 = m_rgfeiEdges[iNeighbor].m_rgpeiEdges[iNeighborEdge];
        }
        else
        {
            peiToUpdate2 = NULL;
        }

        iVertex1 = m_tmTriMesh.m_pFaces[iFace].m_wIndices[iEdge];
        iVertex2 = m_tmTriMesh.m_pFaces[iFace].m_wIndices[(iEdge+1)%3];

        iVertex = m_tmTriMesh.m_pFaces[iFace].m_wIndices[(fli.IGetPointIndex()+1)%3];
        GXASSERT(m_tmTriMesh.BEqualPoints(iPoint1, m_tmTriMesh.m_pFaces[iFace].m_wIndices[fli.IGetPointIndex()]));

        wHead = m_tmTriMesh.WGetPointRep(iVertex);
        wCur = wHead;
        qemTemp = qemNew;
        do
        {
            qemTemp += m_rgqVertexQuadrics[wCur];

            // go to next wedge in the vertex
            wCur = m_rgiWedgeList[wCur];
        } while (wCur != wHead);

        fNormalCost = CalculateNormalCost(iPoint1, iPoint2, pvPoints);

        D3DXVECTOR3 *pvPos1 = m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, iVertex1));
        D3DXVECTOR3 *pvPos2 = m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, iVertex2));

        //peiToUpdate1->m_vPosNew = *pvPos1;
        peiToUpdate1->m_cost = fNormalCost + qemTemp.CalculateCost(*pvPos1);
        m_pheapCosts->Update(peiToUpdate1);


        if (peiToUpdate2)
        {
            //peiToUpdate2->m_vPosNew = *pvPos2;
            peiToUpdate2->m_cost = fNormalCost + qemTemp.CalculateCost(*pvPos2);
            m_pheapCosts->Update(peiToUpdate2);
        }
    }
}

// -------------------------------------------------------------------------------
//  function    GenerateBorderQem
//
//   devnote    Calculate the QEM for a given border
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
bool 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::BGenerateBorderQem
    (
    UINT_IDX iFace, 
    UINT_IDX iEdge, 
    UINT_IDX iNeighbor, 
    PBYTE pvPoints, 
    CQuadric &qemBorder
    )
{
    UINT_IDX *pwFace;
    D3DXVECTOR3 *pvPos0;
    D3DXVECTOR3 *pvPos1;
    D3DXVECTOR3 vEdge1;
    D3DXVECTOR3 vEdge2;
    D3DXVECTOR3 vBorderNormal;
    float d;
    UINT_IDX iEdge1;
    UINT_IDX iNeighborEdge;
    UINT_IDX iWedgeF1;
    UINT_IDX iWedgeF2;
    UINT_IDX iWedgeN1;
    UINT_IDX iWedgeN2;
    float fLength;

    iEdge1 = (iEdge + 1) % 3;
    pwFace = m_tmTriMesh.m_pFaces[iFace].m_wIndices;

    if (iNeighbor == UNUSED)
    {
        pvPos0 = m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwFace[iEdge]));
        pvPos1 = m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwFace[iEdge1]));

        // calculate the two of the edges of the face
        vEdge1 = *pvPos0 - *pvPos1;

        // calculate the normal of the face from the two edge vectors
        D3DXVec3Cross(&vBorderNormal, &vEdge1, &m_rgvFaceNormals[iFace]);

        fLength = D3DXVec3Length(&vBorderNormal);
        if (fLength > 0.0f)
        {
            vBorderNormal /= fLength;
        }

        d = -(D3DXVec3Dot(&vBorderNormal, pvPos0));
        qemBorder.Generate(vBorderNormal.x, vBorderNormal.y, vBorderNormal.z, d);

        qemBorder += qemBorder;
        return true;
    }
    else
    {
        iNeighborEdge = FindEdge(m_tmTriMesh.m_rgpniNeighbors[iNeighbor].m_iNeighbors, iFace);
        GXASSERT(iNeighborEdge < 3);

        iWedgeF1 = pwFace[iEdge];
        iWedgeF2 = pwFace[iEdge1];
        iWedgeN1 = m_tmTriMesh.m_pFaces[iNeighbor].m_wIndices[(iNeighborEdge+1) % 3];
        iWedgeN2 = m_tmTriMesh.m_pFaces[iNeighbor].m_wIndices[iNeighborEdge];

        // if the wedges are different, assume a border
            // if the materials are different also assume a border
        if ((iWedgeF1 != iWedgeN1) || (iWedgeF2 != iWedgeN2)
                )//|| (m_tmTriMesh.m_rgiAttributeIds[iNeighbor] != m_tmTriMesh.m_rgiAttributeIds[iFace]))
        {
            pvPos0 = m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwFace[iEdge]));
            pvPos1 = m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwFace[MOD3(iEdge+1)]));

            vEdge1 = *pvPos0 - *pvPos1;
            vEdge2 = m_rgvFaceNormals[iFace] + m_rgvFaceNormals[iNeighbor];

            D3DXVec3Cross(&vBorderNormal, &vEdge2, &vEdge1);
                
            fLength = D3DXVec3Length(&vBorderNormal);
            if (fLength > 0.0f)
            {
                vBorderNormal /= fLength;
            }
            else  // avoid divide by zero
            {
                vBorderNormal = m_rgvFaceNormals[iFace];
            }

            d = -(D3DXVec3Dot(&vBorderNormal, pvPos0));
            qemBorder.Generate(vBorderNormal.x, vBorderNormal.y, vBorderNormal.z, d);

            qemBorder += qemBorder;
        
            return true;
        }
        else  // no border, return false
        {
            GXASSERT(m_tmTriMesh.m_rgiAttributeIds[iNeighbor] == m_tmTriMesh.m_rgiAttributeIds[iFace]);
            return false;
        }
    }
    
//    qemBorder += qemBorder;
}

// -------------------------------------------------------------------------------
//  function    GenerateVertexQEM_MemoryLess
//
//   devnote    Calculate the QEM for a vertex in the memory less method
//                  adds up the qems for all faces that contain the point, and
//                  adds in the border qems if neccessary
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::GenerateVertexQEM_MemoryLess
    (
    UINT_IDX iFace, 
    UINT_IDX iVertex, 
    UINT_IDX iPoint, 
    PBYTE pvPoints, 
    CQuadric &qemTemp
    )
{
    UINT_IDX *pwNeighbors;
    CQuadric qemBorder;

    qemTemp.Init();

    CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(iFace, iVertex, &m_tmTriMesh, x_iAllFaces);
    while (!fli.BEndOfList())
    {
        iFace = fli.GetNextFace();

        qemTemp += m_rgqFaceQuadrics[iFace];
    }

    // now calculate the border qems

    pwNeighbors = m_tmTriMesh.m_rgpniNeighbors[iFace].m_iNeighbors;
    if (BGenerateBorderQem(iFace, iPoint, pwNeighbors[iPoint], pvPoints, qemBorder))
    {
        qemTemp += qemBorder;
    }

    if (BGenerateBorderQem(iFace, iPoint, pwNeighbors[(iPoint+2) %3], pvPoints, qemBorder))
    {
        qemTemp += qemBorder;
    }
}

// -------------------------------------------------------------------------------
//  function    RecalcEdgeQEMS_MemoryLess
//
//   devnote    Recalculate the costs/split positions of edges affected by a collapse
//                  uses the Garland and Heckbert form of QEMs formed from QEM of wedges
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::RecalcEdgeQEMS_MemoryLess
    (
    UINT_IDX iPoint1, 
    UINT_IDX iPoint2, 
    UINT_IDX iFacePresent, 
    D3DXVECTOR3 &vPosNew, 
    PBYTE pvPoints
    )
{
    UINT_IDX iFace;
    CEdgeInfo<UINT_IDX> *peiInfo;
    CQuadric qemVertex1;
    CQuadric qemTemp;
    float dCost;
    UINT_IDX *pwFace;
    UINT_IDX iPoint;
    UINT_IDX iVertex;
    UINT_IDX iEdge;
    D3DXVECTOR3 vTempPosNew;

    qemVertex1.Init();

    // first update all the affected face normals and QEMs
    CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(iFacePresent, iPoint1, &m_tmTriMesh, x_iAllFaces);
    while (!fli.BEndOfList())
    {
        iFace = fli.GetNextFace();

        GenerateFaceQEM(iFace, pvPoints, m_rgqFaceQuadrics[iFace]);

        qemVertex1 += m_rgqFaceQuadrics[iFace];
    }

    fli.Init(iFacePresent, iPoint1, x_iAllFaces);
    while (!fli.BEndOfList())
    {
        fli.GetNextEdgeFace(iEdge, iFace);

        pwFace = m_tmTriMesh.m_pFaces[iFace].m_wIndices;
        peiInfo = m_rgfeiEdges[iFace].m_rgpeiEdges[iEdge];


        // find the other point on the edge
        if (m_tmTriMesh.BEqualPoints(iPoint1, pwFace[iEdge]))
        {
            iPoint = (iEdge+1) %3;
            iVertex = pwFace[iPoint];
        }
        else
        {
            GXASSERT(m_tmTriMesh.BEqualPoints(iPoint1, pwFace[(iEdge+1)%3]));

            iPoint = iEdge;
            iVertex = pwFace[iPoint];
        }

        GenerateVertexQEM_MemoryLess(iFace, iVertex, iPoint, pvPoints, qemTemp);

        qemTemp += qemVertex1;

        qemTemp.CalculatePosition(vTempPosNew, vPosNew);
        dCost = qemTemp.CalculateCost(vTempPosNew);
    
        peiInfo->m_cost = dCost;
        //peiInfo->m_vPosNew = vTempPosNew;

        m_pheapCosts->Update(peiInfo);
    }
}

// -------------------------------------------------------------------------------
//  function    GenerateBorderQem
//
//   devnote    Calculate the QEM for a given border
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
double 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::DGenerateBorderError
    (
    UINT_IDX iFace, 
    UINT_IDX iEdge, 
    D3DXVECTOR3 *pvPosNew, 
    PBYTE pvPoints
    )
{
    UINT_IDX *pwFace;
    D3DXVECTOR3 *pvPos0;
    D3DXVECTOR3 *pvPos1;
    UINT_IDX iEdge1;
    UINT_IDX iNeighborEdge;
    UINT_IDX iWedgeF1;
    UINT_IDX iWedgeF2;
    UINT_IDX iWedgeN1;
    UINT_IDX iWedgeN2;
    float fLength;
    BOOL bBorder = FALSE;
    D3DXVECTOR3 vOut;
    D3DXVECTOR3 vTemp;
    D3DXVECTOR3 vDir;
    UINT_IDX iNeighbor;
    float fEdgeLenSq;
    float fDot;

    iEdge1 = (iEdge + 1) % 3;
    pwFace = m_tmTriMesh.m_pFaces[iFace].m_wIndices;

    iNeighbor = m_tmTriMesh.m_rgpniNeighbors[iFace].m_iNeighbors[iEdge];
    if (iNeighbor == UNUSED)
    {
        bBorder = TRUE;
    }
    else
    {
        iNeighborEdge = FindEdge(m_tmTriMesh.m_rgpniNeighbors[iNeighbor].m_iNeighbors, iFace);
        GXASSERT(iNeighborEdge < 3);

        iWedgeF1 = pwFace[iEdge];
        iWedgeF2 = pwFace[iEdge1];
        iWedgeN1 = m_tmTriMesh.m_pFaces[iNeighbor].m_wIndices[(iNeighborEdge+1) % 3];
        iWedgeN2 = m_tmTriMesh.m_pFaces[iNeighbor].m_wIndices[iNeighborEdge];

        // if the wedges are different, assume a border
            // if the materials are different also assume a border
        if ((iWedgeF1 != iWedgeN1) || (iWedgeF2 != iWedgeN2))
        {
            bBorder = TRUE;
        }
    }

    if (bBorder)
    {
        pvPos0 = m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwFace[iEdge]));
        pvPos1 = m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwFace[iEdge1]));

        // calculate the two of the edges of the face
        vDir = *pvPos0 - *pvPos1;
        fEdgeLenSq = D3DXVec3LengthSq(&vDir);

        //  d = mag( p - (A+at))
        //    = mag  (  (p-A) - a ( (p-A) dot a) / (a dot a)) )

        vTemp = *pvPosNew;
        vTemp -= *pvPos0;

        // avoid a divide by zero
        fDot = D3DXVec3Dot(&vDir, &vDir);
        if (fDot <= 1.0e-6f)
            return 0.0f;

        vDir *= D3DXVec3Dot(&vTemp, &vDir) / fDot;
        vOut = vTemp;
        vOut -= vDir;

        // return the length from the newpoint to the border
        return D3DXVec3LengthSq(&vOut) * fEdgeLenSq;
    }
    else
    {
        return 0.0f;
    }
    
}

void IntersectTriUnbounded 
(
    D3DXVECTOR3 *pv0,
    D3DXVECTOR3 *pv1,
    D3DXVECTOR3 *pv2,
    D3DXVECTOR3 *pfRayPos,            // ray origin
    D3DXVECTOR3 *pfRayDir,            // ray direction
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
        *pfDist = 0;
        return;
        //GXASSERT(0);
    }

    f = 1 / a;

    u = D3DXVec3Dot(&s, &r);
 
    D3DXVec3Cross(&q, &s, &e1);

    v = D3DXVec3Dot(pfRayDir, &q);

    t = D3DXVec3Dot(&e2, &q);

    t = t * f;
    u = u * f;
    v = v * f;

    *pfU = (float)u;
    *pfV = (float)v;
    *pfDist = (float)t;
}

// -------------------------------------------------------------------------------
//  function    CalcEdgeQEMS_Plane
//
//   devnote    Recalculate the costs/split positions of edges affected by a collapse
//                  uses the Garland and Heckbert form of QEMs formed from QEM of wedges
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::CalcEdgeQEMS_Plane
    (
    UINT_IDX iFace, 
    UINT_IDX iEdge, 
    PBYTE pvPoints
    )
{
    CEdgeInfo<UINT_IDX> *peiInfo;
    UINT_IDX iNeighbor;
    UINT_IDX iWedge1;
    UINT_IDX iWedge2;
    double dError;
    double dFaceError;
    double dErrorTotal;
    double dBorderError;
    float fFaceArea;
    D3DXPLANE planeFace;
    D3DXVECTOR3 *pvPosWedge1;
    D3DXVECTOR3 *pvNormalWedge1;
    UINT_IDX iCurFace;
    UINT_IDX *pwCurFace;
    float fU;
    float fV;
    float fDist;
    D3DXVECTOR3 vInterp;
    D3DXVECTOR3 vDiff;
    D3DXVECTOR3 vNormal;
	float *pfTexCur0;
	float *pfTexCur1;
	float *pfTexCur2;
    float *pfTexWedge1;
    float fTexCur;
    float fDiff;
    DWORD iTexWeight;

    // first get the edge info to update
    peiInfo = m_rgfeiEdges[iFace].m_rgpeiEdges[iEdge];
    GXASSERT(peiInfo != NULL);

    iNeighbor = m_tmTriMesh.m_rgpniNeighbors[iFace].m_iNeighbors[iEdge];
    iWedge1 = m_tmTriMesh.m_pFaces[iFace].m_wIndices[iEdge];  // point to collapse to
    iWedge2 = m_tmTriMesh.m_pFaces[iFace].m_wIndices[(iEdge+1)%3];  // point to remove

    pvPosWedge1 = m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, iWedge1));
    if (m_cfvf.BNormal())
        pvNormalWedge1 = m_cfvf.PvGetNormal(m_cfvf.GetArrayElem(pvPoints, iWedge1));
    else
        pvNormalWedge1 = NULL;

    // go to edge that will change to compute errors
    CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(iFace, iWedge2, &m_tmTriMesh, x_iClockwise);
    dErrorTotal = 0.0f;
    while (!fli.BEndOfList())
    {
        iCurFace = fli.GetNextFace(); 

        // don't count the face we are costing to remove into the collapse cost (should be zero anyhow, but...)
        if (iCurFace == iFace)
            continue;

        // NOTE: iNeighbor might be UNUSED, but then we will just hit the end of list which is what we want
        if (iCurFace == iNeighbor)
            break;

        pwCurFace = m_tmTriMesh.m_pFaces[iCurFace].m_wIndices;

        D3DXPlaneFromPoints(&planeFace, 
            m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwCurFace[0])),
            m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwCurFace[1])),
            m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwCurFace[2])));

        IntersectTriUnbounded(
            m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwCurFace[0])),
            m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwCurFace[1])),
            m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwCurFace[2])),
            pvPosWedge1,
            (D3DXVECTOR3*)&planeFace,
            &fU, &fV, &fDist);


        D3DXVec3Cross(&vNormal, 
            m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwCurFace[0])),
            m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, pwCurFace[1])));

        fFaceArea = D3DXVec3Length(&vNormal) / 2;

#if 0 //def DEBUG
        dFaceError = D3DXPlaneDotCoord(&planeFace, pvPosWedge1);
        double dTest = fabs((double)fabs(fDist) - fabs(dFaceError));
        GXASSERT(dTest < 1.0e-3f);
#endif

        dError = fFaceArea * m_AttributeWeights.Position * (fDist * fDist);

        if (pvNormalWedge1)
        {
            D3DXVec3BaryCentric(&vInterp,
                    m_cfvf.PvGetNormal(m_cfvf.GetArrayElem(pvPoints, pwCurFace[0])),
                    m_cfvf.PvGetNormal(m_cfvf.GetArrayElem(pvPoints, pwCurFace[1])),
                    m_cfvf.PvGetNormal(m_cfvf.GetArrayElem(pvPoints, pwCurFace[2])),
                    fU, fV);

            vDiff = *pvNormalWedge1;
            vDiff -= vInterp;

            dFaceError = (vDiff.x * vDiff.x);
            dFaceError += (vDiff.y * vDiff.y);
            dFaceError += (vDiff.z * vDiff.z);

            dError +=  dFaceError * (fFaceArea * m_AttributeWeights.Normal);  // adjust by normal weighting;
                //-> attrib_factor must have units of length^2 to make units = length^4 * attrib^2   and therefore be scale-invariant with respect to geometric errors above.
                //-> set attrib_factor= square(mesh_radius) * user_attrib_factor
               //(where user_attrib_factor=0.02f for vertex normals
//                 and  user_attrib_factor=0.1f for vertex colors    seem to work well)
        }

        if (m_rgcolorDiffuse != NULL)
        {
            GXASSERT(m_AttributeWeights.Diffuse > 0.0f);

            D3DXVec3BaryCentric(&vInterp,
                    (D3DXVECTOR3*)&m_rgcolorDiffuse[pwCurFace[0]],
                    (D3DXVECTOR3*)&m_rgcolorDiffuse[pwCurFace[1]],
                    (D3DXVECTOR3*)&m_rgcolorDiffuse[pwCurFace[2]],
                    fU, fV);

            vDiff = *(D3DXVECTOR3*)&m_rgcolorDiffuse[iWedge1];
            vDiff -= vInterp;

            dFaceError = (vDiff.x * vDiff.x);
            dFaceError += (vDiff.y * vDiff.y);
            dFaceError += (vDiff.z * vDiff.z);

            dError +=  dFaceError * (fFaceArea * m_AttributeWeights.Diffuse);  // adjust by normal weighting;
        }

        if (m_rgcolorSpecular != NULL)
        {
            GXASSERT(m_AttributeWeights.Specular > 0.0f);

            D3DXVec3BaryCentric(&vInterp,
                    (D3DXVECTOR3*)&m_rgcolorSpecular[pwCurFace[0]],
                    (D3DXVECTOR3*)&m_rgcolorSpecular[pwCurFace[1]],
                    (D3DXVECTOR3*)&m_rgcolorSpecular[pwCurFace[2]],
                    fU, fV);

            vDiff = *(D3DXVECTOR3*)&m_rgcolorSpecular[iWedge1];
            vDiff -= vInterp;

            dFaceError = (vDiff.x * vDiff.x);
            dFaceError += (vDiff.y * vDiff.y);
            dFaceError += (vDiff.z * vDiff.z);

            dError +=  dFaceError * (fFaceArea * m_AttributeWeights.Specular);  // adjust by normal weighting;
        }

        if (m_rgfTexWeights != NULL)
        {
            GXASSERT(m_cTexWeights > 0);
            GXASSERT(m_cfvf.CTexCoords() > 0);

		    pfTexCur0 = (float*)m_cfvf.PuvGetTex1(m_cfvf.GetArrayElem(pvPoints, pwCurFace[0]));
		    pfTexCur1 = (float*)m_cfvf.PuvGetTex1(m_cfvf.GetArrayElem(pvPoints, pwCurFace[1]));
		    pfTexCur2 = (float*)m_cfvf.PuvGetTex1(m_cfvf.GetArrayElem(pvPoints, pwCurFace[2]));
            pfTexWedge1 = (float*)m_cfvf.PuvGetTex1(m_cfvf.GetArrayElem(pvPoints, iWedge1));

            for (iTexWeight = 0; iTexWeight < m_cTexWeights; iTexWeight++)
            {

		        fTexCur = *pfTexCur0
									    + fU * (*pfTexCur1 - *pfTexCur0) 
									    + fV * (*pfTexCur2 - *pfTexCur0);

                fDiff = *pfTexWedge1 - fTexCur;

                dFaceError = (fDiff * fDiff);
                dError +=  dFaceError * (fFaceArea * m_rgfTexWeights[iTexWeight]);  // adjust by normal weighting;

			    pfTexCur0 += 1;
			    pfTexCur1 += 1;
			    pfTexCur2 += 1;
                pfTexWedge1 += 1;
            }
        }

        // add in the error for both the edges on this face
        iEdge = fli.IGetPointIndex();
        dBorderError = DGenerateBorderError(iCurFace, iEdge, pvPosWedge1, pvPoints);

        iEdge = (fli.IGetPointIndex()+2) % 3;
        dBorderError += DGenerateBorderError(iCurFace, iEdge, pvPosWedge1, pvPoints);

        dError += dBorderError * m_AttributeWeights.Boundary;

        // add the current error into the total, including the vertex weight in the result
        dErrorTotal += dError * m_rgfVertexWeights[m_tmTriMesh.WGetPointRep(pwCurFace[fli.IGetPointIndex()])];
    }

    peiInfo->m_cost = (float)dErrorTotal;
}

// -------------------------------------------------------------------------------
//  function    RecalcEdgeQEMS_Plane
//
//   devnote    Recalculate the costs/split positions of edges affected by a collapse
//                  uses the Garland and Heckbert form of QEMs formed from QEM of wedges
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::RecalcEdgeQEMS_Plane
    (
    UINT_IDX iFacePresent, 
    UINT_IDX iWedgePresent, 
    PBYTE pvPoints
    )
{
    UINT_IDX iCurFace;
    UINT_IDX iEdge;

    CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(iFacePresent, iWedgePresent, &m_tmTriMesh, x_iAllFaces);
    while (!fli.BEndOfList())
    {
        iCurFace = fli.GetNextFace(); 

        // UNDONE - need a proper way to only touch faces that have changed
        //if (iCurFace == iNeighbor)
            //break;

        // recalculate both edges that use the point that changed
        iEdge = fli.IGetPointIndex();
        CalcEdgeQEMS_Plane(iCurFace, iEdge, pvPoints);
        m_pheapCosts->Update(m_rgfeiEdges[iCurFace].m_rgpeiEdges[iEdge]);

        iEdge = (fli.IGetPointIndex()+2) % 3;
        CalcEdgeQEMS_Plane(iCurFace, iEdge, pvPoints);
        m_pheapCosts->Update(m_rgfeiEdges[iCurFace].m_rgpeiEdges[iEdge]);
    }
}

// -------------------------------------------------------------------------------
//  function    ColapseEdge
//
//   devnote    Collapse the given edge in the mesh.  Does all the work necessary
//                  to remove the given edge (and specifed faces) from the mesh
//                  and maintain the error values in the heap
//
//   returns    S_OK on success, FAILED(hr) on failure
//                  NOTE: the edge may not have been collapsed in the case of S_OK
//                          i.e. if the mesh would be inverted, etc... so the edge
//                          is just skipped
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::CollapseEdge
    (
    CEdgeInfo<UINT_IDX> *peiInfo, 
    PBYTE pvPoints
    )
{
    HRESULT hr = S_OK;
    SEdgeCollapseContext<UINT_IDX> ecc;

    // fill in the ecc with the info required to process the edge collapse
    hr = GatherEdgeCollapseData(ecc, peiInfo, pvPoints);
    if (FAILED(hr))
        goto e_Exit;

    // perform checks to verify if the edge collapse is possible at this time
    //   if they fail, just exit (successfully)
    if (!BValidEdgeCollapse(ecc, pvPoints))
        goto e_Exit;  // NOTE: this exit is taken FREQUENTLY

#if 1
    // calculate thew new vsplit record, or at least as much as can be calculated at this time
    hr = CalculateVSplitRecord(ecc, pvPoints);
    if (FAILED(hr))
        goto e_Exit;
#endif

    // now actually perform the edge collapse
    hr = PerformEdgeCollapse(ecc, pvPoints);
    if (FAILED(hr))
        goto e_Exit;

#if 1
    // update the information used for fixing up vsplit records
    //   NOTE: must happen before m_cCurrentFaces is updated (or at least changed if so)
    hr = UpdateVSplitAdjustments(ecc);
    if (FAILED(hr))
        goto e_Exit;
#endif

    // removed a logical vertex
    m_cLogicalVertices -= 1;

    // remove one or two faces
    m_cCurrentFaces -= 1;
    if (ecc.iFace2 != UNUSED)
    {
        m_cCurrentFaces -= 1;
    }

    if ((m_cvsMax16BitPos == UINT32UNUSED) && (m_cCurrentFaces < UINT16UNUSED))
    {
        m_cvsMax16BitPos = m_cvsSplitCurFree;
        m_cMaxLogicalVertices = m_cLogicalVertices;
        m_cvsMaxFaces16BitPos = m_cCurrentFaces;
    }

    // now update the costs of the affected edges    
    hr = UpdateErrorMeasurements(ecc, pvPoints);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    return hr;
}

// -------------------------------------------------------------------------------
//  function    GatherEdgeCollapseData
//
//   devnote    Generate the info required to process an edge collapse
//
//                  The faces involved, their edges, their wedges, etc
//
//   returns    S_OK if success, failed otherwise
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::GatherEdgeCollapseData
    (
    SEdgeCollapseContext<UINT_IDX> &ecc, 
    CEdgeInfo<UINT_IDX> *peiInfo, 
    PBYTE pvPoints
    )
{
    ecc.iWedgeRemove1 = UNUSED;
    ecc.iWedgeRemove2 = UNUSED;
    ecc.iWedgeRemove3 = UNUSED;
    ecc.iWedgeRemove4 = UNUSED;
    ecc.iWedgeRemove5 = UNUSED;
    ecc.iWedgeRemove6 = UNUSED;

    // first collect all the info on the first face and the faces above and below it
    ecc.iFace1 = peiInfo->m_wFace;
    ecc.iEdge1 = peiInfo->m_iEdge;
    ecc.iEdge1L = (ecc.iEdge1 + 2) % 3;
    ecc.iEdge1R = (ecc.iEdge1 + 1) % 3;
    ecc.iMaterialLeft = m_tmTriMesh.m_rgiMaterialIndex[ecc.iFace1];

    ecc.iPoint1 = m_tmTriMesh.m_pFaces[ecc.iFace1].m_wIndices[ecc.iEdge1];
    ecc.iPoint2 = m_tmTriMesh.m_pFaces[ecc.iFace1].m_wIndices[ecc.iEdge1R];

    ecc.vPositionNew = *m_cfvf.PvGetPosition(m_cfvf.GetArrayElem(pvPoints, ecc.iPoint1));

    ecc.iFace2 = m_tmTriMesh.m_rgpniNeighbors[ecc.iFace1].m_iNeighbors[ecc.iEdge1];

    ecc.pwNeighbors1 = m_tmTriMesh.m_rgpniNeighbors[ecc.iFace1].m_iNeighbors;
    ecc.pwNeighbors2 = NULL;

    ecc.iNFace11 = ecc.pwNeighbors1[ecc.iEdge1L];
    if(ecc.iNFace11 != UNUSED) 
    {
        ecc.iEdge11 = FindEdge(m_tmTriMesh.m_rgpniNeighbors[ecc.iNFace11].m_iNeighbors, ecc.iFace1);
        GXASSERT(ecc.iEdge11 < 3);
    }

    ecc.iNFace12 = ecc.pwNeighbors1[ecc.iEdge1R];
    if(ecc.iNFace12 != UNUSED) 
    {
        ecc.iEdge12 = FindEdge(m_tmTriMesh.m_rgpniNeighbors[ecc.iNFace12].m_iNeighbors, ecc.iFace1);
        GXASSERT(ecc.iEdge12 < 3);
    }

        // collect the wedges for the different corners of the face
    ecc.iWedgeL1 = m_tmTriMesh.m_pFaces[ecc.iFace1].m_wIndices[ecc.iEdge1];
    ecc.iWedgeL2 = m_tmTriMesh.m_pFaces[ecc.iFace1].m_wIndices[ecc.iEdge1R];
    ecc.iWedge11 = (ecc.iNFace11 == UNUSED) ? UNUSED : m_tmTriMesh.m_pFaces[ecc.iNFace11].m_wIndices[ecc.iEdge11];
    ecc.iWedge12 = (ecc.iNFace12 == UNUSED) ? UNUSED : m_tmTriMesh.m_pFaces[ecc.iNFace12].m_wIndices[(ecc.iEdge12 + 1) % 3];

    // next gather the same data on the second face if it exists
    if(ecc.iFace2 != UNUSED) 
    {
        ecc.iMaterialRight = m_tmTriMesh.m_rgiMaterialIndex[ecc.iFace2];
        ecc.pwNeighbors2 = m_tmTriMesh.m_rgpniNeighbors[ecc.iFace2].m_iNeighbors;

        ecc.iEdge2 = FindEdge(ecc.pwNeighbors2, ecc.iFace1);
        GXASSERT(ecc.iEdge2 < 3);

        ecc.iEdge2L = (ecc.iEdge2 + 2) % 3;
        ecc.iEdge2R = (ecc.iEdge2 + 1) % 3;

        ecc.iNFace21 = ecc.pwNeighbors2[ecc.iEdge2R];
        if(ecc.iNFace21 != UNUSED) 
        {
            ecc.iEdge21 = FindEdge(m_tmTriMesh.m_rgpniNeighbors[ecc.iNFace21].m_iNeighbors, ecc.iFace2);
            GXASSERT(ecc.iEdge21 < 3);
        }

        ecc.iNFace22 = ecc.pwNeighbors2[ecc.iEdge2L];
        if(ecc.iNFace22 != UNUSED) 
        {
            ecc.iEdge22 = FindEdge(m_tmTriMesh.m_rgpniNeighbors[ecc.iNFace22].m_iNeighbors, ecc.iFace2);
            GXASSERT(ecc.iEdge22 < 3);
        }

        // collect the wedges for the different corners of the face
        ecc.iWedgeR1 = m_tmTriMesh.m_pFaces[ecc.iFace2].m_wIndices[ecc.iEdge2R];
        ecc.iWedgeR2 = m_tmTriMesh.m_pFaces[ecc.iFace2].m_wIndices[ecc.iEdge2];

        ecc.iWedge21 = (ecc.iNFace21 == UNUSED) ? UNUSED : m_tmTriMesh.m_pFaces[ecc.iNFace21].m_wIndices[(ecc.iEdge21 +1) % 3];
        ecc.iWedge22 = (ecc.iNFace22 == UNUSED) ? UNUSED : m_tmTriMesh.m_pFaces[ecc.iNFace22].m_wIndices[ecc.iEdge22];
    }
    else
    {
        ecc.iNFace21 = UNUSED;
        ecc.iNFace22 = UNUSED;

        ecc.iWedgeR1 = ecc.iWedgeR2 = ecc.iWedge21 = ecc.iWedge22 = UNUSED;
    }

    return S_OK;
}

// -------------------------------------------------------------------------------
//  function    BValidEdgeCollapse
//
//   devnote    Perform checks verifying that the mesh would be valid after the
//                  specified edge collapse (i.e. mesh inversion, wedge fragmentation, etc)
//
//   returns    true if no problems, false if would cause an inconsistency
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
bool
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::BValidEdgeCollapse
    (
    SEdgeCollapseContext<UINT_IDX> &ecc, 
    PBYTE pvPoints
    )
{
    bool        bRet = false;
    UINT_IDX    iTestEdge;
    UINT_IDX    iFace;
    D3DXVECTOR3    rgvNewTri[3];
    D3DXVECTOR3    rgvOldTri[3];
    UINT_IDX    ifli;
    UINT_IDX    wCurPoint;
    UINT_IDX    iCurFace;
    D3DXVECTOR3   vEdge1;
    D3DXVECTOR3   vEdge2;
    D3DXVECTOR3 vNormal;
    D3DXVECTOR3 vFaceNormal;
    PBYTE       pvPoint;
    UINT_IDX   *pwFace;
    UINT_IDX    iPoint;
	float		fNewLength;
	float		fOldLength;
    CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(&m_tmTriMesh);

    // if doing half edge collapses and there is no iFlclw, then
    //   disallow the collapse, removing a case for code simplicity later
#if 1
    if ((ecc.iNFace12 == UNUSED))
    {
        goto e_Exit;
    }
#endif

    // Condition 0 - must still be manifold
    if ((ecc.iNFace12 != UNUSED) && (ecc.iNFace11 != UNUSED))
    {
        bool bFoundSharedVertex;

        bFoundSharedVertex = false;

        // mark all vertices off of iPoint2
        fli.Init(ecc.iNFace12, ecc.iWedge12, x_iClockwise);
        while(!fli.BEndOfList())
        {
            iFace = fli.GetNextFace();

            // don't want to count the point in iFace2 that is not on the edge being collapsed
            if (iFace == ecc.iNFace22)
                break;

            iPoint = (fli.IGetPointIndex() + 1) % 3;
            wCurPoint = m_tmTriMesh.WGetPointRep(m_tmTriMesh.m_pFaces[iFace].m_wIndices[iPoint]);

            m_rgbVertexSeen[wCurPoint] = true;
        }

        // now walk the other vertex, checking to see if any of the vertices are
        //      shared with edges off of iPoint2
        fli.Init(ecc.iNFace11, ecc.iWedge11, x_iCounterClockwise);
        while(!fli.BEndOfList())
        {
            iFace = fli.GetNextFace();

            // don't want to count the point in iFace2 that is not on the edge being collapsed
            if (iFace == ecc.iNFace21)
                break;

            iPoint = (fli.IGetPointIndex() + 2) % 3;
            wCurPoint = m_tmTriMesh.WGetPointRep(m_tmTriMesh.m_pFaces[iFace].m_wIndices[iPoint]);

            if (m_rgbVertexSeen[wCurPoint])
            {
                bFoundSharedVertex = true;
                break;;
            }
        }


        // now unmark all vertices that were marked above
        fli.Init(ecc.iNFace12, ecc.iWedge12, x_iClockwise);
        while(!fli.BEndOfList())
        {
            iFace = fli.GetNextFace();
            if (iFace == ecc.iNFace22)
                break;

            iPoint = (fli.IGetPointIndex() + 1) % 3;

            wCurPoint = m_tmTriMesh.WGetPointRep(m_tmTriMesh.m_pFaces[iFace].m_wIndices[iPoint]);
            m_rgbVertexSeen[wCurPoint] = false;
        }

        // wait until after cleanup to exit
        if (bFoundSharedVertex)
            goto e_Exit;
    }



    // Check condition 1: preserve holes and corners

    // UNDONE UNDONE... implement


    // Condition 1.5, For the vsplit records (can be disabled for non-PM generation)
    //      make sure that there is a path between flclw and frccw
    if (ecc.iFace2 != UNUSED)
    {
        GXASSERT(ecc.iFace1 != UNUSED);

        if (ecc.iNFace12 == UNUSED)
            goto e_Exit;

        // walk counter clockwise from iNFace12 to iNFace22 to verify that there is a path
        CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(ecc.iNFace12, ecc.iWedge12, &m_tmTriMesh, x_iClockwise);
    
        do 
        {
            iFace = fli.GetNextFace();
        
            if (fli.BEndOfList())
                goto e_Exit;
        }
        while (iFace != ecc.iNFace22);

        GXASSERT(iFace == ecc.iNFace22);
    }


    // Check condition 2: Don't create singularities

    if((ecc.pwNeighbors1[ecc.iEdge1L] == UNUSED) && (ecc.pwNeighbors1[ecc.iEdge1R] == UNUSED) 
                && (ecc.iFace2 != UNUSED))
    {
        goto e_Exit;
    }

    // if the two faces to link together are already connected, don't allow the collapse
    if ((ecc.iNFace11 != UNUSED) && (ecc.iNFace12 != UNUSED))
    {
        iTestEdge = FindEdge(m_tmTriMesh.m_rgpniNeighbors[ecc.iNFace11].m_iNeighbors, ecc.iNFace12);
        if (iTestEdge < 3)
            goto e_Exit;
    }

    // if the two faces to link together are already connected, don't allow the collapse
    if ((ecc.iNFace21 != UNUSED) && (ecc.iNFace22 != UNUSED))
    {
        iTestEdge = FindEdge(m_tmTriMesh.m_rgpniNeighbors[ecc.iNFace21].m_iNeighbors, ecc.iNFace22);
        if (iTestEdge < 3)
            goto e_Exit;
    }

    // Check condition 3: Don't collapse tetrahedrons and triangles

    // if all neighbors are unused, then classify as a triangle, and don't collapse
    if((ecc.pwNeighbors1[ecc.iEdge1L] == UNUSED) && (ecc.pwNeighbors1[ecc.iEdge1R] == UNUSED) 
                && (ecc.pwNeighbors1[ecc.iEdge1] == UNUSED))
    {
        goto e_Exit;
    }

    if (ecc.iFace2 != UNUSED)
    {
        // if sharing two edges with the other triangle, don't collapse
        if ((ecc.pwNeighbors1[ecc.iEdge1L] == ecc.pwNeighbors2[ecc.iEdge2R]) 
                && (ecc.pwNeighbors1[ecc.iEdge1R] == ecc.pwNeighbors2[ecc.iEdge2L]))
        {
            // will get here if no neighbors on the sides.
            if ((ecc.pwNeighbors1[ecc.iEdge1L] != UNUSED) && (ecc.pwNeighbors1[ecc.iEdge1R] != UNUSED))
                goto e_Exit;
        }
    }

    // Check for mesh inversion
    


    for (ifli = 0; ifli < 2; ifli++)
    {

        // first look at all faces around point1, making sure that they will not
        //   invert, then look at the faces around point2
        if (ifli == 0)
        {
            wCurPoint = ecc.iWedgeL1;
        }
        else
        {
            GXASSERT(ifli == 1);
            wCurPoint = ecc.iWedgeL2;
        }

        CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(ecc.iFace1, wCurPoint, &m_tmTriMesh, x_iAllFaces);

        while (!fli.BEndOfList())
        {
            iCurFace = fli.GetNextFace();

            // if the face is either of the two being removed, then skip it
            if ((iCurFace != ecc.iFace1) && (iCurFace != ecc.iFace2))
            {
                pwFace = m_tmTriMesh.m_pFaces[iCurFace].m_wIndices;

                // gather the new positions for the points
                for (iPoint = 0; iPoint < 3; iPoint++) 
                {
                    // if the current point is one of the ones being combined, substitute
                    //   the new position
                    if (m_tmTriMesh.BEqualPoints(pwFace[iPoint], wCurPoint)) 
                    {
                        rgvNewTri[iPoint] = ecc.vPositionNew;

                        // grab the original position
                        pvPoint = m_cfvf.GetArrayElem(pvPoints, pwFace[iPoint]);
                        rgvOldTri[iPoint] = *m_cfvf.PvGetPosition(pvPoint);
                    }
                    else  // otherwise just grab the current position
                    {
                        pvPoint = m_cfvf.GetArrayElem(pvPoints, pwFace[iPoint]);
                        rgvNewTri[iPoint] = *m_cfvf.PvGetPosition(pvPoint);
                        rgvOldTri[iPoint] = *m_cfvf.PvGetPosition(pvPoint);
                    }
                }

                vEdge1 = rgvNewTri[0] - rgvNewTri[1];
                vEdge2 = rgvNewTri[0] - rgvNewTri[2];

                // calculate the normal of the face from the two edge vectors
                D3DXVec3Cross(&vNormal, &vEdge1, &vEdge2);
                
                // UNDONE - if I cache face normals, I could just use that one, but it is easy to generate
                vEdge1 = rgvOldTri[0] - rgvOldTri[1];
                vEdge2 = rgvOldTri[0] - rgvOldTri[2];

                // calculate the normal of the face from the two edge vectors
                D3DXVec3Cross(&vFaceNormal, &vEdge1, &vEdge2);
                
                // if the dot product of the two normals is less than zero
                //    then the collapse would cause a mesh inversion, so skip this edge
				fNewLength = D3DXVec3LengthSq(&vNormal);
				fOldLength = D3DXVec3LengthSq(&vFaceNormal);
                if ( (fNewLength > x_fEpsilon) && (fOldLength > x_fEpsilon) 
                    && D3DXVec3Dot(&vNormal, &vFaceNormal) <= x_fEpsilon) 
                {
                    goto e_Exit;
                }  

				// don't introduce degenerate triangles (if not degenerate already)
                //if ((fNewLength <= x_fEpsilon) && (fOldLength > x_fEpsilon))
                //{
                    //goto e_Exit;
                //}        
            }    
        }
    }

    // Check that this collapse does not cause wedge fragmentation


    if ((ecc.iWedgeL1 == ecc.iWedge11) && (ecc.iWedgeL1 == ecc.iWedgeR1) && (ecc.iWedgeL1 == ecc.iWedge21))
    {
        CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(ecc.iFace1, ecc.iWedgeL1, &m_tmTriMesh, x_iCounterClockwise);
        do
        {
            iFace = fli.GetNextFace();
        }
        while (!fli.BEndOfList() && (iFace != ecc.iFace2));

        // if there is a boundary in between iFace1 and iFace2 or there
        //    is more then one wedge on this vertex
        if ((iFace != ecc.iFace2) || (m_rgiWedgeList[ecc.iWedgeL1] != ecc.iWedgeL1))
        {
            goto e_Exit;
        }
    }

    if ((ecc.iWedgeL2 == ecc.iWedge12) && (ecc.iWedgeL2 == ecc.iWedgeR2) && (ecc.iWedgeL2 == ecc.iWedge22))
    {
        CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(ecc.iFace1, ecc.iWedgeL2, &m_tmTriMesh, x_iClockwise);
        do
        {
            iFace = fli.GetNextFace();
        }
        while (!fli.BEndOfList() && (iFace != ecc.iFace2));

        // if there is a boundary in between iFace1 and iFace2 or there
        //    is more then one wedge on this vertex
        if ((iFace != ecc.iFace2 )  || (m_rgiWedgeList[ecc.iWedgeL2] != ecc.iWedgeL2))
        {
            goto e_Exit;
        }
    }    

    // dart check

    if (((ecc.iWedge11 == ecc.iWedgeL1) && (ecc.iWedge21 == ecc.iWedgeR1)
                        && (ecc.iWedgeL1 == ecc.iWedgeR1) 
                        && (ecc.iWedgeL2 != ecc.iWedgeR2))
        ||
        ((ecc.iWedge12 == ecc.iWedgeL2) && (ecc.iWedge22 == ecc.iWedgeR2) 
                        && (ecc.iWedgeL2 == ecc.iWedgeR2)
                        && (ecc.iWedgeL1 != ecc.iWedgeR1)))
    {
        // Problem is that there are two different attributes that
        //  resulting wedge could take on -> problem in encoding.
        goto e_Exit;
    }

    // unmaintanable wedges.  in half edges, the vertex being moved
    //    must not have any wedges that cannot be maintained, since
    //    we have to be able to do vsplits/edge collapses without affecting
    //    the actual vertex buffer

    if (x_bRestrictForHalfEdgePM)
    {
        // if the corner wedges are the same, then there can only be one
        //    wedge on the vertex.
        //          NOTE: if there is no face 2, then there can only be one wedge
        if ((ecc.iWedgeL2 == ecc.iWedgeR2) || (ecc.iWedgeR2 == UNUSED))
        {
            // if more than one wedge, cannot maintain them
            if (m_rgiWedgeList[ecc.iWedgeL2] != ecc.iWedgeL2)
            {
                goto e_Exit;
            }
        }
        else  // iWedgeL2 != iWedgeR2 and they both exist
        {
            GXASSERT((ecc.iWedgeL2 != UNUSED) && (ecc.iWedgeR2 != UNUSED));

            // first see if there is one wedge
            UINT_IDX iOtherWedge = m_rgiWedgeList[ecc.iWedgeL2];
            if (iOtherWedge != ecc.iWedgeL2)
            {
                // more than one wedge, so check the next item on the list to see
                //   if it points back to the first one, which is fine (max of 2 wedges)
                if (m_rgiWedgeList[iOtherWedge] != ecc.iWedgeL2)
                {
                    // nope, more wedges, not supported with half edge PM
                    goto e_Exit;
                }
                GXASSERT(iOtherWedge == ecc.iWedgeR2);
            }
            else
            {
                // if crease between R2 and L2, must be at least 2 wedges
                GXASSERT(0);
            }

			// two wedges can only be supported on the vertex to remove, if there
			//    are two on the other vertex
			if (ecc.iWedgeL1 == ecc.iWedgeR1)
				goto e_Exit;
        }
    }

    if (ecc.iNFace12 != UNUSED)
    {
        fli.Init(ecc.iNFace12, ecc.iWedge12, x_iClockwise);
        while (!fli.BEndOfList())
        {
            iFace = fli.GetNextFace();

            if (iFace == ecc.iFace2)
                break;

            iPoint = fli.IGetPointIndex();

            // if either of the other two points match the other point to be collapsed.. uh oh, don't collapse
            //    either a problem in the adjacency info (they probably should point to each other) or a weird
            //    issue with collapsing a hole
            if ((m_tmTriMesh.m_pFaces[iFace].m_wIndices[(iPoint+1)%3] == ecc.iWedgeL1)
                || (m_tmTriMesh.m_pFaces[iFace].m_wIndices[(iPoint+2)%3] == ecc.iWedgeL1))
            {
                goto e_Exit;
            }
        }
    }

    if (ecc.iNFace11 != UNUSED)
    {
        fli.Init(ecc.iNFace11, ecc.iWedge11, x_iCounterClockwise);
        while (!fli.BEndOfList())
        {
            iFace = fli.GetNextFace();

            if (iFace == ecc.iFace2)
                break;

            iPoint = fli.IGetPointIndex();

            // if either of the other two points match the other point to be collapsed.. uh oh, don't collapse
            //    either a problem in the adjacency info (they probably should point to each other) or a weird
            //    issue with collapsing a hole
            if ((m_tmTriMesh.m_pFaces[iFace].m_wIndices[(iPoint+1)%3] == ecc.iWedgeL2)
                || (m_tmTriMesh.m_pFaces[iFace].m_wIndices[(iPoint+2)%3] == ecc.iWedgeL2))
            {
                goto e_Exit;
            }
        }
    }

    // passed all checks with flying colors
    bRet = true;

e_Exit:
    return bRet;
}

// -------------------------------------------------------------------------------
//  function    CalculateVSplitRecord
//
//   devnote    Generates the vsplit record for the given edge collapse
//
//   NOTE: only information invalid in the built CHalfEdgeVSplit record is the final offset
//               of flclw, and vs_index was not filled in.  Both of these need to be filled
//               in once the base mesh has been decided.
//                      The vs_index is VALID assuming that flclw is in the base mesh, 
//                      but if not it needs to be adjusted.
//
//   returns    S_OK if success, failed otherwise
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::CalculateVSplitRecord
    (
    SEdgeCollapseContext<UINT_IDX> &ecc, 
    PBYTE pvPoints
    )
{
    CHalfEdgeVSplit *pvs;        
    D3DXVECTOR3 *pvVs;
    D3DXVECTOR3 *pvVt;
    UINT_IDX iPointOffset;
    PBYTE pvPoint;
    UINT_IDX iFace;
    HRESULT hr = S_OK;

    pvs = &m_rgvsSplits[m_cvsSplitCurFree];
    ecc.ivsCurSplit = m_cvsSplitCurFree;
    GXASSERT((m_cvsSplitCurFree > 0) && (m_cvsSplitCurFree < m_cvsSplitsMax));
    m_cvsSplitCurFree -= 1;



    if (ecc.iNFace12 != UNUSED)
    {
        pvs->m_iFlclw = ecc.iNFace12;
        iPointOffset = m_tmTriMesh.FindPoint(m_tmTriMesh.m_pFaces[ecc.iNFace12].m_wIndices, ecc.iPoint2);
    }
    else
    {
        GXASSERT(ecc.iNFace11 != UNUSED);
        pvs->m_iFlclw = ecc.iNFace11;
        iPointOffset = m_tmTriMesh.FindPoint(m_tmTriMesh.m_pFaces[ecc.iNFace11].m_wIndices, ecc.iPoint1);
    }
    GXASSERT(iPointOffset < 3);

    // calculate bits for new corners
    pvs->m_code = 0;

    // set the point offset in the triangle
    pvs->m_code |= (iPointOffset << CHalfEdgeVSplit::VSINDEX_SHIFT);

    if (m_tmTriMesh.m_rgiAttributeIds[pvs->m_iFlclw] != m_tmTriMesh.m_rgiAttributeIds[ecc.iFace1])
        pvs->m_code |= CHalfEdgeVSplit::FLN_MASK;

    if ((ecc.iFace2 != UNUSED) && (m_tmTriMesh.m_rgiAttributeIds[ecc.iNFace22] != m_tmTriMesh.m_rgiAttributeIds[ecc.iFace2]))
        pvs->m_code |= CHalfEdgeVSplit::FRN_MASK;

    pvPoint = m_cfvf.GetArrayElem(pvPoints, ecc.iPoint1);
    pvVs = m_cfvf.PvGetPosition(pvPoint);

    pvPoint = m_cfvf.GetArrayElem(pvPoints, ecc.iPoint2);
    pvVt = m_cfvf.PvGetPosition(pvPoint);

    if (ecc.iWedge11 == ecc.iWedgeL1)
    {
        pvs->m_code |= CHalfEdgeVSplit::S_LSAME;
    }

    if (ecc.iWedgeL1 == ecc.iWedgeR1)
    {
        pvs->m_code |= CHalfEdgeVSplit::S_CSAME;
    }

    if (ecc.iWedgeR1 == ecc.iWedge21)
    {
        pvs->m_code |= CHalfEdgeVSplit::S_RSAME;
    }

    if (ecc.iWedgeL2 == ecc.iWedge12)
    {
        pvs->m_code |= CHalfEdgeVSplit::T_LSAME;
    }

    if (ecc.iWedgeL2 == ecc.iWedgeR2)
    {
        pvs->m_code |= CHalfEdgeVSplit::T_CSAME;
    }

    if (ecc.iWedgeR2 == ecc.iWedge22)
    {
        pvs->m_code |= CHalfEdgeVSplit::T_RSAME;
    }

    // calculate other corners
    UINT_IDX iWedgeTop, iWedgeBottom, iWedgeMiddle;

    iWedgeTop = iWedgeBottom = UNUSED;

    iWedgeMiddle = m_tmTriMesh.m_pFaces[ecc.iFace1].m_wIndices[ecc.iEdge1L];
    if (ecc.iNFace12 != UNUSED)
    {
        iWedgeTop = m_tmTriMesh.m_pFaces[ecc.iNFace12].m_wIndices[ecc.iEdge12];
    }

    if (ecc.iNFace11 != UNUSED)
    {
        iWedgeBottom = m_tmTriMesh.m_pFaces[ecc.iNFace11].m_wIndices[(ecc.iEdge11+1)%3];
    }

    // if the top is not the same, then check to see which the bottom is
    if (iWedgeTop != iWedgeMiddle)
    {
        // if the bottom and top are not the same than a new wedge
        if (iWedgeMiddle != iWedgeBottom)
        {
            pvs->m_code |= CHalfEdgeVSplit::L_NEW;
        }
        else  // else if the top is different, but same as bottom
        {
            pvs->m_code |= CHalfEdgeVSplit::L_BELOW;
        }

    }
    // else  the top is the same, so L_ABOVE, which is a 0 bit, so don't set

    if (ecc.iFace2 != UNUSED)
    {
        iWedgeTop = iWedgeBottom = UNUSED;

        iWedgeMiddle = m_tmTriMesh.m_pFaces[ecc.iFace2].m_wIndices[ecc.iEdge2L];
        if (ecc.iNFace22 != UNUSED)
        {
            iWedgeTop = m_tmTriMesh.m_pFaces[ecc.iNFace22].m_wIndices[(ecc.iEdge22+1)%3];
        }

        if (ecc.iNFace21 != UNUSED)
        {
            iWedgeBottom = m_tmTriMesh.m_pFaces[ecc.iNFace21].m_wIndices[ecc.iEdge21];
        }

        // if the top is not the same, then check to see which the bottom is
        if (iWedgeTop != iWedgeMiddle)
        {
            // if the bottom and top are not the same than a new wedge
            if (iWedgeMiddle != iWedgeBottom)
            {
                pvs->m_code |= CHalfEdgeVSplit::R_NEW;
            }
            else  // else if the top is different, but same as bottom
            {
                pvs->m_code |= CHalfEdgeVSplit::R_BELOW;
            }

        }
        // else  the top is the same, so L_ABOVE, which is a 0 bit, so don't set
    }

    // should always be a iFlclw according to the edge collapse checks
    GXASSERT(ecc.iNFace12 != UNUSED);

    if (ecc.iFace2 == UNUSED)
    {
        pvs->m_oVlrOffset = 0;
    }
    else  // normal case with both iFace1 and iFace2
    {
        // walk counter clockwise from iNFace12 to iNFace22 counting number of moves
        CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(ecc.iNFace12, ecc.iWedge12, &m_tmTriMesh, x_iClockwise);
        
        // start vlroffset1 at 0, since it will be incremented once for iNFace12
        //    initial valid value is 1, which means that both flclw and frccw are the
        //    same triangle, 2 means that flclw and frccw are directly connected and so on
        pvs->m_oVlrOffset = 0;
        do 
        {
            iFace = fli.GetNextFace();
            pvs->m_oVlrOffset += 1;
            
            GXASSERT(!fli.BEndOfList());
        }
        while (iFace != ecc.iNFace22);

        GXASSERT(iFace == ecc.iNFace22);
    }

    if (pvs->m_code & CHalfEdgeVSplit::L_NEW)
    {
		ecc.iWedgeRemove3 = m_tmTriMesh.m_pFaces[ecc.iFace1].m_wIndices[ecc.iEdge1L];
    }

    if (pvs->m_code & CHalfEdgeVSplit::R_NEW)
    {
		ecc.iWedgeRemove6 = m_tmTriMesh.m_pFaces[ecc.iFace2].m_wIndices[ecc.iEdge2L];
    }

	if (ecc.iFace2 == UNUSED) 
	{
		if (!(pvs->m_code & CHalfEdgeVSplit::T_LSAME) && !(pvs->m_code & CHalfEdgeVSplit::S_LSAME))
			ecc.iWedgeRemove1 = ecc.iWedgeL1;

		//if (!(pvs->m_code & CHalfEdgeVSplit::T_LSAME) )
			ecc.iWedgeRemove2 = ecc.iWedgeL2;

		//if ( (pvs->m_code & CHalfEdgeVSplit::T_LSAME) && (pvs->m_code & CHalfEdgeVSplit::S_LSAME))
			//ecc.iWedgeRemove1 = ecc.iWedgeL2;
	}
	else
	{
		if (!(pvs->m_code & CHalfEdgeVSplit::T_LSAME) && !( (pvs->m_code & CHalfEdgeVSplit::S_CSAME) && (pvs->m_code & CHalfEdgeVSplit::S_RSAME)) && ! (pvs->m_code & CHalfEdgeVSplit::S_LSAME))
			ecc.iWedgeRemove1 = ecc.iWedgeL1;

		if (!(pvs->m_code & CHalfEdgeVSplit::T_LSAME) && !( (pvs->m_code & CHalfEdgeVSplit::T_CSAME) && (pvs->m_code & CHalfEdgeVSplit::T_RSAME)))
			ecc.iWedgeRemove2 = ecc.iWedgeL2;

		if ( (pvs->m_code & CHalfEdgeVSplit::T_LSAME) /*&& (pvs->m_code & CHalfEdgeVSplit::S_LSAME)*/)
			ecc.iWedgeRemove2 = ecc.iWedgeL2;

		if (!(pvs->m_code & CHalfEdgeVSplit::T_RSAME) && !(pvs->m_code & CHalfEdgeVSplit::S_CSAME) && ! (pvs->m_code & CHalfEdgeVSplit::S_RSAME))
			ecc.iWedgeRemove4 = ecc.iWedgeR1;

		if (!(pvs->m_code & CHalfEdgeVSplit::T_RSAME) && !(pvs->m_code & CHalfEdgeVSplit::T_CSAME))
			ecc.iWedgeRemove5 = ecc.iWedgeR2;

		if ( (pvs->m_code & CHalfEdgeVSplit::T_RSAME) /*&& (pvs->m_code & CHalfEdgeVSplit::S_RSAME)*/ && (ecc.iWedgeRemove2 != ecc.iWedgeR2))
			ecc.iWedgeRemove5 = ecc.iWedgeR2;
	}

	GXASSERT(ecc.iWedgeRemove2 != UNUSED);


    // maintain the vertex buffer for half edges PMs
    if ( x_bGenerateHalfEdgePM )
    {

		// NOTE:  need to add wedges to HE buffer in reverse order
		if (ecc.iWedgeRemove6 != UNUSED)
			AddHEPoint(pvPoints, ecc.iWedgeRemove6, ecc.iMaterialRight);
		if (ecc.iWedgeRemove4 != UNUSED)
			AddHEPoint(pvPoints, ecc.iWedgeRemove4, ecc.iMaterialRight);
		if (ecc.iWedgeRemove5 != UNUSED)
			AddHEPoint(pvPoints, ecc.iWedgeRemove5, ecc.iMaterialRight);

		if (ecc.iWedgeRemove3 != UNUSED)
			AddHEPoint(pvPoints, ecc.iWedgeRemove3, ecc.iMaterialLeft);
		if (ecc.iWedgeRemove1 != UNUSED)
			AddHEPoint(pvPoints, ecc.iWedgeRemove1, ecc.iMaterialLeft);
		if (ecc.iWedgeRemove2 != UNUSED)
			AddHEPoint(pvPoints, ecc.iWedgeRemove2, ecc.iMaterialLeft);

        if (pvs->m_code & CHalfEdgeVSplit::FRN_MASK)
        {
            GXASSERT((ecc.iFace2 != UNUSED) && (ecc.iNFace22 != UNUSED));
            GXASSERT(m_tmTriMesh.m_rgiAttributeIds[ecc.iNFace22] != m_tmTriMesh.m_rgiAttributeIds[ecc.iFace2]);
            GXASSERT(m_tmTriMesh.m_rgiMaterialIndex[ecc.iNFace22] != m_tmTriMesh.m_rgiMaterialIndex[ecc.iFace2]);
            GXASSERT((m_rgiMaterialNew != NULL) && (m_cMaterialNewFree > 0));

            m_rgiMaterialNew[m_cMaterialNewFree] = m_tmTriMesh.m_rgiMaterialIndex[ecc.iFace2];
            m_cMaterialNewFree -= 1;
        }

        if (pvs->m_code & CHalfEdgeVSplit::FLN_MASK)
        {
            GXASSERT((ecc.iFace1 != UNUSED) && (ecc.iNFace12 != UNUSED));
            GXASSERT(m_tmTriMesh.m_rgiAttributeIds[ecc.iNFace12] != m_tmTriMesh.m_rgiAttributeIds[ecc.iFace1]);
            GXASSERT(m_tmTriMesh.m_rgiMaterialIndex[ecc.iNFace12] != m_tmTriMesh.m_rgiMaterialIndex[ecc.iFace1]);
            GXASSERT((m_rgiMaterialNew != NULL) && (m_cMaterialNewFree > 0));

            m_rgiMaterialNew[m_cMaterialNewFree] = m_tmTriMesh.m_rgiMaterialIndex[ecc.iFace1];
            m_cMaterialNewFree -= 1;
        }
    }

    return hr;
}

// -------------------------------------------------------------------------------
//  function    AddHEPoint
//
//   devnote    Appends the given wedge to the list of vertices to append
//					for a half edge pm
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::AddHEPoint
    (
    PBYTE pvPoints, 
    UINT_IDX iWedge, 
    UINT16 iMaterial
    )
{
    PBYTE pvHEBufPoint;
    PBYTE pvPoint;

    GXASSERT((m_rgcvHEVertexBufferFree[iMaterial] > 0) && (m_rgcvHEVertexBufferFree[iMaterial] < m_rgcvHEVertexBufferMax[iMaterial]));

    pvHEBufPoint = m_cfvf.GetArrayElem(m_pvHEVertexBuffer, m_rgcvHEVertexBufferFree[iMaterial]);
    m_rgiHEVertexBuffer[m_rgcvHEVertexBufferFree[iMaterial]] = iWedge;

    pvPoint = m_cfvf.GetArrayElem(pvPoints, iWedge);
    memcpy(pvHEBufPoint, pvPoint, m_cfvf.m_cBytesPerVertex);
	
    m_rgcvHEVertexBufferFree[iMaterial] -= 1;
}


// -------------------------------------------------------------------------------
//  function    PerformEdgeCollapse
//
//   devnote    Collapse the edge specified by the edge collapse context
//                  maintain the heap, and defragment and remove wedges
//                  as well as the unused faces and edges
//
//   returns    S_OK if success, failed otherwise
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::PerformEdgeCollapse
    (
    SEdgeCollapseContext<UINT_IDX> &ecc, 
    PBYTE pvPoints
    )
{
    UINT_IDX wHead;
    UINT_IDX wCur;
    UINT_IDX wNext;
    UINT_IDX wNewRep;
    PBYTE pvPoint;
    UINT_IDX iWedge;
    UINT_IDX iPoint;
    UINT_IDX iFace;
    UINT_IDX iEdge;

    // link iNFace11 to iNFace12 and vice versa... only if present
    if(ecc.iNFace11 != UNUSED) 
    {
        m_tmTriMesh.m_rgpniNeighbors[ecc.iNFace11].m_iNeighbors[ecc.iEdge11] = ecc.iNFace12;
    }

    if(ecc.iNFace12 != UNUSED) 
    {
        m_tmTriMesh.m_rgpniNeighbors[ecc.iNFace12].m_iNeighbors[ecc.iEdge12] = ecc.iNFace11;
    }
    
    for (iEdge = 0; iEdge < 3; iEdge++)
    {
        m_pheapCosts->Delete(m_rgfeiEdges[ecc.iFace1].m_rgpeiEdges[iEdge]);
        //delete m_rgfeiEdges[ecc.iFace1].m_rgpeiEdges[iEdge];
    }

    // now deal with neigbhors of the second face being removed
    if(ecc.iFace2 != UNUSED) 
    {
        // link the two faces on the sides of iFace2 together, assume they exist of course
        if(ecc.iNFace21 != UNUSED) 
        {
            m_tmTriMesh.m_rgpniNeighbors[ecc.iNFace21].m_iNeighbors[ecc.iEdge21] = ecc.iNFace22;
        }
        if(ecc.iNFace22 != UNUSED) 
        {
            m_tmTriMesh.m_rgpniNeighbors[ecc.iNFace22].m_iNeighbors[ecc.iEdge22] = ecc.iNFace21;
        }
    
        for (iEdge = 0; iEdge < 3; iEdge++)
        {
            m_pheapCosts->Delete(m_rgfeiEdges[ecc.iFace2].m_rgpeiEdges[iEdge]);
            //delete m_rgfeiEdges[ecc.iFace2].m_rgpeiEdges[iEdge];
        }
    }



    wHead = m_tmTriMesh.WGetPointRep(ecc.iPoint1);
	if (ecc.iWedge11 != UNUSED)
	{
		wNewRep = ecc.iWedge11;
	}
	else if (ecc.iWedge21 != UNUSED) 
	{
		wNewRep = ecc.iWedge21;
	}
	else if (ecc.iWedgeRemove2 != ecc.iWedge12)
	{
		wNewRep = ecc.iWedge12;
	}
	else
	{
		GXASSERT(ecc.iWedgeRemove1 != ecc.iPoint1);
		wNewRep = ecc.iPoint1;
	}
    GXASSERT(wNewRep != UNUSED);

    wCur = wHead;
    do
    {
        pvPoint = m_cfvf.GetArrayElem(pvPoints, wCur);

        // update the position
        //m_cfvf.SetPosition(pvPoint, &ecc.vPositionNew);

        // update the representative points of the mesh
        m_tmTriMesh.m_rgwPointReps[wCur] = wNewRep;

        // go to next wedge in the vertex
        wCur = m_rgiWedgeList[wCur];
    } while (wCur != wHead);

    // now update the vertices/wedges of the two vertices together, modifying the 
    //  position at the same time
    wHead = m_tmTriMesh.WGetPointRep(ecc.iPoint2);
    wCur = wHead;
    do
    {
        pvPoint = m_cfvf.GetArrayElem(pvPoints, wCur);

        // update the representative points of the mesh
        m_tmTriMesh.m_rgwPointReps[wCur] = wNewRep;

        // update the position
        //m_cfvf.SetPosition(pvPoint, &ecc.vPositionNew);

        // go to next wedge in the vertex
        wCur = m_rgiWedgeList[wCur];
    } while (wCur != wHead);


    // merge wedges if necessary
    if (/*(ecc.iWedge11 == ecc.iWedgeL1) && */(ecc.iWedge12 == ecc.iWedgeL2)) 
    {
        iWedge = ecc.iWedge12;
        CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(ecc.iNFace12, iWedge, &m_tmTriMesh, x_iClockwise);
        while (!fli.BEndOfList())
        {
            iFace = fli.GetNextFace();
            iPoint = fli.IGetPointIndex();

            if (m_tmTriMesh.m_pFaces[iFace].m_wIndices[iPoint] != iWedge)
                break;

            m_tmTriMesh.m_pFaces[iFace].m_wIndices[iPoint] = ecc.iWedgeL1;
        }
    }



    // if there is a second face, and the wedge to be merged hasn't already been merged
    if ((ecc.iFace2 != UNUSED)  && (ecc.iWedgeRemove1 != ecc.iWedge22))
    {
        // NOTE: this has already been performed if there is only one wedge around R2
        if(/*(ecc.iWedge21 == ecc.iWedgeR1) && */(ecc.iWedge22 == ecc.iWedgeR2) && (m_rgiWedgeList[ecc.iWedge22] != ecc.iWedge22) )
        {
            iWedge = ecc.iWedge22;
            CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(ecc.iNFace22, iWedge, &m_tmTriMesh, x_iCounterClockwise);
            while (!fli.BEndOfList())
            {
                iFace = fli.GetNextFace();
                iPoint = fli.IGetPointIndex();

                if (m_tmTriMesh.m_pFaces[iFace].m_wIndices[iPoint] != iWedge)
                    break;

                m_tmTriMesh.m_pFaces[iFace].m_wIndices[iPoint] = ecc.iWedgeR1;
            }
        }

    }

    GXASSERT((ecc.iWedgeRemove1 != wNewRep) 
		&& (ecc.iWedgeRemove2 != wNewRep)
		&& (ecc.iWedgeRemove3 != wNewRep)
		&& (ecc.iWedgeRemove4 != wNewRep)
		&& (ecc.iWedgeRemove5 != wNewRep)
		&& (ecc.iWedgeRemove6 != wNewRep));


    // now merge to two lists and update the representative points
    //  UNDONE probably a more optimal solution

    wHead = ecc.iPoint2;
    wCur = wHead;
    do
    {
        // save of the next pointer, before overwriting it
        wNext = m_rgiWedgeList[wCur];

        // link the new point in just after the representative vertex
        m_rgiWedgeList[wCur] = m_rgiWedgeList[ecc.iPoint1];
        m_rgiWedgeList[ecc.iPoint1] = wCur;

        wCur = wNext;
    } while (wCur != wHead);

	// remove wedges that are no longer needed from the new wedge list
	RemoveWedgeElement(wNewRep, ecc.iWedgeRemove1, ecc.iMaterialLeft);
	RemoveWedgeElement(wNewRep, ecc.iWedgeRemove2, ecc.iMaterialLeft);
	RemoveWedgeElement(wNewRep, ecc.iWedgeRemove4, ecc.iMaterialRight);
	RemoveWedgeElement(wNewRep, ecc.iWedgeRemove5, ecc.iMaterialRight);

	// handle 3 and 6 separately, since they are on different wedges lists
	if (ecc.iWedgeRemove3 != UNUSED)
	{
		RemoveWedge(ecc.iWedgeRemove3, ecc.iMaterialLeft);
	}
	if (ecc.iWedgeRemove6 != UNUSED)
	{
		RemoveWedge(ecc.iWedgeRemove6, ecc.iMaterialRight);
	}

    // remove the faces from the mesh
    m_tmTriMesh.MarkAsUnused(ecc.iFace1);
    m_rgaeAttributeTableCur[ecc.iMaterialLeft].FaceCount -= 1;
    if (ecc.iFace2 != UNUSED)
    {
        m_tmTriMesh.MarkAsUnused(ecc.iFace2);
        m_rgaeAttributeTableCur[ecc.iMaterialRight].FaceCount -= 1;
    }

    return S_OK;
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::RemoveWedgeElement
    (
    UINT_IDX wHead, 
    UINT_IDX wElement, 
    UINT16 iMaterial
    )
{
	UINT_IDX wCur;
	UINT_IDX wNext;

	GXASSERT(wHead != wElement);

    if (wElement != UNUSED)
    {
        wCur = wHead;
        do
        {
            // save of the next pointer, before overwriting it
            wNext = m_rgiWedgeList[wCur];

		    // if found, remove from list and return
            if (wNext == wElement) 
		    {
			    m_rgiWedgeList[wCur] = m_rgiWedgeList[wNext];
			    m_rgiWedgeList[wNext] = wNext;
			    break;
		    }

            wCur = wNext;
        } while (wCur != wHead);

        // mark the vertex as being deleted for debug purposes
        m_rgbVertexDeleted[wElement] = 1;

        // maintain the attribute table vertex counts
        m_rgaeAttributeTableCur[iMaterial].VertexCount -= 1;

        m_cCurrentWedges -= 1;
    }
}

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::RemoveWedge
    (
    UINT_IDX wWedge, 
    UINT16 iMaterial
    )
{
	UINT_IDX wCur;
	UINT_IDX wHead;
	UINT_IDX wRep;

	wRep = m_tmTriMesh.WGetPointRep(wWedge);

	// if the wedge to be removed is the representative as well, jsut
	if (wRep == wWedge)
	{
		// get the new representative
		wRep = m_rgiWedgeList[wWedge];

		// had better be more than one wedge to be able to remove it
		GXASSERT(wRep != wWedge);

		wHead = wWedge;
		wCur = wHead;
		do
		{
			m_tmTriMesh.m_rgwPointReps[wCur] = wRep;

			wCur = m_rgiWedgeList[wCur];
		} while (wCur != wHead);
	}

	// now remove the element
	RemoveWedgeElement(wRep, wWedge, iMaterial);

}

// -------------------------------------------------------------------------------
//  function    UpdateVSplitAdjustments
//
//   devnote    Updates the face remap array and edge adjustment arrays
//                  required to fixup the vsplit elements that reference
//                  a face that has been collapsed (i.e. will be generated
//                  prior to being referenced).
//
//   returns    S_OK if success, failed otherwise
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::UpdateVSplitAdjustments
    (
    SEdgeCollapseContext<UINT_IDX> &ecc
    )
{
    // if no second face, then just setup the adjustment for face1
    if (ecc.iFace2 == UNUSED)
    {
        // current face indices already updated, so don't need to subtract one
        m_rgiFaceIndex[ecc.iFace1] = m_rgaeAttributeTableCur[ecc.iMaterialLeft].FaceStart + 
            (m_rgaeAttributeTableCur[ecc.iMaterialLeft].FaceCount);
            
        m_rgiEdgeAdjustment[ecc.iFace1] = x_rgiLeftFaceEdgeAdjustment[ecc.iEdge1];
    }
    else // else need to handle both face1 and face2, face2 should have a higher id
    {
        m_rgiEdgeAdjustment[ecc.iFace1] = x_rgiLeftFaceEdgeAdjustment[ecc.iEdge1];
        m_rgiEdgeAdjustment[ecc.iFace2] = x_rgiRightFaceEdgeAdjustment[ecc.iEdge2];

        // current face indices already updated, so don't need to subtract one
        m_rgiFaceIndex[ecc.iFace1] = m_rgaeAttributeTableCur[ecc.iMaterialLeft].FaceStart + 
            (m_rgaeAttributeTableCur[ecc.iMaterialLeft].FaceCount);

        // if same material, then one greater than left, otherwise it is last one in 
        //    other material
        if (ecc.iMaterialLeft == ecc.iMaterialRight)
        {
            m_rgiFaceIndex[ecc.iFace2] = m_rgiFaceIndex[ecc.iFace1] + 1;
        }
        else
        {
            m_rgiFaceIndex[ecc.iFace2] = m_rgaeAttributeTableCur[ecc.iMaterialRight].FaceStart + 
                (m_rgaeAttributeTableCur[ecc.iMaterialRight].FaceCount);
        }
    }

    return S_OK;
}

// -------------------------------------------------------------------------------
//  function    UpdateErrorMeasurements
//
//   devnote    Updates the entries in the heap that are changed by the
//                  edge collapse
//
//   returns    S_OK if success, failed otherwise
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
HRESULT 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::UpdateErrorMeasurements
    (
    SEdgeCollapseContext<UINT_IDX> &ecc, 
    PBYTE pvPoints
    )
{
    UINT_IDX iFacePresent;
    UINT_IDX iWedgePresent;

    // Moved from CalculateVSplitWads, could probably be simplified (if not removed)
    //  UNDONE UNDONE can this be removed?
    ecc.iWedge11 = (ecc.iNFace11 == UNUSED) ? UNUSED : m_tmTriMesh.m_pFaces[ecc.iNFace11].m_wIndices[ecc.iEdge11];
    ecc.iWedge12 = (ecc.iNFace12 == UNUSED) ? UNUSED : m_tmTriMesh.m_pFaces[ecc.iNFace12].m_wIndices[(ecc.iEdge12 + 1) % 3];
    if (ecc.iFace2 != UNUSED)
    {
        ecc.iWedge21 = (ecc.iNFace21 == UNUSED) ? UNUSED : m_tmTriMesh.m_pFaces[ecc.iNFace21].m_wIndices[(ecc.iEdge21 +1) % 3];
        ecc.iWedge22 = (ecc.iNFace22 == UNUSED) ? UNUSED : m_tmTriMesh.m_pFaces[ecc.iNFace22].m_wIndices[ecc.iEdge22];
    }

    // first figure out which of the faces is still present
    iFacePresent = UNUSED;
    if (ecc.iNFace11 != UNUSED) 
    {
        iFacePresent = ecc.iNFace11;
        iWedgePresent = ecc.iWedge11;
    }
    else if (ecc.iNFace12 != UNUSED) 
    {
        iFacePresent = ecc.iNFace12;
        iWedgePresent = ecc.iWedge12;
    }
    else if (ecc.iFace2 != UNUSED)
    {
        if (ecc.iNFace21 != UNUSED) 
        {
            iFacePresent = ecc.iNFace21;
            iWedgePresent = ecc.iWedge21;
        }
        else if (ecc.iNFace22 != UNUSED) 
        {
            iFacePresent = ecc.iNFace22;
            iWedgePresent = ecc.iWedge22;
        }
    }

    // if there are any triangles left around the collapsed vertex, update the error quadrics
    if (iFacePresent != UNUSED)
    {
#ifdef VERTEX_QUADRICS
        // if any wedges were removed, consolidate their error measurement into their representative
        if (ecc.iWedgeRemove1 != UNUSED)
        {
            m_rgqVertexQuadrics[m_tmTriMesh.WGetPointRep(ecc.iWedgeRemove1)] += m_rgqVertexQuadrics[ecc.iWedgeRemove1];
        }

        if (ecc.iWedgeRemove2 != UNUSED)
        {
            m_rgqVertexQuadrics[m_tmTriMesh.WGetPointRep(ecc.iWedgeRemove2)] += m_rgqVertexQuadrics[ecc.iWedgeRemove2];
        }

        if (ecc.iWedgeRemove3 != UNUSED)
        {
            m_rgqVertexQuadrics[m_tmTriMesh.WGetPointRep(ecc.iWedgeRemove3)] += m_rgqVertexQuadrics[ecc.iWedgeRemove3];
        }

        if (ecc.iWedgeRemove4 != UNUSED)
        {
            m_rgqVertexQuadrics[m_tmTriMesh.WGetPointRep(ecc.iWedgeRemove4)] += m_rgqVertexQuadrics[ecc.iWedgeRemove4];
        }

        if (ecc.iWedgeRemove5 != UNUSED)
        {
            m_rgqVertexQuadrics[m_tmTriMesh.WGetPointRep(ecc.iWedgeRemove5)] += m_rgqVertexQuadrics[ecc.iWedgeRemove5];
        }

        if (ecc.iWedgeRemove6 != UNUSED)
        {
            m_rgqVertexQuadrics[m_tmTriMesh.WGetPointRep(ecc.iWedgeRemove6)] += m_rgqVertexQuadrics[ecc.iWedgeRemove6];
        }

        RecalcEdgeQEMS_GH(ecc.iPoint1, ecc.iPoint2, iFacePresent, iWedgePresent, ecc.vPositionNew, pvPoints);
#elif 1
        RecalcEdgeQEMS_Plane(iFacePresent, iWedgePresent, pvPoints);
#else
        // hmmm Broken on flatsurf.m
        RecalcEdgeQEMS_MemoryLess(ecc.iPoint1, ecc.iPoint2, iFacePresent, ecc.vPositionNew, pvPoints);
#endif
    }

    return S_OK;
}
#ifdef _DEBUG

// -------------------------------------------------------------------------------
//  function    BFragmentedWedge
//
//   devnote    Helper function for BFragmentedWedges, verifys that the wedges
//                  around a given point are not fragmented
//
//   returns    true (and asserts) if fragmentation, false if no fragmentation
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
bool
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::BFragmentedWedge
    (
    UINT_IDX iInitialFace, 
    UINT_IDX wWedge, 
    UINT &cWedges, 
    UINT &cWedgesMax , 
    UINT_IDX *&rgiWedgesSeen
    )
{
    bool bRet = false;
    UINT_IDX wCurWedge;
    UINT_IDX wHeadWedge;
    UINT_IDX *pwFace;
    UINT_IDX iNextPoint;
    UINT_IDX iFace;

    cWedges = 0;
    CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(iInitialFace, wWedge, &m_tmTriMesh, x_iAllFaces);

    fli.MoveToCCWFace();

    // prime with the first face/wedge
    iFace = fli.GetNextFace();
    pwFace = m_tmTriMesh.m_pFaces[iFace].m_wIndices;
    wCurWedge = pwFace[fli.IGetPointIndex()];
	wHeadWedge = wCurWedge;

    // add the current wedge to the array of wedges seen
    if (!AddToDynamicArray(&rgiWedgesSeen, wCurWedge, &cWedges, &cWedgesMax))
    {
        GXASSERT(0);
        bRet = true;
        goto e_Exit;
    }

    // now look for the next face that has a different wedge
    //   and on the way update the wedge, if a new one was generated
    while (!fli.BEndOfList())
    {
        iFace = fli.GetNextFace();
        pwFace = m_tmTriMesh.m_pFaces[iFace].m_wIndices;

        iNextPoint = fli.IGetPointIndex();
        GXASSERT(iNextPoint < 3);

        // if a new wedge, see if in the array
        if (pwFace[iNextPoint] != wCurWedge)
        {
            wCurWedge = pwFace[iNextPoint];

            // if we have seen this before then we have fragmented wedges
            if (BInArray(rgiWedgesSeen, cWedges, wCurWedge))
            {
                // if it was the first wedge, then it is possible that we didn't
                //  start the search at the beginning of a wedge
                if (rgiWedgesSeen[0] == wCurWedge)
                {
                    // search the rest of the triangles, if we run out of triangles, then
                    //  the wedge is not fragmented
                    while (!fli.BEndOfList())
                    {
                        iFace = fli.GetNextFace();
                        pwFace = m_tmTriMesh.m_pFaces[iFace].m_wIndices;
                        iNextPoint = fli.IGetPointIndex();

                        // if not the first wedge, then assert that a fragment was found
                        if (pwFace[iNextPoint] != wCurWedge)
                        {
                            GXASSERT(0);

                            bRet = true;
                            goto e_Exit;
                        }
                    }

                }
                else  // not the first wedge, so it is definitely a fragmented wedge
                {
                    GXASSERT(0);

                    bRet = true;
                    goto e_Exit;
                }
            }

			// add the current wedge to the array of wedges seen
			if (!AddToDynamicArray(&rgiWedgesSeen, wCurWedge, &cWedges, &cWedgesMax))
			{
				GXASSERT(0);
				bRet = true;
				goto e_Exit;
			}
        }

    }

	// now that we know the wedges contained in triangles around the given vertex
	//   make sure that there are no extras in the wedge list
	wCurWedge = wHeadWedge;
    do
    {
		if (!BInArray(rgiWedgesSeen, cWedges, wCurWedge))
		{
			GXASSERT(0);
			bRet = true;
			goto e_Exit;
		}

        // go to next wedge in the vertex
        wCurWedge = m_rgiWedgeList[wCurWedge];
    } while (wCurWedge != wHeadWedge);

e_Exit:
    return bRet;
}

// -------------------------------------------------------------------------------
//  function    BFragmentedWedges
//
//   devnote    Asserts that there are no fragmented wedges
//
//   returns    true (and asserts) if fragmentation, false if no fragmentation
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
bool 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::BFragmentedWedges()
{
    UINT_IDX iFace;
    UINT_IDX cFaces = m_tmTriMesh.m_cFaces;
    UINT_IDX iPoint;
    UINT_IDX wPointRep;
    UINT_IDX *rgiWedgesSeen = NULL;
    UINT_IDX wCurWedge;
    bool bRet = false;
    UINT cWedgesMax = 0;
    UINT cWedges = 0;
    UINT_IDX *pwFace;
    UINT ibVertexSeen;

    UINT_IDX iWedge;
    UINT_IDX wHead;
    UINT_IDX wCur;
    bool bFound;

    // seen array should be maintained as all zeros
    for (ibVertexSeen = 0; ibVertexSeen < m_tmTriMesh.m_cVertices; ibVertexSeen++)
    {
        GXASSERT(m_rgbVertexSeen[ibVertexSeen] == false);
    }

    for (iFace = 0; iFace < cFaces; iFace++)
    {
        pwFace = m_tmTriMesh.m_pFaces[iFace].m_wIndices;
        if (pwFace[0] == UNUSED)
            continue;

        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            wCurWedge = pwFace[iPoint];
            wPointRep = m_tmTriMesh.WGetPointRep(wCurWedge);

            // if the vertex has not been seen, and it is not the same as the representative
            //   then check for fragmentation.  if it is the same as the representative and
            //   there are other wedges on the vertex, then other triangles with the vertex
            //   will succeed on this check
            if ((wPointRep != wCurWedge) && !m_rgbVertexSeen[wPointRep])
            {
                // in case another face points to this vertex, mark as seen to skip extra checks
                m_rgbVertexSeen[wPointRep] = true;

                bRet = BFragmentedWedge(iFace, wCurWedge, cWedges, cWedgesMax, rgiWedgesSeen);
                if (bRet)
                {
                    goto e_Exit;
                }

                for (iWedge=0; iWedge < cWedges; iWedge++)
                {

                    wHead = wPointRep;
                    wCur = wHead;
                    bFound = false;
                    do
                    {
                        if (wCur == rgiWedgesSeen[iWedge])
                        {
                            bFound = true;
                            break;
                        }

                        wCur = m_rgiWedgeList[wCur];
                    }
                    while (wHead != wCur);

                    GXASSERT(bFound);
                }
            }
        }
    }

e_Exit:
    // keep the array as zero, so it doesn't need to be cleared out per edge collapse
    memset(m_rgbVertexSeen, 0, m_tmTriMesh.m_cVertices * sizeof(BYTE));

    if (rgiWedgesSeen != NULL)
    {
        delete []rgiWedgesSeen;
    }

    return bRet;
}

// -------------------------------------------------------------------------------
//  function    BValid
//
//   devnote    Asserts that the mesh (both embedded and additional simplication info)
//                  are semantically correct.
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
bool 
GXSimplifyMesh<UINT_IDX,b16BitIndex,UNUSED>::BValid() 
{ 
    UINT_IDX iFace;
    UINT_IDX cFaces = m_tmTriMesh.m_cFaces;
    UINT_IDX iPoint;
    UINT_IDX *pwFace;
    UINT_IDX iCurFace;
    UINT_IDX iVert;
    UINT_IDX wHead;
    UINT_IDX wCur;

    for (iVert = 0; iVert < m_tmTriMesh.m_cVertices; iVert++)
    {
        // if this is the representative of a vertex chain
        //   then check its wedge list
        if (m_tmTriMesh.WGetPointRep(iVert) == iVert)
        {
            wHead = iVert;
            wCur = iVert;
            do
            {
                GXASSERT(wCur < m_tmTriMesh.m_cVertices);
                GXASSERT(m_tmTriMesh.WGetPointRep(wCur) == iVert);

                wCur = m_rgiWedgeList[wCur];
            }
            while (wHead != wCur);
        }
    }

    if (m_pheapCosts != NULL)
    {
        GXASSERT(m_pheapCosts->BValid());
    }

    GXASSERT(!BFragmentedWedges());

    for (iFace = 0; iFace < cFaces; iFace++)
    {
        pwFace = m_tmTriMesh.m_pFaces[iFace].m_wIndices;

        if (pwFace[0] == UNUSED)
            continue;

        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            CFaceListIter<UINT_IDX,b16BitIndex,UNUSED> fli(iFace, pwFace[iPoint], &m_tmTriMesh, x_iAllFaces);

            GXASSERT(!m_rgbVertexDeleted[pwFace[iPoint]]);

            while (!fli.BEndOfList())
            {
                iCurFace = fli.GetNextFace();
                GXASSERT(m_tmTriMesh.FindPoint(m_tmTriMesh.m_pFaces[iCurFace].m_wIndices, pwFace[iPoint]) < 3);
            }
        }
    }

    return m_tmTriMesh.BValid(); 
}
#endif

// -------------------------------------------------------------------------------
//  Implementation of CSimplificationHeap
// -------------------------------------------------------------------------------

// -------------------------------------------------------------------------------
//  function    Add
//
//   devnote    adds an edge to the priority heap
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX>
HRESULT 
CSimplificationHeap<UINT_IDX>::Add
    (
    CEdgeInfo<UINT_IDX> *peiInfo
    )
{
    GXASSERT(peiInfo->m_iEdge < 3);
    GXASSERT(!_isnan(peiInfo->DGetCost()));

    // add the element
    m_rgpeiHeap[m_cElements] = peiInfo;
    peiInfo->SetHeapIndex(m_cElements);

    m_cElements += 1;

    Adjust(m_cElements - 1, true, false);
    return S_OK;
}

// -------------------------------------------------------------------------------
//  function    Update
//
//   devnote    updates the priority of an edge in the heap.
//                  if the edge is currently not in the heap, it is added
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX>
HRESULT
CSimplificationHeap<UINT_IDX>::Update
    (
    CEdgeInfo<UINT_IDX> *peiInfo
    )
{
    UINT iElement;

    GXASSERT(peiInfo->m_iEdge < 3);
    GXASSERT(!_isnan(peiInfo->DGetCost()));

    iElement = peiInfo->IGetHeapIndex();
    if (iElement == UINT_MAX)
        return Add(peiInfo);
    else
    {
        Adjust(iElement, true, true);
    }

    return S_OK;
}

#ifdef _DEBUG
// -------------------------------------------------------------------------------
//  function    DebugDump
//
//   devnote    Dumps all info in the heap for debugging purposes
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX>
HRESULT
CSimplificationHeap<UINT_IDX>::DebugDump
    (
    NeighborInfo<UINT_IDX> *rgpniNeighbors
    )
{
    UINT iElement;
    CEdgeInfo<UINT_IDX> *peiEdgeInfo;
    UINT iNeighbor;
    UINT cElements;

    GXTRACE("Debug heap dump: %d elements\n", m_cElements);

    cElements = min(m_cElements, 40);

    for (iElement = 0; iElement < cElements; iElement++)
    {
        GXTRACE("Element #%d\n", iElement); 

        peiEdgeInfo = m_rgpeiHeap[iElement];
        GXTRACE("    Face %d  Edge %d  Cost %f\n",
                            peiEdgeInfo->m_wFace, peiEdgeInfo->m_iEdge, 
                            peiEdgeInfo->m_cost);
        if (rgpniNeighbors != NULL)
        {
            iNeighbor = rgpniNeighbors[peiEdgeInfo->m_wFace].m_iNeighbors[peiEdgeInfo->m_iEdge];
            GXTRACE("    Alt Face %d\n", iNeighbor);
        }

        GXTRACE("\n");
    }

    return S_OK;
}
#endif

// -------------------------------------------------------------------------------
//  function    Delete
//
//   devnote    Removes the given edge from the heap
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX>
HRESULT 
CSimplificationHeap<UINT_IDX>::Delete(CEdgeInfo<UINT_IDX> *peiInfo)
{
    UINT iElement;

    GXASSERT(peiInfo->m_iEdge < 3);
    iElement = peiInfo->IGetHeapIndex();

    if (iElement == UINT_MAX)
        return S_OK;

    // if not the last element, then switch with the last element, then remove last element
    if (iElement < (m_cElements - 1))
    {
        Switch(iElement, m_cElements - 1);
    }
    m_cElements -= 1;

    Adjust(iElement, true, true);

    peiInfo->SetHeapIndex(UINT_MAX);
    return S_OK;
}

// -------------------------------------------------------------------------------
//  function    PeiExtractMin
//
//   devnote    Removes the next edge with the lowest cost from the heap
//                  and updates the heap
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX>
CEdgeInfo<UINT_IDX> * 
CSimplificationHeap<UINT_IDX>::PeiExtractMin()
{
    CEdgeInfo<UINT_IDX> *peiRet;

    if (m_cElements == 0)
        return NULL;

    // returning the topmost node
    peiRet = m_rgpeiHeap[0];

    // move to tail of queue so it can be removed
    if (m_cElements > 1) 
    {
        Switch(0, m_cElements - 1);
    }

    m_cElements -= 1;
    
    // move the new head to its correct position
    if (m_cElements > 1) 
    {
        Adjust(0, false, true);
    }

    // set heap index to a known bad value
    peiRet->SetHeapIndex(UINT_MAX);
    GXASSERT(!_isnan(peiRet->DGetCost()));
    return peiRet;
}

// -------------------------------------------------------------------------------
//  function    Switch
//
//   devnote    swaps the position of two elements in the heap
//
template <class UINT_IDX>
void
CSimplificationHeap<UINT_IDX>::Switch
    (
    UINT iElement, 
    UINT iOtherElement
    )
{
    CEdgeInfo<UINT_IDX> *peiTemp;

    // swap the positions in the array
    peiTemp = m_rgpeiHeap[iElement];
    m_rgpeiHeap[iElement] = m_rgpeiHeap[iOtherElement];
    m_rgpeiHeap[iOtherElement] = peiTemp;

    // update the indexes in the edge elements
    m_rgpeiHeap[iOtherElement]->SetHeapIndex(iOtherElement);
    m_rgpeiHeap[iElement]->SetHeapIndex(iElement);
}

// -------------------------------------------------------------------------------
//  function    Adjust
//
//   devnote    Adjusts the given element to the proper position in the priority heap
//
template <class UINT_IDX>
void
CSimplificationHeap<UINT_IDX>::Adjust(UINT iElement, bool bUp, bool bDown)
{
    const float fPriority = m_rgpeiHeap[iElement]->DGetCost();

    UINT iNextElement = 0;
    UINT iLeftElement;
    UINT iRightElement;
    float fLeftPriority;
    float fRightPriority;


    for (;;Switch(iElement,iNextElement),iElement=iNextElement) 
    {
        // if can move up, see if the next element up has a lower priority
        //   than the element being adjusted
        if (bUp) 
        {
            iNextElement = (iElement - 1) / 2;
            if ((iElement > 0) && (fPriority < m_rgpeiHeap[iNextElement]->DGetCost())) 
                continue;
        }

        // only process this section if can try moving down
        //    otherwise done adjusting the node up
        if (!bDown) 
            break;

        // get the left node, if no left node then done, since hit bottom (can't have a right without a left)
        iLeftElement = (iElement * 2) + 1;           
        if (iLeftElement >= m_cElements) 
            break;    // no children

        fLeftPriority = m_rgpeiHeap[iLeftElement]->DGetCost();


        // get the right node, if no right node then hit bottom, but might need to switch with left
        iRightElement = (iElement * 2) + 2;           
        if (iRightElement >= m_cElements) 
        {
            // if the left has a lower priority, then we need to switch the two nodes
            //   otherwise done
            if (fLeftPriority < fPriority) 
            { 
                iNextElement = iLeftElement; 
                continue; 
            }
            break;
        }
        fRightPriority = m_rgpeiHeap[iRightElement]->DGetCost();

        // if left has a lower priority than fPriority and it is the lowest child, then 
        //   swap with it
        if ((fLeftPriority < fPriority) && (fLeftPriority < fRightPriority)) 
        { 
            iNextElement=iLeftElement; 
            continue; 
        }

        // else if the right is lower than the priority then switch with it
        if (fRightPriority < fPriority) 
        { 
            iNextElement=iRightElement; 
            continue; 
        }

        break;
    }

}

// -------------------------------------------------------------------------------
//  function    BValid
//
//   devnote    Asserts that the priority heap is maintained correctly
//
//   returns    true if the heap is valid, asserts if not
//
template <class UINT_IDX>
bool
CSimplificationHeap<UINT_IDX>::BValid()
{
    UINT iElement;
    UINT iLeftElement;
    UINT iRightElement;

    GXASSERT(m_cElements <= m_cElementsMax);

    for (iElement = 0; iElement < m_cElements; iElement++)
    {
        GXASSERT(!_isnan(m_rgpeiHeap[iElement]->DGetCost()));

        // should have the correct heap index
        GXASSERT(m_rgpeiHeap[iElement]->IGetHeapIndex() == iElement);
        GXASSERT(m_rgpeiHeap[iElement]->m_iEdge < 3);

        // verify that the priority of the current element is less than that of the left
        //      element
        iLeftElement = (iElement * 2) + 1;           
        if (iLeftElement < m_cElements) 
        {
            GXASSERT(m_rgpeiHeap[iElement]->DGetCost() <= m_rgpeiHeap[iLeftElement]->DGetCost());
        }


        // verify that the priority of the current element is less than that of the right
        //      element
        iRightElement = (iElement * 2) + 2;           
        if (iRightElement < m_cElements) 
        {
            GXASSERT(m_rgpeiHeap[iElement]->DGetCost() <= m_rgpeiHeap[iRightElement]->DGetCost());
        }
    }

    return true;
}

// -------------------------------------------------------------------------------
//  function    Init
//
//   devnote    initializes a heap with the given number of elements
//
//   returns    S_OK if successful, else failure code
//
template <class UINT_IDX>
HRESULT
CSimplificationHeap<UINT_IDX>::Init
    (
    UINT cElements
    )
{
    HRESULT hr = S_OK;

    delete []m_rgpeiHeap;
    m_cElements = 0;

    m_rgpeiHeap = new CEdgeInfo<UINT_IDX>*[cElements];
    if (m_rgpeiHeap == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    m_cElementsMax = cElements;

e_Exit:
    return hr;
}


