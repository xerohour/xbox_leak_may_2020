/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	MipFilter.h
 *
 ***************************************************************************/

#ifndef __MIPFILTER_H__
#define __MIPFILTER_H__

//#include "TestFrameBase.h"
//#include "CD3DTest.h"

//USETESTFRAME

// Defines
#define MAX_TESTS	9

// Base Class definitions
class CMipFilterTest: public TESTFRAME(CD3DTest)
{
    // Data
	public:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
	CTexture8 *		pTexture;
//	CImageData *	pMipImage[9];
    CTexture8 *     pMipLevel[9];
	float			SizeArray[18];
	bool			bNonSquareW;
	bool			bNonSquareH;
	bool			bMipLinear;
	bool			bMinLinear;
	bool			bBias;
	bool			bFull;
	bool			bMax;
	char			msgString[80];
	char			szStatus[80];

    BOOL            bLoadWithUpdate;
    int             nMWidth, nMHeight;
    BOOL            m_bExit;

	public:
	CMipFilterTest();
	~CMipFilterTest();

	// Framework functions
//    bool AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize);
	bool SetDefaultRenderStates(void);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void InitSize(void);
	BOOL InitMipImage(DWORD dwAspectRatio);
	void DrawGrid(float fWidth, float fHeight);
    void UpdateStatus(bool bResult);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

// Point_MinPoint Class definitions
class CPoint_MinPointTest: public CMipFilterTest
{
	public:
	CPoint_MinPointTest();
	~CPoint_MinPointTest();
};

// Point_MinLinear Class definitions
class CPoint_MinLinearTest: public CMipFilterTest
{
	public:
	CPoint_MinLinearTest();
	~CPoint_MinLinearTest();
};

// Point_NonSquareW Class definitions
class CPoint_NonSquareWTest: public CMipFilterTest
{
	public:
	CPoint_NonSquareWTest();
	~CPoint_NonSquareWTest();
};

// Point_NonSquareH Class definitions
class CPoint_NonSquareHTest: public CMipFilterTest
{
	public:
	CPoint_NonSquareHTest();
	~CPoint_NonSquareHTest();
};

// Point_FullSquare Class definitions
class CPoint_FullSquareTest: public CMipFilterTest
{
	public:
	CPoint_FullSquareTest();
	~CPoint_FullSquareTest();
};

// Point_FullNonSquareW Class definitions
class CPoint_FullNonSquareWTest: public CMipFilterTest
{
	public:
	CPoint_FullNonSquareWTest();
	~CPoint_FullNonSquareWTest();
};

// Point_FullNonSquareH Class definitions
class CPoint_FullNonSquareHTest: public CMipFilterTest
{
	public:
	CPoint_FullNonSquareHTest();
	~CPoint_FullNonSquareHTest();
};

// Point_MaxMipLevel Class definitions
class CPoint_MaxMipLevelTest: public CMipFilterTest
{
	public:
	CPoint_MaxMipLevelTest();
	~CPoint_MaxMipLevelTest();
};

// Point_LODBias Class definitions
class CPoint_LODBiasTest: public CMipFilterTest
{
	public:
	CPoint_LODBiasTest();
	~CPoint_LODBiasTest();
};

// Linear_MinPoint Class definitions
class CLinear_MinPointTest: public CMipFilterTest
{
	public:
	CLinear_MinPointTest();
	~CLinear_MinPointTest();
};

// Linear_MinLinear Class definitions
class CLinear_MinLinearTest: public CMipFilterTest
{
	public:
	CLinear_MinLinearTest();
	~CLinear_MinLinearTest();
};

// Linear_NonSquareW Class definitions
class CLinear_NonSquareWTest: public CMipFilterTest
{
	public:
	CLinear_NonSquareWTest();
	~CLinear_NonSquareWTest();
};

// Linear_NonSquareH Class definitions
class CLinear_NonSquareHTest: public CMipFilterTest
{
	public:
	CLinear_NonSquareHTest();
	~CLinear_NonSquareHTest();
};

// Linear_FullSquare Class definitions
class CLinear_FullSquareTest: public CMipFilterTest
{
	public:
	CLinear_FullSquareTest();
	~CLinear_FullSquareTest();
};

// Linear_FullNonSquareW Class definitions
class CLinear_FullNonSquareWTest: public CMipFilterTest
{
	public:
	CLinear_FullNonSquareWTest();
	~CLinear_FullNonSquareWTest();
};

// Linear_FullNonSquareH Class definitions
class CLinear_FullNonSquareHTest: public CMipFilterTest
{
	public:
	CLinear_FullNonSquareHTest();
	~CLinear_FullNonSquareHTest();
};

// Linear_MaxMipLevel Class definitions
class CLinear_MaxMipLevelTest: public CMipFilterTest
{
	public:
	CLinear_MaxMipLevelTest();
	~CLinear_MaxMipLevelTest();
};

// Linear_LODBias Class definitions
class CLinear_LODBiasTest: public CMipFilterTest
{
	public:
	CLinear_LODBiasTest();
	~CLinear_LODBiasTest();
};

#endif
