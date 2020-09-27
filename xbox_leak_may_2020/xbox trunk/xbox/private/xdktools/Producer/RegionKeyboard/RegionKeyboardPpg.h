#if !defined(AFX_REGIONKEYBOARDPPG_H__36CD3197_EE61_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_REGIONKEYBOARDPPG_H__36CD3197_EE61_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// RegionKeyboardPpg.h : Declaration of the CRegionKeyboardPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardPropPage : See RegionKeyboardPpg.cpp.cpp for implementation.

class CRegionKeyboardPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CRegionKeyboardPropPage)
	DECLARE_OLECREATE_EX(CRegionKeyboardPropPage)

// Constructor
public:
	CRegionKeyboardPropPage();

// Dialog Data
	//{{AFX_DATA(CRegionKeyboardPropPage)
	enum { IDD = IDD_PROPPAGE_REGIONKEYBOARD };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CRegionKeyboardPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIONKEYBOARDPPG_H__36CD3197_EE61_11D0_876A_00AA00C08146__INCLUDED)
