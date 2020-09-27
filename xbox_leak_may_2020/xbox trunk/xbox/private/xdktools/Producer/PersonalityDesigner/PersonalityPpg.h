#if !defined(AFX_PERSONALITYPPG_H__D433F96E_B588_11D0_9EDC_00AA00A21BA9__INCLUDED_)
#define AFX_PERSONALITYPPG_H__D433F96E_B588_11D0_9EDC_00AA00A21BA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// PersonalityPpg.h : Declaration of the CPersonalityPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CPersonalityPropPage : See PersonalityPpg.cpp.cpp for implementation.

class CPersonalityPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CPersonalityPropPage)
	DECLARE_OLECREATE_EX(CPersonalityPropPage)

// Constructor
public:
	CPersonalityPropPage();

// Dialog Data
	//{{AFX_DATA(CPersonalityPropPage)
	enum { IDD = IDD_PROPPAGE_PERSONALITY };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CPersonalityPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERSONALITYPPG_H__D433F96E_B588_11D0_9EDC_00AA00A21BA9__INCLUDED)
