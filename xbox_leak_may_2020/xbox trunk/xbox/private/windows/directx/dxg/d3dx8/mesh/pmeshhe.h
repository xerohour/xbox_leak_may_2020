#ifndef __PMESHHE_H__
#define __PMESHHE_H__

// Disable warnings of loss of data conversion assignments
#pragma warning(disable:4242)

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED> class GXSimplifyMesh;

// Vertex split record.
// Records the information necessary to split a vertex of the mesh,
//  in order to add to the mesh 1 new vertex and 1/2 new faces.
class CHalfEdgeVSplit 
{
public:       
    
    inline DWORD CFaces() const {return ((m_oVlrOffset > 0) ? 2:1);}
    
    // This format provides these limits:
    // - maximum number of faces: 1<<32
    // - maximum vertex valence:  1<<16
    // - maximum number of materials: 1<<16
    // Encoding of vertices vs, vl, vr.
    // Face flclw is the face just CLW of vl from vs.
    //  vs is the vs_index\'th vertex of flclw
    //  (vl is the (vs_index+2)%3\'th vertex of face flclw)
    //  vr is the (vlr_offset1-1)\'th vertex when rotating CLW about vs from vl
    // Special cases:
    // - vlr_offset1==0 : no_vr and no_fr
    DWORD m_iFlclw;         // 0..(mesh.numFaces()-1)
    WORD m_oVlrOffset; // 0..(max_vertex_valence) (prob<valence/2)
    WORD m_code;        // (vs_index(2),ii(2),ws(3),wt(3),wl(2),wr(2),
    //  fl_matid>=0(1),fr_matid>=0(1))
    enum {
        B_STMASK=0x0007,
            B_LSAME=0x0001,
            B_RSAME=0x0002,
            B_CSAME=0x0004,
            //
            B_LRMASK=0x0003,
            B_ABOVE=0x0000,
            B_BELOW=0x0001,
            B_NEW  =0x0002,         // must be on separate bit.
    };
    enum {
        VSINDEX_SHIFT=0,
            VSINDEX_MASK=(0x0003<<VSINDEX_SHIFT),
            //
            S_SHIFT=4,
            S_MASK=(B_STMASK<<S_SHIFT),
            S_LSAME=(B_LSAME<<S_SHIFT),
            S_RSAME=(B_RSAME<<S_SHIFT),
            S_CSAME=(B_CSAME<<S_SHIFT),
            //
            T_SHIFT=7,
            T_MASK=(B_STMASK<<T_SHIFT),
            T_LSAME=(B_LSAME<<T_SHIFT),
            T_RSAME=(B_RSAME<<T_SHIFT),
            T_CSAME=(B_CSAME<<T_SHIFT),
            //
            L_SHIFT=10,
            L_MASK=(B_LRMASK<<L_SHIFT),
            L_ABOVE=(B_ABOVE<<L_SHIFT),
            L_BELOW=(B_BELOW<<L_SHIFT),
            L_NEW  =(B_NEW<<L_SHIFT),
            //
            R_SHIFT=12,
            R_MASK=(B_LRMASK<<R_SHIFT),
            R_ABOVE=(B_ABOVE<<R_SHIFT),
            R_BELOW=(B_BELOW<<R_SHIFT),
            R_NEW  =(B_NEW<<R_SHIFT),
            //
            FLN_SHIFT=14,
            FLN_MASK=(1<<FLN_SHIFT),
            //
            FRN_SHIFT=15,
            FRN_MASK=(1<<FRN_SHIFT),
    };
    //Documentation:
    // vs_index: 0..2: index of vs within flace flclw
    // Inside wedges
    //  {S,T}{LSAME}: if exists outside left wedge and if same
    //  {S,T}{RSAME}: if exists outside right wedge and if same
    //  {S,T}{CSAME}: if inside left and right wedges are same
    //  (when no_vr, {S,T}RSAME==1, {S,T}CSAME==0)
    // Outside wedges
    //  (when no_vr, RABOVE==1)
    // New face material identifiers
    //  {L,R}NF: if 1, face matids not predicted correctly using ii,
    //     so included in f{l,r}_matid
    //  (when no_vr, RNF==0 obviously)
    //
    //Probabilities:
    //  vs_index: 0..2 (prob. uniform)
    //  {S,T}LSAME: prob. high
    //  {S,T}RSAME: prob. high
    //  {S,T}CSAME: prob. low
    //  {L,R}ABOVE: prob. high
    //  {L,R}BELOW: prob. low
    //  {L,R}NEW:   prob. low
    // Note: wl, wr, ws, wt are correlated since scalar half-edge
    //  discontinuities usually match up at both ends of edges.
    // -> do entropy coding on (wl,wr,ws,wt) symbol as a whole.
    
