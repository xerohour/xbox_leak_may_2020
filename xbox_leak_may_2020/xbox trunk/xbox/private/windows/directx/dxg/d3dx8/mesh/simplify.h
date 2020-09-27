#ifndef __SIMPLIFY_H__
#define __SIMPLIFY_H__

// Disable warnings of loss of data conversion assignments
#pragma warning(disable:4242)

class CQuadric;
template <class UINT_IDX> class CEdgeInfo;
template <class UINT_IDX> class CSimplificationHeap;
template <class UINT_IDX> struct SEdgeCollapseContext;

template <class UINT_IDX>
struct SFaceEdgeInfo
{
    CEdgeInfo<UINT_IDX> *m_rgpeiEdges[3];
};

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
class GXSimplifyMesh : public ID3DXSPMesh
{
public:
    GXSimplifyMesh( LPDIRECT3DDEVICE8 pD3DDevice, DWORD dwFVF, DWORD dwOptions );
    ~GXSimplifyMesh();
    
    // IUnknown Methods
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);
    STDMETHODIMP QueryInterface(REFIID, LPVOID FAR*);
    
    // ID3DXSPMesh
    STDMETHOD_(DWORD, GetNumFaces)(THIS);
    STDMETHOD_(DWORD, GetNumVertices)(THIS);
    STDMETHOD_(DWORD, GetFVF)(THIS)
            { return m_tmTriMesh.GetFVF();};
    STDMETHOD(GetDeclaration)(THIS_ DWORD Declaration[MAX_FVF_DECL_SIZE])
                    { return m_tmTriMesh.GetDeclaration(Declaration); }
    STDMETHOD_(DWORD, GetOptions)(THIS)
            { return m_tmTriMesh.GetOptions();};
    STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE8* ppDevice)
                    { return m_tmTriMesh.GetDevice(ppDevice); }
    STDMETHOD(CloneMeshFVF)(THIS_ DWORD options, 
                DWORD fvf, LPDIRECT3DDEVICE8 pD3D, 
                DWORD *rgdwAdjacency, DWORD *rgdwVertexRemap, LPD3DXMESH* ppCloneMesh);
    STDMETHOD(CloneMesh)(THIS_ DWORD options, 
                CONST DWORD *pDeclaration, LPDIRECT3DDEVICE8 pD3DDevice, 
                DWORD *rgdwAdjacency, DWORD *rgdwVertexRemap, LPD3DXMESH* ppCloneMesh);
    STDMETHOD(ClonePMeshFVF)(THIS_ DWORD options, 
                DWORD fvf, LPDIRECT3DDEVICE8 pD3D, DWORD *rgdwVertexRemap, LPD3DXPMESH* ppCloneMesh);
    STDMETHOD(ClonePMesh)(THIS_ DWORD options, 
                CONST DWORD *pDeclaration, LPDIRECT3DDEVICE8 pD3D, DWORD *rgdwVertexRemap, LPD3DXPMESH* ppCloneMesh);
    STDMETHOD(ReduceFaces)(THIS_ DWORD faces);
    STDMETHOD(ReduceVertices)(THIS_ DWORD vertices);
    STDMETHOD_(DWORD, GetMaxFaces)(THIS);
    STDMETHOD_(DWORD, GetMaxVertices)(THIS);

    // used by D3DXCreateSPMesh to initialize with a given data source
    HRESULT SetupSimplification(GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED> *ptmSrcMesh, 
                CONST DWORD *rgdwAdjacency,CONST LPD3DXATTRIBUTEWEIGHTS pVertexAttributeWeights, CONST FLOAT *pVertexWeights);
    HRESULT ResetHeap();
