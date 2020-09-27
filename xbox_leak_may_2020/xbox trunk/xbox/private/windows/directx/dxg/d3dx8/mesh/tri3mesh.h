#pragma once

#ifndef __TRI3MESH_H__
#define __TRI3MESH_H__

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
// -@- 08/19/99 (mikemarr)  - started comment history
//                          - added #include <limits.h>
//                          - added #pragma once
//
// @@END_MSINTERNAL
//
//////////////////////////////////////////////////////////////////////////////*/

#include "gxcrackfvf.h"

// Disable warnings of loss of data conversion assignments
#pragma warning(disable:4242)

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED> class GXSimplifyMesh;
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED> class GXHalfEdgePMesh;
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED> class CFaceListIter;
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED> class CVertexCache;
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED> class CMeshStatus;
template <class UINT_IDX, unsigned int UNUSED> class CIndexQueue;
template <class UINT_IDX, unsigned int UNUSED> class CSimVertexCache;


#define UINT32UNUSED (0xffffffff)
#define UINT16UNUSED (0xffff)

// constants for use in template parameters
#define tp32BitIndex unsigned int, false, UINT32UNUSED
#define tp16BitIndex unsigned short, true, UINT16UNUSED

#define D3DXMESHINT_FACEADJACENCY         0x001
#define D3DXMESHINT_POINTREP              0x002
#define D3DXMESHINT_ATTRIBID              0x004
#define D3DXMESHINT_ATTRIBINDEX           0x008
#define D3DXMESHINT_ATTRIBUTETABLE        0x010
#define D3DXMESHINT_SHAREDVB              0x020

// @@BEGIN_MSINTERNAL
#define D3DXMESHINT_VALIDBITS             0x03f
// @@END_MSINTERNAL


struct SFaceCorner
{
    DWORD iFace;
    DWORD iPoint;

    SFaceCorner(DWORD iFaceNew, DWORD iPointNew)
        :iFace(iFaceNew), iPoint(iPointNew) {}
    SFaceCorner()
        :iFace(UNUSED32), iPoint(UNUSED32) {}
};

template <class UINT_IDX>
class NeighborInfo
{
public:
    NeighborInfo() { }
    NeighborInfo( UINT_IDX iNeighbor0, UINT_IDX iNeighbor1, UINT_IDX iNeighbor2 )
    {
        m_iNeighbors[0] = iNeighbor0;
        m_iNeighbors[1] = iNeighbor1;
        m_iNeighbors[2] = iNeighbor2;
    }
    NeighborInfo( UINT_IDX iNeighbors[3] )
    {
        m_iNeighbors[0] = iNeighbors[0];
        m_iNeighbors[1] = iNeighbors[1];
        m_iNeighbors[2] = iNeighbors[2];
    }
    UINT_IDX m_iNeighbors[3];
};


