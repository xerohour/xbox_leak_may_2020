/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Address.h
 *
 ***************************************************************************/

#ifndef __ADDRESS_H__
#define __ADDRESS_H__

//USETESTFRAME

// Defines
#define MAX_TESTS		40

// Class definition
class CAddressTest: public CD3DTest
{
    // Data
	public:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
	CTexture8 *		pTexture;
//	CImageData *	pImage;
	D3DCOLOR	    Gradient[4];
	DWORD			dwAddressCapU;
	DWORD			dwAddressCapV;
	LPCTSTR			szAddrU;
	LPCTSTR			szAddrV;
	bool			bIndependentUV;
	bool			bWrapUV;
	bool			bBorder;
	TCHAR			msgString[80];

    BOOL            m_bExit;

	public:
	CAddressTest();
	~CAddressTest();

	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void DrawGrid(int nTest);
	void DrawWrapGrid(int nTest);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

// Wrap U Class definitions
class CWrapUTest: public CAddressTest
{
	public:
	CWrapUTest();
	~CWrapUTest();

	bool SetDefaultRenderStates(void);
};

// Wrap V Class definitions
class CWrapVTest: public CAddressTest
{
	public:
	CWrapVTest();
	~CWrapVTest();

	bool SetDefaultRenderStates(void);
};

// Wrap UV Class definitions
class CWrapUVTest: public CAddressTest
{
	public:
	CWrapUVTest();
	~CWrapUVTest();

	bool SetDefaultRenderStates(void);
};

// Wrap Class definitions
class CWrapTest: public CAddressTest
{
	public:
	CWrapTest();
	~CWrapTest();

	bool SetDefaultRenderStates(void);
};

// Mirror Class definitions
class CMirrorTest: public CAddressTest
{
	public:
	CMirrorTest();
	~CMirrorTest();

	bool SetDefaultRenderStates(void);
};

// Clamp Class definitions
class CClampTest: public CAddressTest
{
	public:
	CClampTest();
	~CClampTest();

	bool SetDefaultRenderStates(void);
};

// Border Class definitions
class CBorderTest: public CAddressTest
{
	public:
	CBorderTest();
	~CBorderTest();

	bool SetDefaultRenderStates(void);
};

// WrapMirror Class definitions
class CWrapMirrorTest: public CAddressTest
{
	public:
	CWrapMirrorTest();
	~CWrapMirrorTest();

	bool SetDefaultRenderStates(void);
};

// WrapClamp Class definitions
class CWrapClampTest: public CAddressTest
{
	public:
	CWrapClampTest();
	~CWrapClampTest();

	bool SetDefaultRenderStates(void);
};

// WrapBorder Class definitions
class CWrapBorderTest: public CAddressTest
{
	public:
	CWrapBorderTest();
	~CWrapBorderTest();

	bool SetDefaultRenderStates(void);
};

// MirrorWrap Class definitions
class CMirrorWrapTest: public CAddressTest
{
	public:
	CMirrorWrapTest();
	~CMirrorWrapTest();

	bool SetDefaultRenderStates(void);
};

// MirrorClamp Class definitions
class CMirrorClampTest: public CAddressTest
{
	public:
	CMirrorClampTest();
	~CMirrorClampTest();

	bool SetDefaultRenderStates(void);
};

// MirrorBorder Class definitions
class CMirrorBorderTest: public CAddressTest
{
	public:
	CMirrorBorderTest();
	~CMirrorBorderTest();

	bool SetDefaultRenderStates(void);
};

// ClampWrap Class definitions
class CClampWrapTest: public CAddressTest
{
	public:
	CClampWrapTest();
	~CClampWrapTest();

	bool SetDefaultRenderStates(void);
};

// ClampMirror Class definitions
class CClampMirrorTest: public CAddressTest
{
	public:
	CClampMirrorTest();
	~CClampMirrorTest();

	bool SetDefaultRenderStates(void);
};

// ClampBorder Class definitions
class CClampBorderTest: public CAddressTest
{
	public:
	CClampBorderTest();
	~CClampBorderTest();

	bool SetDefaultRenderStates(void);
};

// BorderWrap Class definitions
class CBorderWrapTest: public CAddressTest
{
	public:
	CBorderWrapTest();
	~CBorderWrapTest();

	bool SetDefaultRenderStates(void);
};

// BorderMirror Class definitions
class CBorderMirrorTest: public CAddressTest
{
	public:
	CBorderMirrorTest();
	~CBorderMirrorTest();

	bool SetDefaultRenderStates(void);
};

// BorderClamp Class definitions
class CBorderClampTest: public CAddressTest
{
	public:
	CBorderClampTest();
	~CBorderClampTest();

	bool SetDefaultRenderStates(void);
};

#endif
