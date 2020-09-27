// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// export WinMain to force linkage to this module

extern int AFXAPI AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow);

#ifdef _MAC
extern "C" int PASCAL
#else
extern "C" int WINAPI
#endif
_tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	return AfxWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

/////////////////////////////////////////////////////////////////////////////
// initialize app state such that it points to this module's core state

class _AFX_TERM_APP_STATE
{
public:
	~_AFX_TERM_APP_STATE();
};

_AFX_TERM_APP_STATE::~_AFX_TERM_APP_STATE()
{
	AfxTermLocalData(NULL);
}

// force initialization early
#pragma warning(disable: 4074)
#pragma init_seg(lib)

_AFX_TERM_APP_STATE _afxTermAppState;

/////////////////////////////////////////////////////////////////////////////