template <class UINT_IDX>
class GXTri3Face
{
public:
    GXTri3Face() { }
    GXTri3Face( UINT_IDX w0, UINT_IDX w1, UINT_IDX w2 )
    {
        m_wIndices[0] = w0;
        m_wIndices[1] = w1;
        m_wIndices[2] = w2;
    }
    GXTri3Face( UINT_IDX wIndices[3] )
    {
        m_wIndices[0] = wIndices[0];
        m_wIndices[1] = wIndices[1];
        m_wIndices[2] = wIndices[2];
    }
    UINT_IDX m_wIndices[3];
};

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
class GXTri3Mesh : public ID3DXMesh
{
    friend class GXTri3Mesh<tp16BitIndex>;
    friend class GXTri3Mesh<tp32BitIndex>;
    friend class GXSimplifyMesh<tp16BitIndex>;
    friend class GXSimplifyMesh<tp32BitIndex>;
    friend class GXHalfEdgePMesh<tp16BitIndex>;
    friend class GXHalfEdgePMesh<tp32BitIndex>;
    friend class CFaceListIter<UINT_IDX, b16BitIndex, UNUSED>;
    friend class CVertexCache<UINT_IDX, b16BitIndex, UNUSED>;
	friend class CD3DXSkinMesh;
public:
    GXTri3Mesh( LPDIRECT3DDEVICE8 pD3DDevice, DWORD dwFVF, DWORD dwOptions );
    ~GXTri3Mesh( );

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj);
    virtual ULONG STDMETHODCALLTYPE AddRef( void) 
        
    {
        // if this is contained in another object defer the call to that object
        if (m_punkOuter != NULL)
            return m_punkOuter->AddRef();

        m_cRef += 1;
        return m_cRef;
    }

    virtual ULONG STDMETHODCALLTYPE Release( void) 
    {
        // if this is contained in another object defer the call to that object
        if (m_punkOuter != NULL)
            return m_punkOuter->Release();

        ULONG cRef = m_cRef;
        m_cRef -= 1;

        if (cRef == 1)
            delete this;

        return cRef-1;
    }

    // ID3DXBaseMesh
    STDMETHOD(DrawSubset)(THIS_ DWORD attribId);
    STDMETHOD_(DWORD, GetNumFaces)(THIS);
    STDMETHOD_(DWORD, GetNumVertices)(THIS);
    STDMETHOD_(DWORD, GetFVF)(THIS);
    STDMETHOD(GetDeclaration)(THIS_ DWORD Declaration[MAX_FVF_DECL_SIZE]);
    STDMETHOD_(DWORD, GetOptions)(THIS);
    STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE8* ppDevice)
                { 
                    if (ppDevice == NULL)
                    {
                        DPF(0, "ppDevice pointer is invalid");
                        return D3DERR_INVALIDCALL;
                    }
                    m_pD3DDevice->AddRef(); 
                    *ppDevice = m_pD3DDevice;
                    return S_OK; 
                }
    STDMETHOD(CloneMeshFVF)(THIS_ DWORD options, 
                DWORD fvf, LPDIRECT3DDEVICE8 pD3DDevice, LPD3DXMESH* ppCloneMesh);
    STDMETHOD(CloneMesh)(THIS_ DWORD options, 
                CONST DWORD *pDeclaration, LPDIRECT3DDEVICE8 pD3DDevice, LPD3DXMESH* ppCloneMesh);
    STDMETHOD(GetVertexBuffer)(THIS_ LPDIRECT3DVERTEXBUFFER8* ppVB);
    STDMETHOD(GetIndexBuffer)(THIS_ LPDIRECT3DINDEXBUFFER8* ppIB);
	STDMETHOD(LockVertexBuffer)(THIS_ DWORD flags, BYTE** ppData)
                { return m_pVBVertices->Lock(0, 0, ppData, flags ); }
	STDMETHOD(UnlockVertexBuffer)(THIS)
                { return m_pVBVertices->Unlock(); }
	STDMETHOD(LockIndexBuffer)(THIS_ DWORD flags, BYTE** ppData)
                { return m_pibFaces->Lock(0, 0, ppData, flags ); }
	STDMETHOD(UnlockIndexBuffer)(THIS)
                { return m_pibFaces->Unlock(); }
    STDMETHOD(GetAttributeTable)(
                THIS_ D3DXATTRIBUTERANGE *attribEntry, DWORD* attribTabSize);

    // ID3DXMesh
	STDMETHOD(LockAttributeBuffer)(THIS_ DWORD flags, DWORD** ppData)
                { 
                    // if not locked read only, remove the attribute table
                    if (!(flags & D3DLOCK_READONLY))
                    {
                        delete []m_rgaeAttributeTable;
                        m_rgaeAttributeTable = NULL;
                        m_caeAttributeTable = 0;
                        m_dwOptionsInt &= ~D3DXMESHINT_ATTRIBUTETABLE;
                    }

                    *ppData = m_rgiAttributeIds; 
                    return S_OK; 
                }
	STDMETHOD(UnlockAttributeBuffer)(THIS)
                { return S_OK; }

    STDMETHOD(ConvertPointRepsToAdjacency)(THIS_ CONST DWORD* PRep, DWORD* fAdjacency);
    STDMETHOD(ConvertAdjacencyToPointReps)(THIS_ CONST DWORD* fAdjacency, DWORD* PRep);
    STDMETHOD(GenerateAdjacency)(THIS_ FLOAT fEpsilon, DWORD* pAdjacency);

    STDMETHOD(Optimize)(THIS_ DWORD flags, CONST DWORD* adjacency, DWORD* optAdj, 
                     DWORD* faceRemap, LPD3DXBUFFER *ppbufVertexRemap,  
                     LPD3DXMESH* ppOptMesh);
    STDMETHOD(OptimizeInplace)(THIS_ DWORD Flags, CONST DWORD* pAdjacencyIn, DWORD* pAdjacencyOut, 
                     DWORD* pFaceRemap, LPD3DXBUFFER *ppVertexRemap);


    // internal
    inline HRESULT LockVB(PBYTE *ppvDest)
    {
        return m_pVBVertices->Lock(0,0, ppvDest, 0 );
    }

    inline HRESULT UnlockVB()
    {
        return m_pVBVertices->Unlock();
    }

    inline HRESULT LockIB(PBYTE *ppvDest)
    {
        return m_pibFaces->Lock(0,0, ppvDest, 0 );
    }

    inline HRESULT UnlockIB()
    {
        return m_pibFaces->Unlock();
    }

    HRESULT Resize(UINT cFaces, UINT cVertices);

