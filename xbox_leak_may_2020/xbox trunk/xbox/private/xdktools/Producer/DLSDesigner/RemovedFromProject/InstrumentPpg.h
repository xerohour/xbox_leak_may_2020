#if !defined(AFX_INSTRUMENTPPG_H__BC964EAB_96F7_11D0_89AA_00A0C9054129__INCLUDED_)
#define AFX_INSTRUMENTPPG_H__BC964EAB_96F7_11D0_89AA_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// InstrumentPpg.h : Declaration of the CInstrumentPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CInstrumentPropPage : See InstrumentPpg.cpp.cpp for implementation.

class CInstrumentPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CInstrumentPropPage)
	DECLARE_OLECREATE_EX(CInstrumentPropPage)

// Constructor
public:
	CInstrumentPropPage();

// Dialog Data
	//{{AFX_DATA(CInstrumentPropPage)
	enum { IDD = IDD_PROPPAGE_INSTRUMENT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CInstrumentPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTRUMENTPPG_H__BC964EAB_96F7_11D0_89AA_00A0C9054129__INCLUDED)
