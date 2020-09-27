/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	XFFuncTest.h

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 31-Oct-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	31-Oct-2001 jeffsul
		Initial Version

--*/

#ifndef _XFFUNCTEST_H_
#define _XFFUNCTEST_H_

#include "Test.h"
#include <xfont.h>

#define XFFUNC_CACHE_SIZE	2048
#define XFFUNC_PATH			"T:\\Fonts\\*.ttf"
#define XFFUNC_PATHW		L"T:\\Fonts\\"
#define XFFUNC_TEST_STRING	L"The quick brown fox jumps over the lazy dog.\n1234567890"

class CXFFuncTest : public CTest
{
public:
	CXFFuncTest();
	~CXFFuncTest();

protected:
	HRESULT InitGraphics();
	HRESULT DestroyGraphics();
	HRESULT InitResources();
	HRESULT DestroyResources();
	HRESULT	ReadIni();

	HRESULT ProcessInput();
	HRESULT Render();

private:
	LPDIRECT3DDEVICE8		m_pD3DDevice;
	XFONT*					m_pXFont;
	XFONT*					m_pStatusFont;
	HANDLE					m_hFiles;
	WIN32_FIND_DATA			m_FindData;

	HANDLE					m_hInpDevice;
	XINPUT_STATE			m_PrevInputState;

	UINT					m_uAlignment;
	UINT					m_uHeight;
	UINT					m_uRLEWidth;
	UINT					m_uSpacing;
	UINT					m_uColor;
	UINT					m_uStyle;
	UINT					m_uAALevel;
	INT						m_nBackgroundMode;
	INT						m_nPosX;
	INT						m_nPosY;

	BOOL					m_bAuto;
	BOOL	AutoRand();

};

#endif //#ifndef _XFFUNCTEST_H_
