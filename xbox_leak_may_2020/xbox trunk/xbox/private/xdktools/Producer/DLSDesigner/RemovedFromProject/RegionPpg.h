#if !defined(AFX_REGIONPPG_H__BC964EBA_96F7_11D0_89AA_00A0C9054129__INCLUDED_)
#define AFX_REGIONPPG_H__BC964EBA_96F7_11D0_89AA_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// RegionPpg.h : Declaration of the CRegionPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CRegionPropPage : See RegionPpg.cpp.cpp for implementation.

class CRegionPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CRegionPropPage)
	DECLARE_OLECREATE_EX(CRegionPropPage)

// Constructor
public:
	CRegionPropPage();

// Dialog Data
	//{{AFX_DATA(CRegionPropPage)
	enum { IDD = IDD_PROPPAGE_REGION };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CRegionPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIONPPG_H__BC964EBA_96F7_11D0_89AA_00A0C9054129__INCLUDED)