private:

    virtual bool        BValid(GXTri3Face<UINT_IDX> *pwFacesLocked = NULL);
    bool BValidAttributeGroups(GXTri3Face<UINT_IDX> *pwFaces);

    // simplification support
    HRESULT CopyMesh(GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED> *ptmSrc);
    HRESULT CloneHelper16(GXTri3Mesh<tp16BitIndex> *ptmNewMesh16);
    HRESULT CloneHelper32(GXTri3Mesh<tp32BitIndex> *ptmNewMesh32);
    HRESULT CloneVertexBuffer(DWORD dwFVFNew, LPDIRECT3DVERTEXBUFFER8 pVertexBuffer);

    // optimization functions
    HRESULT Optimize2(DWORD options, DWORD flags, DWORD* adjacency, DWORD* optAdj, DWORD* faceRemap, 
                      LPD3DXBUFFER *ppbufVertexRemap, DWORD FVF, GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>** ppOptMesh);
    HRESULT OptimizeInternal(DWORD dwFlags, CONST DWORD *rgdwNeighbors, DWORD* rgdwNeighborsOut, 
         DWORD* rgiFaceRemapInverseOut, LPD3DXBUFFER *ppbufVertexRemapInverseOut, LPDIRECT3DVERTEXBUFFER8 pOrigVertexBuffer, DWORD dwOrigFVF);
    HRESULT Compact(UINT *rgiFaceRemapUser, UINT *rgiFaceRemapInverse, UINT *rgiVertexRemapUser);
    HRESULT AttributeSort(UINT *rgiFaceRemapUser, UINT *rgiFaceRemapInverse, UINT *rgiVertexRemapUser, BOOL bIgnoreVerts);
    HRESULT VertexOptimize(UINT *rgiFaceRemapInverse, UINT *rgiVertRemap);
    HRESULT SplitSharedVertices(DWORD **prgdwNewVertexOrigin, UINT *pcNewVertices);
    //HRESULT VertexAttributeSort(UINT *rgiFaceRemapInverse, UINT *rgiVertexRemapUser);
    HRESULT StripReorder(DWORD dwOptions, UINT *rgiFaceRemap, UINT *rgiFaceRemapInverse, CONST DWORD *rgdwNeighbors);
    HRESULT ReorderVertexData(UINT *rgiVertRemap, PBYTE pvPoints);
    HRESULT ReorderFaceData(UINT *rgiFaceRemap, UINT *rgiVertRemap);

    // strip reorder optimizations
    HRESULT SGIStripReorder(CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus, UINT *rgiFaceRemap, UINT *rgiFaceRemapInverse);
    HRESULT VCacheStripReorder(CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus, UINT *rgiFaceRemap, UINT *rgiFaceRemapInverse);
    HRESULT OldVCacheStripReorder(CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus, UINT *rgiFaceRemap, UINT *rgiFaceRemapInverse);
    

    // vertex cache simulations
    UINT_IDX IVCFindNextFace(UINT_IDX iFace, UINT_IDX &cFacesToContinueBest, UINT_IDX &cFacesInStrip, CIndexQueue<UINT_IDX,UNUSED> &iqPossibleRestarts, CIndexQueue<UINT_IDX,UNUSED> &iqTemp, CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus, 
            CSimVertexCache<UINT_IDX,UNUSED> &svc, CSimVertexCache<UINT_IDX,UNUSED> &svcTemp);
    HRESULT VCSimulate(UINT cFacesToContinue, UINT_IDX iFace, UINT cFacesInStripOrig, CIndexQueue<UINT_IDX,UNUSED> &iqPossibleRestartsOrig, CIndexQueue<UINT_IDX,UNUSED> &iqPossibleRestarts, CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus, 
                        CSimVertexCache<UINT_IDX,UNUSED> &svcOrig, CSimVertexCache<UINT_IDX,UNUSED> &svc,
                        UINT cMaxFacesPerStrip, float &fEstimate);
    UINT_IDX IVCRestartStrip(CIndexQueue<UINT_IDX,UNUSED> &iqPossibleRestarts, CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus);
    HRESULT CalculateMissRate(UINT *rgiFaceRemapInverse, PUINT pcMisses);
    SFaceCorner ClwFaceCorner(SFaceCorner &cn);
    SFaceCorner CcwFaceCorner(SFaceCorner &cn);
    SFaceCorner CcwCorner(SFaceCorner &cn, CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus);
    SFaceCorner ClwCorner(SFaceCorner &cn, CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus);
    void TryStartingEarlier(SFaceCorner &cn, CMeshStatus<UINT_IDX,b16BitIndex,UNUSED> *pmsMeshStatus);

    // reorder methods, used by simplification only
    HRESULT Reorder(UINT *rgiVertRemap, UINT ciVertRemapUser, 
              UINT *rgiFaceRemap, UINT ciFaceRemapUser);
    void RemapVertex(UINT iVert, UINT *rgiVertRemap);
    void RemapFace(UINT iFace, UINT *rgiVertRemap, UINT *rgiFaceRemap);
    void SwapVertex(UINT iVertSrc, UINT iVertDest, PBYTE pvPoints);
    void SwapFace(UINT iFaceSrc, UINT iFaceDest);
    HRESULT CollectAttributeIds(DWORD **prgiAttribIds, LPDWORD pcattr);

    //HRESULT SplitTriangle( UINT_IDX iFace, UINT_IDX iEdge, UINT_IDX iNewPoint, UINT *pNewFace);
    HRESULT AddVertex( PBYTE pvPoint, UINT_IDX wPointRep);

    //HRESULT ChangeNeighbor(UINT_IDX iFace, UINT_IDX iOldNeighbor, UINT_IDX iNewNeighbor);
    void    MarkAsUnused(UINT_IDX iFace);

    //bool  BPointInFace(UINT_IDX *pwIndices, UINT iPointSearch) const;
    UINT  FindPoint(UINT_IDX *pwIndices, UINT iPointSearch) const;
    UINT  FindWedge(UINT_IDX *pwIndices, UINT iWedgeSearch) const;
    //UINT_IDX  FindPointOffset(UINT_IDX iPointSearch, UINT_IDX iFace) const { return FindPoint(m_pFaces[iFace].m_wIndices, iPointSearch); }
    //UINT  CalculateNewNeighbor(UINT iNeighbor, UINT_IDX *pwFace, UINT_IDX iPointShared) const;
    //bool  EquivalentAttributesOnEdge(UINT_IDX iFace1, UINT_IDX iEdge1, UINT_IDX iFace2, UINT_IDX iEdge2) const;

    HRESULT CreateEmptyMesh();


    // not exposed externally, but used in simplification
    inline UINT BHasNeighborData() const        { return m_dwOptionsInt & D3DXMESHINT_FACEADJACENCY; }
    inline UINT BHasPointRepData() const        { return m_dwOptionsInt & D3DXMESHINT_POINTREP; }

    // always present on a normal mesh, but not present for half edge pms
    inline UINT BHasPerFaceAttributeId()  const { return m_dwOptionsInt & D3DXMESHINT_ATTRIBID; }

    // only used for half edge PMs
    inline UINT BHasPerFaceAttributeIndex() const { return m_dwOptionsInt & D3DXMESHINT_ATTRIBINDEX; }

    inline UINT BHasAttributeTable() const { return m_dwOptionsInt & D3DXMESHINT_ATTRIBUTETABLE; }

	inline UINT BSharedVB() const               { return m_dwOptionsInt & D3DXMESHINT_SHAREDVB; }

    // are the two points logically equivalent, ignoring the color, uv, etc.
    inline bool  BEqualPoints(UINT_IDX iPoint1, UINT_IDX iPoint2) const
    {
        GXASSERT(BHasPointRepData());
        GXASSERT((iPoint1 < m_cVertices) && (iPoint2 < m_cVertices));

        return m_rgwPointReps[iPoint1] == m_rgwPointReps[iPoint2];
    }

    inline UINT_IDX  WGetPointRep(UINT_IDX iPoint) const
    {
        GXASSERT(iPoint < m_cVertices);
        GXASSERT(BHasPointRepData());

        return m_rgwPointReps[iPoint];
    }

    DWORD   m_dwFVF;
    DWORD   m_dwOptions;
    DWORD   m_dwOptionsInt;
    D3DPOOL m_dwPoolIB, m_dwPoolVB;
    DWORD   m_dwUsageIB, m_dwUsageVB;
    LPDIRECT3D8 m_pD3D;
    LPDIRECT3DDEVICE8 m_pD3DDevice;

    // Vertices
    LPDIRECT3DVERTEXBUFFER8 m_pVBVertices;

    UINT    m_cBytesPerVertex;
    UINT    m_cVertices, m_cMaxVertices;

    UINT_IDX *m_rgwPointReps;

    LPDIRECT3DINDEXBUFFER8 m_pibFaces;
    GXTri3Face<UINT_IDX> *m_pFaces;
    NeighborInfo<UINT_IDX> *m_rgpniNeighbors;
    
    DWORD *m_rgiAttributeIds;
    UINT16 *m_rgiMaterialIndex;                   // per face material index instead of attribId

    D3DXATTRIBUTERANGE *m_rgaeAttributeTable;
    UINT m_caeAttributeTable;

    UINT m_cFaces, m_cMaxFaces;
    UINT m_cRef;

    IUnknown *m_punkOuter;
};