    int CExpectedWad() const;
    int CWedgesAdded() const;

    inline BYTE IGetVsIndex() const { return (m_code & CHalfEdgeVSplit::VSINDEX_MASK) >> CHalfEdgeVSplit::VSINDEX_SHIFT; } 
};

// -------------------------------------------------------------------------------
//   Struct contained by HalfEdgePMesh to contain the data required to perform the
//      current vsplit operation.  Contained in the pmesh to not require memory
//      allocation at runtime
//
template <class UINT_IDX>
struct SVSplitInfo
{
    bool bIsFaceR;

    UINT_IDX iVertex1;      // current vertex
    //UINT_IDX iVertex2;      // vertex added

    UINT_IDX iWedgeVtfl;	
    UINT_IDX iWedgeVsfl;	

    UINT_IDX iWedgeVtfr;
    UINT_IDX iWedgeVsfr;

    UINT_IDX iWedgeL;      // left vertex for the new triangle
    UINT_IDX iWedgeR;      // right vertex for the new triangle

    UINT16 iMaterialIndexLeft;
    UINT16 iMaterialIndexRight;

    UINT_IDX iFaceL;
    UINT_IDX iFaceR;

    UINT_IDX iFaceLCLW;
    UINT_IDX iFaceLCCW;

    UINT_IDX iFaceRCLW;
    UINT_IDX iFaceRCCW;

    UINT_IDX iEdgeLCLW;
    UINT_IDX iEdgeRCLW;
    UINT_IDX iEdgeLCCW;
    UINT_IDX iEdgeRCCW;

    // a "list" of pointer to the wedges between LCLW and RCCW inclusive
    //   the only vertex indices that need to change in a vsplit
    UINT_IDX **rgpwLCLWtoRCCW;
    UINT_IDX cpwLCLWtoRCCW;

    UINT_IDX cpwLCLWtoRCCWMax;  // maximum number of entries possible in rgpwLCLWtoRCCW
                                        // the greater of: the maximum vlrOffset+1 in the pmesh
                                                // or the maximum valence in the highest level mesh
};

class CPMSharedData
{
public:
    CPMSharedData()
        :m_rgvsVsplits(NULL), m_rgiMaterialNew(NULL), m_cRef(1) {}

    ~CPMSharedData()
    {
        delete []m_rgvsVsplits;
        delete []m_rgiMaterialNew;
    }

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj)
    {
        return E_NOTIMPL;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef( void) 
        
    {
        return InterlockedIncrement(&m_cRef);
    }

    virtual ULONG STDMETHODCALLTYPE Release( void) 
    {
        ULONG cRef;
            
        cRef = InterlockedDecrement(&m_cRef);

        if (cRef == 0)
            delete this;

        return cRef;
    }

    LONG m_cRef;

    CHalfEdgeVSplit *m_rgvsVsplits;        // Array of CHalfEdgeVSplit record, possibly shared
    UINT16 *m_rgiMaterialNew;              // array of attribute bundles for mispredicts
};


