/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	ZCmp.h
 *
 ***************************************************************************/

#ifndef __ZCMP_H__
#define __ZCMP_H__

//#include "TestFrameBase.h"
//#include "CD3DTest.h"

//USETESTFRAME

// Defines
const int MAX_TESTS = 16;

typedef struct _Buffer
{
    DWORD dwStencilBitMask; // stencil bit mask
    DWORD dwZBitMask;       // z bit mask
    struct _Buffer *pNext;  // address of BUFFER data
} BUFFER, *PBUFFER;

// Base Class definitions
class CZCmpTest: public CD3DTest
{
    // Data
	public:
    D3DTLVERTEX     VertexList[24];
    DWORD           dwVertexCount;
	DWORD			dwZCmpCap;
	char			msgString[80];

    CSurface8*  m_pd3dsOriginalZ;
    UINT        m_uMaxTests;
    BOOL            m_bExit;

	public:
	CZCmpTest();
	~CZCmpTest();

	// Framework functions
//	virtual bool AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize);
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
class CZNeverTest: public CZCmpTest
{
	public:
	CZNeverTest();
	~CZNeverTest();

	bool SetDefaultRenderStates(void);
};

// Less Class definitions
class CZLessTest: public CZCmpTest
{
	public:
	CZLessTest();
	~CZLessTest();

	bool SetDefaultRenderStates(void);
};

// Equal Class definitions
class CZEqualTest: public CZCmpTest
{
    // Data
//    D3DTLVERTEX     BackgroundList[4];

	public:
	CZEqualTest();
	~CZEqualTest();

	bool SetDefaultRenderStates(void);
    bool ClearFrame(void);
//	void SceneRefresh(void);

	// Helper functions
//	void SetupBackground(void);
};

// LessEqual Class definitions
class CZLessEqualTest: public CZCmpTest
{
	public:
	CZLessEqualTest();
	~CZLessEqualTest();

	bool SetDefaultRenderStates(void);
};

// Greater Class definitions
class CZGreaterTest: public CZCmpTest
{
    // Data
//    D3DTLVERTEX     BackgroundList[4];

	public:
	CZGreaterTest();
	~CZGreaterTest();

	bool SetDefaultRenderStates(void);
    bool ClearFrame(void);
//	void SceneRefresh(void);

	// Helper functions
//	void SetupBackground(void);
};

// NotEqual Class definitions
class CZNotEqualTest: public CZCmpTest
{
    // Data
//    D3DTLVERTEX     BackgroundList[4];

	public:
	CZNotEqualTest();
	~CZNotEqualTest();

	bool SetDefaultRenderStates(void);
    bool ClearFrame(void);
//	void SceneRefresh(void);

	// Helper functions
//	void SetupBackground(void);
};

// GreaterEqual Class definitions
class CZGreaterEqualTest: public CZCmpTest
{
    // Data
//    D3DTLVERTEX     BackgroundList[4];

	public:
	CZGreaterEqualTest();
	~CZGreaterEqualTest();

	bool SetDefaultRenderStates(void);
    bool ClearFrame(void);
//	void SceneRefresh(void);

	// Helper functions
//	void SetupBackground(void);
};

// Always Class definitions
class CZAlwaysTest: public CZCmpTest
{
	public:
	CZAlwaysTest();
	~CZAlwaysTest();

	bool SetDefaultRenderStates(void);
};

#endif
