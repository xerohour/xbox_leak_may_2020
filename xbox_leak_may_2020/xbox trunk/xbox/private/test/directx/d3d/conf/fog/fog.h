/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Fog.h
 *
 ***************************************************************************/

#ifndef __FOG_H__
#define __FOG_H__

//USETESTFRAME

// Defines (Fog type)
#define VERTEX			1
#define TABLE_LINEAR	2
#define TABLE_EXP		3
#define TABLE_EXP2		4
#define W_LINEAR		5
#define W_EXP			6
#define W_EXP2			7

// Defines (Number of tests)
#define VERTEX_TESTS		12
#define TABLE_LINEAR_TESTS	22
#define TABLE_EXP_TESTS		11
#define TABLE_EXP2_TESTS	11
#define W_LINEAR_TESTS		40
#define W_EXP_TESTS			11
#define W_EXP2_TESTS		11

#define TEXTURE_VERTEX_TESTS    4   
#define TEXTURE_TABLE_TESTS     3
#define TEXTURE_W_TESTS         3

// Base Class definitions
class CFogTest: public TESTFRAME(CD3DTest)
{
    // Data
	public:
    D3DTLVERTEX     VertexList[24];
    DWORD           dwVertexCount;
	DWORD			dwFog;
	D3DCOLOR		FogColor;
	CTexture8 *		pTexture;
//	CImageData *	pImage;
	char			msgString[80];

	D3DCOLOR		Stripes[8];

    bool            m_bTextures;
    BOOL            m_bExit;

	public:
	CFogTest();
	~CFogTest();

	// Framework functions
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT uTestNum);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void VertexTests(int nTest);
    void TextureVertexTests(int nTest);
	void TableTests(void);
	void WTests(void);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

// Vertex Class definitions
class CVertexTest: public CFogTest
{
	public:
	CVertexTest();
	~CVertexTest();

	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

// TableLinear Class definitions
class CTableLinearTest: public CFogTest
{
	public:
	CTableLinearTest();
	~CTableLinearTest();

	bool SetDefaultRenderStates(void);
	bool SetDefaultMatrices(void);
	UINT TestInitialize(void);
};

// TableExp Class definitions
class CTableExpTest: public CFogTest
{
	public:
	CTableExpTest();
	~CTableExpTest();

	bool SetDefaultRenderStates(void);
	bool SetDefaultMatrices(void);
	UINT TestInitialize(void);
};

// TableExp2 Class definitions
class CTableExp2Test: public CFogTest
{
	public:
	CTableExp2Test();
	~CTableExp2Test();

	bool SetDefaultRenderStates(void);
	bool SetDefaultMatrices(void);
	UINT TestInitialize(void);
};

// WLinear Class definitions
class CWLinearTest: public CFogTest
{
	public:
	CWLinearTest();
	~CWLinearTest();

	bool SetDefaultRenderStates(void);
	bool SetDefaultMatrices(void);
	UINT TestInitialize(void);
};

// WExp Class definitions
class CWExpTest: public CFogTest
{
	public:
	CWExpTest();
	~CWExpTest();

	bool SetDefaultRenderStates(void);
	bool SetDefaultMatrices(void);
	UINT TestInitialize(void);
};

// WExp2 Class definitions
class CWExp2Test: public CFogTest
{
	public:
	CWExp2Test();
	~CWExp2Test();

	bool SetDefaultRenderStates(void);
	bool SetDefaultMatrices(void);
	UINT TestInitialize(void);
};

// Texture Vertex Class definitions
class CTextureVertexTest: public CFogTest
{
	public:
	CTextureVertexTest();
	~CTextureVertexTest();

	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

// Texture TableLinear Class definitions
class CTextureTableLinearTest: public CFogTest
{
	public:
	CTextureTableLinearTest();
	~CTextureTableLinearTest();

	bool SetDefaultRenderStates(void);
	bool SetDefaultMatrices(void);
	UINT TestInitialize(void);
};

// Texture TableExp Class definitions
class CTextureTableExpTest: public CFogTest
{
	public:
	CTextureTableExpTest();
	~CTextureTableExpTest();

	bool SetDefaultRenderStates(void);
	bool SetDefaultMatrices(void);
	UINT TestInitialize(void);
};

// Texture TableExp2 Class definitions
class CTextureTableExp2Test: public CFogTest
{
	public:
	CTextureTableExp2Test();
	~CTextureTableExp2Test();

	bool SetDefaultRenderStates(void);
	bool SetDefaultMatrices(void);
	UINT TestInitialize(void);
};

// Texture WLinear Class definitions
class CTextureWLinearTest: public CFogTest
{
	public:
	CTextureWLinearTest();
	~CTextureWLinearTest();

	bool SetDefaultRenderStates(void);
	bool SetDefaultMatrices(void);
	UINT TestInitialize(void);
};

// Texture WExp Class definitions
class CTextureWExpTest: public CFogTest
{
	public:
	CTextureWExpTest();
	~CTextureWExpTest();

	bool SetDefaultRenderStates(void);
	bool SetDefaultMatrices(void);
	UINT TestInitialize(void);
};

// Texture WExp2 Class definitions
class CTextureWExp2Test: public CFogTest
{
	public:
	CTextureWExp2Test();
	~CTextureWExp2Test();

	bool SetDefaultRenderStates(void);
	bool SetDefaultMatrices(void);
	UINT TestInitialize(void);
};

#endif
