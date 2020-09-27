/////////////////////////////////////////////////////////////////////////////
// vwprtbar.h
//
// email	date		change
// briancr	11/03/94	created
//
// copyright 1994 Microsoft

// Interface of the CViewportBar class

#ifndef __VWPRTBAR_H__
#define __VWPRTBAR_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CSuiteBar class

class CViewportBar : public CToolBar
{
// ctor/dtor
public:
	CViewportBar()		{ }
	~CViewportBar()	{ }

// operations
public:
	CreateBar(CWnd* pParent);
};

#endif // __VWPRTBAR_H__
