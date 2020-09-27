#ifndef __VBBatch_h__
#define __VBBatch_h__

//USETESTFRAME

#define DPVB_API  1
#define DIPVB_API 2

class VBBatch : public CD3DTest
{
public:
    VBBatch();
    ~VBBatch();

	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
    virtual bool ClearFrame(void);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
    virtual bool TestTerminate(void);

	virtual bool SetDefaultMatrices(void);
	virtual bool SetDefaultMaterials(void);
	virtual bool SetDefaultLightStates(void);
	virtual bool SetDefaultRenderStates(void);
	virtual bool SetDefaultLights(void);

private:
//	CMaterial*		m_pMaterial;
//	CLight*			m_pLight;
	CTexture8*		m_pTextureArray[8];
    CVertexBuffer8* m_pSrcVertexBuffer;
//    CVertexBuffer*  m_pDstVertexBuffer;
//    CVertexBuffer*  m_pRefVertexBuffer;
    CShapes*        m_pShape;
    WORD*           m_pRemappedIndices;

    // Hardware parameters
    WORD            m_wMaxSimultaneousTextures;
    WORD            m_wMaxTextureBlendStages;

    // Test control parameters
    DWORD           m_dwCurrentTestNumber;
    DWORD           m_dwBatchSize;
    bool            m_bForceSystemMemory;
    DWORD           m_dwFVF;
    DWORD           m_dwAPI;
    bool            m_bLighting;
    bool            m_bSpecular;
    bool            m_bSrcOptimize;
    bool            m_bProcessVertices;
    bool            m_bDstForceSystemMemory;
    DWORD           m_dwDstFVF;
    DWORD           m_dwTotalTests;

    // Current vertex buffer parameters. 
    // Used to prevent VB creation on each test
    bool            m_bSrcVBForceSystemMemory;
    DWORD           m_dwSrcVBFVF;
    DWORD           m_dwSrcVBAPI;
    bool            m_bSrcVBOptimize;

    bool            m_bDstVBForceSystemMemory;
    DWORD           m_dwDstVBFVF;
    DWORD           m_dwDstVBAPI;
    bool            m_bDstVBOptimize;

    // Other internal parameters
    DWORD           m_dwVertices;
    DWORD           m_dwIndices;

    bool            ResolveTestNumber(void);
    bool            SetProperties(void);
    bool            CreateVertexBuffers(void);
    bool            FillVertexBuffer(CVertexBuffer8* pVB, DWORD dwSize, DWORD dwFVF, DWORD dwAPI);
    void            CopyVertex(void **buf, DWORD dwFVF, WORD n);
};

#endif // __LineTex_h__