template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
class GXHalfEdgePMesh : public ID3DXPMesh
{
    friend class GXSimplifyMesh<tp32BitIndex>;
    friend class GXSimplifyMesh<tp16BitIndex>;
    friend class GXHalfEdgePMesh<tp32BitIndex>;
    friend class GXHalfEdgePMesh<tp16BitIndex>;
public:
    GXHalfEdgePMesh( LPDIRECT3DDEVICE8 pD3D, DWORD dwFVF, DWORD dwOptions );
    ~GXHalfEdgePMesh();
    
    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj);
    virtual ULONG STDMETHODCALLTYPE AddRef( void) 
        
    {
        m_cRef += 1;
        return m_cRef;
    }

    virtual ULONG STDMETHODCALLTYPE Release( void) 
    {
        ULONG cRef = m_cRef;
        m_cRef -= 1;

        if (cRef == 1)
            delete this;

        return cRef-1;
    }

    // ID3DXBaseMesh
    // ID3DXBaseMesh
    STDMETHOD(DrawSubset)(THIS_ DWORD attribId)
                    { return m_tmTriMesh.DrawSubset(attribId); }
    STDMETHOD_(DWORD, GetNumFaces)(THIS)
                    { return m_cCurrentFaces; }
    STDMETHOD_(DWORD, GetNumVertices)(THIS)
                    { return m_cCurrentWedges; }
    STDMETHOD_(DWORD, GetFVF)(THIS)
                    { return m_tmTriMesh.GetFVF(); }
    STDMETHOD(GetDeclaration)(THIS_ DWORD Declaration[MAX_FVF_DECL_SIZE])
                    { return m_tmTriMesh.GetDeclaration(Declaration); }
    STDMETHOD_(DWORD, GetOptions)(THIS)
                    { return m_tmTriMesh.GetOptions(); }
    STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE8* ppDevice)
                    { return m_tmTriMesh.GetDevice(ppDevice); }

    STDMETHOD(CloneMeshFVF)(THIS_ DWORD options, 
                DWORD fvf, LPDIRECT3DDEVICE8 pD3DDevice, LPD3DXMESH* ppCloneMesh);
    STDMETHOD(CloneMesh)(THIS_ DWORD options, 
                CONST DWORD *pDeclaration, LPDIRECT3DDEVICE8 pD3DDevice, LPD3DXMESH* ppCloneMesh);
    STDMETHOD(GetVertexBuffer)(THIS_ LPDIRECT3DVERTEXBUFFER8* ppVB)
                    { return m_tmTriMesh.GetVertexBuffer(ppVB); }
    STDMETHOD(GetIndexBuffer)(THIS_ LPDIRECT3DINDEXBUFFER8* ppIB)
                    { return m_tmTriMesh.GetIndexBuffer(ppIB); }
	STDMETHOD(LockVertexBuffer)(THIS_ DWORD flags, BYTE** ppData)
                    { return m_tmTriMesh.LockVertexBuffer(flags, ppData); }
	STDMETHOD(UnlockVertexBuffer)(THIS)
                    { return m_tmTriMesh.UnlockVertexBuffer(); }
	STDMETHOD(LockIndexBuffer)(THIS_ DWORD flags, BYTE** ppData)
    {
        if (flags & D3DLOCK_READONLY)
            return m_tmTriMesh.LockIndexBuffer(flags, ppData);
        DPF(0, "The index buffer of a ID3DXPMesh object cannot be modified. "
               "Use D3DLOCK_READONLY when locking the index buffer");
        return D3DXERR_CANNOTMODIFYINDEXBUFFER;
    }
	STDMETHOD(UnlockIndexBuffer)(THIS)
                    { return m_tmTriMesh.UnlockIndexBuffer(); }
    STDMETHOD(GetAttributeTable)(
                THIS_ D3DXATTRIBUTERANGE *attribEntry, DWORD* attribTabSize)
                    { return m_tmTriMesh.GetAttributeTable(attribEntry, attribTabSize); }

    //ID3DXPMesh
    STDMETHOD(ClonePMeshFVF)(THIS_ DWORD options, 
                DWORD fvf, LPDIRECT3DDEVICE8 pD3D, LPD3DXPMESH* ppCloneMesh);
    STDMETHOD(ClonePMesh)(THIS_ DWORD options, 
                CONST DWORD *pDeclaration, LPDIRECT3DDEVICE8 pD3D, LPD3DXPMESH* ppCloneMesh);
    STDMETHOD(SetNumFaces)(THIS_ DWORD faces);
    STDMETHOD(SetNumVertices)(THIS_ DWORD vertices);
    STDMETHOD_(DWORD, GetMaxFaces)(THIS);
    STDMETHOD_(DWORD, GetMinFaces)(THIS);
    STDMETHOD_(DWORD, GetMaxVertices)(THIS);
    STDMETHOD_(DWORD, GetMinVertices)(THIS);
    STDMETHOD(Save)(THIS_ IStream *pstream, D3DXMATERIAL *rgMaterials, DWORD cMaterials);

    STDMETHOD(Optimize)(THIS_ DWORD Flags, DWORD* pAdjacencyOut, 
                     DWORD* pFaceRemap, LPD3DXBUFFER *ppVertexRemap,  
                     LPD3DXMESH* ppOptMesh);
    STDMETHOD(GetAdjacency)(THIS_ DWORD* pAdjacency);

    HRESULT LoadFromData(LPD3DXMESH ptmMesh, DWORD *rgdwAdjacency, PBYTE pbPMData, DWORD cbPMData);