// -------------------------------------------------------------------------------
//  function    FindPoint
//
//   devnote    Simple helper function to determine where a point is in a face
//                  the pwIndices array is a three element array specifiying 
//                  points in a triangle face  
//
//   returns    if < 3, then iPointSearch is the position in the given face
//                      else when >= 3, the point is not in the given array
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
inline UINT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::FindPoint(UINT_IDX *pwIndices, UINT iPointSearch) const
{
    // get the representative for the point, so that we can compare
    //   them, this function compares the logical points in the mesh
    UINT_IDX wPointSearchRep = WGetPointRep(iPointSearch);


	if (WGetPointRep(pwIndices[0])==wPointSearchRep)
		return 0;
	else if (WGetPointRep(pwIndices[1])==wPointSearchRep)
		return 1;
    else if (WGetPointRep(pwIndices[2])==wPointSearchRep)
	    return 2;
    else
        return 3;
}

// -------------------------------------------------------------------------------
//  function    FindWedge
//
//   devnote    Simple helper function to determine where a wedge is in a face
//                  the pwIndices array is a three element array specifiying 
//                  points in a triangle face  
//
//   returns    if < 3, then iWedgeSearch is the position in the given face
//                      else when >= 3, the point is not in the given array
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
inline UINT
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::FindWedge(UINT_IDX *pwIndices, UINT iWedgeSearch) const
{
	if (pwIndices[0]==iWedgeSearch)
		return 0;
	else if (pwIndices[1]==iWedgeSearch)
		return 1;
    else if (pwIndices[2]==iWedgeSearch)
	    return 2;
    else
        return 3;
}


