// custpage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomizePage class definition
#ifndef _CUSTOMIZETAB_
#define _CUSTOMIZETAB_

class CCustomizePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCustomizePage)

// Construction
public:
	CCustomizePage();
	~CCustomizePage();

// Dialog Data
	//{{AFX_DATA(CCustomizePage)
	enum { IDD = IDD_CUSTOMIZE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCustomizePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCustomizePage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif ; //_CUSTOMIZETAB_
