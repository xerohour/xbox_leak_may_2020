#if !defined(AFX_GRIDPPG_H__EAB0CD55_9459_11D0_8C10_00A0C92E1CAC__INCLUDED_)
#define AFX_GRIDPPG_H__EAB0CD55_9459_11D0_8C10_00A0C92E1CAC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// GridPpg.h : Declaration of the CGridPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CGridPropPage : See GridPpg.cpp.cpp for implementation.

class CGridPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CGridPropPage)
	DECLARE_OLECREATE_EX(CGridPropPage)

// Constructor
public:
	CGridPropPage();

// Dialog Data
	//{{AFX_DATA(CGridPropPage)
	enum { IDD = IDD_PROPPAGE_GRID };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CGridPropPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDPPG_H__EAB0CD55_9459_11D0_8C10_00A0C92E1CAC__INCLUDED)