// -------------------------------------------------------------------------------
//  function    FindEdge
//
//   devnote    Simple helper function to determine which edge of a neighbor
//                  face, points back to the given face
//                  the pwIndices array is a three element array specifiying 
//                  neighbors to a triangle face  
//
//   returns    if < 3, then iFaceSearch is the position in the given neighbor posiistion
//                      else when >= 3, the face is not in the given array
//
template<class UINT_IDX>
inline UINT
FindEdge(UINT_IDX *rgiIndices, UINT_IDX iFaceSearch)
{
    UINT iNeighborEdge;

    // find the edge that points to this triangle in the neighbor
    for (iNeighborEdge = 0; iNeighborEdge < 3; iNeighborEdge++)
    {
        if (rgiIndices[iNeighborEdge] == iFaceSearch)
        {
            break;
        }
    }

    return iNeighborEdge;
}


const BYTE x_iAllFaces = 0;
const BYTE x_iClockwise = 1;
const BYTE x_iCounterClockwise = 2;

// -------------------------------------------------------------------------------
//  class    CFaceListIter
//
//   devnote    used to simplify orbiting a vertex
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
class CFaceListIter
{
public:
    CFaceListIter(GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED> *ptmTriMesh)
        :m_iFace(UNUSED), m_wPoint(UNUSED), m_iCurFace(UNUSED), 
                        m_ptmTriMesh(ptmTriMesh), m_bClockwise(false), 
                        m_bStopOnBoundary(false)
    {
        // used to allow creation of the stack, and then intialization later
    }

