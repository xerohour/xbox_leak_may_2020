/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	AlphaBld.h
 *
 ***************************************************************************/

#ifndef __ALPHABLD_H__
#define __ALPHABLD_H__

//USETESTFRAME

// Defines
#define MAX_TESTS		30

// Class definition
class CAlphaBldTest: public CD3DTest
{
    // Data
	public:
    D3DTLVERTEX     VertexList[40];
    D3DTLVERTEX     BackgroundList[400];
    DWORD           dwVertexCount;
	DWORD			dwSrcCap;
	DWORD			dwDestCap;
	int				nAlphaEnable;
	int				nAlphaValue[5];
	bool			bCircle;
    bool            bBlendOps;
	int 			nBlendOp;
    DWORD           dwBlendOverride;
    DWORD           BlendOp[5];
    LPCSTR			szSrcName;
	LPCSTR			szDestName;
	char			msgString[80];
	char			szStatus[80];
	char			szBlend[80];

    BOOL            m_bExit;

	public:
	CAlphaBldTest();
	~CAlphaBldTest();

	// Framework functions
//    virtual bool AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize);
	virtual UINT TestInitialize(void);
    virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
    virtual bool TestTerminate(void);

	// Helper functions
	void DrawBackground(void);
	void DrawGrid(int nTest, int nMesh);
	void DrawCircle(D3DVECTOR* pCenter, float Radius, int nDivisions, D3DCOLOR Color);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

#endif
