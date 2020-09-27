#ifndef _STATEBEAST_H_
#define _STATEBEAST_H_

//#define D3D_OVERLOADS
//#include "TestFrameBase.h"
//#include "CD3DTest.h"
//#include "CCommandLine.h"
//#include "CShapes.h"
//#include "cd3d.h"


//USETESTFRAME


#define NTEXTURES 10
#define NLIGHTS   4

#define SAFEDELETE(p) if(p){delete p; p=NULL;}
#define D3DSBT_CUSTOM 4

struct FLAGS
{
	DWORD dwFlags;
	TCHAR tcsDesc[1024];
};

struct VIEWPORT
{
	DWORD dwWidth, dwHeight;
	DWORD dwX, dwY;
	float dwMinZ, dwMaxZ;
};

struct BLOCKINFO
{
	int   nVersion;
    DWORD dwSrcHandle;
    DWORD dwRefHandle;
	DWORD dwCreateType;
	TCHAR tcsDesc[MAX_PATH];
};

typedef struct BEAST  _BEAST;
typedef struct OBJECT _OBJECT;

class CStateBeast : public TESTFRAME(CD3DTest)
{
public:

    VIEWPORT  *m_pViewports;
    int           m_nViewports;

	CShapes		  m_FrontPlane;
    CShapes		  m_FrontPlane2;
    CShapes		  m_BackPlane;
//    CMaterial    *m_pFrontMat, *m_pFrontMat2;
    D3DMATERIAL8  m_FrontMat, m_FrontMat2;
//    CMaterial    *m_pBackMat;
    D3DMATERIAL8  m_BackMat;


	// State blocks data
    BLOCKINFO	 *m_pBlocks;
    int          m_nCurrentBlock;
	int			 m_nTotalBlocks;
	int			 m_nStates;
	int			 m_nTotalDX7;
	int			 m_nTotalDX8;
    BLOCKINFO    m_OriginalBlock;
    
    // App vars
	bool			m_bCmp;					// Compare against SetRenderState
    bool            m_bSWEmulation;         // Allow SW emulation for test
    DWORD 			m_dwPosLightsSupported; // Positions lights supported?

    // Rendering objects and vars
    D3DVECTOR   *m_pvLoc;                   // Camera location
    D3DVECTOR   *m_pvUp;                    // VUP
    D3DMATRIX   *m_pVMatrix;
    D3DMATRIX   *m_pProjMatrix;
//    CLight      *m_pLight[NLIGHTS];
    D3DLIGHT8   m_Light[NLIGHTS];
    CTexture8   *m_pTextures[NTEXTURES];
	int			m_nLightOff;
	int			m_nUnInitLights;

	// DX8 components
	DWORD			m_dwFVF;
	DWORD		 	m_dwVSShader, m_dwVSShader2; 
	DWORD			m_dwPSShader;
    CVertexBuffer8 *m_pSrcVB, *m_pRefVB;
	CIndexBuffer8  *m_pSrcIB, *m_pRefIB;


public: // class functions
	CStateBeast();
	~CStateBeast();

    
	// Framework overloads
	bool SetDefaultMatrices(void);
	bool SetDefaultMaterials(void);
	bool SetDefaultLights(void);
	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
	bool ProcessFrame(void);
    bool ExecuteTest(UINT);
	bool TestTerminate(void);
    void CommandLineHelp(void);
    bool PreDeviceSetup(bool fReference);
//	bool Initialize(void);

    // Statebeast code
	void ClearState();
    void ClearState8();
    void PostApply(UINT nBlock);
    void PostApply8(UINT nBlock);
	bool SetState(int nIndex, TCHAR *szName, int *nVersion, DWORD dwFlags=CD3D_BOTH);
    void CreateStateBlocks(); 
    void ReleaseAll();
    bool SetViewport(VIEWPORT*, DWORD dwFlags);
    bool SetClipPlane(DWORD, D3DVALUE*, DWORD dwFlags);

	// DX8 and above
	bool Init8();
	bool SetupBuffers(CShapes *pShapes, CVertexBuffer8 **ppVB, CIndexBuffer8 **ppIB, bool bReference);
    bool SetState8(int nIndex, TCHAR *szName, int *nVersion, DWORD dwFlags=CD3D_BOTH);
    void SetDefaultShader();
};

#endif