    CFaceListIter(UINT_IDX iFace, UINT_IDX wPoint, GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED> *ptmTriMesh, BYTE iWalkType)
        :m_iFace(iFace), m_wPoint(wPoint), m_iCurFace(iFace), 
                        m_ptmTriMesh(ptmTriMesh), m_bClockwise(iWalkType != x_iCounterClockwise), 
                        m_bStopOnBoundary(iWalkType != x_iAllFaces)
    { 
        GXASSERT(iWalkType >= 0 && iWalkType <= 2);

        // find the next edge to jump to the next triangle
        m_iNextEdge = m_ptmTriMesh->FindWedge(m_ptmTriMesh->m_pFaces[iFace].m_wIndices, wPoint);
        GXASSERT(m_iNextEdge < 3);

        if (!m_bClockwise)
        {
            m_iNextEdge = (m_iNextEdge + 2) % 3;
        }

        m_iCurEdge = m_iNextEdge;
    }

    void Init(UINT_IDX iFace, UINT_IDX wPoint, BYTE iWalkType)
    {
        GXASSERT(iWalkType >= 0 && iWalkType <= 2);

        m_iFace = iFace;
        m_wPoint = wPoint;
        m_iCurFace = iFace;
        m_bClockwise = (iWalkType != x_iCounterClockwise);
        m_bStopOnBoundary = (iWalkType != x_iAllFaces);

        // find the next edge to jump to the next triangle
        m_iNextEdge = m_ptmTriMesh->FindWedge(m_ptmTriMesh->m_pFaces[iFace].m_wIndices, wPoint);
        GXASSERT(m_iNextEdge < 3);
        if (!m_bClockwise)
        {
            m_iNextEdge = (m_iNextEdge + 2) % 3;
        }

        m_iCurEdge = m_iNextEdge;
    }

    bool BEndOfList() { return m_iCurFace == UNUSED; }

    UINT_IDX GetNextFace();
    void GetNextEdgeFace(UINT_IDX &iEdge, UINT_IDX &iFace);

    bool MoveToCCWFace();

    void PeekNextFace(UINT_IDX &iFace, int &iPointOffset) const 
        {
            iFace = m_iCurFace;
            iPointOffset = m_iNextEdge;
        }


    UINT_IDX IGetPointIndex() const { return m_bClockwise ? m_iCurEdge : (m_iCurEdge + 1) % 3; }


