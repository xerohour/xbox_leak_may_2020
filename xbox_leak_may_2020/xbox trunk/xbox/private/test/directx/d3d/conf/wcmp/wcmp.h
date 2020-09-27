/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	WCmp.h
 *
 ***************************************************************************/

#ifndef __WCMP_H__
#define __WCMP_H__

//#include "TestFrameBase.h"
//#include "CD3DTest.h"

//USETESTFRAME

// Defines
#define MAX_TESTS	16

typedef struct _Buffer
{
    DWORD dwStencilBitMask; // stencil bit mask
    DWORD dwZBitMask;       // z bit mask
    struct _Buffer *pNext;  // address of BUFFER data
} BUFFER, *PBUFFER;

// Base Class definitions
class CWCmpTest: public CD3DTest
{
    // Data
	public:
    D3DTLVERTEX     VertexList[24];
    DWORD           dwVertexCount;
	DWORD			dwWCmpCap;
	char			msgString[80];

    // Hack Data
	D3DVERTEX	D3DVertexList[100];
// 	CMaterial   *pMaterial;

    CSurface8*  m_pd3dsOriginalZ;
    UINT        m_uMaxTests;
    BOOL            m_bExit;

    UINT        m_uZBufferFormats;
    D3DFORMAT   m_fmtd[4];

	public:
	CWCmpTest();
	~CWCmpTest();

	// Framework functions
//	virtual bool AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize);
	virtual bool SetDefaultMatrices(void);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT uTestNum);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);

	// Helper functions
	void ComboTests(float Zval);
	void AlwaysTests(int nTest);
	void LessTests(int nTest);
	bool EqualTests(int nTest);
	void GreaterTests(int nTest);

    virtual BOOL    AbortedExit(void);

    protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

// Never Class definitions
class CWNeverTest: public CWCmpTest
{
	public:
	CWNeverTest();
	~CWNeverTest();

	bool SetDefaultRenderStates(void);
};

// Less Class definitions
class CWLessTest: public CWCmpTest
{
	public:
	CWLessTest();
	~CWLessTest();

	bool SetDefaultRenderStates(void);
};

// Equal Class definitions
class CWEqualTest: public CWCmpTest
{
    // Data
    D3DTLVERTEX     BackgroundList[4];

	public:
	CWEqualTest();
	~CWEqualTest();

	bool SetDefaultRenderStates(void);
	void SceneRefresh(void);

	// Helper functions
	void SetupBackground(void);
};

// LessEqual Class definitions
class CWLessEqualTest: public CWCmpTest
{
	public:
	CWLessEqualTest();
	~CWLessEqualTest();

	bool SetDefaultRenderStates(void);
};

// Greater Class definitions
class CWGreaterTest: public CWCmpTest
{
    // Data
    D3DTLVERTEX     BackgroundList[4];

	public:
	CWGreaterTest();
	~CWGreaterTest();

	bool SetDefaultRenderStates(void);
	void SceneRefresh(void);

	// Helper functions
	void SetupBackground(void);
};

// NotEqual Class definitions
class CWNotEqualTest: public CWCmpTest
{
    // Data
    D3DTLVERTEX     BackgroundList[4];

	public:
	CWNotEqualTest();
	~CWNotEqualTest();

	bool SetDefaultRenderStates(void);
	void SceneRefresh(void);

	// Helper functions
	void SetupBackground(void);
};

// GreaterEqual Class definitions
class CWGreaterEqualTest: public CWCmpTest
{
    // Data
    D3DTLVERTEX     BackgroundList[4];

	public:
	CWGreaterEqualTest();
	~CWGreaterEqualTest();

	bool SetDefaultRenderStates(void);
	void SceneRefresh(void);

	// Helper functions
	void SetupBackground(void);
};

// Always Class definitions
class CWAlwaysTest: public CWCmpTest
{
	public:
	CWAlwaysTest();
	~CWAlwaysTest();

	bool SetDefaultRenderStates(void);
};

#endif
