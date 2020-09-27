#ifndef __Ball_h__
#define __Ball_h__

//USETESTFRAME

struct IBVERTEX
{
    float   x, y, z;
    float   nx, ny, nz;
    float   tu, tv;
};

const DWORD FVF_IBVERTEX = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);

const DWORD SHAPES_NUMBER = 8;

typedef struct _VBDESC {
    DWORD       m_dwFVF;
    DWORD       m_dwVertexCount;
    DWORD       m_dwSize;
    DWORD       m_dwUsage;
    bool        m_fReference;
} VBDESC, *PVBDESC;

typedef struct _IBDESC {
    bool        m_fReference;
    D3DFORMAT   m_d3dfFormat;
    DWORD       m_dwIndexCount;
    DWORD       m_dwSize;
    DWORD       m_dwUsage;
} IBDESC, *PIBDESC;

class IBuffer : public CD3DTest
{
public:
    IBuffer();
    ~IBuffer();

	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
    virtual bool TestTerminate(void);

	virtual bool SetDefaultMatrices(void);
	virtual bool SetDefaultMaterials(void);
	virtual bool SetDefaultLightStates(void);
	virtual bool SetDefaultRenderStates(void);
	virtual bool SetDefaultLights(void);

private:
//    CMaterial       *m_pMaterial;
//    CLight          *m_pLight;
    CShapes         *m_pShapes[SHAPES_NUMBER];
    CVertexBuffer8  *m_pSrcVertexBuffer;
    CVertexBuffer8  *m_pRefVertexBuffer;
    CIndexBuffer8   *m_pSrcIndexBuffer;
    CIndexBuffer8   *m_pRefIndexBuffer;

    VBDESC          m_vbdesc;
    IBDESC          m_ibdesc;

    // Current settings
    DWORD           m_dwCurrentTestNumber;
    DWORD           m_dwCurrentShape;
    D3DPOOL         m_eIndexPool;
    D3DPOOL         m_eVertexPool;
    bool            m_bRecreateBuffers;
    bool            m_bFillSumBuffer;
    
    // UsageScenarios:
    //  0 - Recreate 16 bit buffer
    //  1 - Rewrite 16 bit buffer
    //  2 - Recreate 32 bit buffers
    //  3 - Rewrite 32 bit buffer
    //  4 - Single 32 bit buffer
    DWORD           m_dwUsageScenario;

    // Parameters
    DWORD           m_dwBufferVertices;
    DWORD           m_dwBufferIndices;
    DWORD           m_dwMinIndex;
    DWORD           m_dwRenderVertices;
    DWORD           m_dwStartIndex;
    DWORD           m_dwRenderIndices;
    D3DFORMAT       m_eIndexFormat;

    // Summary 
    DWORD           m_dwMaxVertices;
    DWORD           m_dwSumVertices;
    DWORD           m_dwMaxIndices;
    DWORD           m_dwSumIndices;

    DWORD           ResolveTestNumber(void);
    bool            CreateBuffers(void);
    bool            FillBuffers(void);
};

#endif