    void GetCCWFace(UINT_IDX iFace, UINT_IDX wPointRep, UINT_IDX &iFaceCCW, int &iPointOffset) const
    {
        UINT_IDX iNextEdge;

        iNextEdge = m_ptmTriMesh->FindPoint(m_ptmTriMesh->m_pFaces[iFace].m_wIndices, wPointRep);
        iNextEdge = (m_iNextEdge + 2) % 3;

        iFaceCCW = m_ptmTriMesh->m_rgpniNeighbors[iFace].m_iNeighbors[iNextEdge];
        if (iFaceCCW != UNUSED)
        {
            iPointOffset = m_ptmTriMesh->FindPoint(m_ptmTriMesh->m_pFaces[iFaceCCW].m_wIndices, wPointRep);
        }
    }

private:
    UINT_IDX m_iFace;
    UINT_IDX m_wPoint;

    UINT_IDX m_iCurFace;
    UINT_IDX m_iCurEdge;
    UINT_IDX m_iNextEdge;
    GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED> *m_ptmTriMesh;

    bool m_bClockwise;
    bool m_bStopOnBoundary;
};

#ifdef _DEBUG

inline BOOL
CheckAdjacency
    (
    CONST DWORD *rgdwAdjacency,
    DWORD cFaces
    )
{
    DWORD iIndex;

    if (rgdwAdjacency != NULL)
    {
        for (iIndex = 0; iIndex < cFaces * 3; iIndex++)
        {
            GXASSERT((rgdwAdjacency[iIndex] == UNUSED32) || (rgdwAdjacency[iIndex] < cFaces));      

            if (rgdwAdjacency[iIndex] != UNUSED32)
            {
                GXASSERT(FindEdge(&rgdwAdjacency[rgdwAdjacency[iIndex] * 3], iIndex / 3) < 3);
            }
        }
    }

    return TRUE;
}

#endif

// struct used to contain state for cross fvf vertex copies
struct SVertexCopyContext
{
    DXCrackFVF cfvfSrc;
    DWORD rgdwTexCoordSizesSrc[8];

    DXCrackFVF cfvfDest;
    DWORD rgdwTexCoordSizesDest[8];

    DWORD cTexCoordsToCopy;
    DWORD rgdwTexCoordMins[8];
    DWORD rgdwTexCoordZero[8];
    DWORD cWeightsToCopy;
    DWORD cWeightsToZero;

    BOOL bCopyNormals;
    BOOL bCopyDiffuse;
    BOOL bCopySpecular;
    BOOL bCopyPointSize;

    // setup info required for fvf conversion
    SVertexCopyContext(DWORD dwFVFSrc, DWORD dwFVFDest)
        :cfvfSrc(dwFVFSrc), cfvfDest(dwFVFDest)
    {
        DWORD iTexCoord;

        cfvfSrc.GetTexCoordSizes(rgdwTexCoordSizesSrc);
        cfvfDest.GetTexCoordSizes(rgdwTexCoordSizesDest);

        // setup texture coord copy info
        cTexCoordsToCopy = min(cfvfSrc.CTexCoords(), cfvfDest.CTexCoords());
        for (iTexCoord = 0; iTexCoord < cTexCoordsToCopy; iTexCoord++)
        {
            GXASSERT(rgdwTexCoordSizesDest[iTexCoord] != 0);
            GXASSERT(rgdwTexCoordSizesSrc[iTexCoord] != 0);
            rgdwTexCoordMins[iTexCoord] = min(rgdwTexCoordSizesDest[iTexCoord], rgdwTexCoordSizesSrc[iTexCoord]);
            rgdwTexCoordZero[iTexCoord] = rgdwTexCoordSizesDest[iTexCoord] - rgdwTexCoordMins[iTexCoord];
        }

        // setup copy flags
        bCopyNormals = cfvfSrc.BNormal() && cfvfDest.BNormal();
        bCopyDiffuse = cfvfSrc.BDiffuse() && cfvfDest.BDiffuse();
        bCopySpecular = cfvfSrc.BSpecular() && cfvfDest.BSpecular();
        bCopyPointSize = cfvfSrc.BPointSize() && cfvfDest.BPointSize();

        cWeightsToCopy = min(cfvfDest.CWeights(), cfvfSrc.CWeights());
        cWeightsToZero = cfvfDest.CWeights() - cWeightsToCopy;
    }

