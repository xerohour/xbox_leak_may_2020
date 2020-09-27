/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	OverScanTest.h

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 18-Dec-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	18-Dec-2001 jeffsul
		Initial Version

--*/

#ifndef _OVERSCANTEST_H_
#define _OVERSCANTEST_H_

#include "Test.h"

class COverScanTest : public CTest
{
public:
	COverScanTest();
	~COverScanTest();

protected:
	HRESULT	InitGraphics();
	HRESULT	DestroyGraphics();
	HRESULT	InitResources();
	HRESULT	DestroyResources();

	HRESULT	ProcessInput();
	HRESULT	Update();
	HRESULT	Render();

private:
	LPDIRECT3DDEVICE8		m_pD3DDevice;
	HANDLE                  m_hInputDevice;				// input device
	XINPUT_STATE			m_PrevInputState;           // previous input state

	D3DCOLOR				m_OverScanColor;
};

#endif //#ifndef _OVERSCANTEST_H_
