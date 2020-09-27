/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	ZBuffer.h
 *
 ***************************************************************************/

#ifndef __ZBUFFER_H__
#define __ZBUFFER_H__

//#include "TestFrameBase.h"
//#include "CD3DTest.h"

//USETESTFRAME

// Defines
const int MAX_TESTS = 1;

typedef struct _Buffer
{
    DWORD dwStencilBitMask; // stencil bit mask
    DWORD dwZBitMask;       // z bit mask
    struct _Buffer *pNext;  // address of BUFFER data
} BUFFER, *PBUFFER;

// Base Class definitions
class CZBufferTest: public CD3DTest
{
    // Data
	public:
    D3DTLVERTEX     VertexList[3];
    DWORD           dwVertexCount;
	bool			bEnable;
	bool			bPrint;
	char			msgString[80];

    CSurface8*      m_pd3dsOriginalZ;
    UINT            m_uMaxTests;
    BOOL            m_bExit;

	public:
	CZBufferTest();
	~CZBufferTest();

	// Framework functions
//	virtual bool AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);

    virtual BOOL    AbortedExit(void);

    protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

// Verify Class definitions
class CZVerifyTest: public CZBufferTest
{
	// Private Data
	bool	bValidFormat;
	char 	errString[1024];

	public:
	CZVerifyTest();
	~CZVerifyTest();

	// Framework functions
	bool ExecuteTest(UINT);
	void SceneRefresh(void);
	bool ProcessFrame(void);

	// Helper functions
	bool VerifyFormat(void);
    bool VerifyFormat8(void);
};

// Print Class definitions
class CZPrintTest: public CZBufferTest
{
	public:
	CZPrintTest();
	~CZPrintTest();

	// Framework functions
	UINT TestInitialize(void);
	bool ExecuteTest(UINT uTestNum);

	// Helper functions
//	void PrintFormat(const PIXELFORMAT *pDDPF, int nFormat);
};

// Enable Class definitions
class CZEnableTest: public CZBufferTest
{
	public:
	CZEnableTest();
	~CZEnableTest();

	// Framework functions
	bool ExecuteTest(UINT uTestNum);

	// Helper functions
	void DrawTriangle(void);
};

#endif
