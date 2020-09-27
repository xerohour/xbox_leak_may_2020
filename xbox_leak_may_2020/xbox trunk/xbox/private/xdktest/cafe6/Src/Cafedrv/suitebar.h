/////////////////////////////////////////////////////////////////////////////
// suitebar.h
//
// email	date		change
// briancr	11/03/94	created
//
// copyright 1994 Microsoft

// Interface of the CSubSuiteBar class

#ifndef __SUITEBAR_H__
#define __SUITEBAR_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CSuiteBar class

class CSubSuiteBar : public CToolBar
{
// ctor/dtor
public:
	CSubSuiteBar()		{ }
	~CSubSuiteBar()	{ }

// operations
public:
	CreateBar(CWnd* pParent);
};

#endif // __SUITEBAR_H__
