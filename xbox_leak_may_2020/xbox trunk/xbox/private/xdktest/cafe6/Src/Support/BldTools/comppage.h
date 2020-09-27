// comppage.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CCompilerDriverPage dialog

class CCompilerDriverPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCompilerDriverPage)

// Construction
public:
	CCompilerDriverPage(CSettings* pSettings = NULL);
	~CCompilerDriverPage();

// Dialog Data
	//{{AFX_DATA(CCompilerDriverPage)
	enum { IDD = IDD_CompDrv };
	CString	m_strCmdLine;
	CString	m_strFilename;
	CString	m_strWorkDir;
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCompilerDriverPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCompilerDriverPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