    // copy a vertex doing the fvf conversion in the process
    void CopyVertex(PBYTE pvCurPointSrc, PBYTE pvCurPointDest)
    {
        PBYTE pbTexCoordsSrc;
        PBYTE pbTexCoordsDest;
        DWORD iTexCoord;
        float *pfWeights;

        cfvfDest.SetPosition(pvCurPointDest, cfvfSrc.PvGetPosition(pvCurPointSrc));

        pfWeights = (float*)(pvCurPointDest + sizeof(D3DXVECTOR3));
        if (cWeightsToCopy > 0)
        {
            memcpy(pfWeights, pvCurPointSrc + sizeof(D3DXVECTOR3), sizeof(float) * cWeightsToCopy);
            pfWeights += cWeightsToCopy;
        }
        if (cWeightsToZero > 0)
        {
            memset(pfWeights, 0, sizeof(float) * cWeightsToZero);
        }

        if (bCopyNormals)
        {
            cfvfDest.SetNormal(pvCurPointDest, cfvfSrc.PvGetNormal(pvCurPointSrc));
        }
        else if (cfvfDest.BNormal())
        {
            memset(pvCurPointDest + cfvfDest.m_oNormal, 0, sizeof(D3DXVECTOR3));
        }

        if (bCopyPointSize)
        {
            cfvfDest.SetPointSize(pvCurPointDest, cfvfSrc.FGetPointSize(pvCurPointSrc));
        }
        else if (cfvfDest.BPointSize())
        {
            cfvfDest.SetPointSize(pvCurPointDest, 0.0);
        }

        if (bCopyDiffuse)
        {
            cfvfDest.SetDiffuse(pvCurPointDest, cfvfSrc.ColorGetDiffuse(pvCurPointSrc));
        }
        else if (cfvfDest.BDiffuse())
        {
            cfvfDest.SetDiffuse(pvCurPointDest, 0);
        }

        if (bCopySpecular)
        {
            cfvfDest.SetSpecular(pvCurPointDest, cfvfSrc.ColorGetSpecular(pvCurPointSrc));
        }
        else if (cfvfDest.BSpecular())
        {
            cfvfDest.SetSpecular(pvCurPointDest, 0);
        }

        // copy tex coord data to be saved
        pbTexCoordsSrc = (PBYTE)cfvfSrc.PuvGetTex1(pvCurPointSrc);
        pbTexCoordsDest = (PBYTE)cfvfDest.PuvGetTex1(pvCurPointDest);
        for (iTexCoord = 0; iTexCoord < cTexCoordsToCopy; iTexCoord++)
        {
            GXASSERT(rgdwTexCoordMins[iTexCoord] > 0);
            memcpy(pbTexCoordsDest, pbTexCoordsSrc, rgdwTexCoordMins[iTexCoord]);

            // set unavailable ones to 0
            if (rgdwTexCoordZero[iTexCoord] > 0)
            {
                memset(pbTexCoordsDest + rgdwTexCoordMins[iTexCoord], 0, rgdwTexCoordZero[iTexCoord]);
            }

            pbTexCoordsSrc += rgdwTexCoordSizesSrc[iTexCoord];
            pbTexCoordsDest += rgdwTexCoordSizesDest[iTexCoord];
        }

        // memset any tex coords unavailable in src to copy to zero
        for (iTexCoord = cTexCoordsToCopy; iTexCoord < cfvfDest.CTexCoords(); iTexCoord++)
        {
            memset(pbTexCoordsDest, 0, rgdwTexCoordSizesDest[iTexCoord]);

            pbTexCoordsDest += rgdwTexCoordSizesDest[iTexCoord];
        }
    }
};

#include "tri3drawmesh.inl"
#include "tri3optmesh.inl"
#include "tri3mesh.inl"
#include "tri3editmesh.inl"

#pragma warning(default:4242)

#endif
