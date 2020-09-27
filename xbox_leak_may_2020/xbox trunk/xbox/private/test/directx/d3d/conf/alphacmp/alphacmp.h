/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	AlphaCmp.h
 *
 ***************************************************************************/

#ifndef __ALPHACMP_H__
#define __ALPHACMP_H__

//USETESTFRAME

// Defines
#define MAX_TESTS	5

// Base Class definitions
class CAlphaCmpTest: public CD3DTest
{
    // Data
	public:
    D3DTLVERTEX     VertexList[3];
    DWORD           dwVertexCount;
	DWORD			dwAlphaCmpCap;
	int				nAlphaValue[5];
	bool			bGouraud;
	char			msgString[80];
	char			szStatus[80];

    BOOL            m_bExit;

	public:
	CAlphaCmpTest();
	~CAlphaCmpTest();

	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool SetDefaultRenderStates(void);
	virtual bool ExecuteTest(UINT uTestNum);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);

	// Helper functions
	void DrawTriangle(int nAlpha);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};


// Never Class definitions
class CAlphaNeverTest: public CAlphaCmpTest
{
	public:
	CAlphaNeverTest();
	~CAlphaNeverTest();

	bool SetDefaultRenderStates(void);
};

// Less Class definitions
class CAlphaLessTest: public CAlphaCmpTest
{
	public:
	CAlphaLessTest();
	~CAlphaLessTest();

	bool SetDefaultRenderStates(void);
};

// Equal Class definitions
class CAlphaEqualTest: public CAlphaCmpTest
{
	public:
	CAlphaEqualTest();
	~CAlphaEqualTest();

	bool SetDefaultRenderStates(void);
};

// LessEqual Class definitions
class CAlphaLessEqualTest: public CAlphaCmpTest
{
	public:
	CAlphaLessEqualTest();
	~CAlphaLessEqualTest();

	bool SetDefaultRenderStates(void);
};

// Greater Class definitions
class CAlphaGreaterTest: public CAlphaCmpTest
{
	public:
	CAlphaGreaterTest();
	~CAlphaGreaterTest();

	bool SetDefaultRenderStates(void);
};

// NotEqual Class definitions
class CAlphaNotEqualTest: public CAlphaCmpTest
{
	public:
	CAlphaNotEqualTest();
	~CAlphaNotEqualTest();

	bool SetDefaultRenderStates(void);
};

// GreaterEqual Class definitions
class CAlphaGreaterEqualTest: public CAlphaCmpTest
{
	public:
	CAlphaGreaterEqualTest();
	~CAlphaGreaterEqualTest();

	bool SetDefaultRenderStates(void);
};

// Always Class definitions
class CAlphaAlwaysTest: public CAlphaCmpTest
{
	public:
	CAlphaAlwaysTest();
	~CAlphaAlwaysTest();

	bool SetDefaultRenderStates(void);
};

#endif
