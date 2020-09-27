// comppage.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CX86ExecutablePage dialog

class CX86ExecutablePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CX86ExecutablePage)

// Construction
public:
	CX86ExecutablePage(CSettings* pSettings = NULL);
	~CX86ExecutablePage();

// Dialog Data
	//{{AFX_DATA(CX86ExecutablePage)
	enum { IDD = IDD_Executable };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CX86ExecutablePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CX86ExecutablePage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
