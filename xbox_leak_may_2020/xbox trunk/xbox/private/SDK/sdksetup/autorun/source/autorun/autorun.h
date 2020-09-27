//-----------------------------------------------------------------------------
// File: autorun.h
//
// Desc:Main header file for the AUTORUN application
//
// Hist: 10.27.00 Emily Wang Created For XBOX
//       
// Copyright (c) 1998-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#if !defined(AFX_AUTORUN_H__03A697D8_6DE4_4754_8108_A425119BCF60__INCLUDED_)
#define AFX_AUTORUN_H__03A697D8_6DE4_4754_8108_A425119BCF60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAutorunApp:
// See autorun.cpp for the implementation of this class
//

typedef struct SAutorunLink
{
	PTSTR pszName;
	RECT  rcRect;
	PTSTR pszCommand;
} *PSAutorunLink;

typedef struct SAutorunInfo
{
	PTSTR         pszTitle;                 // Title
	TCHAR         szFontName[LF_FACESIZE];  // Font name
	TCHAR		  szFolderName[MAX_PATH];
	INT           nFontSize;
	COLORREF      crFont;
	COLORREF      crHighLight;
	TCHAR         szImagePath[MAX_PATH];
    TCHAR         szIconPath[MAX_PATH];
	TCHAR		  szPath[MAX_PATH];
	TCHAR		  szCursorPath[MAX_PATH];
	INT           cnLinks;
	PSAutorunLink psLinks;
} *PSAutorunInfo;

class CAutorunApp : public CWinApp
{
public:
	CAutorunApp();
   ~CAutorunApp();

	PSAutorunInfo GetInfo(void) { return &m_sInfo; }


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutorunApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAutorunApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	SAutorunInfo m_sInfo;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTORUN_H__03A697D8_6DE4_4754_8108_A425119BCF60__INCLUDED_)
