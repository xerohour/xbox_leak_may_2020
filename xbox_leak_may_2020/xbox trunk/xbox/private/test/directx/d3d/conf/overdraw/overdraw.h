/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Overdraw.h
 *
 ***************************************************************************/

#ifndef __OVERDRAW_H__
#define __OVERDRAW_H__

//#include "TestFrameBase.h"
//#include "CD3DTest.h"

//USETESTFRAME

// Defines
#define MAX_TESTS	40
#define RAND_SEED	4321

// Class definition
class COverdrawTest: public TESTFRAME(CD3DTest)
{
    // Data
	public:
	char	msgString[80];
    bool    bAlpha;
	int		nOffset;

    BOOL    m_bExit;

	public:
	COverdrawTest();
	~COverdrawTest();

	// Framework functions
	virtual bool SetDefaultRenderStates(void);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual bool ProcessFrame(void);

	// Helper functions
	bool AlphaGrid(int nTest, float fGridXPos, float fGridYPos, int nXDivisions, int nYDivisions);
	bool BltGrid(int nTest, float fGridXPos, float fGridYPos, int nXDivisions, int nYDivisions);
    void OrBlt(CSurface8 * pDest, CSurface8 * pSrc);
	void ReverseVertices(D3DTLVERTEX * pVertices, int nVertices);

    virtual BOOL Render();

    virtual bool Present8(POINT &Pt);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

// Alpha Class definitions
class CAlpha_OverdrawTest: public COverdrawTest
{
	public:
	CAlpha_OverdrawTest();
	~CAlpha_OverdrawTest();
};

// Blt Class definitions
class CBlt_OverdrawTest: public COverdrawTest
{
	public:
	CBlt_OverdrawTest();
	~CBlt_OverdrawTest();
};

#endif
