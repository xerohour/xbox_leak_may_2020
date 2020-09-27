#if !defined(AFX_SCRIPTPPG_H__D135DB68_66ED_11D3_B45D_00105A2796DE__INCLUDED_)
#define AFX_SCRIPTPPG_H__D135DB68_66ED_11D3_B45D_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ScriptPpg.h : Declaration of the CScriptPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CScriptPropPage : See ScriptPpg.cpp.cpp for implementation.

class CScriptPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CScriptPropPage)
	DECLARE_OLECREATE_EX(CScriptPropPage)

// Constructor
public:
	CScriptPropPage();

// Dialog Data
	//{{AFX_DATA(CScriptPropPage)
	enum { IDD = IDD_PROPPAGE_SCRIPT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CScriptPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTPPG_H__D135DB68_66ED_11D3_B45D_00105A2796DE__INCLUDED)
