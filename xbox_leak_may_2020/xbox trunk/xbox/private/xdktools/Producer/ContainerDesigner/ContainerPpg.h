#if !defined(AFX_CONTAINERPPG_H__778A0B9F_6F81_11D3_B45F_00105A2796DE__INCLUDED_)
#define AFX_CONTAINERPPG_H__778A0B9F_6F81_11D3_B45F_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ContainerPpg.h : Declaration of the CContainerPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CContainerPropPage : See ContainerPpg.cpp.cpp for implementation.

class CContainerPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CContainerPropPage)
	DECLARE_OLECREATE_EX(CContainerPropPage)

// Constructor
public:
	CContainerPropPage();

// Dialog Data
	//{{AFX_DATA(CContainerPropPage)
	enum { IDD = IDD_PROPPAGE_CONTAINER };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CContainerPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTAINERPPG_H__778A0B9F_6F81_11D3_B45F_00105A2796DE__INCLUDED)