private:
    
    // Private data
    DWORD m_cRef; // Reference count

    DXCrackFVF m_cfvf;
    GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED> m_tmTriMesh;
    float m_fMeshRadiusSq;

    SFaceEdgeInfo<UINT_IDX> *m_rgfeiEdges;
    UINT_IDX *m_rgiWedgeList;
    CQuadric *m_rgqVertexQuadrics;
    CQuadric *m_rgqFaceQuadrics;
    D3DXVECTOR3 *m_rgvFaceNormals;
    BYTE *m_rgbVertexSeen;

    UINT m_cLogicalVertices;
    UINT m_cCurrentFaces;
    UINT m_cCurrentWedges;
    UINT m_cMaxLogicalVertices;
    
    UINT m_cvsMax16BitPos;
    UINT m_cvsMaxFaces16BitPos;
    
    UINT m_cMaxFaces;
    UINT m_cMaxVertices;

    CSimplificationHeap<UINT_IDX> *m_pheapCosts;
            
    // vsplit record information
    CHalfEdgeVSplit *m_rgvsSplits;          // array of vsplits, are not true vsplits until combined with info
                                                // in m_rgiFaceIndex and m_rgiEdgeRemoved
    BYTE    *m_rgbRemappedVSplit;   // array of flags indicating whether FaceIndex has been used to remap iFLCLW in the split record
                                                // if this is false, then need to use the remap array from compact to adjust as well
    ULONG    m_cvsSplitsMax;        // total number of vsplit records allocated
    ULONG    m_cvsSplitCurFree;     // current offset of the next vsplit record (starts at end of array and moves to the beginning)

    UINT    *m_rgiFaceIndex;           // index of the face in the mesh when created from base mesh and 
                                            // vsplit records, calculated when an edge collapse occurs
    UINT_IDX *m_rgiEdgeAdjustment;      // the offset of the number of indexes in the triangle caused by 
                                            // being generated from a vsplit record
                                            // used to calculate the offsets in the triangle for other vsplits 
                                            // that reference this triangle

    PBYTE    m_pvHEVertexBuffer;    // array of points to append after the base mesh in a half edge PM to generate the correct vertex buffer
    DWORD    *m_rgiHEVertexBuffer;    // src position of this item in the he vertex buffer
    ULONG    *m_rgcvHEVertexBufferMax;      // maximum offset of the vertices for each material
    ULONG    *m_rgcvHEVertexBufferFree;     // current offset of the next vertex buffer (per material)
                                                        //(starts at end of array and moves to the beginning)

    D3DXATTRIBUTERANGE *m_rgaeAttributeTableOrig;
    D3DXATTRIBUTERANGE *m_rgaeAttributeTableCur;

    UINT16  *m_rgiMaterialNew;  // array of mispredicted faces (used to be stored in the vsplit records)
    ULONG    m_cMaterialNewMax;                // size of mispredicted face array
    ULONG    m_cMaterialNewFree;               // current offset of the next free attribute slot (starts at end of array and moves to the beginning)

    BYTE *m_rgbVertexDeleted;       // is the given vertex deleted from the mesh

    D3DXATTRIBUTEWEIGHTS m_AttributeWeights;        // user input on how to weight the vertex components
    float *m_rgfVertexWeights;                      // user provided per vertex importance
    float *m_rgfTexWeights;                         // per float weights to handle texture coords error metric
    DWORD m_cTexWeights;                            //  number of per float texture coordinate weights
    D3DXCOLOR *m_rgcolorDiffuse;                    // if using diffuse attribute, needs to be in floating point format
    D3DXCOLOR *m_rgcolorSpecular;                   // if using specular attribute, needs to be in floating point format

    // Private methods
    
    HRESULT CreateEmptyMesh();
    HRESULT CreateHeap(PBYTE pvPoints);
    HRESULT GenerateWedgeList();
    HRESULT ComputePointReps();
    HRESULT DefragmentWedges();    
    HRESULT AllocateVSplitRecords();
    HRESULT SetupWeightInfo(CONST LPD3DXATTRIBUTEWEIGHTS pVertexAttributeWeights, CONST FLOAT *rgfVertexWeights, PBYTE pvPoints);
    HRESULT CollapseEdge(CEdgeInfo<UINT_IDX> *peiInfo, PBYTE pvPoints);
    HRESULT GenerateInitialQuadrics(PBYTE pvPoints);
    void CalcEdgeQEMS_Plane(UINT_IDX iFace, UINT_IDX iEdge, PBYTE pvPoints);
    void RecalcEdgeQEMS_Plane(UINT_IDX iFacePresent, UINT_IDX iWedgePresent, PBYTE pvPoints);
    void RecalcEdgeQEMS_GH(UINT_IDX iPoint, UINT_IDX iPoint2, UINT_IDX iFacePresent, UINT_IDX iWedgePresent, D3DXVECTOR3 &vPosNew, PBYTE pvPoints);
    void RecalcEdgeQEMS_MemoryLess(UINT_IDX iPoint1, UINT_IDX iPoint2, UINT_IDX iFacePresent, D3DXVECTOR3 &vPosNew, PBYTE pvPoints);
    void GenerateFaceQEM(UINT_IDX iFace, PBYTE pvPoints, CQuadric &qemFace);
    void GenerateVertexQEM_MemoryLess(UINT_IDX iFace, UINT_IDX iVertex, UINT_IDX iPoint, PBYTE pvPoints, CQuadric &qemTemp);
    bool BGenerateBorderQem(UINT_IDX iFace, UINT_IDX iEdge, UINT_IDX iNeighbor, PBYTE pvPoints, CQuadric &qemBorder);
    double DGenerateBorderError(UINT_IDX iFace, UINT_IDX iEdge, D3DXVECTOR3 *pvPosNew, PBYTE pvPoints);
    HRESULT FixupVSplitRecords();
    HRESULT RemapVSplitRecords(UINT *rgiFaceRemap);
    HRESULT RemapWedgeList(UINT *rgiVertRemap, UINT_IDX cMaxVertices);
    HRESULT RemapHEPointReps(UINT *rgiVertRemap, UINT_IDX cMaxVertices);
    HRESULT RemapInternals(UINT *rgiVertRemap, UINT cVerticesPreRemap, UINT *rgiFaceIndex, UINT cFacesPreRemap);
    float CalculateNormalCost(UINT_IDX iVertex1, UINT_IDX iVertex2, PBYTE pvPoints);
    HRESULT CalculateVSplitRecord(SEdgeCollapseContext<UINT_IDX> &ecc, PBYTE pvPoints);
    HRESULT CalculateVSplitWads(SEdgeCollapseContext<UINT_IDX> &ecc, PBYTE pvPoints);
    HRESULT GatherEdgeCollapseData(SEdgeCollapseContext<UINT_IDX> &ecc, CEdgeInfo<UINT_IDX> *peiInfo, PBYTE pvPoints);
    bool    BValidEdgeCollapse(SEdgeCollapseContext<UINT_IDX> &ecc, PBYTE pvPoints);
    HRESULT PerformEdgeCollapse(SEdgeCollapseContext<UINT_IDX> &ecc, PBYTE pvPoints);
    HRESULT UpdateVSplitAdjustments(SEdgeCollapseContext<UINT_IDX> &ecc);
    HRESULT UpdateErrorMeasurements(SEdgeCollapseContext<UINT_IDX> &ecc, PBYTE pvPoints);

	void RemoveWedge(UINT_IDX wWedge, UINT16 iMaterial);
	void RemoveWedgeElement(UINT_IDX wHead, UINT_IDX wElement, UINT16 iMaterial);

	void AddHEPoint(PBYTE pvPoints, UINT_IDX iWedge, UINT16 iMaterial);

    HRESULT Compact(UINT *rgiVertRemap, UINT cVertexRemap, UINT *rgFaceRemap, UINT cFaceRemap);
    // Debug methods
#if DBG
    bool BValid();
    bool BFragmentedWedges();
    bool BFragmentedWedge(UINT_IDX iFace, UINT_IDX wPointRep, UINT &cWedges, UINT &cWedgesMax , UINT_IDX *&rgiWedgesSeen);
#endif    
};

// UNDONE currently UINT_IDX, should change to be a byte, only 4 possible values 
#define EDGEADJ_UNUSED UNUSED

#include "simplify.inl"

#pragma warning(default:4242)

#endif