// projtype.h : header file
//

#include "seqdlg.h"
#ifndef VS_PACKAGE
#include <slob.h>
#include <utilctrl.h>
#endif	// VS_PACKAGE

/////////////////////////////////////////////////////////////////////////////
// CProjDlg dialog

class CProjDlg : public CSeqDlg
{
// Construction
public:
	CProjDlg(CWnd* pParent = NULL);	// standard constructor
	virtual void WriteValues();         // Writes to projectOptions
	virtual UINT ChooseBitmap();

// Dialog Data
	//{{AFX_DATA(CProjDlg)
	enum { IDD = IDD_PROJTYPE };
	int		m_nPTRadio;
	BOOL	m_bDocView;
	//}}AFX_DATA

	virtual BOOL CanDestroy();

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void ReadValues();          // Reads from projectOptions
	void WriteProjType();
	// LATER: Uncomment when we allow multiple langauges
	//CCheckList m_chklstLang;
	
	// Generated message map functions
	//{{AFX_MSG(CProjDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnClickedRadio(UINT nID);	
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

