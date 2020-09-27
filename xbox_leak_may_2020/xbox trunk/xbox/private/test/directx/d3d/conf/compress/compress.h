/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	compress.h
 *
 ***************************************************************************/

#ifndef __COMPRESS_H__
#define __COMPRESS_H__

//USETESTFRAME

// Defines
#define MAX_TESTS		18

// Class definition
class CCompressTest: public CD3DTest
{
    // Data
	public:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
	CTexture8 *		pTexture;
	CTexture8 *		pDXTnTexture;
	CTexture8 *		pSysDXTnTexture;
//	CImageData *	pImage;
//	CImageData *	pAlphaImage;
//	CImageData *	pTinyImage;
	DWORD			dwFourCC;
    D3DFORMAT       fmtCompressed;
	DWORD			dwPerspectiveCap;
	DWORD			dwSrcAlphaCap;
	DWORD			dwOneCap;
	DWORD			dwInvSrcAlphaCap;
	char			msgString[80];
	char			szStatus[80];

	D3DCOLOR		Stripes[8];
	D3DCOLOR		AlphaStripes[8];
	D3DCOLOR		White[4];

    BOOL            m_bExit;

	public:
	CCompressTest();
	~CCompressTest();

	// Framework functions
	virtual bool SetDefaultRenderStates(void);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
	virtual bool TestTerminate(void);

	// Helper functions
	bool CheckDXTnFormat(D3DFORMAT fmtDXTn);
	void DrawBasicGrid(int nTest, int nTexFormats);
	void DrawBlendingGrid(int nAlpha);
    bool DXTnLoad8(void);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

// DXT1 Class definitions
class CDXT1Test: public CCompressTest
{
	public:
	CDXT1Test();
	~CDXT1Test();
};

// DXT2 Class definitions
class CDXT2Test: public CCompressTest
{
	public:
	CDXT2Test();
	~CDXT2Test();
};

// DXT3 Class definitions
class CDXT3Test: public CCompressTest
{
	public:
	CDXT3Test();
	~CDXT3Test();
};

// DXT4 Class definitions
class CDXT4Test: public CCompressTest
{
	public:
	CDXT4Test();
	~CDXT4Test();
};

// DXT5 Class definitions
class CDXT5Test: public CCompressTest
{
	public:
	CDXT5Test();
	~CDXT5Test();
};

#endif

