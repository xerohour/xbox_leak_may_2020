//////////////////////////////////////////////////////////////////////
// Copyright (C) Microsoft Corporation, 2000.
//
// clear_test.h
//
// CClearTest class - Tests many possible IDirect3DDeviceX::Clear() parameter combinations
//
// History: 8/21/2000 Bobby Paulose     - Created
//
//////////////////////////////////////////////////////////////////////


#ifndef __CLEAR_TEST_H__
#define __CLEAR_TEST_H__


//USETESTFRAME

//BEGINTESTFRAME

// Get access to the framework
//extern CD3DWindowFramework   *g_pD3DFramework;

//ENDTESTFRAME // end TESTFRAME



#define CLEAR_COLOR_FULL	D3DRGBA(1.0, 0.0, 0.0, 1.0) //red 
#define CLEAR_COLOR_FULL_2	D3DRGBA(0.0, 0.0, 1.0, 1.0) //blue 
#define CLEAR_COLOR_FULL_ALPHA0	D3DRGBA(1.0, 0.0, 0.0, 0.0) 
#define CLEAR_COLOR_FULL_ALPHA1	D3DRGBA(1.0, 0.0, 0.0, 1.0) // Alpha 1.0 is ok for even formats with A just 1 bit.

#define CLEAR_COLOR_0	D3DRGBA(0.0, 0.0, 0.0, 1.0) 

#define CLEAR_COLOR__RECTS	D3DRGBA(0.0, 0.0, 1.0, 0.0) //blue

#define RENDERING_COLOR		D3DRGBA(0.0, 1.0, 0.0, 1.0) // Green used to render triangles.



#define LOWER_Z 0.49f
#define HIGHER_Z 0.51f
#define STENCIL_0 0
#define STENCIL_1 1



#ifndef UNDER_XBOX
#define TEST_CASES 14
#else
#define TEST_CASES 10
#endif
#define NUM_VERTICES 6
#define IMGHEIGHT	256
#define IMGWIDTH	256
#define ARGB_MAKE(a, r, g, b) RGBA_MAKE(r, g, b, a) 


// Class definition
class CClearTest: public CD3DTest
{
    // Private Data

    CTexture8 *  m_pTextureRT;
    CSurface8 * m_pOffSrcSurf, * m_pOffRefSurf;
    CSurface8 * m_pSrcZ, * m_pRefZ;
    
    CSurface8 * m_pOriginalSrcRT, * m_pOriginalRefRT; 
    CSurface8 * m_pOriginalSrcZ, * m_pOriginalRefZ; 

    int m_nOriginalZFormat, m_nTexFormat, m_nZFormat, m_nTestCase;

	bool m_bAlphaPresent, m_bStencilPresent;

	static const DWORD VertexFVF;

	typedef struct { float x,y,z,rhw; D3DCOLOR d; } Vertex;

	static const DWORD TexVertexFVF;

	typedef struct { float x,y,z,rhw; D3DCOLOR d; float u, v;} TexVertex;

	static const DWORD dwVertexCount;

	static const Vertex VertexList[NUM_VERTICES];

	static const TexVertex Grid[NUM_VERTICES];
    TexVertex VPGrid[NUM_VERTICES];

	BOOL boolResults[TEST_CASES];

	protected:
    bool m_bOffscreen;

    BOOL            m_bExit;

    // Public fns
	public:
	CClearTest();
	~CClearTest();

	// Framework functions
//	virtual bool AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual void DisplaySetup(void);
	virtual bool ProcessFrame();
	virtual bool TestTerminate(void);


	//helper fns
	bool CreateOffScrRendTrgts(int nTexFormat);
	bool CreateZStencil(int nZFormat);
	bool CopyToTexture(void);
	void DescribeFormat(char * pBuf, int nTexFormat, int nZFormat);
	void DescribeTestcase(char * pBuf, int nTestcase);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};




// Clear Texture as render target.
class CTextureClearTest: public CClearTest
{
	public:
	CTextureClearTest();
	~CTextureClearTest();
};


// Clear Offscreen as render target.
class COffscreenClearTest: public CClearTest
{
	public:
	COffscreenClearTest();
	~COffscreenClearTest();
};



#endif  //__CLEAR_TEST_H__
