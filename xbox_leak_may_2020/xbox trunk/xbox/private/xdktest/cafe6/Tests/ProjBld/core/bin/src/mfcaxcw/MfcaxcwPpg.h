#if !defined(AFX_MFCAXCWPPG_H__7EFBEC0B_CAA8_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_MFCAXCWPPG_H__7EFBEC0B_CAA8_11D0_84BF_00AA00C00848__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// MfcaxcwPpg.h : Declaration of the CMfcaxcwPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CMfcaxcwPropPage : See MfcaxcwPpg.cpp.cpp for implementation.

class CMfcaxcwPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CMfcaxcwPropPage)
	DECLARE_OLECREATE_EX(CMfcaxcwPropPage)

// Constructor
public:
	CMfcaxcwPropPage();

// Dialog Data
	//{{AFX_DATA(CMfcaxcwPropPage)
	enum { IDD = IDD_PROPPAGE_MFCAXCW };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CMfcaxcwPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCAXCWPPG_H__7EFBEC0B_CAA8_11D0_84BF_00AA00C00848__INCLUDED)
