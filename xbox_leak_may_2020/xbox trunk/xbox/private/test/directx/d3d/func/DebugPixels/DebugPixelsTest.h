/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	DebugPixelsTest.h

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 10-Dec-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	10-Dec-2001 jeffsul
		Initial Version

--*/

#ifndef _DEBUGPIXELSTEST_H_
#define _DEBUGPIXELSTEST_H_

#include "Test.h"

class CDbgPixelsTest : public CTest
{
public:
	CDbgPixelsTest();
	~CDbgPixelsTest();

protected:
	HRESULT ReadIni();
	HRESULT InitGraphics();
	HRESULT	DestroyGraphics();
	HRESULT InitResources();
	HRESULT	DestroyResources();

	HRESULT ProcessInput();
	HRESULT Update();
	HRESULT Render();

private:
	LPDIRECT3DDEVICE8	m_pD3DDevice;
	XINPUT_STATE		m_PrevInputState;
	HANDLE              m_hInpDevice;				// input device

	LPDIRECT3DTEXTURE8	m_pTexture;
	D3DFORMAT			m_Format;
	INT				m_pvTextureOrSurface;
};

#endif //#ifndef _DEBUGPIXELSTEST_H_
