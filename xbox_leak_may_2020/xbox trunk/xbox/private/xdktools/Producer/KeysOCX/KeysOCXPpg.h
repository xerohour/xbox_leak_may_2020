#if !defined(AFX_KEYSOCXPPG_H__CDD09F95_E73C_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_KEYSOCXPPG_H__CDD09F95_E73C_11D0_89AB_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// KeysOCXPpg.h : Declaration of the CKeysOCXPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CKeysOCXPropPage : See KeysOCXPpg.cpp.cpp for implementation.

class CKeysOCXPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CKeysOCXPropPage)
	DECLARE_OLECREATE_EX(CKeysOCXPropPage)

// Constructor
public:
	CKeysOCXPropPage();

// Dialog Data
	//{{AFX_DATA(CKeysOCXPropPage)
	enum { IDD = IDD_PROPPAGE_KEYSOCX };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CKeysOCXPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYSOCXPPG_H__CDD09F95_E73C_11D0_89AB_00A0C9054129__INCLUDED)
