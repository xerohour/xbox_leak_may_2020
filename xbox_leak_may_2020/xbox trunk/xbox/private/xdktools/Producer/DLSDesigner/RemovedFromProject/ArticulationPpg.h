#if !defined(AFX_ARTICULATIONPPG_H__BC964EB5_96F7_11D0_89AA_00A0C9054129__INCLUDED_)
#define AFX_ARTICULATIONPPG_H__BC964EB5_96F7_11D0_89AA_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ArticulationPpg.h : Declaration of the CArticulationPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CArticulationPropPage : See ArticulationPpg.cpp.cpp for implementation.

class CArticulationPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CArticulationPropPage)
	DECLARE_OLECREATE_EX(CArticulationPropPage)

// Constructor
public:
	CArticulationPropPage();

// Dialog Data
	//{{AFX_DATA(CArticulationPropPage)
	enum { IDD = IDD_PROPPAGE_ARTICULATION };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CArticulationPropPage)
	afx_msg void OnCancelMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARTICULATIONPPG_H__BC964EB5_96F7_11D0_89AA_00A0C9054129__INCLUDED)
