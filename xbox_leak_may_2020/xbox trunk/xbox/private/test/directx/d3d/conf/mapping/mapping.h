/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Mapping.h
 *
 ***************************************************************************/

#ifndef __MAPPING_H__
#define __MAPPING_H__

//USETESTFRAME

// Defines
#define MAX_TESTS	316

// UV structure
typedef struct _UV{
	float u;
	float v;

public:
	_UV() {u=0.0f; v=0.0f;};
	_UV(float u1, float v1) {u=u1; v=v1;};

} UV;

// Base Class definitions
class CMappingTest: public TESTFRAME(CD3DTest)
{
    // Data
	public:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
	CTexture8 *		pTexture;
//	CImageData *	pImage;
	DWORD			dwMinFilterCap;
	DWORD			dwMagFilterCap;
	UV				TopLeft[4];
	UV				TopRight[4];
	UV				BottomLeft[4];
	UV				BottomRight[4];
	char			msgString[80];

    int             nTextureFormat;
	D3DCOLOR	    Gradient[4];
    BOOL            m_bExit;

	public:
	CMappingTest();
	~CMappingTest();

	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void DrawGrid(int nTest, int nTexFormats);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

// Point Class definitions
class CMapPointTest: public CMappingTest
{
	public:
	CMapPointTest();
	~CMapPointTest();

	bool SetDefaultRenderStates(void);
};

// Linear Class definitions
class CMapLinearTest: public CMappingTest
{
	public:
	CMapLinearTest();
	~CMapLinearTest();

	bool SetDefaultRenderStates(void);
};

#endif
