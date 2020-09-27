/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Blend.h
 *
 ***************************************************************************/

#ifndef __BLEND_H__
#define __BLEND_H__

//USETESTFRAME

// Defines
#define MAX_TESTS		20

// Class definition
class CBlendTest: public CD3DTest
{
    // Data
	public:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
	CTexture8*		pTexture;
//	CImageData *	pImage;
//	CImageData *	pAlphaImage;
	DWORD			dwBlendCap;
	DWORD			dwBlendState;
	DWORD			dwSrcAlphaCap;
	DWORD			dwInvSrcAlphaCap;
	DWORD			dwAlphaGouraudCap;
    DWORD           dwTextureOpCap;
    DWORD           dwTextureOpCap2;
	LPCSTR			szTextureOp;
	LPCSTR			szTextureOp2;
    char			msgString[80];
	char			szStatus[80];

	D3DCOLOR		Stripes[8];
	D3DCOLOR		AlphaStripes[8];

    BOOL            m_bExit;

	public:
	CBlendTest();
	~CBlendTest();

	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
	virtual bool TestTerminate(void);

	// Helper functions
	virtual void DrawBasicGrid(int nTest, int nTexFormats);
	virtual void DrawBlendingGrid(int nAlpha);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

// Default Class definitions
class CDefaultTest: public CBlendTest
{
	public:
	CDefaultTest();
	~CDefaultTest();

	bool SetDefaultRenderStates(void);
};

// Decal Class definitions
class CDecalTest: public CBlendTest
{
	public:
	CDecalTest();
	~CDecalTest();

	bool SetDefaultRenderStates(void);
};

// Modulate Class definitions
class CModulateTest: public CBlendTest
{
	public:
	CModulateTest();
	~CModulateTest();

	bool SetDefaultRenderStates(void);
};

// Decal Alpha Class definitions
class CDecalAlphaTest: public CBlendTest
{
	public:
	CDecalAlphaTest();
	~CDecalAlphaTest();

	bool SetDefaultRenderStates(void);
};

// Modulate Alpha Class definitions
class CModulateAlphaTest: public CBlendTest
{
	public:
	CModulateAlphaTest();
	~CModulateAlphaTest();

	bool SetDefaultRenderStates(void);
};

// Copy Class definitions
class CCopyTest: public CBlendTest
{
	public:
	CCopyTest();
	~CCopyTest();

	bool SetDefaultRenderStates(void);
};

// Add Class definitions
class CAddTest: public CBlendTest
{
	public:
	CAddTest();
	~CAddTest();

	bool SetDefaultRenderStates(void);
	void DrawBasicGrid(int nTest, int nTexFormats);
	void DrawBlendingGrid(int nAlpha);
};

#endif
