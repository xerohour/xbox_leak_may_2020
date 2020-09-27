/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Gradient.h
 *
 ***************************************************************************/

#ifndef __GRADIENT_H__
#define __GRADIENT_H__

//USETESTFRAME

// Defines
#define MAX_TESTS	240

// Base Class definitions
class CGradientTest: public TESTFRAME(CD3DTest)
{
    // Data
	public:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
	CTexture8 *		pTexture;
//	CImageData *	pImage;
	int				nTextureSize;
	int				nMaxScale;
	char 			msgString[80];

	D3DCOLOR	    Gradient[4];

    BOOL            m_bExit;

	public:
	CGradientTest();
	~CGradientTest();

	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void DrawGrid(int nTest);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

// Texture 8x8 Class definitions
class CTexture8x8Test: public CGradientTest
{
	public:
	CTexture8x8Test();
	~CTexture8x8Test();
};

// Texture 16x16 Class definitions
class CTexture16x16Test: public CGradientTest
{
	public:
	CTexture16x16Test();
	~CTexture16x16Test();
};

// Texture 32x32 Class definitions
class CTexture32x32Test: public CGradientTest
{
	public:
	CTexture32x32Test();
	~CTexture32x32Test();
};

// Texture 64x64 Class definitions
class CTexture64x64Test: public CGradientTest
{
	public:
	CTexture64x64Test();
	~CTexture64x64Test();
};

// Texture 128x128 Class definitions
class CTexture128x128Test: public CGradientTest
{
	public:
	CTexture128x128Test();
	~CTexture128x128Test();
};

// Texture 256x256 Class definitions
class CTexture256x256Test: public CGradientTest
{
	public:
	CTexture256x256Test();
	~CTexture256x256Test();
};

// Texture 512x512 Class definitions
class CTexture512x512Test: public CGradientTest
{
	public:
	CTexture512x512Test();
	~CTexture512x512Test();
};

// Texture 1024x1024 Class definitions
class CTexture1024x1024Test: public CGradientTest
{
	public:
	CTexture1024x1024Test();
	~CTexture1024x1024Test();
};

// Texture 2048x2048 Class definitions
class CTexture2048x2048Test: public CGradientTest
{
	public:
	CTexture2048x2048Test();
	~CTexture2048x2048Test();
};

#endif
