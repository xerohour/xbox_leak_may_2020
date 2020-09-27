//////////////////////////////////////////////////////////////////////
// Cubemap.h: interface for the CCubemap class.
//////////////////////////////////////////////////////////////////////

#ifndef __CUBEMAP_H__
#define __CUBEMAP_H__

//USETESTFRAME

struct TVertex {									// Cubemap vertex with 3D texture coords
	D3DVECTOR v;
	D3DVECTOR n;
	D3DVECTOR t;
};

struct TGVertex {									// Cubemap vertex without 3D texture coords
	D3DVECTOR v;
	D3DVECTOR n;
};

class CCubemap: public TESTFRAME(CD3DTest)
{
protected:
	UINT m_uTotalTests;
//	CMaterial *m_paMaterial;
	CCubeTexture8 *m_paTexture;
	CCubeTexture8 *m_paSysTexture;

	float m_flZDepthMin,							// Minimum Z depth for all tests
		m_flZDepthMax;								// Maximum Z depth for Mipmap test
	int m_dTextureWidthMax,							// Maximum texture width (top mipmap level)
		m_dTextureHeightMax;						// Maximum texture height (top mipmap level)

	TVertex *m_paVertex;							// Sphere vertices with texture vectors
	TGVertex *m_paTGVertex;							// Sphere vertices without texture vectors
	int m_cVertexMax;
    bool bTexGen;
    bool bVideo;
    bool bD3DManage;
    bool bManage;
    bool bLock;
    bool bBlt;
    bool bUpdate;
    bool bCopy;

    int nCubeFormat;

    UINT            m_uCommonCubeFormats;
    D3DFORMAT       m_fmtCommonCube[17];

    BOOL            m_bExit;

public:
	CCubemap();
	~CCubemap();

	virtual bool SetDefaultMatrices(void);
	virtual bool SetDefaultMaterials(void);
	virtual bool SetDefaultRenderStates(void);
	virtual UINT TestInitialize(void);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
	virtual bool TestTerminate(void);

	bool LoadSphere();
    bool BltCubicTexture(void);
    bool LoadCubicTexture8(void);
    bool CheckMipCubeCap8(void);
    bool CheckCubeFilterCaps8(DWORD, DWORD, DWORD);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

class CYawRotation: public CCubemap
// Turns 360 degrees around Y axis
{
protected:
	float m_flDegInc;								// Degree increase per test
//	CImageData *pImage[6];

	DWORD dwGradient[6][4];

public:
	CYawRotation();

//    virtual bool AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize);
	virtual void CommandLineHelp(void);
	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);
};

class CPitchRotation: public CCubemap
// Turns 360 degrees around X axis
{
protected:
	float m_flDegInc;								// Degree increase per test
//	CImageData *pImage[6];

	DWORD dwGradient[6][4];

public:
	CPitchRotation();

//    virtual bool AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize);
	virtual void CommandLineHelp(void);
	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);
};

class CMipmaps: public CCubemap
// Turns 360 degrees around X axis for each mipmap level
{
protected:
	float m_flDegInc;								// Degree increase per test
	int m_cLevelMax,								// Max levels to test
		m_cTestMax;									// Total tests
//	CImageData *pImage[5];

	D3DCOLOR		RedStripes[7];
	D3DCOLOR		GreenStripes[7];
	D3DCOLOR		BlueStripes[7];
	D3DCOLOR		YellowStripes[7];
	D3DCOLOR		MagentaStripes[7];

public:
	CMipmaps();

//    virtual bool AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize);
	virtual void CommandLineHelp(void);
	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);
};

class CTexGen: public CCubemap
// Texture generation cases
{
protected:
	float m_flDegInc;								// Degree increase per test
//	CImageData *pImage[6];

	DWORD dwGradient[6][4];

public:
	CTexGen();

	virtual void CommandLineHelp(void);
	virtual bool SetDefaultMatrices(void);
	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);
};

class CCubeFilter: public CCubemap
// Cubic Filtering cases
{
protected:
	float m_flDegInc;			// Degree increase per test
//    CImageData *pMinImage;
//	CImageData *pMagImage;
//	CImageData *pMipImage[3];
	char szStr[100];

	D3DCOLOR		RedStripes[8];
	D3DCOLOR		GreenStripes[8];
	D3DCOLOR		BlueStripes[8];
	D3DCOLOR		YellowStripes[8];
	D3DCOLOR		MagentaStripes[8];

public:
	CCubeFilter();

	virtual void CommandLineHelp(void);
	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);

    bool SetupFilterMode(int);
};

class CCubeBlend: public CCubemap
// Cubic Blending cases
{
protected:
//	CImageData *pImage;
//	CImageData *pAlphaImage;
	char szStr[100];

	D3DCOLOR		Stripes[8];
	D3DCOLOR		AlphaStripes[8];

public:
	CCubeBlend();

	virtual bool SetDefaultRenderStates(void);
	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT);
	virtual bool TestTerminate(void);

    bool ApplyMaterial(int, bool);
    bool SetupBlendMode(int);
};

#endif