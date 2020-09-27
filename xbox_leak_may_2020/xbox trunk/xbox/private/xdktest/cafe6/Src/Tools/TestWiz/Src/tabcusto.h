// tabcusto.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TabCustomize dialog
#ifndef _CUSTOMIZETAB_
#define _CUSTOMIZETAB_

class TabCustomize : public CPropertyPage
{
	DECLARE_DYNCREATE(TabCustomize)

// Construction
public:
	TabCustomize();
	~TabCustomize();

// Dialog Data
	//{{AFX_DATA(TabCustomize)
	enum { IDD = IDD_CUSTOMIZE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(TabCustomize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(TabCustomize)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif ; //_CUSTOMIZETAB_