private:
    STDMETHOD(CloneMeshInternal)(THIS_ DWORD options, 
                DWORD fvf, LPDIRECT3DDEVICE8 pD3DDevice, 
                BOOL bIgnoreVertices, DWORD dwFlags, DWORD* rgdwAdjacencyOut, 
                DWORD* faceRemap, LPD3DXBUFFER *ppbufVertexRemap,  LPD3DXMESH* ppCloneMesh);

    DWORD m_cRef; // Reference count

    GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED> m_tmTriMesh;  // mesh data
    CHalfEdgeVSplit *m_rgvsVsplits;        // Array of CHalfEdgeVSplit record, possibly shared
    DWORD m_cvsVsplits;                         // number of elements in the vsplit array
    UINT16 *m_rgiMaterialNew;              // array of attribute bundles for mispredicts
    DWORD m_cMaterialNewCur;               //  current position in the mispredict array
    DWORD m_cMaterialNewMax;               //  maximum size of the mispredict array
    CPMSharedData *m_pPMSharedData; //  actual owner of memory for vsplits and other data
    
    D3DXATTRIBUTERANGE *m_rgaeAttributeTableFull; // the attribute LOD if at full LOD
    
    // Mesh Values
    DWORD m_cBaseVertices;       // Number of vertices in the base mesh.
    DWORD m_cMaxVertices;        // Total number of vertices when at maximum level of detail	
    DWORD m_iCurPos;             // Current position in the Vsplit array
    DWORD m_cBaseFaces;			 // Number of faces in the base mesh
    DWORD m_cMaxFaces;	         // Total number of faces when at maximum level of detail
    DWORD m_cCurrentFaces;       // current number of faces in the mesh

    DWORD m_cMaxWedges;	         // Total number of wedges when at maximum level of detail
    DWORD m_cBaseWedges;		 // Number of wedges in the base mesh
    DWORD m_cCurrentWedges;      // current number of wedges in the mesh
    
    SVSplitInfo<UINT_IDX> m_vsi;
            
    // Private methods
    
    STDMETHODIMP CreateEmptyMesh();

    // Applying and undoing vsplits
    // these two call the common or uncommon equivalents based on vspl.IsCommon()
    void ApplyVSplit(CHalfEdgeVSplit &vsNext);
    void UndoVSplit(CHalfEdgeVSplit &vsPrev);

    // helper functions
    void GatherVSplitInfo(CHalfEdgeVSplit &vsNext);
    void GatherECollapseInfo(CHalfEdgeVSplit &vsLast);

    // Debug methods
#if DBG
    bool BValid();
    bool BFragmentedWedge(UINT_IDX iInitialFace, UINT_IDX wWedge, UINT &cWedges, UINT &cWedgesMax , UINT_IDX *&rgiWedgesSeen);
    bool BFragmentedWedges();

    BYTE *m_rgbVertexSeen;
#endif
	
	inline UINT_IDX GetNextWedge(UINT iae) 
		{ 
        m_cCurrentWedges += 1;
        // return the offset of the next wedge in the given material
		return (UINT_IDX)(m_tmTriMesh.m_rgaeAttributeTable[iae].VertexStart + m_tmTriMesh.m_rgaeAttributeTable[iae].VertexCount++);
		}
};

// currently defined in pmesh.h, but if we remove the old pmesh stuff it will
//   need to be reenabled here
#if 0
static const int x_rgmod3[6]={0,1,2,0,1,2};
inline int MOD3(int j)
{
    GXASSERT(j>=0 && j<6);
    return x_rgmod3[j];
}

// Pointer checking
#if DBG
inline BOOL DWORD_PTR_OK(DWORD *p) {return !IsBadWritePtr(p, sizeof(DWORD));}
#else
inline BOOL DWORD_PTR_OK(DWORD *p) {return (p? TRUE:FALSE);}
#endif
#endif

#include "pmeshhe.inl"

#pragma warning(default:4242)

#endif