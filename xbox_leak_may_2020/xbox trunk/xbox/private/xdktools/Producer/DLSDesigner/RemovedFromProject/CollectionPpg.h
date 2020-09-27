#if !defined(AFX_COLLECTIONPPG_H__BC964EA6_96F7_11D0_89AA_00A0C9054129__INCLUDED_)
#define AFX_COLLECTIONPPG_H__BC964EA6_96F7_11D0_89AA_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CollectionPpg.h : Declaration of the CCollectionPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CCollectionPropPage : See CollectionPpg.cpp.cpp for implementation.

class CCollectionPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CCollectionPropPage)
	DECLARE_OLECREATE_EX(CCollectionPropPage)

// Constructor
public:
	CCollectionPropPage();

// Dialog Data
	//{{AFX_DATA(CCollectionPropPage)
	enum { IDD = IDD_PROPPAGE_COLLECTION };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CCollectionPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLLECTIONPPG_H__BC964EA6_96F7_11D0_89AA_00A0C9054129__INCLUDED)
