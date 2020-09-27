#if !defined(AFX_WAVEPPG_H__BC964EB0_96F7_11D0_89AA_00A0C9054129__INCLUDED_)
#define AFX_WAVEPPG_H__BC964EB0_96F7_11D0_89AA_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// WavePpg.h : Declaration of the CWavePropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CWavePropPage : See WavePpg.cpp.cpp for implementation.

class CWavePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CWavePropPage)
	DECLARE_OLECREATE_EX(CWavePropPage)

// Constructor
public:
	CWavePropPage();

// Dialog Data
	//{{AFX_DATA(CWavePropPage)
	enum { IDD = IDD_PROPPAGE_WAVE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CWavePropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAVEPPG_H__BC964EB0_96F7_11D0_89AA_00A0C9054129__INCLUDED)
