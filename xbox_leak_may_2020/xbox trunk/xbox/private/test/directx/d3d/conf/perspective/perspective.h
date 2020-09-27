/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Perspective.h
 *
 ***************************************************************************/

#ifndef __PERSPECTIVE_H__
#define __PERSPECTIVE_H__

//#include "TestFrameBase.h"
//#include "CD3DTest.h"

//USETESTFRAME

// Defines
#define MAX_GRID_TESTS	32
#define MAX_TRI_TESTS	11

// Class definition
class CPerspectiveTest: public TESTFRAME(CD3DTest)
{
    // Data
    public:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
	CTexture8 *		pTexture;
//	CImageData *	pImage;
	char			msgString[80];

	D3DCOLOR		Stripes[8];
    BOOL            m_bExit;

	public:
	CPerspectiveTest();
	~CPerspectiveTest();

	// Framework functions
	virtual bool SetDefaultRenderStates(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void DrawTexGrid(int nTest);
	void DrawColorGrid(int nTest);
	void DrawTriangle(int nTest);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

// Texture Class definitions
class CTextureTest: public CPerspectiveTest
{
	public:
	CTextureTest();
	~CTextureTest();

    // Framework functions
	UINT TestInitialize(void);

};

// Color Class definitions
class CColorTest: public CPerspectiveTest
{
	public:
	CColorTest();
	~CColorTest();

	// Framework functions
	UINT TestInitialize(void);
};

#endif
